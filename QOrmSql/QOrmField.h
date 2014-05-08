#ifndef QORMSQL_QORMFIELD_H
#define QORMSQL_QORMFIELD_H

#include <QVariant>
#include <QDateTime>
#include <QString>
#include <QDate>

class QOrmAbstractField
{
public:
    QOrmAbstractField()
        : m_isDirty(false)
    {
    }

    inline void markClean()
    {
        m_isDirty = false;
    }

    inline void markDirty()
    {
        m_isDirty = true;
    }

    inline bool isDirty() const
    {
        return m_isDirty;
    }

private:
    bool m_isDirty;
};

template <class T> class QOrmBaseField : public QOrmAbstractField
{
    Q_DISABLE_COPY(QOrmBaseField);
public:
    QOrmBaseField()
        : m_value(0)
    {
    }

    static QVariant::Type type()
    {
        return (QVariant::Type)qMetaTypeId<T>();
    }

    QVariant toVariant() const
    {
        return QVariant::fromValue<T>(m_value);
    }

    void setVariant(const QVariant &variant)
    {
        T value = variant.value<T>();
        if (m_value != value) {
            markDirty();
            m_value = value;
        }
    }

protected:
    T m_value;
};

template <> QOrmBaseField<QString>::QOrmBaseField();
template <> QOrmBaseField<QDateTime>::QOrmBaseField();
template <> QOrmBaseField<QDate>::QOrmBaseField();

template <class T> class QOrmPublic : public QOrmBaseField<T>
{
    Q_DISABLE_COPY(QOrmPublic);
public:
    QOrmPublic()
    {
    }

    inline const T &operator()() const
    {
        return QOrmBaseField<T>::m_value;
    }

    inline void operator()(const T &value)
    {
        if (QOrmBaseField<T>::m_value != value) {
            QOrmBaseField<T>::markDirty();
            QOrmBaseField<T>::m_value = value;
        }
    }
};

template <class T> class QOrmPrivate : public QOrmBaseField<T>
{
public:
    QOrmPrivate()
    {
    }

    inline operator const T &() const
    {
        return QOrmBaseField<T>::m_value;
    }

    inline QOrmPrivate &operator =(const T &value)
    {
        if (QOrmBaseField<T>::m_value != value) {
            QOrmBaseField<T>::markDirty();
            QOrmBaseField<T>::m_value = value;
        }
        return *this;
    }

    inline QOrmPrivate &operator =(const QOrmPrivate<T> &other)
    {
        if (QOrmBaseField<T>::m_value != other.m_value) {
            QOrmBaseField<T>::markDirty();
            QOrmBaseField<T>::m_value = other.value;
        }
        return *this;
    }
};

#endif // QORMSQL_QORMFIELD_H
