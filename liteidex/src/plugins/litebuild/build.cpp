/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: build.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: build.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "build.h"
#include "fileutil/fileutil.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QFileInfo>
#include <QDir>
#include <QMenu>
#include <QAction>
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


Build::Build(QObject *parent)
    : LiteApi::IBuild(parent)
{
}

Build::~Build()
{
    qDeleteAll(m_lookupList);
    qDeleteAll(m_actionList);
    qDeleteAll(m_configList);
    qDeleteAll(m_customList);
    qDeleteAll(m_debugList);
    qDeleteAll(m_idMenuMap);
}

QString Build::mimeType() const
{
    return m_mimeType;
}

QString Build::id() const
{
    return m_id;
}

QString Build::work() const
{
    return m_work;
}

QList<BuildAction*> Build::actionList() const
{
    return m_actionList;
}

QList<BuildLookup*> Build::lookupList() const
{
    return m_lookupList;
}

QList<BuildConfig*> Build::configList() const
{
    return m_configList;
}

QList<BuildCustom*> Build::customList() const
{
    return m_customList;
}

QList<BuildDebug*>  Build::debugList() const
{
    return m_debugList;
}

BuildAction *Build::findAction(const QString &id)
{
    foreach(BuildAction *act, m_actionList) {
        if (act->id() == id) {
            return act;
        }
    }
    return 0;
}

QList<QAction*> Build::actions()
{
    if (m_actions.isEmpty()) {
        this->make();
    }
    return m_actions;
}

void Build::make()
{
    foreach(LiteApi::BuildAction *ba, m_actionList) {
        QString id = ba->menu();
        if (!id.isEmpty()) {
            QMenu *menu = m_idMenuMap.value(id);
            if (!menu) {
                menu = new QMenu;
                m_idMenuMap.insert(id,menu);
            }
        }
    }

    foreach(LiteApi::BuildAction *ba,m_actionList) {
        QAction *act = this->makeAction(ba);
        QString idMenu = ba->menu();
        if (idMenu.isEmpty()) {
            QMenu *menu = m_idMenuMap.value(ba->id());
            if (menu) {
                QAction *menuAction = menu->menuAction();
                menuAction->setObjectName(act->objectName());
                menuAction->setText(act->text());
                menuAction->setToolTip(act->toolTip());
                menuAction->setIcon(act->icon());
                connect(menuAction,SIGNAL(triggered()),this,SLOT(slotBuildAction()));
                menu->addAction(act);
                m_actions.append(menuAction);
            } else {
                m_actions.append(act);
            }
        } else {
            QMenu *menu = m_idMenuMap.value(idMenu);
            if (menu) {
                menu->addAction(act);
            } else {
                m_actions.append(act);
            }
        }
   }
}

QAction *Build::makeAction(BuildAction *ba)
{
    QAction *act = new QAction(ba->id(),this);
    act->setObjectName(ba->id());
    if (ba->isSeparator()) {
        act->setSeparator(true);
    } else {
        if (!ba->key().isEmpty()) {
            QList<QKeySequence> list;
            foreach(QString key, ba->key().split(";")) {
                list.append(QKeySequence(key));
            }
            act->setShortcuts(list);
            act->setToolTip(QString("%1 (%2)").arg(ba->id()).arg(ba->key()));
        }
        if (!ba->img().isEmpty()) {
            QIcon icon(ba->img());
            if (!icon.isNull()) {
                act->setIcon(icon);
            }
        }
    }
    connect(act,SIGNAL(triggered()),this,SLOT(slotBuildAction()));
    return act;
}

void Build::slotBuildAction()
{
    QAction *act = (QAction*)sender();
    if (act) {
        BuildAction *ba = this->findAction(act->objectName());
        if (ba) {
            emit buildAction(this,ba);
        }
    }
}

void Build::setType(const QString &mimeType)
{
    m_mimeType = mimeType;
}

void Build::setId(const QString &id)
{
    m_id = id;
}

void Build::setWork(const QString &work)
{
    m_work = work;
}

void Build::appendAction(BuildAction *act)
{
    m_actionList.append(act);
}

void Build::appendLookup(BuildLookup *lookup)
{
    m_lookupList.append(lookup);
}

void Build::appendConfig(BuildConfig *config)
{
    m_configList.append(config);
}

void Build::appendCustom(BuildCustom *custom)
{
    m_customList.append(custom);
}

void Build::appendDebug(BuildDebug *debug)
{
    m_debugList.append(debug);
}

bool Build::loadBuild(LiteApi::IBuildManager *manager, const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        return false;
    }
    return Build::loadBuild(manager,&file,fileName);
}

static int build_ver = 1;

bool Build::loadBuild(LiteApi::IBuildManager *manager, QIODevice *dev, const QString &fileName)
{
    QXmlStreamReader reader(dev);
    QXmlStreamAttributes attrs;
    QDir dir = QFileInfo(fileName).absoluteDir();
    Build *build = 0;
    BuildAction *act = 0;
    BuildLookup *lookup = 0;
    BuildConfig *config = 0;
    BuildCustom *custom = 0;
    BuildDebug  *debug = 0;
    while (!reader.atEnd()) {
        switch (reader.readNext()) {
        case QXmlStreamReader::StartElement:
            attrs = reader.attributes();
            if (reader.name() == "mime-type" && build == 0) {
                int ver = attrs.value("ver").toString().toInt();
                if (ver >= build_ver) {
                    build = new Build;
                    build->setType(attrs.value("type").toString());
                    build->setId(attrs.value("id").toString());
                    build->setWork(attrs.value("work").toString());
                }
            } else if (reader.name() == "lookup" && lookup == 0 && build != 0) {
                lookup = new BuildLookup;
                lookup->setMimeType(attrs.value("mime-type").toString());
                lookup->setFile(attrs.value("file").toString());
                lookup->setTop(attrs.value("top").toString());
            } else if (reader.name() == "action" && act == 0 && build != 0) {
                act = new BuildAction;
                act->setId(attrs.value("id").toString());
                act->setMenu(attrs.value("menu").toString());
                act->setKey(attrs.value("key").toString());
                act->setCmd(attrs.value("cmd").toString());
                act->setArgs(attrs.value("args").toString());
                act->setSave(attrs.value("save").toString());
                act->setOutput(attrs.value("output").toString());
                act->setReadline(attrs.value("readline").toString());
                act->setCodec(attrs.value("codec").toString());
                act->setRegex(attrs.value("regex").toString());
                act->setWork(attrs.value("work").toString());
                QString img = attrs.value("img").toString();
                if (!img.isEmpty()) {
                    if (img.at(0) != ':') {
                        img = QFileInfo(dir,img).canonicalFilePath();
                    }
                    act->setImg(img);
                }
                QString task = attrs.value("task").toString();
                if (!task.isEmpty()) {
                    act->setTask(task.split(";",QString::SkipEmptyParts));
                }
                QString separator = attrs.value("separator").toString();
                if (!separator.isEmpty()) {
                    act->setSeparator(true);
                }
            } else if (reader.name() == "config" && config == 0 && build != 0) {
                config = new BuildConfig;
                config->setId(attrs.value("id").toString());
                config->setName(attrs.value("name").toString());
                config->setValue(attrs.value("value").toString());
            } else if (reader.name() == "custom" && custom == 0 && build != 0) {
                custom = new BuildCustom;
                custom->setId(attrs.value("id").toString());
                custom->setName(attrs.value("name").toString());
                custom->setValue(attrs.value("value").toString());
            } else if (reader.name() == "debug" && debug == 0 && build != 0) {
                debug = new BuildDebug;
                debug->setId(attrs.value("id").toString());
                debug->setCmd(attrs.value("cmd").toString());
                debug->setArgs(attrs.value("args").toString());
                debug->setWork(attrs.value("work").toString());
            }
            break;
        case QXmlStreamReader::EndElement:
            if (reader.name() == "mime-type") {
                if (build) {
                     manager->addBuild(build);
                }
                build = 0;
            } else if (reader.name() == "action") {
                if (build && act) {
                    build->appendAction(act);
                }
                act = 0;
            } else if (reader.name() == "lookup") {
                if (build && lookup) {
                    build->appendLookup(lookup);
                }
                lookup = 0;
            } else if (reader.name() == "config") {
                if (build && config) {
                    build->appendConfig(config);
                }
                config = 0;
            } else if (reader.name() == "custom") {
                if (build && custom) {
                    build->appendCustom(custom);
                }
                custom = 0;
            } else if (reader.name() == "debug") {
                if (build && debug) {
                    build->appendDebug(debug);
                }
                debug = 0;
            }
            break;
        default:
            break;
        }
    }
    return true;
}
