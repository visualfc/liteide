/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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
// Module: fileutil.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <QObject>
#include <QMap>
#include <QIODevice>
#include <QStringList>
#include <QProcessEnvironment>

struct FileUtil
{
    static bool compareFile(const QString &fileName1, const QString &fileName2, bool canonical = true);
    static QStringList removeFiles(const QStringList &files);
    static QStringList removeWorkDir(const QString &workDir, const QStringList &filters);
    static QMap<QString,QStringList> readFileContext(QIODevice *dev);
    static QString findExecute(const QString &target);
    static QString canExec(QString file, QStringList exts = QStringList());
    static QString lookPath(const QString &fileName,const QProcessEnvironment &env, bool bLocalPriority);
    static QString lookPathInDir(const QString &fileName,const QString &dir);
};

struct GoExecute
{
public:
    GoExecute(const QString &cmdPath);
    bool isReady();
    QString cmd() const;
    bool exec(const QString &workPath, const QString &target, const QStringList &args = QStringList());
protected:
    QString m_goexec;
};

#endif // FILEUTIL_H
