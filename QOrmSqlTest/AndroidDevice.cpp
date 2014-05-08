#include "AndroidDevice.h"

AndroidDevice::AndroidDevice(QObject *parent) :
    QOrmUserModel<AndroidDevice>(parent)
{
}

QList<AndroidDevice *> AndroidDevice::findByName(const QString &name, QOrmDatabase *database)
{
    QOrmSelectQuery query(database);
    query.setWhere("name=?", QVariantList() << name);
    return query.selectAll<AndroidDevice>();
}

QString AndroidDevice::name() const
{
    return m_name;
}

void AndroidDevice::setName(const QString &name)
{
    m_name = name;
}

QString AndroidDevice::androidVersion() const
{
    return m_androidVersion;
}

void AndroidDevice::setAndroidVersion(const QString &androidVersion)
{
    m_androidVersion = androidVersion;
}

QString AndroidDevice::tableName() const
{
    return "device";
}

void AndroidDevice::accept(QOrmModelVisitor &visitor)
{
    visitor.enterField("name", m_name);
    visitor.enterField("androidVersion", m_androidVersion);
}
