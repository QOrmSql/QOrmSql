#-------------------------------------------------
#
# Project created by QtCreator 2014-04-29T09:32:55
#
#-------------------------------------------------

QT       += sql

QT       -= gui

TARGET = QOrmSql
TEMPLATE = lib

DEFINES += QORMSQL_LIBRARY

SOURCES += QOrmSql.cpp \
    QOrmField.cpp \
    QOrmModel.cpp \
    QOrmSaveQuery.cpp \
    QOrmModelVisitor.cpp \
    QOrmDeleteQuery.cpp \
    QOrmSelectQuery.cpp \
    QOrmDatabase.cpp \
    QOrmMetaTable.cpp

HEADERS += QOrmSql.h\
        qormsql_global.h \
    QOrmField.h \
    QOrmModel.h \
    QOrmSaveQuery.h \
    QOrmModelVisitor.h \
    QOrmDeleteQuery.h \
    QOrmSelectQuery.h \
    QOrmDatabase.h \
    QOrmMetaTable.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
