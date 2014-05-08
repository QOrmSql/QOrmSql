#ifndef QORMSQL_QORMDELETEQUERY_H
#define QORMSQL_QORMDELETEQUERY_H

#include <QString>

class QOrmModel;
class QOrmDatabase;

class QOrmDeleteQuery
{
public:
    QOrmDeleteQuery(QOrmDatabase *database = NULL);
    bool exec(QOrmModel &model);

private:
    QOrmDatabase *m_database;
};

#endif // QORMSQL_QORMDELETEQUERY_H
