/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
// Module: jsonedit.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef JSONEDIT_H
#define JSONEDIT_H

#include <liteapi/liteapi.h>
#include <liteeditorapi/liteeditorapi.h>
#include <qjson/include/QJson/Parser>
#include <qjson/include/QJson/Serializer>

class JsonEdit : public QObject
{
    Q_OBJECT
public:
    explicit JsonEdit(LiteApi::IApplication *app, LiteApi::IEditor *editor, QObject *parent = 0);

signals:

public slots:
    void editorSaved(LiteApi::IEditor*);
    void verify();
    void indentNone();
    void indentCompact();
    void indentMinimum();
    void indentMedium();
    void indentFull();
protected:
    void format(QJson::IndentMode mode);
    LiteApi::IApplication *m_liteApp;
    LiteApi::ILiteEditor  *m_editor;
};

#endif // JSONEDIT_H
