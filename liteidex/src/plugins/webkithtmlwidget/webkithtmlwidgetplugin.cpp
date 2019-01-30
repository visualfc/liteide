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
// Module: webkithtmlwidgetplugin.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "webkithtmlwidgetplugin.h"
#include "webviewhtmlwidget.h"
#include "webkitbrowser.h"
#include <QtPlugin>
#include <QFileDialog>
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

WebKitHtmlWidgetPlugin::WebKitHtmlWidgetPlugin()
{
}

bool WebKitHtmlWidgetPlugin::load(LiteApi::IApplication *app)
{
    m_liteApp = app;
    LiteApi::IHtmlWidgetFactory *factory = new WebViewHtmlWidgetFactory(this);
    app->htmlWidgetManager()->addFactory(factory);
    app->htmlWidgetManager()->setDefaultClassName(factory->className());
    /*
    WebKitBrowser *wb = new WebKitBrowser(app,this);
    app->toolWindowManager()->addToolWindow(Qt::RightDockWidgetArea,
                                            wb->widget(),"WebKitBrowser",tr("WebKitBrowser"),
                                            false);
    */
    m_browser = new WebKitBrowser(app,this);
    m_browserAct = app->editorManager()->registerBrowser(m_browser);
    app->actionManager()->insertViewMenu(LiteApi::ViewMenuBrowserPos,m_browserAct);
    //
//    QMenu *fileMenu = app->actionManager()->loadMenu("menu/file");
//    IActionContext *actionContext =  app->actionManager()->getActionContext(app,"App");
//    LiteApi::ActionInfo *info = actionContext->actionInfo("AddFolder");
//    if (fileMenu && info && info->action) {
//        QAction *openUrl = new QAction(tr("Open File With WebKit ..."),this);
//        connect(openUrl,SIGNAL(triggered()),this,SLOT(openHtmlWithWebkit()));
//        fileMenu->insertAction(info->action,openUrl);
//    }
    return true;
}

void WebKitHtmlWidgetPlugin::openHtmlWithWebkit()
{
    QString dir = m_liteApp->settings()->value("WebKitBrowser/home","").toString();
    QString filePath = QFileDialog::getOpenFileName(m_liteApp->mainWindow(),tr("Open Html or Markdown File"),dir,
                                                    "Html or Markdown File (*.html *.htm *.md *.markdown);;Html File (*.html *.htm);; Markdown File (*.md *.markdown)");
    if (!filePath.isEmpty()) {
        m_liteApp->settings()->setValue("WebKitBrowser/home",QFileInfo(filePath).absolutePath());
        m_liteApp->editorManager()->activeBrowser(m_browser);
        m_browser->loadUrl(QUrl::fromLocalFile(filePath));
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
