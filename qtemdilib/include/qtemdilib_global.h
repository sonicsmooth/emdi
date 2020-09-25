#ifndef QTEMDILIB_GLOBAL_H
#define QTEMDILIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTEMDILIB_LIBRARY)
#  define QTEMDILIB_EXPORT Q_DECL_EXPORT
#else
#  define QTEMDILIB_EXPORT Q_DECL_IMPORT
#endif

#endif // QTEMDILIB_GLOBAL_H
