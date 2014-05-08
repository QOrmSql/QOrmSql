#ifndef QORMSAVEQUERY_H
#define QORMSAVEQUERY_H

#include "QOrmModelVisitor.h"

class QOrmModel;
class QOrmDatabase;

class QOrmSaveQueryPrivate;

class QOrmSaveQuery : public QOrmModelVisitor
{
    Q_DISABLE_COPY(QOrmSaveQuery)
public:
    QOrmSaveQuery(QOrmDatabase *database = NULL);
    ~QOrmSaveQuery();
    bool exec(QOrmModel &model);

protected:
    void visit() Q_DECL_OVERRIDE;
    QOrmSaveQueryPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QOrmSaveQuery);
};

#endif // QORMSAVEQUERY_H
