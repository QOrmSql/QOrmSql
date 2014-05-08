#ifndef SINGLEDATABASETEST_H
#define SINGLEDATABASETEST_H

#include <QObject>

class QOrmDatabase;

class SingleDatabaseTest : public QObject
{
    Q_OBJECT
public:
    explicit SingleDatabaseTest(QObject *parent = 0);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
    void testCreation();
    void testLoading();
    void testSaving();
    void testIncompleteSaving();
    void testDeletion();

private:
    void copyDatabaseTo(const QString &targetFilePath);

    QOrmDatabase *m_mainDb;
    QOrmDatabase *m_secondaryDb;
};

#endif // SINGLEDATABASETEST_H
