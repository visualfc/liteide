/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QDir>
#include <QSettings>
#include <QSplashScreen>
#include <QTextCodec>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#endif
#include <QDebug>
#include "mainwindow.h"
#include "liteapp.h"
#include "goproxy.h"
#include "cdrv.h"

#include "memory.h"

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

#if QT_VERSION >= 0x050100
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    //QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
    QTranslator translator;
    QTranslator qtTranslator;

    QString resPath = LiteApp::getResoucePath();
    QString locale = QLocale::system().name();
    QString qss;
    QSettings global(resPath+"/liteapp/config/global.ini",QSettings::IniFormat);
    bool storeLocal = global.value(LITEIDE_STORELOCAL,false).toBool();
    if (storeLocal) {
        const QSettings settings(resPath+"/liteapp/config/liteide.ini", QSettings::IniFormat);
        locale = settings.value(LITEAPP_LANGUAGE,locale).toString();
        qss = settings.value(LITEAPP_QSS,"default.qss").toString();
    } else {
        const QSettings settings(QSettings::IniFormat,QSettings::UserScope,"liteide","liteide");
        locale = settings.value(LITEAPP_LANGUAGE,locale).toString();
        qss = settings.value(LITEAPP_QSS,"default.qss").toString();
    }

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
    if (!qss.isEmpty()) {
        QFile f(resPath+"/liteapp/qss/"+qss);
        if (f.open(QFile::ReadOnly)) {
            QString styleSheet = QLatin1String(f.readAll());
            app.setStyleSheet(styleSheet);
        }
    }

    QDir::addSearchPath("icon",resPath);
    QDir::addSearchPath("icon",resPath+"/liteapp");
    QDir::addSearchPath("icon",":/");
#if QT_VERSION >= 0x050000
    QString storage = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    QString storage = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif
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

    IApplication *liteApp = LiteApp::NewApplication(true,0);

    if (fileList.size() == 1) {
        QString file = fileList.at(0);
        QFileInfo f(file);
        if (f.isFile()) {
            liteApp->fileManager()->addFolderList(f.path());
            liteApp->fileManager()->openEditor(file);
        } else if (f.isDir()) {
            liteApp->fileManager()->addFolderList(file);
        }
    } else {
        foreach(QString file, fileList) {
            QFileInfo f(file);
            if (f.isFile()) {
                liteApp->fileManager()->openEditor(file);
            } else if (f.isDir()) {
                liteApp->fileManager()->addFolderList(file);
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
