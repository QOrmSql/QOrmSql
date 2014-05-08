# Adds QOrmSql shared library include paths and linker commands.

isEmpty(QORMSQL_OUT_PWD) {
QORMSQL_OUT_PWD=$$relative_path($$PWD, $$_PRO_FILE_PWD_)
}

win32:CONFIG(release, debug|release): LIBS += -L$$QORMSQL_OUT_PWD/release/ -lQOrmSql
else:win32:CONFIG(debug, debug|release): LIBS += -L$$QORMSQL_OUT_PWD/debug/ -lQOrmSql
else:unix: LIBS += -L"$$QORMSQL_OUT_PWD" -lQOrmSql

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
