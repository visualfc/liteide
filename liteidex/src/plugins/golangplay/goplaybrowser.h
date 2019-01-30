/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
// Module: goplaybrowser.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOPLAYBROWSER_H
#define GOPLAYBROWSER_H

#include "liteapi/liteapi.h"

class QPlainTextEdit;
class ProcessEx;
class TextOutput;
class QTextCodec;
class QLabel;
class GoplayBrowser : public LiteApi::IBrowserEditor
{
    Q_OBJECT
public:
    GoplayBrowser(LiteApi::IApplication *app, QObject *parent);
    virtual ~GoplayBrowser();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QString mimeType() const;
    virtual void onActive();
public slots:
    void run();
    void stop();
    void newPlay();
    void loadPlay();
    void savePlay();
    void shell();
    void runOutput(const QByteArray &data,bool);
    void runFinish(bool,int,const QString &msg);
    void runStarted();
protected:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_widget;
    LiteApi::IEditor *m_editor;
    TextOutput       *m_output;
    ProcessEx         *m_process;
    QTextCodec       *m_codec;
    QLabel           *m_editLabel;
    QString           m_dataPath;
    QString           m_playFile;
    QString           m_editFile;
};

#endif // GOPLAYBROWSER_H
