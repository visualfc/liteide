/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE Team. All rights reserved.
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
// Module: recentmanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef RECENTMANAGER_H
#define RECENTMANAGER_H

#include "liteapi/liteapi.h"

using namespace LiteApi;

class RecentManager : public IRecentManager
{
    Q_OBJECT
public:
    RecentManager(QObject *parent = 0);
    virtual bool initWithApp(IApplication *app);

    virtual void registerRecent(IRecent *recent);
    virtual QList<IRecent*> recentList() const;
    virtual IRecent *findRecent(const QString &type) const;
    virtual QStringList recentTypeList() const;

    virtual void addRecent(const QString &name, const QString &type);
    virtual void removeRecent(const QString &name, const QString &type);
    virtual QStringList recentNameList(const QString &type);
    virtual void clearRecentNameList(const QString &type);
    virtual void openRecent(const QString &name, const QString &type);
    virtual void updateRecentMenu(const QString &type);
protected:
    void updateAppListRecentMenu(const QString &type);
    void emitRecentNameListChanged(const QString &type);
public slots:
    void applyOption(const QString &opt);
    void openRecentAction();
    void clearRecentMenu();
    void clearAllRecentMenu();
protected:
    QStringList     m_recentTypeList;
    QList<IRecent*> m_recentList;
    int            m_maxRecentFiles;
    QMenu        *m_recentMenu;
    QAction      *m_recentSeparator;
    QMap<QString,QMenu*> m_mapRecentTypeMenu;
};

class BookmarkRecent : public ISettingRecent
{
    Q_OBJECT
public:
    BookmarkRecent(LiteApi::IApplication *app, QObject *parent) : ISettingRecent(app->settings(),parent), m_liteApp(app)
    {
    }

    virtual QString type() const
    {
        return "bookmark";
    }

    virtual QString displyType() const
    {
        return tr("Bookmarks");
    }

    virtual void addRecent(const QString &name,int maxRecent)
    {
        ISettingRecent::addRecent(QDir::toNativeSeparators(name), maxRecent);
    }

    virtual void openRecent(const QString &name)
    {
        int pos = name.lastIndexOf(":");
        if (pos == -1) {
            return;
        }
        bool ok = false;
        int line = name.mid(pos+1).toInt(&ok);
        if (!ok) {
            return;
        }
        QString filePath = name.left(pos);
        LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(filePath,true,false);
        if (!editor) {
            return;
        }
        LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
        if (!textEditor) {
            return;
        }
        textEditor->gotoLine(line,0,true);
    }
protected:
    virtual QString recentKey() const
    {
        return QString("Bookmark/%1").arg(type());
    }
    LiteApi::IApplication *m_liteApp;
};


class FileRecent : public ISettingRecent
{
    Q_OBJECT
public:
    FileRecent(LiteApi::IApplication *app, QObject *parent) : ISettingRecent(app->settings(),parent), m_liteApp(app)
    {
    }

    virtual QString type() const
    {
        return "file";
    }

    virtual QString displyType() const
    {
        return tr("Files");
    }

    virtual void addRecent(const QString &name,int maxRecent)
    {
        ISettingRecent::addRecent(QDir::toNativeSeparators(name), maxRecent);
    }

    virtual void openRecent(const QString &name)
    {
        m_liteApp->fileManager()->openFile(name);
    }
protected:
    LiteApi::IApplication *m_liteApp;
};

class FolderRecent : public ISettingRecent
{
    Q_OBJECT
public:
    FolderRecent(LiteApi::IApplication *app, QObject *parent) : ISettingRecent(app->settings(),parent), m_liteApp(app)
    {
    }

    virtual QString type() const
    {
        return "folder";
    }

    virtual QString displyType() const
    {
        return tr("Folders");
    }

    virtual void addRecent(const QString &name,int maxRecent)
    {
        ISettingRecent::addRecent(QDir::toNativeSeparators(name), maxRecent);
    }

    virtual void openRecent(const QString &name)
    {
        m_liteApp->fileManager()->addFolderList(name);
    }
protected:
    LiteApi::IApplication *m_liteApp;
};

class SessionRecent : public ISettingRecent
{
    Q_OBJECT
public:
    SessionRecent(LiteApi::IApplication *app, QObject *parent) : ISettingRecent(app->settings(),parent), m_liteApp(app)
    {
    }

    virtual QString type() const
    {
        return "session";
    }

    virtual QString displyType() const
    {
        return tr("Sessions");
    }

    virtual void addRecent(const QString &name, int maxRecent)
    {
        QString key = recentKey();
        QStringList files = m_settings->value(key).toStringList();
        files.removeAll(name);
        files.prepend(name);
        while (files.size() > maxRecent) {
            files.removeLast();
        }
        m_settings->setValue(key, files);
    }

    virtual void removeRecent(const QString &name)
    {
        if (name == "default") {
            return;
        }
        ISettingRecent::removeRecent(name);
    }

    virtual QStringList recentNameList()
    {
        QString key = recentKey();
        QStringList values = m_settings->value(key).toStringList();
        values.prepend("default");
        values.removeDuplicates();
        return values;
    }

    virtual void clearRecentNameList()
    {
        QString key = recentKey();
        QStringList values = m_settings->value(key).toStringList();
        values.clear();
        values.append("default");
        m_settings->setValue(key, values);
    }

    virtual void openRecent(const QString &name)
    {
        if (name != m_liteApp->currentSession()) {
            m_liteApp->newInstance(name);
        } else {
            m_liteApp->loadSession(name);
        }
    }
protected:
    LiteApi::IApplication *m_liteApp;
};


#endif // RECENTMANAGER_H
