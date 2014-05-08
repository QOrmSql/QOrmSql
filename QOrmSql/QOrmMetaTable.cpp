#include "QOrmMetaTable.h"
#include "QOrmField.h"
#include "QOrmModel.h"
#include "QOrmDatabase.h"
#include "QOrmModelVisitor.h"
#include <QMetaObject>
#include <QSqlQuery>
#include <QMetaMethod>
#include <QScopedPointer>
#include <QDebug>

namespace {

class QOrmTableColumn
{
public:
    typedef QVariant (QOrmAbstractField::*FieldGetter)() const;
    typedef void (QOrmAbstractField::*FieldSetter)(const QVariant &variant);
    QOrmTableColumn()
        : m_type(QVariant::Invalid)
        , m_getter(NULL)
        , m_setter(NULL)
    {
    }

    QString m_fieldName;
    QVariant::Type m_type;
    FieldGetter m_getter;
    FieldSetter m_setter;
};

} // anonymous namespace

class QOrmMetaTablePrivate
{
public:
    QList<QOrmTableColumn> m_columns;
    QString m_tableName;
    QHash<int, QOrmModel *> m_persistentModels;
    QSet<QOrmModel *> m_temporaryModels;
    const QMetaObject *m_metaobject;
};

namespace {

class QOqmMetaTableLoader : public QOrmModelVisitor
{
public:
    QOqmMetaTableLoader(QOrmMetaTablePrivate *tablePrivate);

protected:
    void visit() Q_DECL_OVERRIDE;

private:
    QOrmMetaTablePrivate *m_tablePrivate;
};

class QOrmModelWriter : public QOrmModelVisitor
{
public:
    QOrmModelWriter(QSqlQuery &query);

protected:
    void visit() Q_DECL_OVERRIDE;

private:
    QSqlQuery &m_query;
    int m_valueIndex;
};

QOqmMetaTableLoader::QOqmMetaTableLoader(QOrmMetaTablePrivate *tablePrivate)
    : m_tablePrivate(tablePrivate)
{
}

void QOqmMetaTableLoader::visit()
{
    QOrmTableColumn column;
    column.m_fieldName = fieldName();
    column.m_type = type();
    column.m_getter = getter();
    column.m_setter = setter();
    m_tablePrivate->m_columns.append(column);
}

QOrmModelWriter::QOrmModelWriter(QSqlQuery &query)
    : m_query(query)
    , m_valueIndex(0)
{
}

void QOrmModelWriter::visit()
{
    ++m_valueIndex;
    QOrmAbstractField *f = field();
    FieldSetter func = setter();
    (f->*func)(m_query.value(m_valueIndex));
    f->markClean();
}

} // anonymous namespace

QOrmMetaTable::QOrmMetaTable(QObject *parent, const QMetaObject *metaobject)
    : QObject(parent)
    , d_ptr(new QOrmMetaTablePrivate)
{
    Q_D(QOrmMetaTable);
    d->m_metaobject = metaobject;

    QScopedPointer<QOrmModel> model(qobject_cast<QOrmModel *>(d->m_metaobject->newInstance(Q_ARG(QObject *, this))));
    visitPrototype(*model);
}

QOrmDatabase *QOrmMetaTable::ormDatabase() const
{
    return qobject_cast<QOrmDatabase *>(parent());
}

void QOrmMetaTable::visitPrototype(QOrmModel &model)
{
    Q_D(QOrmMetaTable);
    d->m_tableName = model.tableName();
    QOqmMetaTableLoader loader(d);
    loader.start(model);
}

bool QOrmMetaTable::validateMetaObject(QOrmDatabase *parent, const QMetaObject *metaobject)
{
    bool hasValidConstructor = false;
    for (int i = 0, n = metaobject->constructorCount(); i < n; ++i) {
        QMetaMethod ctor = metaobject->constructor(i);
        if (ctor.parameterCount() != 1)
            continue;
        if (ctor.parameterType(0) != qMetaTypeId<QObject *>())
            continue;
        hasValidConstructor = true;
    }
    if (!hasValidConstructor) {
        QByteArray warning = "Qt ORM model cannot be registered: constructor <";
        warning += metaobject->className();
        warning += "(QObject *parent)> does not exist.";
        parent->logger().logWarning(QString::fromLatin1(warning));
        return false;
    }
    return true;
}

QOrmModel *QOrmMetaTable::internalNewModel()
{
    Q_D(QOrmMetaTable);
    QOrmModel *model = qobject_cast<QOrmModel *>(d->m_metaobject->newInstance(Q_ARG(QObject *, this)));
    connect(model, SIGNAL(destroyed(QObject *)), this, SLOT(onDestroyModel(QObject *)));
    return model;
}

void QOrmMetaTable::onDestroyModel(QObject *modelObject)
{
    Q_D(QOrmMetaTable);
    QOrmModel *model = static_cast<QOrmModel *>(modelObject);
    if (model->isCreated()) {
        d->m_temporaryModels.remove(model);
    } else {
        d->m_persistentModels.remove(model->rowid());
    }
}

QOrmMetaTable *QOrmMetaTable::newInstance(QOrmDatabase *parent, const QMetaObject *metaobject)
{
    if (validateMetaObject(parent, metaobject)) {
        return new QOrmMetaTable(parent, metaobject);
    }
    return NULL;
}

QOrmMetaTable::~QOrmMetaTable()
{
    delete d_ptr;
}

QOrmModel *QOrmMetaTable::newModel()
{
    Q_D(QOrmMetaTable);
    QOrmModel *model = internalNewModel();
    model->setDatabase(ormDatabase());
    d->m_temporaryModels.insert(model);
    return model;
}

QOrmModel *QOrmMetaTable::loadModelInVisitorOrder(QSqlQuery &query)
{
    Q_D(QOrmMetaTable);
    if (query.next()) {
        int rowid = query.value(0).toInt();
        QOrmModel *model  = d->m_persistentModels.value(rowid);
        if (!model) {
            model = internalNewModel();
            model->setRowid(rowid);
            d->m_persistentModels[rowid] = model;
        }
        QOrmModelWriter writer(query);
        writer.start(*model);
        return model;
    }
    return NULL;
}

QList<QOrmModel *> QOrmMetaTable::loadModelList(QSqlQuery &query)
{
    QList<QOrmModel *> ret;
    QOrmDatabase *ormDb = ormDatabase();
    while (QOrmModel *model = loadModelInVisitorOrder(query)) {
        model->setDatabase(ormDb);
        ret.append(model);
    }
    return ret;
}

QString QOrmMetaTable::tableName() const
{
    return d_ptr->m_tableName;
}

QString QOrmMetaTable::commaSeparatedFields() const
{
    QString ret;
    ret.reserve(64);
    foreach (const QOrmTableColumn &column, d_ptr->m_columns) {
        if (!ret.isEmpty())
            ret += QLatin1Char(',');
        ret += QLatin1Char('`');
        ret += column.m_fieldName;
        ret += QLatin1Char('`');
    }
    return ret;
}

void QOrmMetaTable::markAsPersistent(QOrmModel *model)
{
    Q_D(QOrmMetaTable);
    if (!d->m_temporaryModels.contains(model))
        return;
    if (model->rowid() <= 0)
        return;
    d->m_temporaryModels.remove(model);
    d->m_persistentModels[model->rowid()] = model;
}

QString QOrmMetaTable::rowidName() const
{
    return ormDatabase()->isSqlite() ? "rowid" : "id";
}
