#-------------------------------------------------
#
# Project created by QtCreator 2014-05-03T22:47:05
#
#-------------------------------------------------

QT       += testlib sql

QT       -= gui

TARGET = tst_QOrmSqlTestTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_QOrmSqlTestTest.cpp \
    SingleDatabaseTest.cpp \
    AndroidDevice.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../QOrmSql/qormsql.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QOrmSql/release/ -lQOrmSql
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QOrmSql/debug/ -lQOrmSql
else:unix: LIBS += -L$$OUT_PWD/../QOrmSql/ -lQOrmSql

INCLUDEPATH += $$PWD/../QOrmSql
DEPENDPATH += $$PWD/../QOrmSql

OTHER_FILES +=

RESOURCES += \
    QOrmSqlTest.qrc

HEADERS += \
    SingleDatabaseTest.h \
    AndroidDevice.h
