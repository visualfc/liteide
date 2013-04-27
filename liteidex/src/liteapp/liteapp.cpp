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
// Module: liteapp.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "liteapp.h"
#include "filemanager.h"
#include "editormanager.h"
#include "projectmanager.h"
#include "pluginmanager.h"
#include "actionmanager.h"
#include "mimetypemanager.h"
#include "optionmanager.h"
#include "toolwindowmanager.h"
#include "htmlwidgetmanager.h"
#include "mainwindow.h"
#include "liteappoptionfactory.h"
#include "folderprojectfactory.h"
#include "textbrowserhtmlwidget.h"
#include "pluginsdialog.h"
#include "liteapp_global.h"
#ifdef Q_OS_MAC
#include "macsupport.h"
#endif

#include <QApplication>
#include <QSplashScreen>
#include <QMenuBar>
#include <QDir>
#include <QToolBar>
#include <QAction>
#include <QDateTime>
#include <QSplitter>
#include <QTextCursor>
#include <QTextBlock>
#include <QTimer>
#include <QPainter>
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

QString LiteApp::getRootPath()
{
    QDir rootDir = QApplication::applicationDirPath();
    rootDir.cdUp();
    return rootDir.canonicalPath();
}

QString LiteApp::getPluginPath()
{
    QString root = getRootPath();
#ifdef Q_OS_MAC
    return root+"/PlugIns";
#else
    return root+"/lib/liteide/plugins";
#endif
}

QString LiteApp::getResoucePath()
{
    QString root = getRootPath();
#ifdef Q_OS_MAC
    return root+"/Resources";
#else
    return root+"/share/liteide";
#endif
}

QString LiteApp::getStoragePath()
{
    QString root = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    return root+"/liteide";
}

IApplication* LiteApp::NewApplication(bool loadSession)
{
    LiteApp *app = new LiteApp;
    app->load(loadSession);
    return app;
}

PluginManager *LiteApp::pluginManager()
{
    static PluginManager manager;
    return &manager;
}

QMap<QString,QVariant> LiteApp::m_cookie;

LiteApp::LiteApp()
    : m_applicationPath(QApplication::applicationDirPath()),
      m_pluginPath(LiteApp::getPluginPath()),
      m_resourcePath(LiteApp::getResoucePath()),
      m_storagePath(LiteApp::getStoragePath()),
      m_settings(new QSettings(QSettings::IniFormat,QSettings::UserScope,"liteide","liteide",this)),
      m_extension(new Extension),
      m_mainwindow(new MainWindow(this)),
      m_toolWindowManager(new ToolWindowManager),
      m_htmlWidgetManager(new HtmlWidgetManager),
      m_actionManager(new ActionManager),
      m_projectManager(new ProjectManager),
      m_editorManager(new EditorManager),
      m_fileManager(new FileManager),
      m_mimeTypeManager(new MimeTypeManager),
      m_optionManager(new OptionManager)
{    
    m_goProxy = new GoProxy(this);
    m_actionManager->initWithApp(this);
    m_toolWindowManager->initWithApp(this);
    m_mimeTypeManager->initWithApp(this);
    m_projectManager->initWithApp(this);
    m_editorManager->initWithApp(this);
    m_fileManager->initWithApp(this);
    m_optionManager->initWithApp(this);        

    //m_mainwindow->setCentralWidget(m_editorManager->widget());
    m_mainwindow->splitter()->addWidget(m_editorManager->widget());
    //m_mainwindow->splitter()->addWidget(m_outputManager->widget());
    m_mainwindow->splitter()->setStretchFactor(0,50);
    //m_mainwindow->setStatusBar(m_outputManager->statusBar());

    m_htmlWidgetManager->addFactory(new TextBrowserHtmlWidgetFactory(this));

    m_extension->addObject("LiteApi.IMimeTypeManager",m_mimeTypeManager);
    m_extension->addObject("LiteApi.IProjectManager",m_projectManager);
    m_extension->addObject("LiteApi.IEditManager",m_editorManager);
    m_extension->addObject("LiteApi.IOptoinManager",m_optionManager);
    m_extension->addObject("LiteApi.IToolWindowManager",m_toolWindowManager);
    m_extension->addObject("LiteApi.QMainWindow",m_mainwindow);
    m_extension->addObject("LiteApi.QMainWindow.QSplitter",m_mainwindow->splitter());
    m_extension->addObject("LiteApi.IHtmlWidgetManager",m_htmlWidgetManager);

    //add actions
    connect(m_projectManager,SIGNAL(currentProjectChanged(LiteApi::IProject*)),this,SLOT(currentProjectChanged(LiteApi::IProject*)));
    connect(m_editorManager,SIGNAL(currentEditorChanged(LiteApi::IEditor*)),m_projectManager,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_editorManager,SIGNAL(currentEditorChanged(LiteApi::IEditor*)),m_mainwindow,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_editorManager,SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_editorManager,SIGNAL(tabAddRequest()),m_fileManager,SLOT(openEditors()));
    connect(m_editorManager,SIGNAL(editorSaved(LiteApi::IEditor*)),m_fileManager,SLOT(editorSaved(LiteApi::IEditor*)));
    connect(m_editorManager,SIGNAL(editorCreated(LiteApi::IEditor*)),m_fileManager,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_editorManager,SIGNAL(editorAboutToClose(LiteApi::IEditor*)),m_fileManager,SLOT(editorAboutToClose(LiteApi::IEditor*)));
    connect(m_editorManager,SIGNAL(doubleClickedTab()),m_mainwindow,SLOT(showOrHideToolWindow()));
    connect(m_optionManager,SIGNAL(applyOption(QString)),m_fileManager,SLOT(applyOption(QString)));
    connect(m_optionManager,SIGNAL(applyOption(QString)),m_projectManager,SLOT(applyOption(QString)));

    QAction *esc = new QAction(tr("Escape"),this);
    m_actionManager->getActionContext(this,"App")->regAction(esc,"Escape","ESC");
    connect(esc,SIGNAL(triggered()),this,SLOT(escape()));
    m_mainwindow->addAction(esc);

    createActions();
    createMenus();
    createToolBars();

    m_editorManager->createActions();

    m_logOutput = new TextOutput(this);
    //m_outputManager->addOutuput(m_logOutput,tr("Console"));
    m_logAct = m_toolWindowManager->addToolWindow(Qt::BottomDockWidgetArea,m_logOutput,"eventlog",tr("Event Log"),true);
    connect(m_logOutput,SIGNAL(dbclickEvent(QTextCursor)),this,SLOT(dbclickLogOutput(QTextCursor)));
    m_optionAct = m_editorManager->registerBrowser(m_optionManager->browser());
    //m_viewMenu->addAction(m_optionAct);
    m_actionManager->insertViewMenu(LiteApi::ViewMenuBrowserPos,m_optionAct);
    m_optionManager->setAction(m_optionAct);

    this->appendLog("LiteApp","Initializing");

    m_liteAppOptionFactory = new LiteAppOptionFactory(this,this);

    m_optionManager->addFactory(m_liteAppOptionFactory);

    m_projectManager->addFactory(new FolderProjectFactory(this,this));

    connect(m_goProxy,SIGNAL(done(QByteArray,QByteArray)),this,SLOT(goproxyDone(QByteArray,QByteArray)));
    connect(this,SIGNAL(key_escape()),m_mainwindow,SLOT(hideToolWindow()));
    connect(m_mainwindow,SIGNAL(fullScreenStateChanged(bool)),m_fullScreent,SLOT(setChecked(bool)));
}

static QImage makeSplashImage(LiteApi::IApplication *app)
{
    QRect r(0,0,400,280);
    QImage image(r.size(),QImage::Format_ARGB32_Premultiplied);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing,true);

    QLinearGradient lg(0,0,r.width(),r.height());
    lg.setColorAt(0.0,qRgb(0,125,127));
    lg.setColorAt(1.0,qRgb(0,88,127));

    QBrush bk(lg);
    painter.fillRect(r,bk);
    int th = r.height()*2/5;
    int bh = r.height()-th;
    painter.fillRect(1,th,r.width()-2,bh-1,Qt::white);

    QFont font("Timer",32);
    font.setPointSize(32);
    font.setItalic(true);
    painter.setPen(Qt::white);
    painter.setFont(font);
    painter.drawText(2,2,r.width()-2,th,Qt::AlignCenter,app->ideFullName());

    font.setPointSize(9);
    font.setItalic(false);
    font.setBold(true);
    painter.setPen(Qt::black);
    painter.setFont(font);
    painter.drawText(10,th,r.width()-10,bh,Qt::AlignLeft|Qt::AlignVCenter,app->ideCopyright());

    painter.drawImage(r.width()-145,r.height()-145,QImage("icon:/images/liteide-logo128.png"));
    return image;
}

void LiteApp::load(bool bUseSession)
{
    QSplashScreen *splash = 0;
    bool bSplash = m_settings->value(LITEAPP_SPLASHVISIBLE,true).toBool();
    if (bSplash) {
        splash = new QSplashScreen(QPixmap::fromImage(makeSplashImage(this)),Qt::WindowStaysOnTopHint);
    }
    if (bSplash) {
        splash->show();
    }

    if (bSplash) {
        splash->showMessage("Scanning plugins...",Qt::AlignLeft|Qt::AlignBottom);
    }

    qApp->processEvents();

    loadMimeType();
    loadPlugins();

    if (bSplash) {
        splash->showMessage("Loading plugins...",Qt::AlignLeft|Qt::AlignBottom);
    }

    qApp->processEvents();
    initPlugins();

    if (bSplash) {
        splash->showMessage("Loading state...",Qt::AlignLeft|Qt::AlignBottom);
    }

    qApp->processEvents();

    loadState();
    m_mainwindow->show();

    emit loaded();
    m_projectManager->setCurrentProject(0);

    if (bSplash) {
        splash->showMessage("Loading session...",Qt::AlignLeft|Qt::AlignBottom);
    }

    qApp->processEvents();

    bool b = m_settings->value(LITEAPP_AUTOLOADLASTSESSION,true).toBool();
    if (b && bUseSession) {
        loadSession("default");
    }

    if (bSplash) {
        m_mainwindow->raise();
        splash->finish(m_mainwindow);
        splash->deleteLater();
    }

    this->appendLog("HtmlWidgetFactory",m_htmlWidgetManager->classNameList().join(" "));
    this->appendLog("DefaultHtmlWidgetFactory",m_htmlWidgetManager->defaultClassName());

    m_goProxy->call("version");
    m_goProxy->call("cmdlist");    
	
    appendLog("LiteApp","Finished loading");
}

LiteApp::~LiteApp()
{
    cleanup();
}

IExtension *LiteApp::extension()
{
    return m_extension;
}

void LiteApp::cleanup()
{
    qDeleteAll(m_pluginList);
    m_pluginList.clear();

    delete m_projectManager;
    delete m_editorManager;
    delete m_htmlWidgetManager;
    delete m_liteAppOptionFactory;
    delete m_fileManager;
    delete m_actionManager;
    delete m_mimeTypeManager;
    delete m_optionManager;
    delete m_logOutput;
    delete m_toolWindowManager;
    delete m_extension;
    delete m_settings;
}

void LiteApp::aboutPlugins()
{
    PluginsDialog *dlg = new PluginsDialog(this,m_mainwindow);
    foreach (LiteApi::IPluginFactory *factory, pluginManager()->factoryList()) {
        dlg->appendInfo(factory->info());
    }
    dlg->exec();
}

void LiteApp::escape()
{
    emit key_escape();
    IEditor *editor = m_editorManager->currentEditor();
    if (editor) {
        editor->onActive();
    }
}

bool LiteApp::hasGoProxy() const
{
    return GoProxy::hasProxy();
}

IGoProxy *LiteApp::createGoProxy(QObject *parent)
{
    return new GoProxy(parent);
}

IApplication *LiteApp::newInstance(bool loadSession)
{
    return LiteApp::NewApplication(loadSession);
}

IEditorManager *LiteApp::editorManager()
{
    return m_editorManager;
}

IFileManager *LiteApp::fileManager()
{
    return m_fileManager;
}

IProjectManager *LiteApp::projectManager()
{
    return m_projectManager;
}

IActionManager  *LiteApp::actionManager()
{
    return m_actionManager;
}

IMimeTypeManager *LiteApp::mimeTypeManager()
{
    return m_mimeTypeManager;
}

IOptionManager  *LiteApp::optionManager()
{
    return m_optionManager;
}

IToolWindowManager *LiteApp::toolWindowManager()
{
    return m_toolWindowManager;
}

IHtmlWidgetManager *LiteApp::htmlWidgetManager()
{
    return m_htmlWidgetManager;
}

QMainWindow *LiteApp::mainWindow() const
{
    return m_mainwindow;
}

QSettings *LiteApp::settings()
{
    return m_settings;
}

QMap<QString,QVariant> &LiteApp::globalCookie()
{
    return m_cookie;
}

QString LiteApp::resourcePath() const
{
    return m_resourcePath;
}

QString LiteApp::applicationPath() const
{
    return m_applicationPath;
}

QString LiteApp::pluginPath() const
{
    return m_pluginPath;
}

QString LiteApp::storagePath() const
{
    return m_storagePath;
}

QString LiteApp::ideVersion() const
{
    return "X18";
}

QString LiteApp::ideFullName() const
{
    return this->ideName()+" "+this->ideVersion();
}

QString LiteApp::ideName() const
{
    return "LiteIDE";
}

QString LiteApp::ideCopyright() const
{
    static QString s_info =
    "2011-2013(c)\n"
    "visualfc@gmail.com\n"
    "\n"
    "https://code.google.com/p/liteide\n"
    "https://code.google.com/p/golangide\n"
    "https://github.com/visualfc/liteide";
    return s_info;
}

void LiteApp::setPluginPath(const QString &path)
{
    m_pluginPath = path;
    appendLog("LiteApp","Set plugin path to "+path);
}

void LiteApp::setResourcePath(const QString &path)
{
    m_resourcePath = path;
    appendLog("LiteApp","Set resource path to "+path);
}


QList<IPlugin*> LiteApp::pluginList() const
{
    return m_pluginList;
}

void LiteApp::appendLog(const QString &model, const QString &log, bool error)
{
    QDateTime dt = QDateTime::currentDateTime();
    QString text = dt.toString("hh:mm:ss");
    text += QLatin1Char(' ');
    text += model;
    text += ": ";
    text += log;
    text += QLatin1Char('\n');
    m_logOutput->updateExistsTextColor();
    if (error) {
        m_logOutput->append(text,Qt::red);
        m_logAct->setChecked(true);
    } else {
        m_logOutput->append(text);
    }
}

void LiteApp::sendBroadcast(const QString &module, const QString &id, const QString &param)
{
    emit broadcast(module,id,param);
}

void LiteApp::loadPlugins()
{
    pluginManager()->loadPlugins(m_pluginPath);
}

void LiteApp::loadMimeType()
{
    QDir dir(m_resourcePath+"/liteapp/mimetype");
    if (dir.exists()) {
        m_mimeTypeManager->loadMimeTypes(dir.absolutePath());
    }
}

void LiteApp::initPlugins()
{
    foreach (IPluginFactory *factory,pluginManager()->factoryList()) {
        bool load = m_settings->value(QString("liteapp/%1_load").arg(factory->id()),true).toBool();
        if (!load) {
            continue;
        }
        LiteApi::IPlugin *plugin = factory->createPlugin();
        if (plugin) {
            bool ret = plugin->load(this);
            if (ret) {
                m_pluginList.append(plugin);
            }
            appendLog("LiteApp",QString("%1 %2").arg(ret?"Loaded":"ERROR while loading").arg(factory->id()),!ret);
        }
    }
}

void LiteApp::createActions()
{
    IActionContext *actionContext =  m_actionManager->getActionContext(this,"App");

    m_newAct = new QAction(QIcon("icon:images/new.png"),tr("New..."),m_mainwindow);
    actionContext->regAction(m_newAct,"New",QKeySequence::New);

    m_openFileAct = new QAction(QIcon("icon:images/openfile.png"),tr("Open File..."),m_mainwindow);
    actionContext->regAction(m_openFileAct,"OpenFile",QKeySequence::Open);

    m_openFolderAct = new QAction(QIcon("icon:images/openfolder.png"),tr("Open Folder..."),m_mainwindow);
    actionContext->regAction(m_openFolderAct,"OpenFolder","");

    m_openFolderNewInstanceAct = new QAction(QIcon("icon:images/openfolder.png"),tr("Open Folder in New Instance..."),m_mainwindow);
    actionContext->regAction(m_openFolderNewInstanceAct,"OpenFolderNewInstance","");

    m_newInstance = new QAction(tr("New Instance"),m_mainwindow);
    actionContext->regAction(m_newInstance,"NewInstance","");

    m_closeAct = new QAction(QIcon("icon:images/close.png"),tr("Close File"),m_mainwindow);
    actionContext->regAction(m_closeAct,"CloseFile","Ctrl+W");

    m_closeAllAct = new QAction(QIcon("icon:images/closeall.png"),tr("Close All Files"),m_mainwindow);
    actionContext->regAction(m_closeAllAct,"CloseAllFiles","");

    m_openProjectAct = new QAction(QIcon("icon:images/openproject.png"),tr("Open Project"),m_mainwindow);    

    m_saveProjectAct = new QAction(QIcon("icon:images/saveproject.png"),tr("Save Project"),m_mainwindow);

    m_closeProjectAct = new QAction(QIcon("icon:images/closeproject.png"),tr("Close Folders"),m_mainwindow);
    actionContext->regAction(m_closeProjectAct,"CloseFolders","");

    m_saveAct = new QAction(QIcon("icon:images/save.png"),tr("Save File"),m_mainwindow);
    actionContext->regAction(m_saveAct,"SaveFile",QKeySequence::Save);

    m_saveAsAct = new QAction(tr("Save File As..."),m_mainwindow);
    actionContext->regAction(m_saveAsAct,"SaveFileAs",QKeySequence::SaveAs);

    m_saveAllAct = new QAction(QIcon("icon:images/saveall.png"),tr("Save All Files"),m_mainwindow);
    actionContext->regAction(m_saveAllAct,"SaveAllFiles","");

    m_exitAct = new QAction(tr("Exit"),m_mainwindow);
    actionContext->regAction(m_exitAct,"Exit",QKeySequence::Quit);

    m_fullScreent = new QAction(tr("Full Screen"),m_mainwindow);
    m_fullScreent->setCheckable(true);
    actionContext->regAction(m_fullScreent,"FullScreen","Ctrl+Shift+F11");

    m_aboutAct = new QAction(tr("About LiteIDE"),m_mainwindow);
    actionContext->regAction(m_aboutAct,"About","");

    m_aboutPluginsAct = new QAction(tr("About Plugins"),m_mainwindow);
    actionContext->regAction(m_aboutPluginsAct,"AboutPlugins","");

    connect(m_newAct,SIGNAL(triggered()),m_fileManager,SLOT(newFile()));
    connect(m_openFileAct,SIGNAL(triggered()),m_fileManager,SLOT(openFiles()));
    connect(m_openFolderAct,SIGNAL(triggered()),m_fileManager,SLOT(openFolder()));
    connect(m_openFolderNewInstanceAct,SIGNAL(triggered()),m_fileManager,SLOT(openFolderNewInstance()));
    connect(m_newInstance,SIGNAL(triggered()),m_fileManager,SLOT(newInstance()));
    connect(m_closeAct,SIGNAL(triggered()),m_editorManager,SLOT(closeEditor()));
    connect(m_closeAllAct,SIGNAL(triggered()),m_editorManager,SLOT(closeAllEditors()));
    connect(m_openProjectAct,SIGNAL(triggered()),m_fileManager,SLOT(openProjects()));
    connect(m_saveProjectAct,SIGNAL(triggered()),m_projectManager,SLOT(saveProject()));
    connect(m_closeProjectAct,SIGNAL(triggered()),m_projectManager,SLOT(closeProject()));
    connect(m_saveAct,SIGNAL(triggered()),m_editorManager,SLOT(saveEditor()));
    connect(m_saveAsAct,SIGNAL(triggered()),m_editorManager,SLOT(saveEditorAs()));
    connect(m_saveAllAct,SIGNAL(triggered()),m_editorManager,SLOT(saveAllEditors()));
    connect(m_exitAct,SIGNAL(triggered()),m_mainwindow,SLOT(close()));
    connect(m_aboutAct,SIGNAL(triggered()),m_mainwindow,SLOT(about()));
    connect(m_aboutPluginsAct,SIGNAL(triggered()),this,SLOT(aboutPlugins()));
    connect(m_fullScreent,SIGNAL(toggled(bool)),m_mainwindow,SLOT(setFullScreen(bool)));
}

void LiteApp::createMenus()
{
    m_fileMenu = m_actionManager->loadMenu("menu/file");
    m_viewMenu = m_actionManager->loadMenu("menu/view");
    m_helpMenu = m_actionManager->loadMenu("menu/help");

    m_fileMenu->addAction(m_newAct);
    m_fileMenu->addAction(m_openFileAct);
    m_fileMenu->addAction(m_closeAct);
    m_fileMenu->addAction(m_closeAllAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_saveAct);
    m_fileMenu->addAction(m_saveAsAct);
    m_fileMenu->addAction(m_saveAllAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_newInstance);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_openFolderAct);
    m_fileMenu->addAction(m_openFolderNewInstanceAct);
    m_fileMenu->addAction(m_closeProjectAct);
    //m_fileMenu->addAction(m_openProjectAct);
    //m_fileMenu->addAction(m_saveProjectAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAct);

#ifdef Q_OS_MAC
    if (MacSupport::isLionOrHigh()) {
        MacSupport::setFullScreen(m_mainwindow);
    } else {
        m_viewMenu->addAction(m_fullScreent);
    }
#else
    m_viewMenu->addAction(m_fullScreent);
#endif

    m_viewMenu->addSeparator();

    m_helpMenu->addAction(m_aboutAct);
    m_helpMenu->addAction(m_aboutPluginsAct);
}

void LiteApp::createToolBars()
{
    m_stdToolBar = m_actionManager->loadToolBar("toolbar/std");
    m_stdToolBar->addAction(m_newAct);
    m_stdToolBar->addSeparator();
    m_stdToolBar->addAction(m_openFileAct);
    m_stdToolBar->addAction(m_openFolderAct);
    m_stdToolBar->addAction(m_saveAct);
    m_stdToolBar->addAction(m_saveAllAct);
    //m_stdToolBar->addSeparator();
    //m_stdToolBar->addAction(m_openProjectAct);
    //m_stdToolBar->addAction(m_saveProjectAct);
    //m_stdToolBar->addAction(m_closeProjectAct);
}

void LiteApp::projectReloaded()
{
    LiteApi::IProject *project = (LiteApi::IProject*)sender();
    if (project) {
        //m_outputManager->setProjectInfo(project->filePath());
    }
}

void LiteApp::currentProjectChanged(IProject *project)
{
    bool b = (project != 0);
    m_saveProjectAct->setEnabled(b);
    m_closeProjectAct->setEnabled(b);
    if (project) {
        //m_outputManager->setProjectInfo(project->filePath());
        connect(project,SIGNAL(reloaded()),this,SLOT(projectReloaded()));
    } else {
        //m_outputManager->setProjectInfo("");
    }
}

void LiteApp::currentEditorChanged(IEditor *editor)
{
    bool b = (editor != 0);

    if (b) {
        connect(editor,SIGNAL(modificationChanged(bool)),this,SLOT(editorModifyChanged(bool)));
    }
    m_saveAct->setEnabled(b && editor->isModified() && !editor->isReadOnly());
    m_saveAsAct->setEnabled(editor && !editor->filePath().isEmpty());
    m_saveAllAct->setEnabled(b);
    m_closeAct->setEnabled(b);
    m_closeAllAct->setEnabled(b);
}

void LiteApp::editorModifyChanged(bool /*b*/)
{
    IEditor *editor = (IEditor*)sender();
    if (editor && editor->isModified() && !editor->isReadOnly()) {
        m_saveAct->setEnabled(true);
    } else {
        m_saveAct->setEnabled(false);
    }
}

void LiteApp::loadState()
{
    QByteArray  geometry = m_settings->value("liteapp/geometry").toByteArray();
    if (!geometry.isEmpty()) {
        m_mainwindow->restoreGeometry(geometry);
    } else {
        m_mainwindow->resize(640,480);
    }
    m_mainwindow->restoreState(m_settings->value("liteapp/state").toByteArray());
}

void LiteApp::saveState()
{
    m_settings->setValue("liteapp/geometry",m_mainwindow->saveGeometry());
    m_settings->setValue("liteapp/state",m_mainwindow->saveState());
    m_settings->setValue("liteapp/toolState",m_mainwindow->saveToolState());
}


void LiteApp::loadSession(const QString &name)
{
    QString session = "session/"+name;
    QString projectName = m_settings->value(session+"_project").toString();
    QString scheme = m_settings->value(session+"_scheme").toString();
    QString editorName = m_settings->value(session+"_cureditor").toString();
    QStringList fileList = m_settings->value(session+"_alleditor").toStringList();

    if (!projectName.isEmpty()) {
        if (scheme.isEmpty()) {
            m_fileManager->openProject(projectName);
        } else {
            m_fileManager->openProjectScheme(projectName,scheme);
        }
    } else {
        m_projectManager->closeProject();
    }

    foreach(QString fileName, fileList) {
        m_fileManager->openEditor(fileName,false);
    }
    if (!editorName.isEmpty()) {
        m_fileManager->openEditor(editorName,true);
    } else if (!fileList.isEmpty()){
        m_fileManager->openEditor(fileList.last(),true);
    }
}

void LiteApp::saveSession(const QString &name)
{
    QString projectName;
    QString editorName;
    QString scheme;
    IProject *project = m_projectManager->currentProject();
    if (project) {
        projectName = project->filePath();
        IMimeType *type = m_mimeTypeManager->findMimeType(project->mimeType());
        if (type) {
            scheme = type->scheme();
        }
    }

    QStringList fileList;
    foreach (IEditor* ed,m_editorManager->sortedEditorList()) {
//        if (ed->mimeType().indexOf("liteide/") == 0) {
//            continue;
//        }
        if (ed->filePath().isEmpty()) {
            continue;
        }
        if (ed == m_editorManager->currentEditor()) {
            editorName = ed->filePath();
        } else {
            fileList.append(ed->filePath());
        }
    }
    QString session = "session/"+name;
    m_settings->setValue(session+"_project",projectName);
    m_settings->setValue(session+"_scheme",scheme);
    m_settings->setValue(session+"_cureditor",editorName);
    m_settings->setValue(session+"_alleditor",fileList);
}

void LiteApp::dbclickLogOutput(QTextCursor cur)
{
    QRegExp rep("(\\w?\\:?[\\w\\d\\_\\-\\\\/\\.]+):(\\d+):");
    int index = rep.indexIn(cur.block().text().trimmed());
    if (index < 0)
        return;
    QStringList capList = rep.capturedTexts();

    if (capList.count() < 3)
        return;
    QString fileName = capList[1];
    QString fileLine = capList[2];

    bool ok = false;
    int line = fileLine.toInt(&ok);
    if (!ok)
        return;

    LiteApi::IEditor *editor = m_fileManager->openEditor(fileName);
    if (editor) {
        editor->widget()->setFocus();
        LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
        if (textEditor) {
            textEditor->gotoLine(line-1,0,true);
        }
    }
}

void LiteApp::goproxyDone(const QByteArray &id, const QByteArray &reply)
{
    this->appendLog("GoProxy",QString("%1 = %2").arg(QString::fromUtf8(id)).arg(QString::fromUtf8(reply)));
}
