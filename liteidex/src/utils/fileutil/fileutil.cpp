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
// Module: fileutil.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "fileutil.h"
#include <QFileInfo>
#include <QDir>
#include <QProcess>
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


bool FileUtil::compareFile(const QString &fileName1, const QString &fileName2, bool canonical)
{
    if (fileName1.isEmpty() || fileName2.isEmpty()) {
        return false;
    }
    if (canonical) {
        return QFileInfo(fileName1).canonicalFilePath() == QFileInfo(fileName2).canonicalFilePath();
    }
    return QFileInfo(fileName1).filePath() == QFileInfo(fileName2).filePath();
}

QStringList FileUtil::removeFiles(const QStringList &files)
{
    QStringList result;
    foreach (QString file, files) {
        if (QFile::exists(file) && QFile::remove(file)) {
            result << file;
        }
    }
    return result;
}

QStringList FileUtil::removeWorkDir(const QString &workDir, const QStringList &filters)
{
    QStringList result;
    QDir dir(workDir);
    if (!dir.exists())
        return result;

    QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    foreach (QFileInfo d, dirs) {
        removeWorkDir(d.filePath(),filters);
    }

    QFileInfoList files = dir.entryInfoList(filters,QDir::Files);
    foreach (QFileInfo f, files) {
        bool b = QFile::remove(f.filePath());
        if (b) {
            result << f.fileName();
        }
    }
    return result;
}

QMap<QString,QStringList> FileUtil::readFileContext(QIODevice *dev)
{
    QMap<QString,QStringList> contextMap;
    QStringList list;
    QString line;
    bool bnext = false;
    while (!dev->atEnd()) {
        QByteArray ar = dev->readLine().trimmed();
        if (!ar.isEmpty() && ar.right(1) == "\\") {
            bnext = true;
            ar[ar.length()-1] = ' ';
        } else {
            bnext = false;
        }
        line.push_back(ar);
        if (!bnext && !line.isEmpty()) {
            list.push_back(line);
            line.clear();
        }
    }

    if (!line.isEmpty()) {
        list.push_back(line);
    }

    foreach (QString line, list) {
        if (line.size() >= 1 && line.at(0) == '#')
            continue;
        QStringList v = line.split(QRegExp("\\+="),QString::SkipEmptyParts);
        if (v.count() == 1) {
            v = line.split(QRegExp("="),QString::SkipEmptyParts);
            if (v.count() == 2) {
                QStringList v2 = v.at(1).split(" ",QString::SkipEmptyParts);
                if (!v2.isEmpty()) {
                    contextMap[v.at(0).trimmed()] = v2;
                }
            }
        } else if (v.count() == 2) {
            QStringList v2 = v.at(1).split(" ",QString::SkipEmptyParts);
            if (!v2.isEmpty())
                contextMap[v.at(0).trimmed()].append(v2);
        }
    }
    return contextMap;
}

#ifdef Q_OS_WIN
QString FileUtil::canExec(QString fileName, QStringList exts)
{
    QFileInfo info(fileName);
    QString suffix = info.suffix();
    if (!suffix.isEmpty()) {
        suffix = "."+suffix;
        foreach(QString ext, exts) {
            if (suffix == ext && info.exists()) {
                return info.canonicalFilePath();
            }
        }
    }
    foreach(QString ext, exts) {
        QFileInfo info(fileName+ext);
        if (info.exists()) {
            return info.filePath();
        }
    }
    return QString();
}

QString FileUtil::lookPath(const QString &file, const QProcessEnvironment &env, bool bLocalPriority)
{
    QString fileName = file;
    QStringList exts;
    QString extenv = env.value("PATHEXT");
    if (!extenv.isEmpty()) {
        foreach(QString ext, extenv.split(';',QString::SkipEmptyParts)) {
            if (ext.isEmpty()) {
                continue;
            }
            if (ext[0] != '.') {
                ext= '.'+ext;
            }
            exts.append(ext.toLower());
        }
    }

    if (fileName.contains('\\') || fileName.contains('/')) {
        QString exec = canExec(fileName,exts);
        if (!exec.isEmpty()) {
            return exec;
        }
    }
    if (bLocalPriority) {
        QString exec = canExec(".\\"+fileName,exts);
        if (!exec.isEmpty()) {
            return exec;
        }
    }
    QString pathenv = env.value("PATH");
    if (pathenv.isEmpty()) {
        QString exec = canExec(".\\"+fileName,exts);
        if (!exec.isEmpty()) {
            return exec;
        }
    } else {
        foreach(QString dir,pathenv.split(';',QString::SkipEmptyParts)) {
            QFileInfo info(QDir(dir),fileName);
            QString exec = canExec(info.filePath(),exts);
            if (!exec.isEmpty()) {
                return exec;
            }
        }
    }
    return QString();
}

QString FileUtil::lookPathInDir(const QString &file, const QString &dir)
{
    QString fileName = file;
    QStringList exts;
    QString extenv =  QProcessEnvironment::systemEnvironment().value("PATHEXT");
    if (!extenv.isEmpty()) {
        foreach(QString ext, extenv.split(';',QString::SkipEmptyParts)) {
            if (ext.isEmpty()) {
                continue;
            }
            if (ext[0] != '.') {
                ext= '.'+ext;
            }
            exts.append(ext.toLower());
        }
    }

    if (fileName.contains('\\') || fileName.contains('/')) {
        QString exec = canExec(fileName,exts);
        if (!exec.isEmpty()) {
            return exec;
        }
    }
    QFileInfo info(QDir(dir),fileName);
    QString exec = canExec(info.filePath(),exts);
    if (!exec.isEmpty()) {
        return exec;
    }
    return QString();
}

#else
QString FileUtil::canExec(QString fileName, QStringList /*exts*/)
{
    QFileInfo info(fileName);
    if (info.exists() && info.isExecutable()) {
        return info.canonicalFilePath();
    }
    return QString();
}

QString FileUtil::lookPath(const QString &file, const QProcessEnvironment &env, bool bLocalPriority)
{
    QString fileName = file;
    if (fileName.contains('/')) {
        QString exec = canExec(fileName);
        if (!exec.isEmpty()) {
            return exec;
        }
    }
    if (bLocalPriority) {
        QString exec = canExec("./"+fileName);
        if (!exec.isEmpty()) {
            return exec;
        }
    }
    QString pathenv = env.value("PATH");
    foreach(QString dir,pathenv.split(':',QString::KeepEmptyParts)) {
        if (dir == "") {
            dir = ".";
        }
        QString exec = canExec(dir+"/"+file);
        if (!exec.isEmpty()) {
            return exec;
        }
    }
    return QString();
}

QString FileUtil::lookPathInDir(const QString &file, const QString &dir)
{
    QString fileName = file;
    if (fileName.contains('/')) {
        QString exec = canExec(fileName);
        if (!exec.isEmpty()) {
            return exec;
        }
    }
    QString exec = canExec(dir+"/"+file);
    if (!exec.isEmpty()) {
        return exec;
    }
    return QString();
}
#endif

QString FileUtil::findExecute(const QString &target)
{
    QStringList targetList;
#ifdef Q_OS_WIN
    targetList << target+".exe";
#endif
    targetList << target;
    foreach (QString fileName, targetList) {
        if (QFile::exists(fileName)) {
            QFileInfo info(fileName);
            if (info.isExecutable()) {
                return info.canonicalFilePath();
            }
        }
    }
    return QString();
}

GoExecute::GoExecute(const QString &cmdPath)
{
#ifdef Q_OS_WIN
    QString goexec = "goexec.exe";
#else
    QString goexec = "goexec";
#endif
    m_goexec = QFileInfo(QDir(cmdPath),goexec).absoluteFilePath();
}

bool GoExecute::isReady()
{
    return QFile::exists(m_goexec);
}

QString GoExecute::cmd() const
{
    return m_goexec;
}

bool GoExecute::exec(const QString &workPath, const QString &target, const QStringList &args)
{
#ifdef Q_OS_WIN
    QStringList iargs;
    if (!workPath.isEmpty()) {
        iargs << "-w" << workPath;
    }
    iargs << target << args;
    return QProcess::startDetached(m_goexec,iargs);
#else
    QStringList iargs;
    iargs << "-e" << m_goexec;
    if (!workPath.isEmpty()) {
        iargs << "-w" << workPath;
    }
    iargs << target << args;
    return QProcess::startDetached("/usr/bin/xterm",iargs);
#endif
}
