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
// Module: findapiwidget.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "findapiwidget.h"
#include "liteenvapi/liteenvapi.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QActionGroup>
#include <QAction>
#include <QPlainTextEdit>
#include <QListView>
#include <QStandardItemModel>
#include <QStandardItem>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

FindApiThread::FindApiThread(QObject *parent) : QThread(parent)
{
    m_bMatchCase = true;
}

FindApiThread::~FindApiThread()
{
    stopFind();
}

void FindApiThread::setFileName(const QString &fileName)
{
    m_fileName = fileName;
}

void FindApiThread::findApi(const QString &text)
{
    if (this->isRunning()) {
        this->terminate();
    }
    m_text = text;
    this->start();
}

void FindApiThread::stopFind()
{
    this->terminate();
}

void FindApiThread::setMatchCase(bool b)
{
    m_bMatchCase = b;
}

void FindApiThread::run()
{
    QFile f(m_fileName);
    if (!f.open(QFile::ReadOnly)) {
        return;
    }
    QTextStream *stream = new QTextStream(&f);
    QRegExp reg("^pkg\\s([\\w\\-\\.\\/]+)(\\s\\(([\\w\\-]+)\\))?,\\s(\\w+)");
    QRegExp regm("\\(\\*?([\\w\\-]+)\\)\\s*(\\w+)");
    while (!stream->atEnd()) {
        QString line = stream->readLine();
        int pos = reg.indexIn(line);
        if (pos < 0) {
            continue;
        }
        // 1 pkgname
        // 2 ? (system)
        // 3 ? system
        // 4 const|func|method|var|type
        QString pkgName = reg.cap(1);
//        if (!reg.cap(3).isEmpty()) {
//            pkg = reg.cap(2)+"."+pkg;
//        }
//        if (!lastPkg || lastPkg->name != pkgName) {
//            lastPkg = m_pkgs.findPackage(pkgName);
//            if (!lastPkg) {
//                lastPkg = new Package(pkgName);
//                m_pkgs.pkgList.append(lastPkg);
//                lastType = 0;
//            }
//        }
        QString right = line.mid(reg.cap().length()).trimmed();
        QString findText;
        QStringList findUrl;
        QString flag = reg.cap(4);
        if (flag == "var") {
            ///pkg archive/tar, var ErrFieldTooLong error
            int pos = right.indexOf(" ");
            QString name = right.left(pos);
            if (pos != -1) {
                findText = pkgName+"."+name+" "+right.mid(pos+1);
                findUrl << pkgName+"#variables"<< name;
            }
            //if (pos != -1 && lastPkg->findValue(name) == 0) {
            //    lastPkg->valueList.append(new Value(VarApi,name,right.mid(pos+1)));
            //}
        } else if (flag == "const") {
            //pkg syscall (windows-amd64), const ECOMM Errno
            int pos = right.indexOf(" ");
            QString name = right.left(pos);
            if (pos != -1) {
                QString exp = right.mid(pos+1);
                findText = pkgName+"."+name+" "+exp;
                //if (exp.startsWith("ideal-") || exp == "uint16") {
                findUrl << pkgName+"#constants" << name;
//                } else {
//                    findUrl << pkgName+"."+name;
//                }
            }
//            if (pos != -1 && lastPkg->findValue(name) == 0) {
//                lastPkg->valueList.append(new Value(ConstApi,name,right.mid(pos+1)));
//            }
        } else if (flag == "func") {
            //pkg bytes, func FieldsFunc([]byte, func(rune) bool) [][]byte
            int pos = right.indexOf("(");
            QString name = right.left(pos);
            if (pos != -1) {
                findText = pkgName+"."+name+" "+right.mid(pos);
                findUrl << pkgName+"#"+name;
            }
//            if (pos != -1 && lastPkg->findValue(name) == 0) {
//                lastPkg->valueList.append(new Value(FuncApi,name,right.mid(pos)));
//            }
        } else if (flag == "method") {
            //pkg archive/tar, method (*Reader) Next() (*Header, error)
            //pkg archive/zip, method (*File) Open() (io.ReadCloser, error)
            //pkg bufio, method (ReadWriter) Available() int
            int pos = regm.indexIn(right);
            if (pos != -1) {
                QString typeName = regm.cap(1);
                QString name = regm.cap(2);
                QString exp = right.mid(regm.cap().length()).trimmed();
                findText = pkgName+"."+typeName+"."+name+" "+exp;
                findUrl << pkgName+"#"+typeName+"."+name;
//                if (lastType == 0 || lastType->name != typeName || lastType->typ == StructApi) {
//                    lastType = lastPkg->findType(typeName);
//                    if (!lastType) {
//                        lastType = new Type(StructApi,typeName,"struct");
//                        lastPkg->typeList.append(lastType);
//                    }
//                }
//                if (lastType->findValue(name) == 0) {
//                    lastType->valueList.append(new Value(TypeMethodApi,name,exp));
//                }
            }
        } else if (flag == "type") {
            //pkg go/ast, type ObjKind int
            //pkg archive/tar, type Header struct
            //pkg archive/tar, type Header struct, AccessTime time.Time
            //pkg container/heap, type Interface interface { Len, Less, Pop, Push, Swap }
            //pkg container/heap, type Interface interface, Len() int
            int pos = right.indexOf(" ");
            if (pos != -1) {
                QString typeName = right.left(pos);
                QString exp = right.mid(pos+1);
                if (exp == "struct") {
                    findText = pkgName+"."+typeName+" "+exp;
                    findUrl << pkgName+"#"+typeName;
//                    lastType = lastPkg->findType(typeName);
//                    if (!lastType) {
//                        lastType = new Type(StructApi,typeName,exp);
//                        lastPkg->typeList.append(lastType);
//                    }
                } else if (exp.startsWith("struct,")) {
                    QString last = exp.mid(7).trimmed();
                    int pos2 = last.indexOf(" ");
                    if (pos2 != -1) {
                        findText = pkgName+"."+typeName;
//                        if (lastType == 0 || lastType->name != typeName || lastType->typ == StructApi) {
//                            lastType = lastPkg->findType(typeName);
//                            if (!lastType) {
//                                lastType = new Type(StructApi,typeName,"struct");
//                                lastPkg->typeList.append(lastType);
//                            }
//                        }
                        QString name = last.left(pos2);
                        if (name == "embedded") {
                            QString emName = last.mid(pos2+1);
                            findText = pkgName+"."+typeName+"."+emName;
                            findUrl << pkgName+"#"+typeName << emName;
//                            if (!lastType->embeddedList.contains(emName)) {
//                                lastType->embeddedList.append(emName);
//                            }
                        } else {
                            findText = pkgName+"."+typeName+"."+name+" "+last.mid(pos2+1);
                            findUrl << pkgName+"#"+typeName << name;
                        }
                        /*else if (lastType->findValue(name) == 0){
                            lastType->valueList.append(new Value(TypeVarApi,name,last.mid(pos2+1)));
                        }*/
                    }
                } else if (exp.startsWith("interface {")) {
                    findText = pkgName+"."+typeName+" "+exp;
                    findUrl << pkgName+"#"+typeName;
//                    lastType = lastPkg->findType(typeName);
//                    if (!lastType) {
//                        lastType = new Type(InterfaceApi,typeName,exp);
//                        lastPkg->typeList.append(lastType);
//                    }
                } else if (exp.startsWith("interface,")) {
                    QString last = exp.mid(10).trimmed();
                    int pos2 = last.indexOf("(");
                    if (pos2 != -1) {

//                        if (lastType == 0 || lastType->name != typeName || lastType->typ == StructApi) {
//                            lastType = lastPkg->findType(typeName);
//                            if (!lastType) {
//                                lastType = new Type(InterfaceApi,typeName,"struct");
//                                lastPkg->typeList.append(lastType);
//                            }
//                        }
                        QString name = last.left(pos2);
                        //if (lastType->findValue(name) == 0) {
                             findText = pkgName+"."+typeName+"."+name+" "+last.mid(pos2);
                             findUrl << pkgName+"#"+typeName+"."+name;
                             //lastType->valueList.append(new Value(TypeMethodApi,name,last.mid(pos2)));
                        //}
                    }
                } else {
                    findText = pkgName+"."+typeName+" "+exp;
                    findUrl << pkgName+"#"+typeName;
//                    lastType = lastPkg->findType(typeName);
//                    if (!lastType) {
//                        lastType = new Type(TypeApi,typeName,exp);
//                        lastPkg->typeList.append(lastType);
//                    }
                }
            }
        }
        if (findText.indexOf(m_text,0,m_bMatchCase?Qt::CaseSensitive:Qt::CaseInsensitive) >= 0) {
            emit findApiOut(findText,line,findUrl);
        }
    }
}

FindApiWidget::FindApiWidget(LiteApi::IApplication *app, QWidget *parent) :
    QWidget(parent), m_liteApp(app)
{
    m_findEdit = new FindApiEdit;
    m_findEdit->setPlaceholderText(tr("Search"));

    m_chaseWidget = new ChaseWidget;
    m_chaseWidget->setMinimumSize(QSize(16,16));
    m_chaseWidget->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

    m_listView = new QListView;
    m_model = new QStandardItemModel(this);
    m_listView->setModel(m_model);
    m_listView->setEditTriggers(QListView::NoEditTriggers);

    QToolButton *findBtn = new QToolButton;
    findBtn->setPopupMode(QToolButton::MenuButtonPopup);
    findBtn->setText(tr("Find"));

    QHBoxLayout *findLayout = new QHBoxLayout;
    findLayout->setMargin(2);
    findLayout->addWidget(m_findEdit);
    findLayout->addWidget(findBtn);
    findLayout->addWidget(m_chaseWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(1);
    mainLayout->setSpacing(1);
    mainLayout->addLayout(findLayout);
    mainLayout->addWidget(m_listView);

    this->setLayout(mainLayout);

    m_findThread = new FindApiThread(this);
    m_findThread->setFileName(m_liteApp->storagePath()+"/golangapi.txt");
    connect(m_findThread,SIGNAL(findApiOut(QString,QString,QStringList)),this,SLOT(findApiOut(QString,QString,QStringList)));
    connect(m_findThread,SIGNAL(finished()),this,SLOT(findApiFinished()));
    //connect(m_findThread,SIGNAL(terminated()),this,SLOT(findApiTerminated()));
    //connect(findBtn,SIGNAL(clicked()),this,SLOT(findApi()));
    connect(m_findEdit,SIGNAL(returnPressed()),this,SLOT(findApi()));
    connect(m_findEdit,SIGNAL(rightButtonClicked()),m_findThread,SLOT(terminate()));
    connect(m_listView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedApi(QModelIndex)));

    m_rebuildThread = new ProcessEx(this);

    m_findAct = new QAction("Search",this);
    m_caseCheckAct = new QAction("Match case",this);
    m_caseCheckAct->setCheckable(true);
    m_rebuildAct = new QAction(tr("Rebuild database"),this);

    QMenu *menu = new QMenu(tr("Find"),findBtn);
    menu->addAction(m_caseCheckAct);
    menu->addSeparator();
    menu->addAction(m_rebuildAct);
    findBtn->setMenu(menu);
    findBtn->setDefaultAction(m_findAct);

    connect(m_findAct,SIGNAL(triggered()),this,SLOT(findApi()));
    connect(m_rebuildAct,SIGNAL(triggered()),this,SLOT(rebuildApiData()));

    m_caseCheckAct->setChecked(m_liteApp->settings()->value("goapisearch/match_case",true).toBool());
}

FindApiWidget::~FindApiWidget()
{
    m_liteApp->settings()->setValue("goapisearch/match_case",m_caseCheckAct->isChecked());
    if (m_rebuildThread->isRunning()) {
        if (!m_rebuildThread->waitForFinished(10000)) {
            m_rebuildThread->terminate();
        }
    }
}

void FindApiWidget::findApi()
{
    QString text = m_findEdit->text().trimmed();
    if (text.length() <= 2) {
        return;
    }
    m_model->clear();
    m_chaseWidget->setAnimated(true);
    m_findEdit->showStopButton(true);
    m_findThread->setMatchCase(m_caseCheckAct->isChecked());
    m_findThread->findApi(text);
}

void FindApiWidget::findApiOut(QString api, QString text, QStringList url)
{
    QStandardItem *item = new QStandardItem(api);
    item->setToolTip(text);
    item->setData(url,Qt::UserRole+1);
    m_model->appendRow(item);
}

void FindApiWidget::findApiFinished()
{
    m_chaseWidget->setAnimated(false);
    m_findEdit->showStopButton(false);
}

void FindApiWidget::doubleClickedApi(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    emit openApiUrl(index.data(Qt::UserRole+1).toStringList());
}

void FindApiWidget::rebuildApiData()
{
    if (m_rebuildThread->isRunning()) {
        return;
    }
    m_rebuildThread->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    QString cmd = LiteApi::getGotools(m_liteApp);
    m_rebuildThread->setWorkingDirectory(m_liteApp->storagePath());
    QStringList args;
    args << "goapi" << "-o" << "golangapi.txt" << "all";
    m_rebuildThread->start(cmd,args);
}

QString FindApiWidget::apiDataFile() const
{
    return m_liteApp->storagePath()+"/golangapi.txt";
}
