#ifndef ANDROIDDEVICE_H
#define ANDROIDDEVICE_H

#include <QObject>
#include "QOrmModel.h"
#include "QOrmDatabase.h"

class AndroidDevice : public QOrmUserModel<AndroidDevice>
{
    Q_OBJECT
public:
    Q_INVOKABLE AndroidDevice(QObject *parent);
    static QList<AndroidDevice *> findByName(const QString &name, QOrmDatabase *database = NULL);

    QString name() const;
    QString androidVersion() const;

    void setName(const QString &name);
    void setAndroidVersion(const QString &androidVersion);

protected:
    QString tableName() const;
    void accept(QOrmModelVisitor &visitor);

private:
    QOrmPrivate<QString> m_name;
    QOrmPrivate<QString> m_androidVersion;
};

#endif // ANDROIDDEVICE_H
