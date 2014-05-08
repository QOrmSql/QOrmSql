#ifndef QORMSELECTQUERY_H
#define QORMSELECTQUERY_H

#include "QOrmModelVisitor.h"
#include <QString>
#include <QList>
#include <QObject>

class QOrmModel;
class QOrmDatabase;

class QOrmSelectQuery
{
public:
    QOrmSelectQuery(QOrmDatabase *database = NULL);

    void setWhere(const QString &condition, const QVariantList &values);
    void setLimit(int limit);
    void setOrderBy(const QString &field, bool ascending);
    void setOrderBy(const QStringList &fields, bool ascending);

    template <class T>
    QList<T *> selectAll()
    {
        QList<QOrmModel *> models = _selectWithMetaobject(&T::staticMetaObject);
        QList<T *> ret;
        ret.reserve(models.size());
        foreach (QOrmModel *model, models)
            ret.append(static_cast<T *>(model));
        return ret;
    }

    template <class T>
    T *selectOne()
    {
        setLimit(1);
        QList<QOrmModel *> result = _selectWithMetaobject(&T::staticMetaObject);
        if (result.isEmpty())
            return NULL;
        return static_cast<T *>(result.front());
    }

    template <class T>
    int selectCount()
    {
        return _selectCountWithMetaobject(&T::staticMetaObject);
    }

private:
    QList<QOrmModel *> _selectWithMetaobject(const QMetaObject *metaobject);
    int _selectCountWithMetaobject(const QMetaObject *metaobject);
    void _addConditionAndSuffix(QString &code);

    QOrmDatabase *m_database;
    QString m_condition;
    QVariantList m_values;
    int m_limit;
    QStringList m_orderByFields;
    bool m_orderAscending;
};

#endif // QORMSELECTQUERY_H
