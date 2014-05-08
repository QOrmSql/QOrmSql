#include "QOrmModelVisitor.h"
#include "QOrmModel.h"

QOrmModelVisitor::QOrmModelVisitor()
{
}

void QOrmModelVisitor::start(QOrmModel &model)
{
    model.accept(*this);
}

QString QOrmModelVisitor::fieldName() const
{
    return m_fieldName;
}

QVariant::Type QOrmModelVisitor::type() const
{
    return m_type;
}

QOrmModelVisitor::FieldGetter QOrmModelVisitor::getter() const
{
    return m_getter;
}

QOrmModelVisitor::FieldSetter QOrmModelVisitor::setter() const
{
    return m_setter;
}

QOrmAbstractField *QOrmModelVisitor::field() const
{
    return m_field;
}
