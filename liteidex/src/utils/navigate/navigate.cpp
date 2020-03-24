/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
// Module: navigate.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "navigate.h"
#include "quickopenapi/quickopenapi.h"
#include <QDir>
#include <QAction>
#include <QLabel>
#include <QTreeView>

static QString escaped(const QString &text)
{
#if QT_VERSION >= 0x050000
    return text.toHtmlEscaped();
#else
    return Qt::escape(text);
#endif
}

NavigateBar::NavigateBar(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent), m_liteApp(app), m_toolBar(0)
{
}

NavigateBar::~NavigateBar()
{

}

QToolBar *NavigateBar::createToolBar(const QString &title, QWidget *parent)
{
    if (m_toolBar) {
        return  m_toolBar;
    }
    m_toolBar = new QToolBar(title,parent);
    m_toolBar->setIconSize(LiteApi::getToolBarIconSize(m_liteApp));
    return  m_toolBar;
}

void NavigateBar::LoadPath(const QString &path)
{
    m_filePath = path;
    QFileInfo info(path);
    if (!info.filePath().startsWith("//")) {
        QStringList paths = QDir::fromNativeSeparators(info.filePath()).split("/");
        if (paths.size() >= 2) {
            QString head = "<style> a{text-decoration: none; color:darkgray;} </style>";
            //m_editNavHeadAct = m_editNavBar->addSeparator();
            QString last;
#ifdef Q_OS_WIN
            last = paths[0];
#endif
            for (int i = 1; i < paths.size(); i++) {
                QString name = paths[i];
#ifdef Q_OS_WIN
                if (i == 1) {
                    name = paths[0]+"\\"+paths[1];
                }
#endif
                QString path = last+"/"+paths[i];
                last = path;
                if (i != paths.size()-1) {
                    name += ">";
                }
                QString text = QString("<a href=\"%1\">%2</a>").arg(escaped(path)).arg(escaped(name));
                QLabel *lbl = new QLabel;
                lbl->setText(head+text);
                m_toolBar->addWidget(lbl);
                connect(lbl,SIGNAL(linkActivated(QString)),this,SLOT(pathLinkActivated(QString)));
            }
            m_navHeadAct = m_toolBar->actions().first();
        }
    }
    QAction *emptyAct = new QAction(this);
    m_toolBar->addAction(emptyAct);
}

QToolBar *NavigateBar::createNavToolBar(QWidget *parent)
{
    QFileInfo info(m_filePath);

    QStringList paths = QDir::fromNativeSeparators(info.filePath()).split("/");
    if (paths.size() < 2) {
        return 0;
    }
    QString head = "<style> a{text-decoration: none; color:darkgray;} </style>";

    QToolBar *toolBar = new QToolBar(parent);
    toolBar->setIconSize(LiteApi::getToolBarIconSize(m_liteApp));
    QString last;
#ifdef Q_OS_WIN
    last = paths[0];
#endif
    for (int i = 1; i < paths.size(); i++) {
        QString name = paths[i];
#ifdef Q_OS_WIN
        if (i == 1) {
            name = paths[0]+"\\"+paths[1];
        }
#endif
        QString path = last+"/"+paths[i];
        last = path;
        if (i != paths.size()-1) {
            name += ">";
        }
        QString text = QString("<a href=\"%1\">%2</a>").arg(escaped(path)).arg(escaped(name));
        QLabel *lbl = new QLabel;
        lbl->setText(head+text);
        toolBar->addWidget(lbl);
        connect(lbl,SIGNAL(linkActivated(QString)),this,SLOT(quickPathLinkActivated(QString)));
    }
    QAction *empytAct = new QAction(toolBar);
    toolBar->addAction(empytAct);
    return  toolBar;
}


void NavigateBar::pathLinkActivated(const QString &path)
{
    QString dirpath = QFileInfo(path).absolutePath();
    LiteApi::IQuickOpenManager *mgr = LiteApi::getQuickOpenManager(m_liteApp);
    if (mgr) {
        LiteApi::IQuickOpenFileSystem *fileSystem = LiteApi::getQuickOpenFileSystem(mgr);
        if (fileSystem) {
            fileSystem->setRootPath(dirpath);
            fileSystem->setPlaceholderText(QString(tr("Browser Files in %1").arg(QDir::toNativeSeparators(dirpath))));
            mgr->setCurrentFilter(fileSystem);
            mgr->modelView()->setRootIndex(fileSystem->rootIndex());
            QModelIndex index = fileSystem->indexForPath(path);
            mgr->modelView()->setCurrentIndex(index);
            mgr->setTempToolBar(this->createNavToolBar(mgr->widget()));
            QRect rc = m_toolBar->actionGeometry(m_navHeadAct);
            QPoint pt = m_toolBar->mapToGlobal(rc.topLeft());
            mgr->showPopup(&pt);
            mgr->modelView()->scrollTo(index);
            return;
        }
    }
}

void NavigateBar::quickPathLinkActivated(const QString &path)
{
    QString dirpath = QFileInfo(path).absolutePath();
    LiteApi::IQuickOpenManager *mgr = LiteApi::getQuickOpenManager(m_liteApp);
    if (mgr) {
        LiteApi::IQuickOpenFileSystem *fileSystem = LiteApi::getQuickOpenFileSystem(mgr);
        if (fileSystem) {
            fileSystem->setRootPath(dirpath);
            fileSystem->setPlaceholderText(QString(tr("Browser Files in %1").arg(QDir::toNativeSeparators(dirpath))));
            mgr->setCurrentFilter(fileSystem);
            mgr->modelView()->setRootIndex(fileSystem->rootIndex());
            QModelIndex index = fileSystem->indexForPath(path);
            mgr->modelView()->setCurrentIndex(index);
        }
    }
}

