#ifndef QORMSQL_GLOBAL_H
#define QORMSQL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QORMSQL_LIBRARY)
#  define QORMSQLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QORMSQLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QORMSQL_GLOBAL_H
