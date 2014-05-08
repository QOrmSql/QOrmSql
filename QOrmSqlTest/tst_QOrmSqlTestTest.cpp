#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include "SingleDatabaseTest.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);

    int status = 0;

    SingleDatabaseTest sdt;
    status |= QTest::qExec(&sdt, argc, argv);

    return status;
}
