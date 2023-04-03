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
#ifdef Q_OS_MACOS
#include <QtConcurrent/QtConcurrent>
#endif
#include <QDebug>
#include <QtGlobal>
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

#ifdef Q_OS_MACOS
class LiteIDEApplication : public QApplication {
public:
    IApplication *liteApp = nullptr;

    LiteIDEApplication(int &argc, char **argv) : QApplication(argc, argv) {}

    bool event(QEvent *event) override
    {
        if (event->type() == QEvent::FileOpen) {
            QString filePath = static_cast<QFileOpenEvent *>(event)->file();
            if (liteApp == nullptr) {
                // Cold start, wait liteApp ready
                QFuture<void> future = QtConcurrent::run([this, filePath](){
                    while(this->liteApp == nullptr) QThread::sleep(1);
                    QMetaObject::invokeMethod(this, [this, filePath]() {
                        openFileOrFolder(filePath);
                    }, Qt::QueuedConnection);
                });
            } else {
                openFileOrFolder(filePath);
            }
        }
        return QApplication::event(event);
    }

private:
    void openFileOrFolder(QString filePath) {
        QFileInfo f(filePath);
        if (!f.exists() || liteApp == nullptr) return;
        if (f.isFile()) {
            liteApp->fileManager()->openEditor(filePath);
        } else if (f.isDir()) {
            liteApp->fileManager()->addFolderList(filePath);
        }
    }
};
#endif

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
    
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

#ifdef Q_OS_MACOS
    LiteIDEApplication app(argc, argv);
#else
    QApplication app(argc, argv);
#endif
    QStringList arguments = app.arguments();

    //init load file or folder list
    QStringList fileList;

    //liteide --select-env [system|win32|cross-linux64|...]     select init environment id
    //liteide --reset-setting   reset current setting
    //liteide --local-setting   force user local setting
    //liteide --user-setting    force use user setting
    QString flagSelectEnv = "--select-env";
    QString argSelectEnv;
    QString flagResetSetting = "--reset-setting";
    QString flagLocalSetting = "--local-setting";
    QString flagUserSetting = "--user-setting";
    bool argResetSetting = false;
    bool argLocalSetting = false;
    bool argUserSetting = false;
    for(int i = 1; i < arguments.size(); i++) {
        QString arg = arguments[i];
        if (arg.startsWith("-")) {
            if (arg.indexOf(flagSelectEnv+"=") == 0) {
                argSelectEnv = arg.mid(flagSelectEnv.length()+1);
            } else if (arg == flagSelectEnv) {
                i++;
                if (i < arguments.size()) {
                    argSelectEnv = arguments[i];
                }
            } else if (arg == flagResetSetting) {
                argResetSetting = true;
            } else if (arg == flagLocalSetting) {
                argLocalSetting = true;
            } else if (arg == flagUserSetting) {
                argUserSetting = true;
            }
            continue;
        }
        fileList.append(arg);
    }

    //save to global
    if (!argSelectEnv.isEmpty()) {
        LiteApp::s_cookie.insert(flagSelectEnv,argSelectEnv);
    }
    if (argLocalSetting) {
        LiteApp::s_cookie.insert(flagLocalSetting,true);
    }
    if (argUserSetting) {
        LiteApp::s_cookie.insert(flagUserSetting,true);
    }

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

    if (argUserSetting) {
        storeLocal = false;
    } else if (argLocalSetting) {
        storeLocal = true;
    }

    if (storeLocal) {
        QSettings settings(resPath+"/liteapp/config/liteide.ini", QSettings::IniFormat);
        if (argResetSetting) {
            settings.clear();
        }
        locale = settings.value(LITEAPP_LANGUAGE,locale).toString();
        qss = settings.value(LITEAPP_QSS,"default.qss").toString();
    } else {
        QSettings settings(QSettings::IniFormat,QSettings::UserScope,"liteide","liteide");
        if (argResetSetting) {
            settings.clear();
        }
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

    IApplication *liteApp = LiteApp::NewApplication("default",0);

    app.liteApp = liteApp;

    foreach(QString file, fileList) {
        QFileInfo f(file);
        if (f.isFile()) {
            liteApp->fileManager()->openEditor(file);
        } else if (f.isDir()) {
            liteApp->fileManager()->addFolderList(file);
        }
    }

    int ret = app.exec();
    return ret;
}

int cdrv_main(int argc, char **argv)
{
    return liteapp_main(argc,argv);
}
