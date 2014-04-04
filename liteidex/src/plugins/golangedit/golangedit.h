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
// Module: golangedit.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGEDIT_H
#define GOLANGEDIT_H

#include <liteapi/liteapi.h>
#include <liteeditorapi/liteeditorapi.h>
#include <liteenvapi/liteenvapi.h>
#include "processex/processex.h"

class GolangEdit : public QObject
{
    Q_OBJECT
public:
    explicit GolangEdit(LiteApi::IApplication *app, QObject *parent = 0);
public slots:
    void editorCreated(LiteApi::IEditor*);
    void currentEditorChanged(LiteApi::IEditor*);
    void editorFindInfo();
    void editorJumpToDecl();
    void findDefStarted();
    void findDefOutput(QByteArray,bool);
    void findDefFinish(bool,int,QString);
    void findInfoStarted();
    void findInfoOutput(QByteArray,bool);
    void findInfoFinish(bool,int,QString);
protected:
    LiteApi::IApplication *m_liteApp;
    LiteApi::ITextEditor  *m_editor;
    QPlainTextEdit        *m_plainTextEdit;
    QAction *m_findInfoAct;
    QAction *m_jumpDeclAct;
    ProcessEx  *m_findDefProcess;
    ProcessEx  *m_findInfoProcess;
    QByteArray  m_findDefData;
    QByteArray  m_srcData;
    QTextCursor m_lastCursor;
    QByteArray  m_findInfoData;
};

#endif // GOLANGEDIT_H
