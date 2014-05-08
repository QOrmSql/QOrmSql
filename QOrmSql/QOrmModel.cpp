#include "QOrmModel.h"
#include "QOrmSaveQuery.h"
#include "QOrmDeleteQuery.h"
#include "QOrmDatabase.h"
#include "QOrmMetaTable.h"

QOrmModel::QOrmModel(QObject *parent)
    : QObject(parent)
    , m_rowid(0)
    , m_database(NULL)
{
}

int QOrmModel::rowid() const
{
    return m_rowid;
}

void QOrmModel::setRowid(int value)
{
    m_rowid = value;
}

/// @return False if model loaded from persistent storage.
bool QOrmModel::isCreated() const
{
    return (m_rowid == 0);
}

/// Saves model changes to persistent storage.
bool QOrmModel::save()
{
    QOrmMetaTable *table = m_database->findOrCreateTable(metaObject());
    QOrmSaveQuery query(m_database);
    if (query.exec(*this)) {
        table->markAsPersistent(this);
        return true;
    }
    return false;
}

/// Deletes model on persistent storage, than in memory.
void QOrmModel::deleteOnPersistentStorage()
{
    QOrmDeleteQuery query(m_database);
    query.exec(*this);
    delete this;
}

QString QOrmModel::helperRowidName(const QMetaObject *metaobject, QOrmDatabase *database)
{
    if (!database)
        database = QOrmDatabase::database();
    QOrmMetaTable *table = database->findOrCreateTable(metaobject);
    return table->rowidName();
}

QOrmModel *QOrmModel::helperCreate(const QMetaObject *metaobject, QOrmDatabase *database)
{
    if (!database)
        database = QOrmDatabase::database();
    QOrmMetaTable *table = database->findOrCreateTable(metaobject);
    if (!table)
        return NULL;
    return table->newModel();
}
QOrmDatabase *QOrmModel::database() const
{
    return m_database;
}

void QOrmModel::setDatabase(QOrmDatabase *database)
{
    m_database = database;
}

