#ifndef QORMSQL_QORMFIELDVISITOR_H
#define QORMSQL_QORMFIELDVISITOR_H

#include "QOrmField.h"

class QOrmModel;

class QOrmModelVisitor
{
public:
    QOrmModelVisitor();

    void start(QOrmModel &model);

    template <class T> void enterField(const QString &fieldName, QOrmPublic<T> &field)
    {
        m_fieldName = fieldName;
        m_type = QOrmPublic<T>::type();
        m_getter = static_cast<FieldGetter>(&QOrmPublic<T>::toVariant);
        m_setter = static_cast<FieldSetter>(&QOrmPublic<T>::setVariant);
        m_field = &field;
        visit();
    }

    template <class T> void enterField(const QString &fieldName, QOrmPrivate<T> &field)
    {
        m_fieldName = fieldName;
        m_type = QOrmPrivate<T>::type();
        m_getter = static_cast<FieldGetter>(&QOrmPrivate<T>::toVariant);
        m_setter = static_cast<FieldSetter>(&QOrmPrivate<T>::setVariant);
        m_field = &field;
        visit();
    }

protected:
    typedef QVariant (QOrmAbstractField::*FieldGetter)() const;
    typedef void (QOrmAbstractField::*FieldSetter)(const QVariant &variant);

    virtual void visit() = 0;
    QString fieldName() const;
    QVariant::Type type() const;
    FieldGetter getter() const;
    FieldSetter setter() const;
    QOrmAbstractField *field() const;

private:
    QString m_fieldName;
    QVariant::Type m_type;
    FieldGetter m_getter;
    FieldSetter m_setter;
    QOrmAbstractField *m_field;
};

#endif // QORMSQL_QORMFIELDVISITOR_H
