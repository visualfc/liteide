/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
// Module: goplaybrowser.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-3-8
// $Id: goplaybrowser.cpp,v 1.0 2012-3-8 visualfc Exp $

#include "goplaybrowser.h"
#include "liteenvapi/liteenvapi.h"
#include "fileutil/fileutil.h"
#include "textoutput/textoutput.h"
#include "processex/processex.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QToolBar>
#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QProcess>
#include <QTextCodec>
#include <QDesktopServices>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QUrl>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

QString data = "package main\n\nimport \"fmt\"\n\nfunc main(){\n\tfmt.Println(\"Hello World\")\n}";
GoplayBrowser::GoplayBrowser(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IBrowserEditor(parent),
      m_liteApp(app)
{
    QDir dir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
    dir.mkpath("liteide/goplay");

    dir.cd("liteide");
    m_dataPath = dir.path()+"/goplay";

    m_playFile = QFileInfo(dir,"goplay.go").filePath();
    QFile file(m_playFile);
    if (file.open(QFile::WriteOnly|QIODevice::Text)) {
        file.write(data.toUtf8());
        file.close();
    }

    m_widget = new QWidget;

    m_editor = m_liteApp->fileManager()->createEditor(data,"text/x-gosrc");
    m_editor->open(m_playFile,"text/x-gosrc");

    QToolBar *toolBar = LiteApi::findExtensionObject<QToolBar*>(m_editor,"LiteApi.QToolBar");
    if (toolBar) {
        toolBar->hide();
    }

    m_output = new TextOutput;

    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *head = new QHBoxLayout;
    QSplitter *spliter = new QSplitter(Qt::Vertical);

    QLabel *label = new QLabel("<h2>Go Playground</h2>");
    QPushButton *run = new QPushButton("Run");
    QPushButton *stop = new QPushButton("Stop");
    QPushButton *_new = new QPushButton("New");
    QPushButton *load = new QPushButton("Load");
    QPushButton *save = new QPushButton("Save");
    QPushButton *shell = new QPushButton("Explorer");
    m_editLabel  = new QLabel;

    head->addWidget(label);
    head->addWidget(run);
    head->addWidget(stop);
    head->addWidget(_new);
    head->addWidget(load);
    head->addWidget(save);
    head->addWidget(shell);
    head->addWidget(m_editLabel);
    head->addStretch();

    layout->addLayout(head);

    spliter->addWidget(m_editor->widget());
    spliter->addWidget(m_output);
    spliter->setStretchFactor(0,2);
    spliter->setStretchFactor(1,1);
    layout->addWidget(spliter);

    m_widget->setLayout(layout);

    m_process = new ProcessEx(this);
    m_process->setWorkingDirectory(dir.path());
    m_codec = QTextCodec::codecForName("utf-8");


    connect(run,SIGNAL(clicked()),this,SLOT(run()));
    connect(stop,SIGNAL(clicked()),this,SLOT(stop()));
    connect(_new,SIGNAL(clicked()),this,SLOT(newPlay()));
    connect(load,SIGNAL(clicked()),this,SLOT(loadPlay()));
    connect(save,SIGNAL(clicked()),this,SLOT(savePlay()));
    connect(shell,SIGNAL(clicked()),this,SLOT(shell()));
    connect(m_process,SIGNAL(started()),this,SLOT(runStarted()));
    connect(m_process,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(runOutput(QByteArray,bool)));
    connect(m_process,SIGNAL(extFinish(bool,int,QString)),this,SLOT(runFinish(bool,int,QString)));

    m_liteApp->extension()->addObject("LiteApi.Goplay",this);
    m_liteApp->extension()->addObject("LiteApi.Goplay.IEditor",m_editor);
}

GoplayBrowser::~GoplayBrowser()
{
    delete m_editor;
    delete m_widget;
}

QWidget *GoplayBrowser::widget()
{
    return m_widget;
}

QString GoplayBrowser::name() const
{
    return tr("Go Playground");
}

QString GoplayBrowser::mimeType() const
{
    return "browser/goplay";
}

void GoplayBrowser::run()
{
    m_editor->saveAs(m_playFile);
    if (!m_editFile.isEmpty()) {
        m_editor->saveAs(m_editFile);
    }
    QProcessEnvironment env = LiteApi::getCurrentEnvironment(m_liteApp);
    QString go = FileUtil::lookPath("go",env,false);
    QStringList args;
    args << "run";
    args << "goplay.go";
    if (m_process->isRuning()) {
        m_process->kill();
    }
    m_output->clear();
    m_output->appendTag0(QString("Run start ...\n\n"));
    m_process->setEnvironment(env.toStringList());

    m_process->start(go,args);
}

void GoplayBrowser::stop()
{
    if (m_process->isRuning()) {
        if (!m_process->waitForFinished(100)) {
            m_process->kill();
        }
    }
}

void GoplayBrowser::runOutput(const QByteArray &data,bool)
{
    m_output->append(m_codec->toUnicode(data));
}

void GoplayBrowser::runFinish(bool err,int code,const QString &msg)
{
    m_output->setReadOnly(true);
    if (err) {
        m_output->appendTag1(QString("\nError %1.\n").arg(msg));
    } else {
        m_output->appendTag0(QString("\nRun finished %1, %2.").arg(code).arg(msg));
    }
}

void GoplayBrowser::runStarted()
{
    m_output->setReadOnly(false);
}

void GoplayBrowser::newPlay()
{
    /*
    if (m_editor->isModified()) {
        int r = QMessageBox::question(m_widget,tr("Goplay"),tr("File is Modified, save or cancel?"),QMessageBox::Save,QMessageBox::No,QMessageBox::Cancel);
        if (r == QMessageBox::Cancel) {
            return;
        }
        if (r == QMessageBox::Save) {
            savePlay();
        }
    }
    */
    m_editFile.clear();
    QPlainTextEdit *edit = LiteApi::findExtensionObject<QPlainTextEdit*>(m_editor,"LiteApi.QPlainTextEdit");
    if (edit) {
        edit->setPlainText(data);
    }
    m_editLabel->setText("");
}

void GoplayBrowser::loadPlay()
{
    /*
    if (m_editor->isModified()) {
        int r = QMessageBox::question(m_widget,tr("Goplay"),tr("File is Modified, save or cancel?"),QMessageBox::Save,QMessageBox::No,QMessageBox::Cancel);
        if (r == QMessageBox::Cancel) {
            return;
        }
        if (r == QMessageBox::Save) {
            savePlay();
        }
    }
    */

    QDir dir(m_dataPath);
    QStringList items = dir.entryList(QStringList() << "*.go",QDir::Files);
    if (items.isEmpty()) {
        return;
    }
    bool ok = false;
    QString name = QInputDialog::getItem(m_widget,tr("Load File"),tr("Items"),items,0,false,&ok);
    if (!ok) {
        return;
    }
    if (name.isEmpty()) {
        return;
    }
    m_editLabel->setText(name);
    m_editFile = QFileInfo(QDir(m_dataPath),name).filePath();
    m_editor->open(m_editFile,"text/x-gosrc");
}

void GoplayBrowser::savePlay()
{
    if (m_editFile.isEmpty()) {
        bool ok = false;
        QString name = QInputDialog::getText(m_widget,tr("Save File"),tr("New FileName:"),QLineEdit::Normal,"",&ok);
        if (!ok) {
            return;
        }
        if (name.isEmpty()) {
            return;
        }
        if (name.right(3) != ".go") {
            name += ".go";
        }
        m_editLabel->setText(name);
        m_editFile = QFileInfo(QDir(m_dataPath),name).filePath();
    }
    m_editor->saveAs(m_editFile);
}

void GoplayBrowser::shell()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_dataPath));
}
