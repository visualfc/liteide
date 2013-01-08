/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**************************************************************************/
// Module: textoutput.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef TEXTOUTPUT_H
#define TEXTOUTPUT_H

#include "terminaledit.h"

class TextOutput : public TerminalEdit
{
    Q_OBJECT
public:
    explicit TextOutput(bool readOnly = true, QWidget *parent = 0);
    void append(const QString &text);
    void append(const QString &text,const QBrush &foreground);
    void appendTag0(const QString &text, bool error = false);
    void appendTag1(const QString &text, bool error = false);
    void updateExistsTextColor(const QBrush &foreground = Qt::gray);
    void setMaxLine(int max);
protected:
    TerminalEdit  *m_editor;
    QTextCharFormat m_fmt;
};

#endif // TEXTOUTPUT_H
