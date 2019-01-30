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
// Module: finddocwidget.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FINDDOCWIDGET_H
#define FINDDOCWIDGET_H

#include "liteapi/liteapi.h"
#include "qtc_editutil/filterlineedit.h"
#include "qtc_editutil/fancylineedit.h"
#include "qt_browser/chasewidget.h"
#include "processex/processex.h"

class SearchEdit;
class QTextBrowser;

class FindDocWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FindDocWidget(LiteApi::IApplication *app, QWidget *parent = 0);
    ~FindDocWidget();    
protected slots:
    void findDoc();
    void extOutput(QByteArray,bool);    
    void extFinish(bool,int,QString);
    void abortFind();
    void stateChanged(QProcess::ProcessState);
    void openUrl(QUrl);
    void showHelp();
protected:
    QStringList docToHtml(const QString &url,const QString &file,const QStringList &comment);
    QStringList parserDoc(QString findText);
    QStringList parserPkgDoc(QString findText);
    LiteApi::IApplication *m_liteApp;
    SearchEdit            *m_findEdit;
    ChaseWidget           *m_chaseWidget;
    QAction* m_matchWordCheckAct;
    QAction* m_matchCaseCheckAct;
    QAction* m_useRegexpCheckAct;
    ProcessEx *m_process;
    LiteApi::IHtmlWidget *m_browser;
    QString    m_findFlag;
    QString    m_templateData;
    QString    m_htmlData;
};

#endif // FINDDOCWIDGET_H
