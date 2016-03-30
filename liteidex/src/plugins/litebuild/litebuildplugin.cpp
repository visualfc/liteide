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
// Module: litebuildplugin.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "litebuildplugin.h"
#include "litebuild.h"
#include "litebuildoptionfactory.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QCompleter>
#include <QLabel>
#include <QPushButton>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QApplication>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


LiteBuildPlugin::LiteBuildPlugin()
{
}

void LiteBuildPlugin::load_execute(const QString& path, QComboBox *combo)
{
    QDir dir = path;
    m_liteApp->appendLog("Execute commands","Loading "+path);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList("*.api"));
    foreach (QFileInfo info, dir.entryInfoList()) {
        QFile f(info.filePath());
        if (f.open(QFile::ReadOnly)) {
            foreach (QByteArray line, f.readAll().split('\n')) {
                combo->addItem(QString(line.trimmed()));
            }
        }
    }
}

bool LiteBuildPlugin::load(LiteApi::IApplication *app)
{
    m_liteApp = app;
    m_build = new LiteBuild(app,this);
    app->optionManager()->addFactory(new LiteBuildOptionFactory(app,this));
    connect(m_build,SIGNAL(buildPathChanged(QString)),this,SLOT(buildPathChanged(QString)));

    //execute editor
    QLayout *layout=app->editorManager()->widget()->layout();
    m_executeWidget = new QWidget;
    m_executeWidget->hide();
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setMargin(1);
    m_executeWidget->setLayout(hbox);
    m_commandCombo = new QComboBox;
    m_commandCombo->setEditable(true);
    m_commandCombo->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    QCompleter *c = m_commandCombo->completer();
    if (c != 0) {
        c->setCaseSensitivity(Qt::CaseSensitive);
    }
    load_execute(m_liteApp->resourcePath()+"/litebuild/command",m_commandCombo);
    m_commandCombo->installEventFilter(this);

    m_workLabel = new ElidedLabel("");
    m_workLabel->setElideMode(Qt::ElideMiddle);


    QPushButton *close = new QPushButton();
    close->setIcon(QIcon("icon:images/closetool.png"));
    close->setIconSize(QSize(16,16));
    close->setFlat(true);
    close->setToolTip(tr("Close"));

    connect(close,SIGNAL(clicked()),m_executeWidget,SLOT(hide()));
    hbox->addWidget(new QLabel(tr("Execute:")));
    hbox->addWidget(m_commandCombo,1);
    hbox->addWidget(m_workLabel,1);
    hbox->addWidget(close);
    layout->addWidget(m_executeWidget);

    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(m_build,"Build");
    QAction *execAct = new QAction(tr("Execute File"),this);
    actionContext->regAction(execAct,"ExecuteFile","Ctrl+`");
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuLastPos,execAct);

    connect(execAct,SIGNAL(triggered()),this,SLOT(showExecute()));
    connect(m_commandCombo->lineEdit(),SIGNAL(returnPressed()),this,SLOT(execute()));
    connect(m_liteApp,SIGNAL(key_escape()),this,SLOT(closeRequest()));
    return true;
}

void LiteBuildPlugin::showExecute()
{
    m_executeWidget->show();
    m_commandCombo->lineEdit()->selectAll();
    m_commandCombo->lineEdit()->setFocus();
//    QString work = m_liteApp->applicationPath();
//    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
//    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
//    if (textEditor) {
//        work = QFileInfo(textEditor->filePath()).path();
//    }
//    m_workLabel->setText(work);
}

void LiteBuildPlugin::execute()
{
    QString text = m_commandCombo->lineEdit()->text().trimmed();
    if (text.isEmpty()) {
        return;
    }
    m_commandCombo->lineEdit()->selectAll();
    QString cmd = text;
    QString args;
    int find = text.indexOf(" ");
    if (find != -1) {
        cmd = text.left(find);
        args = text.right(text.length()-find-1);
    }
    QString work = m_build->currentBuildPath();
    if (work.isEmpty()) {
        work = m_liteApp->applicationPath();
    }
//            m_liteApp->applicationPath();
//    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
//    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
//    if (textEditor) {
//        work = QFileInfo(textEditor->filePath()).path();
//    }
    m_build->execCommand(cmd.trimmed(),args.trimmed(),work);
}

void LiteBuildPlugin::buildPathChanged(const QString &buildPath)
{
    m_workLabel->setText(buildPath);
    m_workLabel->setToolTip(buildPath);
}

void LiteBuildPlugin::closeRequest()
{
    m_executeWidget->hide();
}

bool LiteBuildPlugin::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_commandCombo) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Tab) {
                QLineEdit *edit = m_commandCombo->lineEdit();
                if (edit->completer()->widget()->isVisible()) {
                    edit->setText(edit->completer()->currentCompletion());
                }
                return true;
            }
        }
    }
    return LiteApi::IPlugin::eventFilter(obj,event);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
