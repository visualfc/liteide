#ifndef MDTOHTML_H
#define MDTOHTML_H

#include <QByteArray>

QByteArray mdtohtml(const QByteArray &data);
void fix_qt_textbrowser(bool b);

#endif // MDTOHTML_H
