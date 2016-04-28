/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: fakevimedit.h
// Creator: jsuppe <jon.suppe@gmail.com>

#ifndef FAKEVIMEDIT_H
#define FAKEVIMEDIT_H

#include <liteapi/liteapi.h>
#include <liteeditorapi/liteeditorapi.h>
#include <liteenvapi/liteenvapi.h>
#include "processex/processex.h"
#include "textoutput/textoutput.h"

#include "fakevim/fakevim/fakevimhandler.h"

using namespace FakeVim::Internal;

class QLabel;
class FakeVimEdit : public QObject
{
    Q_OBJECT
public:
    explicit FakeVimEdit(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~FakeVimEdit(){}
    static QFont commandLabelFont();

public slots:
    void applyOption(const QString &option);
    void editorCreated(LiteApi::IEditor*);
    void currentEditorChanged(LiteApi::IEditor*);
    void toggledEnableUseFakeVim(bool b);

protected slots:
    void showMessage(QString contents, int);
    void plainTextEditDestroyed(QObject *);

    void handleExCommandRequested(bool*,ExCommand);

private:
    FakeVimEdit(const FakeVimEdit&);
    FakeVimEdit& operator=(const FakeVimEdit&);
    void _addFakeVimToEditor(LiteApi::IEditor *editor);
    void _removeFakeVimFromEditor(LiteApi::IEditor *editor);
    void _addCommandLabel();
    void _removeCommandLabel();
    void _enableFakeVim();
    void _disableFakeVim();
    LiteApi::IApplication *m_liteApp;
    LiteApi::ILiteEditor  *m_editor;
    QPlainTextEdit        *m_plainTextEdit;

    bool m_enableUseFakeVim;

    QLabel *m_commandLabel;

    QMap<QObject *,FakeVimHandler *> m_editorMap;
    QAction *m_enableUseFakeVimAct;
};

#endif // FAKEVIMEDIT_H
