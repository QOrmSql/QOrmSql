#include "QOrmDeleteQuery.h"
#include "QOrmModel.h"
#include "QOrmDatabase.h"
#include "QOrmMetaTable.h"
#include <QSqlQuery>

QOrmDeleteQuery::QOrmDeleteQuery(QOrmDatabase *database)
    : m_database(database ? database : QOrmDatabase::database())
{
}

bool QOrmDeleteQuery::exec(QOrmModel &model)
{
    QOrmMetaTable *table = m_database->findOrCreateTable(model.metaObject());
    QString code = "DELETE FROM `%1` WHERE `";
    code += table->rowidName();
    code += "`=%2";
    QSqlQuery query(m_database->sqlDatabase());
    bool execOk = query.exec(code.arg(model.tableName(), QString::number(model.rowid())));
    if (!execOk) {
        m_database->logger().logQueryError(query);
    }
    return execOk;
}
