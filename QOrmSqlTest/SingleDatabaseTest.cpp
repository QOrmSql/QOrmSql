#include "SingleDatabaseTest.h"
#include "AndroidDevice.h"
#include <QFile>
#include <QList>
#include <QtTest>

const char SECONDARY_CONN_NAME[] = "secondary";

SingleDatabaseTest::SingleDatabaseTest(QObject *parent)
    : QObject(parent)
    , m_mainDb(NULL)
    , m_secondaryDb(NULL)
{
}

void SingleDatabaseTest::initTestCase()
{
    copyDatabaseTo("QOrmSqlTest1.db");
    copyDatabaseTo("QOrmSqlTest2.db");
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("QOrmSqlTest1.db");
        db.open();
        m_mainDb = QOrmDatabase::createOrmDatabase(db);
    }
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", SECONDARY_CONN_NAME);
        db.setDatabaseName("QOrmSqlTest2.db");
        db.open();
        m_secondaryDb = QOrmDatabase::createOrmDatabase(db);
    }
}

void SingleDatabaseTest::cleanupTestCase()
{
    delete m_mainDb;
}

void SingleDatabaseTest::testCase1()
{
    QVERIFY2(QSqlDatabase::database().isOpen(), "Cannot create/open main SQLite database.");
    QVERIFY2(QOrmDatabase::database() != NULL, "Cannot create main ORM database");
    QVERIFY2(QSqlDatabase::database(SECONDARY_CONN_NAME).isOpen(), "Cannot create/open second SQLite database.");
    QVERIFY2(QOrmDatabase::database(SECONDARY_CONN_NAME) != NULL, "Cannot create second ORM database");
}

void SingleDatabaseTest::testCreation()
{
    AndroidDevice *device1 = AndroidDevice::create();
    QVERIFY(device1);
    AndroidDevice *device2 = AndroidDevice::create();
    delete device1;
    device1 = NULL;
    QVERIFY(device2);
    AndroidDevice *device3 = AndroidDevice::create();
    delete device3;
    device3 = NULL;
    delete device2;
    device2 = NULL;
}

void SingleDatabaseTest::testLoading()
{
    QList<AndroidDevice *> devices = AndroidDevice::findAll();
    QVERIFY(devices.size() == 2 || devices.size() == 3);
}

void SingleDatabaseTest::testSaving()
{
    {
        QList<AndroidDevice *> devices = AndroidDevice::findByName("Samsung S3");
        QVERIFY(devices.size() == 0);
        devices = AndroidDevice::findByName("Samsung S3", QOrmDatabase::database(SECONDARY_CONN_NAME));
        QVERIFY(devices.size() == 0);
    }
    {
        AndroidDevice *device = AndroidDevice::create();
        device->setName("Samsung S3");
        device->setAndroidVersion("4.2 Ice Cream Sandwitch");
        QVERIFY(device->save());
        delete device;
    }
    {
        QList<AndroidDevice *> devices = AndroidDevice::findByName("Samsung S3");
        QVERIFY(devices.size() == 1);
        devices = AndroidDevice::findByName("Samsung S3", QOrmDatabase::database(SECONDARY_CONN_NAME));
        QVERIFY(devices.size() == 0);
    }
    {
        AndroidDevice *device = AndroidDevice::create(QOrmDatabase::database(SECONDARY_CONN_NAME));
        device->setName("Samsung S3");
        device->setAndroidVersion("4.2 Ice Cream Sandwitch");
        QVERIFY(device->save());
        delete device;
    }
    {
        QList<AndroidDevice *> devices = AndroidDevice::findByName("Samsung S3");
        QVERIFY(devices.size() == 1);
        devices = AndroidDevice::findByName("Samsung S3", QOrmDatabase::database(SECONDARY_CONN_NAME));
        QVERIFY(devices.size() == 1);
    }
}

void SingleDatabaseTest::testIncompleteSaving()
{
    {
        QList<AndroidDevice *> devices = AndroidDevice::findByName("HTC One");
        QVERIFY(devices.size() == 0);
    }
    {
        AndroidDevice *device = AndroidDevice::create();
        device->setName("HTC One");
        QVERIFY(device->save());
        delete device;
    }
    {
        QList<AndroidDevice *> devices = AndroidDevice::findByName("HTC One");
        QVERIFY(devices.size() == 1);
    }
}

void SingleDatabaseTest::testDeletion()
{
    {
        QList<AndroidDevice *> devices = AndroidDevice::findByName("Samsung S4");
        QVERIFY(devices.size() == 0);
    }
    {
        AndroidDevice *device = AndroidDevice::create();
        device->setName("Samsung S4");
        device->setAndroidVersion("4.3 Kitkat");
        QVERIFY(device->save());
        delete device;
    }
    {
        QList<AndroidDevice *> devices = AndroidDevice::findByName("Samsung S4");
        QVERIFY(devices.size() == 1);
        devices.front()->deleteOnPersistentStorage();
    }
    {
        QList<AndroidDevice *> devices = AndroidDevice::findByName("Samsung S4");
        QVERIFY(devices.size() == 0);
    }
    {
        AndroidDevice *device = AndroidDevice::create();
        device->setName("Samsung S4");
        device->setAndroidVersion("4.4 Kitkat");
        QVERIFY(device->save());
        delete device;
    }
    {
        QList<AndroidDevice *> devices = AndroidDevice::findByName("Samsung S4");
        QVERIFY(devices.size() == 1);
    }


    {
        QList<AndroidDevice *> devices = AndroidDevice::findByName("Samsung S4", QOrmDatabase::database(SECONDARY_CONN_NAME));
        QVERIFY(devices.size() == 0);
    }
    {
        AndroidDevice *device = AndroidDevice::create(QOrmDatabase::database(SECONDARY_CONN_NAME));
        device->setName("Samsung S4");
        device->setAndroidVersion("4.3 Kitkat");
        QVERIFY(device->save());
        delete device;
    }
    {
        QList<AndroidDevice *> devices = AndroidDevice::findByName("Samsung S4", QOrmDatabase::database(SECONDARY_CONN_NAME));
        QVERIFY(devices.size() == 1);
        devices.front()->deleteOnPersistentStorage();
    }
    {
        QList<AndroidDevice *> devices = AndroidDevice::findByName("Samsung S4", QOrmDatabase::database(SECONDARY_CONN_NAME));
        QVERIFY(devices.size() == 0);
    }
    {
        AndroidDevice *device = AndroidDevice::create(QOrmDatabase::database(SECONDARY_CONN_NAME));
        device->setName("Samsung S4");
        device->setAndroidVersion("4.4 Kitkat");
        QVERIFY(device->save());
        delete device;
    }
    {
        QList<AndroidDevice *> devices = AndroidDevice::findByName("Samsung S4", QOrmDatabase::database(SECONDARY_CONN_NAME));
        QVERIFY(devices.size() == 1);
    }
}

void SingleDatabaseTest::copyDatabaseTo(const QString &targetFilePath)
{
    QFile in("://QOrmSqlTest.db");
    QFile out(targetFilePath);
    if (!in.open(QFile::ReadOnly) || !out.open(QFile::WriteOnly))
        return;
    out.write(in.readAll());
}
