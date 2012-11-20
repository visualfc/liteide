#ifndef HELLO_GLOBAL_H
#define HELLO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(HELLO_LIBRARY)
#  define HELLOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define HELLOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // HELLO_GLOBAL_H
