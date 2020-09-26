#ifndef EMDILIB_GLOBAL_H
#define EMDILIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(EMDILIB_LIBRARY)
#  define EMDILIB_EXPORT Q_DECL_EXPORT
#else
#  define EMDILIB_EXPORT Q_DECL_IMPORT
#endif

#endif // EMDILIB_GLOBAL_H
