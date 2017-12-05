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
// Module: quickopenfiles.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef QUICKOPENFILES_H
#define QUICKOPENFILES_H

#include "quickopenapi/quickopenapi.h"
#include <QThread>
#include <QSet>

class QStandardItemModel;
class QSortFilterProxyModel;

class FindFilesThread : public QThread
{
    Q_OBJECT
public:
    FindFilesThread(QObject *parent);
    void setFolderList(const QStringList &folderLis, const QSet<QString> &extSet, const QSet<QString> &exceptFiles, int maxCount);
    void stop(int time = 100);
protected:
    virtual void run();
    void findFolder(QString folder);
signals:
    void findResult(const QString &fileName, const QString &filePath);
protected:
    QStringList m_folderList;
    QSet<QString> m_exceptFiles;
    QSet<QString> m_extSet;
    QSet<QString> m_processFolderSet;
    int           m_maxCount;
    int           m_filesCount;
    bool          m_cancel;
};


class QuickOpenFiles : public LiteApi::IQuickOpen
{
    Q_OBJECT
public:
    QuickOpenFiles(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~QuickOpenFiles();
    virtual QString id() const;
    virtual QString info() const;
    virtual void activate();
    virtual QAbstractItemModel *model() const;
    virtual void updateModel();
    virtual QModelIndex filterChanged(const QString &text);
    virtual void indexChanged(const QModelIndex &index);
    virtual bool selected(const QString &text, const QModelIndex &index);
    virtual void cancel();
    void startFindThread();
public slots:
    void findResult(const QString &fileName,const QString &filePath);
protected:
    LiteApi::IApplication *m_liteApp;
    FindFilesThread *m_thread;
    QStandardItemModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
    QStringList        m_editors;
    Qt::CaseSensitivity m_matchCase;
    int                 m_maxCount;
};

#endif // QUICKOPENFILES_H
