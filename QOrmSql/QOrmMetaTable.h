#ifndef QORMSQL_QORMMETATABLE_H
#define QORMSQL_QORMMETATABLE_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QSqlQuery;
QT_END_NAMESPACE

class QOrmMetaTablePrivate;
class QOrmModel;
class QOrmDatabase;

class QOrmMetaTable : public QObject
{
    Q_OBJECT
public:
    static QOrmMetaTable *newInstance(QOrmDatabase *parent, const QMetaObject *metaobject);
    ~QOrmMetaTable();

    QOrmModel *newModel();
    QOrmModel *loadModelInVisitorOrder(QSqlQuery &query);
    QList<QOrmModel *> loadModelList(QSqlQuery &query);
    QString tableName() const;
    QString commaSeparatedFields() const;
    void markAsPersistent(QOrmModel *model);
    QString rowidName() const;

protected:
    QOrmMetaTable(QObject *parent, const QMetaObject *metaobject);
    QOrmMetaTablePrivate *d_ptr;

    QOrmDatabase *ormDatabase() const;
    void visitPrototype(QOrmModel &model);
    static bool validateMetaObject(QOrmDatabase *parent, const QMetaObject *metaobject);
    QOrmModel *internalNewModel();

private slots:
    void onDestroyModel(QObject *modelObject);

private:
    Q_DECLARE_PRIVATE(QOrmMetaTable);
};

#endif // QORMSQL_QORMMETATABLE_H
