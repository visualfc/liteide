/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2018 LiteIDE Team. All rights reserved.
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
// Module: findfilesthread.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FINDFILESTHREAD_H
#define FINDFILESTHREAD_H

#include <QThread>
#include <QString>
#include <QStringList>
#include <QSet>

class FindFilesThread : public QThread
{
    Q_OBJECT
public:
    FindFilesThread(QObject *parent);
    void setFolderList(const QStringList &folderLis, const QSet<QString> &extSet, const QSet<QString> &exceptFiles, int maxCount);
    void stop(int time = 10);
protected:
    virtual void run();
    void findFolder(QString folder);
    void sendResult(const QStringList &fileList);
signals:
    void findResult(const QStringList &fileList);
protected:
    QStringList m_folderList;
    QSet<QString> m_exceptFiles;
    QSet<QString> m_extSet;
    QSet<QString> m_processFolderSet;
    int           m_maxFileCount;
    int           m_filesCount;
    int           m_maxBlockSendCount;
    bool          m_cancel;
};

#endif // FINDFILESTHREAD_H
