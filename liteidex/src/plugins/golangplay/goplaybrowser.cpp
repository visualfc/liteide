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
// Module: goplaybrowser.cpp
// Creator: visualfc <visualfc@gmail.com>

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

QString data = "package main\n\nimport(\n\t\"fmt\"\n)\n\nfunc main(){\n\tfmt.Println(\"Hello World\")\n}";
GoplayBrowser::GoplayBrowser(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IBrowserEditor(parent),
      m_liteApp(app)
{
    m_widget = new QWidget;
    m_output = new TextOutput(m_liteApp);

    m_editor = m_liteApp->fileManager()->createEditor(data,"text/x-gosrc");

    QToolBar *toolBar = new QToolBar;

    QVBoxLayout *layout = new QVBoxLayout;
    QSplitter *spliter = new QSplitter(Qt::Vertical);

    QLabel *label = new QLabel(QString("%1").arg(tr("Go Playground")));

    QAction *run = new QAction(tr("Run"),this);
    QAction *stop = new QAction(tr("Stop"),this);
    QAction *_new = new QAction(tr("New"),this);
    QAction *load = new QAction(tr("Load..."),this);
    QAction *save = new QAction(tr("Save..."),this);
    QAction *shell = new QAction(tr("Explore Folder"),this);
    m_editLabel  = new QLabel;

    toolBar->addWidget(label);
    toolBar->addSeparator();
    toolBar->addAction(run);
    toolBar->addAction(stop);
    toolBar->addAction(_new);
    toolBar->addAction(load);
    toolBar->addAction(save);
    toolBar->addSeparator();
    toolBar->addAction(shell);
    toolBar->addSeparator();
    toolBar->addWidget(m_editLabel);

    layout->addWidget(toolBar);

    spliter->addWidget(m_editor->widget());
    spliter->addWidget(m_output);
    spliter->setStretchFactor(0,2);
    spliter->setStretchFactor(1,1);
    layout->addWidget(spliter);

    m_widget->setLayout(layout);

    m_process = new ProcessEx(this);
    m_codec = QTextCodec::codecForName("utf-8");

    connect(run,SIGNAL(triggered()),this,SLOT(run()));
    connect(stop,SIGNAL(triggered()),this,SLOT(stop()));
    connect(_new,SIGNAL(triggered()),this,SLOT(newPlay()));
    connect(load,SIGNAL(triggered()),this,SLOT(loadPlay()));
    connect(save,SIGNAL(triggered()),this,SLOT(savePlay()));
    connect(shell,SIGNAL(triggered()),this,SLOT(shell()));
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

void GoplayBrowser::onActive()
{
    static bool init = false;
    if (!init) {
        init = true;
        QDir dir(m_liteApp->storagePath());
        dir.mkpath("goplay");
        m_dataPath = dir.path()+"/goplay";
        m_playFile = QFileInfo(dir,"goplay.go").filePath();
        QFile file(m_playFile);
        if (file.open(QFile::WriteOnly|QIODevice::Text)) {
            file.write(data.toUtf8());
            file.close();
        }
        m_process->setWorkingDirectory(dir.path());

        m_editor->open(m_playFile,"text/x-gosrc");
    }
}

void GoplayBrowser::run()
{
    m_editor->saveAs(m_playFile);
    if (!m_editFile.isEmpty()) {
        m_editor->saveAs(m_editFile);
    }
    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
    QString go = FileUtil::lookPath("go",env,false);
    QStringList args;
    args << "run";
    args << "goplay.go";
    if (m_process->isRunning()) {
        m_process->kill();
    }
    m_output->clear();
    m_output->appendTag(tr("Running...")+"\n\n");
    m_process->setEnvironment(env.toStringList());

    m_process->start(go,args);
}

void GoplayBrowser::stop()
{
    if (m_process->isRunning()) {
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
    if (err || code != 0) {
        m_output->appendTag("\n"+tr("Error: %1.").arg(msg),true);
    } else {
        m_output->appendTag("\n"+tr("Success: %2.").arg(msg));
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
    QString name = QInputDialog::getItem(m_widget,tr("Load File"),tr("Select a file to load:"),items,0,false,&ok);
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
        QString name = QInputDialog::getText(m_widget,tr("Save File"),tr("New File Name:"),QLineEdit::Normal,"",&ok);
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
