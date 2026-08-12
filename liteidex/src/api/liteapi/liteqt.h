#ifndef LITEQT_H
#define LITEQT_H

#include <Qt>
#include <QString>
#include <QTextStream>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#define qtKeepEmptyParts Qt::KeepEmptyParts
#define qtSkipEmptyParts Qt::SkipEmptyParts
#else
#define qtKeepEmptyParts QString::KeepEmptyParts
#define qtSkipEmptyParts QString::SkipEmptyParts
#endif

inline void qtSetUtf8Encoding(QTextStream &stream)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif
}


#endif // LITEQT_H
