/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
// Module: litewordcompleter.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEWORDCOMPLETER_H
#define LITEWORDCOMPLETER_H

#include "litecompleter.h"
#include <QHash>
#include <QSet>
#include <QRegExp>

class QStandardItemModel;
class QStandardItem;
class QTextCursor;
class LiteWordCompleter : public LiteCompleter
{
    Q_OBJECT
public:
    explicit LiteWordCompleter(QObject *parent = 0);
public slots:
    virtual void completionPrefixChanged(QString);
protected:
    virtual QString textUnderCursor(QTextCursor tc) const;
    QSet<QString>   m_wordSet;
    QIcon           m_icon;
};

#endif // LITEWORDCOMPLETER_H
