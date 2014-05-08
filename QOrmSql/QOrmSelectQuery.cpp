#include "QOrmSelectQuery.h"
#include "QOrmDatabase.h"
#include "QOrmMetaTable.h"
#include <QSqlQuery>
#include <QVariant>

QOrmSelectQuery::QOrmSelectQuery(QOrmDatabase *database)
    : m_database(database ? database : QOrmDatabase::database())
    , m_limit(0)
    , m_orderAscending(false)
{
}

void QOrmSelectQuery::setWhere(const QString &condition, const QVariantList &values)
{
    m_condition = condition;
    m_values = values;
}

void QOrmSelectQuery::setLimit(int limit)
{
    m_limit = limit;
}

void QOrmSelectQuery::setOrderBy(const QString &field, bool ascending)
{
    m_orderByFields.append(field);
    m_orderAscending = ascending;
}

void QOrmSelectQuery::setOrderBy(const QStringList &fields, bool ascending)
{
    m_orderByFields = fields;
    m_orderAscending = ascending;
}

QList<QOrmModel *> QOrmSelectQuery::_selectWithMetaobject(const QMetaObject *metaobject)
{
    QOrmMetaTable *table = m_database->findOrCreateTable(metaobject);
    if (table != NULL) {
        QString code = "SELECT `";
        code += table->rowidName();
        code += "`,";
        code += table->commaSeparatedFields();
        code += " FROM `";
        code += table->tableName();
        code += "`";
        _addConditionAndSuffix(code);

        QSqlQuery query(m_database->sqlDatabase());
        query.prepare(code);
        if (!m_condition.isEmpty()) {
            for (int i = 0, n = m_values.size(); i < n; ++i)
                query.bindValue(0, m_values[i]);
        }
        if (!query.exec()) {
            m_database->logger().logQueryError(query);
        } else {
            return table->loadModelList(query);
        }
    }
    return QList<QOrmModel *>();
}

/// @return -1 on error, rows count otherwise.
int QOrmSelectQuery::_selectCountWithMetaobject(const QMetaObject *metaobject)
{
    QOrmMetaTable *table = m_database->findOrCreateTable(metaobject);
    if (table == NULL)
        return -1;

    QString code = "SELECT COUNT(*) FROM `";
    code += table->tableName();
    code += "`";
    _addConditionAndSuffix(code);

    QSqlQuery query(m_database->sqlDatabase());
    query.prepare(code);
    if (!m_condition.isEmpty()) {
        for (int i = 0, n = m_values.size(); i < n; ++i)
            query.bindValue(0, m_values[i]);
    }
    if (!query.exec()) {
        m_database->logger().logQueryError(query);
    } else {
        return query.value(0).toInt();
    }
    return -1;
}

void QOrmSelectQuery::_addConditionAndSuffix(QString &code)
{
    if (!m_condition.isEmpty()) {
        code += " WHERE ";
        code += m_condition;
    }
    if (m_limit > 0) {
        code += " LIMIT ";
        code += QString::number(m_limit);
    }
    if (!m_orderByFields.isEmpty()) {
        code += " ORDER BY ";
        for (int i = 0, n = m_orderByFields.size(); i < n; ++i) {
            if (i != 0)
                code + QLatin1Char(',');
            code += QLatin1Char('`');
            code += m_orderByFields[i];
            code += QLatin1Char('`');
        }
        if (m_orderAscending) {
            code += "ASC";
        } else {
            code += "DESC";
        }
    }
}
