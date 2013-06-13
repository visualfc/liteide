#ifndef LITETTY_GLOBAL_H
#define LITETTY_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LITETTY_LIBRARY)
#  define LITETTYSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LITETTYSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LITETTY_GLOBAL_H
