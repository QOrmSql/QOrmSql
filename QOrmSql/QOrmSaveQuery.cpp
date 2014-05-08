#include "QOrmSaveQuery.h"
#include "QOrmModel.h"
#include "QOrmDatabase.h"
#include "QOrmMetaTable.h"
#include <QSqlQuery>
#include <QSqlError>

class QOrmSaveQueryPrivate
{
public:
    QOrmDatabase *database;
    bool isBinding;
    bool isInserting;
    bool isFirstName;
    int fieldIndex;
    QString code;
    QSqlQuery query;

    QOrmSaveQueryPrivate(QOrmDatabase *database)
        : database(database)
        , query(database->sqlDatabase())
    {
        reset();
    }

    void reset()
    {
        isBinding = false;
        isInserting = false;
        isFirstName = true;
        fieldIndex = 0;
        code.clear();
    }
};

QOrmSaveQuery::QOrmSaveQuery(QOrmDatabase *database)
    : d_ptr(new QOrmSaveQueryPrivate(database ? database : QOrmDatabase::database()))
{
}

QOrmSaveQuery::~QOrmSaveQuery()
{
    delete d_ptr;
}

bool QOrmSaveQuery::exec(QOrmModel &model)
{
    Q_D(QOrmSaveQuery);
    d->reset();
    d->isInserting = model.isCreated();
    if (d->isInserting) {
        d->code = "INSERT INTO `";
        d->code += model.tableName();
        d->code += "` (";
    } else {
        d->code += "UPDATE `";
        d->code += model.tableName();
        d->code += "` SET ";
    }

    start(model);
    if (d->fieldIndex == 0) {
        // Nothing to save.
        return false;
    }

    if (d->isInserting) {
        d->code += ") VALUES (";
        const int n = d->fieldIndex - 1;
        if (n >= 0) {
            for (int i = 0; i < n; ++i)
                d->code += "?,";
            d->code += "?)";
        }
    } else {
        QOrmMetaTable *table = d->database->findOrCreateTable(model.metaObject());
        d->code += " WHERE `";
        d->code += table->rowidName();
        d->code += "`=?";
    }

    d->query.prepare(d->code);
    d->isBinding = true;
    d->fieldIndex = 0;
    start(model);
    if (!d->isInserting) {
        d->query.bindValue(d->fieldIndex, model.rowid());
    }

    if (d->query.exec()) {
        if (model.isCreated()) {
            model.setRowid(d->query.lastInsertId().toInt());
        }
        return true;
    } else {
        d->database->logger().logQueryError(d->query);
    }
    return false;
}

void QOrmSaveQuery::visit()
{
    Q_D(QOrmSaveQuery);
    if (!field()->isDirty())
        return;

    if (d->isBinding) {
        QOrmAbstractField *f = field();
        FieldGetter func = getter();
        d->query.bindValue(d->fieldIndex, (f->*func)());
    } else if (d->isInserting) {
        if (!d->isFirstName)
            d->code += ",";
        d->isFirstName = false;
        d->code += "`";
        d->code += fieldName();
        d->code += "`";
    } else {
        if (!d->isFirstName)
            d->code += ",";
        d->isFirstName = false;
        d->code += "`";
        d->code += fieldName();
        d->code += "`=?";
    }
    ++d->fieldIndex;
}
