/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
#include <QProcessEnvironment>
#include <QDesktopServices>
#include <QDebug>

#ifdef WIN32
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#endif

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
#if defined(WIN32)
        return (QFileInfo(fileName1).canonicalFilePath().compare(QFileInfo(fileName2).canonicalFilePath(), Qt::CaseInsensitive) == 0);
#else
        return QFileInfo(fileName1).canonicalFilePath() == QFileInfo(fileName2).canonicalFilePath();
#endif
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
    exts << ".exe" << ".bat" << ".cmd";
    exts.removeDuplicates();

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
    exts << ".exe" << ".bat" << ".cmd";
    exts.removeDuplicates();
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
    if (info.exists() && info.isFile()  && info.isExecutable()) {
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
            if (info.isFile() && info.isExecutable()) {
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

QString FileUtil::lookupGoBin(const QString &bin, LiteApi::IApplication *app, const QProcessEnvironment env, bool bLiteAppPriority)
{
    if (bLiteAppPriority) {
        QString find = FileUtil::findExecute(app->applicationPath()+"/"+bin);
        if (!find.isEmpty()) {
            return find;
        }
    }
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif

    QString goos = env.value("GOOS");
//    if (goos.isEmpty()) {
//        goos = LiteApi::getDefaultGOOS();
//    }
    QString goarch = env.value("GOARCH");
//    QString goroot = env.value("GOROOT");
//    if (goroot.isEmpty()) {
//        goroot = LiteApi::getDefaultGOROOT();
//    }
    QStringList pathList;
    foreach (QString path, env.value("GOPATH").split(sep,QString::SkipEmptyParts)) {
        pathList.append(QDir::toNativeSeparators(path));
    }
//    foreach (QString path, app->settings()->value("liteide/gopath").toStringList()) {
//        pathList.append(QDir::toNativeSeparators(path));
//    }
//    pathList.removeDuplicates();
//    env.insert("GOPATH",pathList.join(sep));

//    if (!goroot.isEmpty()) {
//        pathList.prepend(goroot);
//    }

    QStringList binList;
    QString gobin = env.value("GOBIN");
    if (!gobin.isEmpty()) {
        binList.append(gobin);
    }
    foreach (QString path, pathList) {
        binList.append(QFileInfo(path,"bin").filePath());
        binList.append(QFileInfo(path,"bin/"+goos+"_"+goarch).filePath());
    }
    foreach(QString path, binList) {
        QString find = FileUtil::findExecute(path+"/"+bin);
        if (!find.isEmpty()) {
            return find;
        }
    }
    return FileUtil::lookupLiteBin(bin,app,env);
}

QString FileUtil::lookupLiteBin(const QString &bin, LiteApi::IApplication *app, const QProcessEnvironment env)
{
    QString find = FileUtil::findExecute(app->applicationPath()+"/"+bin);
    if (find.isEmpty()) {
        find = FileUtil::lookPath(bin,env,true);
    }
    return find;
}

bool FileUtil::CopyDirectory(const QString &src, const QString &dest)
{
    QDir dir(src);
    foreach(QFileInfo info, dir.entryInfoList(QDir::Files)) {
        if (info.isFile() && !info.isSymLink()) {
            QFile in(info.filePath());
            if (!in.open(QFile::ReadOnly)) {
                return false;
            }
            QFile out(dest+"/"+info.fileName());
            if (!out.open(QFile::WriteOnly)) {
                return false;
            }
            out.write(in.readAll());
        }
    }
    return true;
}

#ifdef Q_OS_WIN
bool openBrowser(LPCTSTR lpszFileName)
{
    HINSTANCE hl= LoadLibrary(TEXT("liteshell.dll"));
    typedef BOOL (*BrowseToFileProc)(const wchar_t* filename);
    if(!hl)
        return false;
    bool b = false;
    BrowseToFileProc proc = (BrowseToFileProc)GetProcAddress(hl,"BrowseToFile");
    if (proc) {
        b = proc(lpszFileName);
    }
    FreeLibrary(hl);
    return b;
}
bool shellOpenFolder(LPCTSTR filename)
{
    HINSTANCE hl= LoadLibrary(TEXT("Shell32.dll"));
    if (!hl) {
        return false;
    }
    typedef LPITEMIDLIST(*ILCreateFromPathProc)(LPCTSTR);
    typedef void (*ILFreeProc)(LPITEMIDLIST);
    typedef void (*SHOpenFolderAndSelectItemsProc)(LPCITEMIDLIST pidlFolder, UINT cidl, LPCITEMIDLIST *apidl, DWORD dwFlags);
    ILCreateFromPathProc fnILCreateFromPath =0;
    ILFreeProc fnILFree = 0;
    SHOpenFolderAndSelectItemsProc fnSHOpenFolderAndSelectItems = 0;
    fnILCreateFromPath = (ILCreateFromPathProc)GetProcAddress(hl,"ILCreateFromPath");
    fnILFree = (ILFreeProc)GetProcAddress(hl,"ILFree");
    fnSHOpenFolderAndSelectItems = (SHOpenFolderAndSelectItemsProc)GetProcAddress(hl,"SHOpenFolderAndSelectItems");
    bool b = false;
    if(fnILCreateFromPath && fnILFree && fnSHOpenFolderAndSelectItems){
        ITEMIDLIST *pidl=0;
        pidl = fnILCreateFromPath(filename);
        if (pidl) {
            fnSHOpenFolderAndSelectItems(pidl,0,0,0);
            fnILFree(pidl);
            b = true;
        }
    }
    FreeLibrary(hl);
    return b;
}
#endif

void FileUtil::openInExplorer(const QString &path)
{
#ifdef Q_OS_WIN
    if (openBrowser((LPCTSTR)QDir::toNativeSeparators(path).utf16())) {
        return;
    }
    if (shellOpenFolder((LPCTSTR)QDir::toNativeSeparators(path).utf16())) {
        return;
    }
    const QString explorer = FileUtil::lookPath("explorer.exe",QProcessEnvironment::systemEnvironment(),false);
    if (!explorer.isEmpty()) {
        QStringList param;
        if (!QFileInfo(path).isDir())
            param += QLatin1String("/select,");
        param += QDir::toNativeSeparators(path);
        QProcess::startDetached(explorer, param);
        return;
    }
#endif
#ifdef Q_OS_MAC
        QStringList scriptArgs;
        scriptArgs << QLatin1String("-e")
                   << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
                                         .arg(path);
        QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
        scriptArgs.clear();
        scriptArgs << QLatin1String("-e")
                   << QLatin1String("tell application \"Finder\" to activate");
        QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
        return;
#endif
    QFileInfo info(path);
    if (info.isDir()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(info.filePath()));
    } else {
        QDesktopServices::openUrl(QUrl::fromLocalFile(info.path()));
    }
}

static bool startDetachedEx(const QString &cmd, const QStringList &args, const QString &workDir)
{
#ifdef Q_OS_WIN
    return (intptr_t)ShellExecuteW(NULL, L"open", (LPCWSTR)cmd.toStdWString().data(), (LPCWSTR)args.join(" ").toStdWString().data(), (LPCWSTR)workDir.toStdWString().data(), SW_SHOW) > 32;
#else
    return QProcess::startDetached(cmd, args, workDir);
#endif
}

void FileUtil::openInShell(const QProcessEnvironment &env, const QString &file)
{
    QFileInfo info(file);
    QDir dir;
    if (info.isDir()) {
        dir.setCurrent(file);
    } else {
        dir = info.dir();
    }
    //check shell
    QString shell = env.value("LITEIDE_SHELL");
#ifdef Q_OS_LINUX
    if (shell.isEmpty()) {
        shell="gnome-terminal;lxterminal;konsole;xfce4-terminal;xterm";
    }
#endif
    if (!shell.isEmpty()) {
        foreach (QString info, shell.split(";",QString::SkipEmptyParts)) {
            QStringList ar = info.split(" ",QString::SkipEmptyParts);
            if (ar.size() >= 1) {
                QString cmd = FileUtil::lookPath(ar[0],env,false);
                if (!cmd.isEmpty()) {
                    QString path = dir.path();
                    ar.pop_front();
#ifdef Q_OS_MAC
                    ar.push_back(path);
#endif
#ifdef Q_OS_WIN
    if (path.length() == 2 && path.right(1) == ":") {
        path += "/";
    }
#endif
                    QProcess::startDetached(cmd,ar,path);
                    return;
                }
            }
        }
        return;
    }
    //check LITEIDE_TERM
    QString cmd = env.value("LITEIDE_TERM");
    QStringList args = env.value("LITEIDE_TERMARGS").split(" ",QString::SkipEmptyParts);
#ifdef Q_OS_MAC
    if (cmd.isEmpty()) {
        cmd = "/usr/bin/open";
    }
    if (args.isEmpty()) {
        args << "-a" << "Terminal";
    }
#endif
#ifdef Q_OS_WIN
    if (cmd.isEmpty()) {
        cmd = env.value("COMSPEC");
    }
#endif

    QString path = dir.path();
#ifdef Q_OS_MAC
    args.append(path);
#endif
#ifdef Q_OS_WIN
    if (path.length() == 2 && path.right(1) == ":") {
        path += "/";
    }
#endif
//#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
//    QProcess p;
//    p.setWorkingDirectory(path);
//    p.setProcessEnvironment(env);
//    p.setProgram(cmd);
//    p.setArguments(args);
//    p.startDetached();
//#else
    startDetachedEx(cmd,args,path);
}

