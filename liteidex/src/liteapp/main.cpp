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
// Module: main.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: main.cpp,v 1.0 2011-5-12 visualfc Exp $

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QDir>
#include <QSettings>
#include <QSplashScreen>
#include <QDebug>
#include "mainwindow.h"
#include "liteapp.h"
#include "goproxy.h"
#include "cdrv.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

#ifdef LITEAPP_LIBRARY
int liteapp_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
#ifndef LITEAPP_LIBRARY
    #if defined(_MSC_VER) && defined(_DEBUG)
        _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
    #endif
#endif
    QApplication app(argc, argv);

    QTranslator translator;
    QTranslator qtTranslator;
    const QSettings settings(QSettings::IniFormat,QSettings::UserScope,"liteide","liteide");
    QString locale = QLocale::system().name();
    locale = settings.value(LITEAPP_LANGUAGE,locale).toString();
    QString resPath = LiteApp::getResoucePath();
    if (!locale.isEmpty()) {
        const QString &liteideTrPath = resPath+"/translations";
        if (translator.load(QLatin1String("liteide_") + locale, liteideTrPath)) {
            const QString &qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
            const QString &qtTrFile = QLatin1String("qt_") + locale;
            // Binary installer puts Qt tr files into creatorTrPath            
            app.installTranslator(&translator);
            if (qtTranslator.load(qtTrFile, qtTrPath) || qtTranslator.load(qtTrFile, liteideTrPath)) {
                app.installTranslator(&qtTranslator);
            }
            app.setProperty("liteide_locale", locale);
        }
    }
    QDir::addSearchPath("icon",resPath);
    QDir::addSearchPath("icon",resPath+"/liteapp");
    QDir::addSearchPath("icon",":/");

    QString storage = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QDir dir(storage);
    dir.mkdir("liteide");

    QStringList argList;
    QStringList fileList;
    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            QString arg = argv[i];
            if (arg.startsWith("-")) {
                argList.append(arg);
                continue;
            } else {
                fileList.append(arg);
            }
        }
    }

    IApplication *liteApp = LiteApp::NewApplication(true);

    if (fileList.size() == 1) {
        QString file = fileList.at(0);
        QFileInfo f(file);
        if (f.isFile()) {
            liteApp->fileManager()->openFolderProject(f.path());
            liteApp->fileManager()->openEditor(file);
        } else if (f.isDir()) {
            liteApp->fileManager()->openFolderProject(file);
        }
    } else {
        foreach(QString file, fileList) {
            QFileInfo f(file);
            if (f.isFile()) {
                liteApp->fileManager()->openEditor(file);
            } else if (f.isDir()) {
                liteApp->fileManager()->openFolderProject(file);
            }
        }
    }
    int ret = app.exec();
    return ret;
}

int cdrv_main(int argc, char **argv)
{
    return liteapp_main(argc,argv);
}
