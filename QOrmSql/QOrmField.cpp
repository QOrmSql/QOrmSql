#include "QOrmField.h"

template <>
QOrmBaseField<QDate>::QOrmBaseField()
{
}

template <>
QOrmBaseField<QString>::QOrmBaseField()
{
}

template <>
QOrmBaseField<QDateTime>::QOrmBaseField()
{
}
