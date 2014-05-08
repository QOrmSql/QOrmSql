#include "QOrmDatabase.h"
#include "QOrmMetaTable.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QHash>
#include <QDebug>

namespace {

class DatabaseMap : public QObject
{
    Q_OBJECT
public:
    void insert(QOrmDatabase *ormDb)
    {
        m_databases[ormDb->sqlConnectionName()] = ormDb;
        m_keys[ormDb] = ormDb->sqlConnectionName();
        connect(ormDb, SIGNAL(destroyed()), this, SLOT(onDatabaseDestroyed()));
    }

    QOrmDatabase *value(const QString &connectionName) const
    {
        return m_databases.value(connectionName);
    }

private slots:
    void onDatabaseDestroyed()
    {
        QObject *ormDb = sender();
        QString key = m_keys.value(ormDb);
        m_keys.remove(ormDb);
        m_databases.remove(key);
    }

private:
    QHash<QString, QOrmDatabase *> m_databases;
    QHash<QObject *, QString> m_keys;
};

Q_GLOBAL_STATIC(DatabaseMap, g_globalDatabaseMap);

class QOrmDatabaseLogger : public QOrmDatabase::ILogger
{
    // ILogger interface
public:
    void logQueryError(const QSqlQuery &query) Q_DECL_OVERRIDE;
    void logWarning(const QString &errorMessage) Q_DECL_OVERRIDE;
};

void QOrmDatabaseLogger::logQueryError(const QSqlQuery &query)
{
    QSqlError error = query.lastError();
    if (error.type() != QSqlError::NoError) {
        QString message = query.lastError().text();
        message += "\nSource code:\n";
        message += query.lastQuery();
        qWarning() << message;
    }
}

void QOrmDatabaseLogger::logWarning(const QString &errorMessage)
{
    qWarning() << errorMessage;
}

} // anonymous namespace

class QOrmDatabasePrivate
{
public:
    QOrmDatabasePrivate();

    bool isMysql;
    bool isSqlite;
    QSqlDatabase db;
    QHash<const QMetaObject *, QOrmMetaTable *> tables;
    QSet<const QMetaObject *> brokenModels;
    QOrmDatabaseLogger logger;
};

QOrmDatabasePrivate::QOrmDatabasePrivate()
    : isMysql(false)
    , isSqlite(false)
{
}

QOrmDatabase *QOrmDatabase::createOrmDatabase(const QSqlDatabase &db)
{
    QOrmDatabase *ormDb = new QOrmDatabase(db);
    g_globalDatabaseMap->insert(ormDb);
    return ormDb;
}

void QOrmDatabase::deleteOrmDatabase(QOrmDatabase *database)
{
    delete database;
}

QOrmDatabase *QOrmDatabase::database(const QString &connection)
{
    if (connection.isEmpty())
        return g_globalDatabaseMap->value(QSqlDatabase::defaultConnection);
    return g_globalDatabaseMap->value(connection);
}

QOrmDatabase::QOrmDatabase(const QSqlDatabase &db) :
    d_ptr(new QOrmDatabasePrivate)
{
    Q_D(QOrmDatabase);
    d->db = db;
    QString name = db.driverName();
    if (name == "QMYSQL") {
        d->isMysql = true;
    } else if (name == "QSQLITE" || name == "QSQLITE2") {
        d->isSqlite = true;
    }
}

QOrmDatabase::~QOrmDatabase()
{
//    QSqlDatabase::removeDatabase(sqlConnectionName());
    delete d_ptr;
}

QOrmMetaTable *QOrmDatabase::registerModelByMetaObject(const QMetaObject *object)
{
    Q_D(QOrmDatabase);
    QOrmMetaTable *table = QOrmMetaTable::newInstance(this, object);
    if (table) {
        d->tables[object] = table;
    }
    return table;
}

bool QOrmDatabase::isMysql() const
{
    return d_ptr->isMysql;
}

bool QOrmDatabase::isSqlite() const
{
    return d_ptr->isSqlite;
}

QString QOrmDatabase::sqlDatabaseName() const
{
    return d_ptr->db.databaseName();
}

QString QOrmDatabase::sqlConnectionName() const
{
    return d_ptr->db.connectionName();
}

const QSqlDatabase &QOrmDatabase::sqlDatabase() const
{
    return d_ptr->db;
}

QOrmMetaTable *QOrmDatabase::findOrCreateTable(const QMetaObject *object)
{
    if (d_ptr->brokenModels.contains(object))
        return NULL;

    QOrmMetaTable *table = d_ptr->tables.value(object);
    if (!table) {
        table = registerModelByMetaObject(object);
        if (!table) {
            d_ptr->brokenModels.insert(object);
        }
    }
    return table;
}

QOrmDatabase::ILogger &QOrmDatabase::logger()
{
    return d_ptr->logger;
}

#include "QOrmDatabase.moc"
