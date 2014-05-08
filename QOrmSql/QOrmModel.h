#ifndef QORMSQL_QORMMODEL_H
#define QORMSQL_QORMMODEL_H

#include <QObject>
#include "QOrmField.h"
#include "QOrmModelVisitor.h"
#include "QOrmSelectQuery.h"

class QOrmModel : public QObject
{
    Q_OBJECT
public:
    explicit QOrmModel(QObject *parent);

    virtual QString tableName() const = 0;
    virtual void accept(QOrmModelVisitor &visitor) = 0;
    int rowid() const;
    void setRowid(int value);
    QOrmDatabase *database() const;
    void setDatabase(QOrmDatabase *database);
    bool isCreated() const;

    bool save();
    void deleteOnPersistentStorage();

protected:
    static QString helperRowidName(const QMetaObject *metaobject, QOrmDatabase *database = NULL);
    static QOrmModel *helperCreate(const QMetaObject *metaobject, QOrmDatabase *database = NULL);

private:
    int m_rowid;
    QOrmDatabase *m_database;
};

template <class T>
class QOrmUserModel : public QOrmModel
{
public:
    explicit QOrmUserModel(QObject *parent) : QOrmModel(parent) {}

    static T *create(QOrmDatabase *database = NULL)
    {
        return static_cast<T *>(helperCreate(&T::staticMetaObject, database));
    }

    static QList<T *> findAll(QOrmDatabase *database = NULL)
    {
        QOrmSelectQuery query(database);
        return query.selectAll<T>();
    }

    static T *findById(const int rowid, QOrmDatabase *database = NULL)
    {
        QString code = helperRowidName(&T::staticMetaObject, database) + "=?";
        QOrmSelectQuery query(database);
        query.setWhere(code, QVariantList() << rowid);
        return query.selectOne<T>();
    }
};

#endif // QORMSQL_QORMMODEL_H
