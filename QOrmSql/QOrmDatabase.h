#ifndef QORMDATABASE_H
#define QORMDATABASE_H

#include "qormsql_global.h"
#include <QObject>
#include <QSqlDatabase>

class QSqlQuery;
class QOrmModel;
class QOrmMetaTable;

class QOrmDatabasePrivate;

class QOrmDatabase : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QOrmDatabase)
public:
    class ILogger
    {
    public:
        virtual ~ILogger() {}
        virtual void logQueryError(const QSqlQuery &query) = 0;
        virtual void logWarning(const QString &errorMessage) = 0;
    };

    static QOrmDatabase *createOrmDatabase(const QSqlDatabase &db);
    static void deleteOrmDatabase(QOrmDatabase *database);
    static QOrmDatabase *database(const QString &connection = QString());

    ~QOrmDatabase();
    bool isMysql() const;
    bool isSqlite() const;
    QString sqlDatabaseName() const;
    QString sqlConnectionName() const;
    const QSqlDatabase &sqlDatabase() const;
    QOrmMetaTable *findOrCreateTable(const QMetaObject *object);
    ILogger &logger();

protected:
    QOrmDatabase(const QSqlDatabase &db);
    QOrmMetaTable *registerModelByMetaObject(const QMetaObject *object);

    QOrmDatabasePrivate *d_ptr;
private:
    Q_DECLARE_PRIVATE(QOrmDatabase)
};

#endif // QORMDATABASE_H
