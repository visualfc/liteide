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
// Module: syntaxcompleter.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef SYNTAXCOMPLETER_H
#define SYNTAXCOMPLETER_H

#include <QCompleter>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QAbstractItemView>

class SyntaxCompleter : public QCompleter
{
public:
    SyntaxCompleter(QObject *parent = 0) : QCompleter(parent)
    {
    }
    virtual bool underCursor(int key, QTextCursor cur, const QString &text, const QString &word)
    {
        return false;
    }
    virtual void setFileName(const QString &path)
    {
        fileName = path;
    }
    virtual void showPopup(const QString &completionPrefix)
    {
    }

    virtual void hidePopup()
    {
    }
    virtual void endCompletion()
    {

    }

public:
    QString fileName;
};


#endif // SYNTAXCOMPLETER_H
