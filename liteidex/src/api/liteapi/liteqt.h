#ifndef LITEQT_H
#define LITEQT_H

#include <Qt>
#include <QString>

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#define qtKeepEmptyParts Qt::KeepEmptyParts
#define qtSkipEmptyParts Qt::SkipEmptyParts
#else
#define qtKeepEmptyParts QString::KeepEmptyParts
#define qtSkipEmptyParts QString::SkipEmptyParts
#endif


#endif // LITEQT_H
