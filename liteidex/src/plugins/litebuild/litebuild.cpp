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
// Module: litebuild.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "litebuild.h"
#include "buildmanager.h"
#include "fileutil/fileutil.h"
#include "processex/processex.h"
#include "textoutput/textoutput.h"
#include "buildconfigdialog.h"
#include "litedebugapi/litedebugapi.h"

#include <QToolBar>
#include <QComboBox>
#include <QAction>
#include <QMenu>
#include <QDir>
#include <QFileInfo>
#include <QTextBlock>
#include <QTextCodec>
#include <QProcessEnvironment>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QLabel>
#include <QToolButton>
#include <QTime>
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


enum {
    ID_CMD = 0,
    ID_ARGS = 1,
    ID_CODEC = 2,
    ID_MIMETYPE = 3,
    ID_TASKLIST = 4,
    ID_EDITOR = 5
};

LiteBuild::LiteBuild(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::ILiteBuild(parent),
    m_liteApp(app),
    m_buildManager(new BuildManager(this)),
    m_build(0),
    m_envManager(0)
{
    m_nullMenu = new QMenu;
    if (m_buildManager->initWithApp(m_liteApp)) {
        m_buildManager->load(m_liteApp->resourcePath()+"/litebuild");
        m_liteApp->extension()->addObject("LiteApi.IBuildManager",m_buildManager);
    }    
    m_bProjectBuild = false;
//    m_toolBar = m_liteApp->actionManager()->loadToolBar("toolbar/build");
//    if (!m_toolBar) {
//        m_toolBar = m_liteApp->actionManager()->insertToolBar("toolbar/build",tr("Build ToolBar"));
//    }
    m_buildMenu = m_liteApp->actionManager()->loadMenu("menu/build");
    if (!m_buildMenu) {
        m_buildMenu = m_liteApp->actionManager()->insertMenu("menu/build",tr("&Build"),"menu/help");
    }

    //m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuToolBarPos,m_toolBar->toggleViewAction());

    m_liteideModel = new QStandardItemModel(0,2,this);
    m_liteideModel->setHeaderData(0,Qt::Horizontal,tr("Name"));
    m_liteideModel->setHeaderData(1,Qt::Horizontal,tr("Value"));

    m_configModel = new QStandardItemModel(0,2,this);
    m_configModel->setHeaderData(0,Qt::Horizontal,tr("Name"));
    m_configModel->setHeaderData(1,Qt::Horizontal,tr("Value"));

    m_customModel = new QStandardItemModel(0,2,this);
    m_customModel->setHeaderData(0,Qt::Horizontal,tr("Name"));
    m_customModel->setHeaderData(1,Qt::Horizontal,tr("Value"));

    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"Build");

    m_configAct = new QAction(QIcon("icon:litebuild/images/config.png"),tr("Build Configuration..."),this);
    actionContext->regAction(m_configAct,"Config","");

    m_process = new ProcessEx(this);
    m_output = new TextOutput(m_liteApp);
    m_output->setMaxLine(2048);

    m_stopAct = new QAction(tr("Stop Action"),this);
    m_stopAct->setIcon(QIcon("icon:litebuild/images/stopaction.png"));
    actionContext->regAction(m_stopAct,"Stop","");

    m_clearAct = new QAction(tr("Clear Output"),this);
    m_clearAct->setIcon(QIcon("icon:images/cleanoutput.png"));
    actionContext->regAction(m_clearAct,"ClearOutput","");

    connect(m_stopAct,SIGNAL(triggered()),this,SLOT(stopAction()));
    connect(m_clearAct,SIGNAL(triggered()),m_output,SLOT(clear()));

    m_buildMenu->addAction(m_configAct);
    m_buildMenu->addSeparator();
    m_buildMenu->addAction(m_stopAct);
    m_buildMenu->addAction(m_clearAct);
    m_buildMenu->addSeparator();

    //m_liteApp->outputManager()->addOutuput(m_output,tr("Build Output"));
    m_outputAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::BottomDockWidgetArea,
                                                                m_output,"buildoutput",
                                                                tr("Build Output"),
                                                                false,
                                                                QList<QAction*>() << m_stopAct << m_clearAct);

    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));
    //connect(m_liteApp->projectManager(),SIGNAL(currentProjectChanged(LiteApi::IProject*)),this,SLOT(currentProjectChanged(LiteApi::IProject*)));
    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));

    connect(m_process,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(extOutput(QByteArray,bool)));
    connect(m_process,SIGNAL(extFinish(bool,int,QString)),this,SLOT(extFinish(bool,int,QString)));
    connect(m_output,SIGNAL(dbclickEvent(QTextCursor)),this,SLOT(dbclickBuildOutput(QTextCursor)));
    connect(m_output,SIGNAL(enterText(QString)),this,SLOT(enterTextBuildOutput(QString)));
    connect(m_configAct,SIGNAL(triggered()),this,SLOT(config()));

    m_liteAppInfo.insert("LITEAPPDIR",m_liteApp->applicationPath());

    m_liteApp->extension()->addObject("LiteApi.ILiteBuild",this);

    foreach(LiteApi::IBuild *build, m_buildManager->buildList()) {
        connect(build,SIGNAL(buildAction(LiteApi::IBuild*,LiteApi::BuildAction*)),this,SLOT(buildAction(LiteApi::IBuild*,LiteApi::BuildAction*)));
        QList<QAction*> actionList;
        foreach(QAction *act, build->actions()) {
            QMenu *subMenu = act->menu();
            if (subMenu) {
                actionList.append(subMenu->actions());
            } else {
                actionList.append(act);
            }
        }
        foreach(QAction *act, actionList) {
            QStringList shortcuts;
            foreach(QKeySequence key, act->shortcuts()) {
                shortcuts.append(key.toString());
            }
            actionContext->regAction(act,act->objectName(),shortcuts.join(";"));
        }
    }    
}

LiteBuild::~LiteBuild()
{
    stopAction();
    delete m_output;
    if (!m_nullMenu->parent()) {
        delete m_nullMenu;
    }
}

void LiteBuild::rebuild()
{
    if (!m_build) {
        return;
    }
    BuildAction *ba = m_build->findAction("Build");
    if (!ba) {
        return;
    }
    if (m_process->isRunning()) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
    this->execAction(m_build->mimeType(),ba->id());
    if (!m_process->waitForStarted(1000)) {
        return;
    }
    m_process->waitForFinished(1000);
}

QString LiteBuild::envValue(LiteApi::IBuild *build, const QString &value)
{
    QString buildFilePath;
    if (m_buildTag.isEmpty()) {
        LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
        if (editor) {
            QString filePath = editor->filePath();
            if (!filePath.isEmpty()) {
                buildFilePath = QFileInfo(filePath).path();
            }
        }
    }

    QMap<QString,QString> env = buildEnvMap(build,buildFilePath);
    QProcessEnvironment sysenv = LiteApi::getGoEnvironment(m_liteApp);
    return this->envToValue(value,env,sysenv);
}

QString LiteBuild::envToValue(const QString &value,QMap<QString,QString> &liteEnv,const QProcessEnvironment &env)
{
    QString v = value;
    QMapIterator<QString,QString> i(liteEnv);
    while(i.hasNext()) {
        i.next();
        v.replace("$("+i.key()+")",i.value());
    }
    QRegExp rx("\\$\\((\\w+)\\)");
    int pos = 0;
    QStringList list;
    while ((pos = rx.indexIn(v, pos)) != -1) {
         list << rx.cap(1);
         pos += rx.matchedLength();
    }

    foreach (QString str, list) {
         if (env.contains(str)) {
            v.replace("$("+str+")",env.value(str));
        }
    }
    return v;
}

QString LiteBuild::buildTag() const
{
    return m_buildTag;
}

LiteApi::IBuildManager *LiteBuild::buildManager() const
{
    return m_buildManager;
}

void LiteBuild::appendOutput(const QString &str, const QBrush &brush, bool active, bool updateExistsTextColor)
{
    if (updateExistsTextColor) {
        m_output->updateExistsTextColor();
    }
    if (active) {
        m_outputAct->setChecked(true);
    }
    m_output->append(str,brush);
}

void LiteBuild::appLoaded()
{
    m_envManager = LiteApi::getEnvManager(m_liteApp);
    if (m_envManager) {
        connect(m_envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(currentEnvChanged(LiteApi::IEnv*)));
        currentEnvChanged(m_envManager->currentEnv());
    }
}

void LiteBuild::debugBefore()
{
}

void LiteBuild::config()
{
    if (!m_build) {
        return;
    }

    BuildConfigDialog dlg;
    dlg.setBuild(m_build->id(),m_buildTag);
    dlg.setModel(m_liteideModel,m_configModel,m_customModel);
    if (dlg.exec() == QDialog::Accepted) {
        QString key;
        if (!m_buildTag.isEmpty()) {
            key = "litebuild-custom/"+m_buildTag;
        }
        for (int i = 0; i < m_customModel->rowCount(); i++) {
            QStandardItem *name = m_customModel->item(i,0);
            QStandardItem *value = m_customModel->item(i,1);
            //m_customMap.insert(name->text(),value->text());
            if (!key.isEmpty()) {
                m_liteApp->settings()->setValue(key+"#"+name->text(),value->text());
            }
        }
    }
}

void LiteBuild::currentEnvChanged(LiteApi::IEnv*)
{
    m_process->setEnvironment(m_envManager->currentEnvironment().toStringList());
    LiteApi::IEnv *env = m_envManager->currentEnv();
    if (!env) {
        return;
    }

    bool b = m_liteApp->settings()->value("litebuild/goenvcheck",false).toBool();
    if (!b) {
        return;
    }
    m_output->updateExistsTextColor();
    m_output->appendTag(tr("Current environment change id \"%1\"").arg(env->id())+"\n");
    m_output->append(m_envManager->currentEnv()->orgEnvLines().join("\n")+"\n",Qt::black);

    QString gobin = FileUtil::lookupGoBin("go",m_liteApp);

    this->executeCommand(gobin,"env",LiteApi::getGoroot(m_liteApp),false);
}

void LiteBuild::loadProjectInfo(const QString &filePath)
{    
    m_projectInfo.clear();
    if (filePath.isEmpty()) {
        return;
    }
    QFileInfo info(filePath);
    /*
PROJECT_NAME
PROJECT_PATH
PROJECT_DIR
PROJECT_DIRNAME
PROJECT_TARGETNAME
PROJECT_TARGETATH
    */
    if (info.isDir()) {
        m_projectInfo.insert("PROJECT_NAME",info.fileName());
        m_projectInfo.insert("PROJECT_PATH",info.filePath());
        m_projectInfo.insert("PROJECT_DIR",info.filePath());
        m_projectInfo.insert("PROJECT_DIRNAME",info.fileName());
    } else {
        m_projectInfo.insert("PROJECT_NAME",info.fileName());
        m_projectInfo.insert("PROJECT_PATH",info.filePath());
        m_projectInfo.insert("PROJECT_DIR",info.path());
        m_projectInfo.insert("PROJECT_DIRNAME",QFileInfo(info.path()).fileName());
    }
}

LiteApi::IBuild *LiteBuild::findProjectBuild(LiteApi::IProject *project)
{
    if (!project) {
        return 0;
    }
    LiteApi::IBuild *build = m_buildManager->findBuild(project->mimeType());
    return build;
}

void LiteBuild::reloadProject()
{
    LiteApi::IProject *project = (LiteApi::IProject*)sender();
    if (project) {
        loadProjectInfo(project->filePath());
        m_targetInfo = project->targetInfo();
    }
}

void LiteBuild::currentProjectChanged(LiteApi::IProject *project)
{
    return;
    m_buildTag.clear();
    m_projectInfo.clear();
    m_targetInfo.clear();
    m_bProjectBuild = false;
    if (project) {
        connect(project,SIGNAL(reloaded()),this,SLOT(reloadProject()));
        loadProjectInfo(project->filePath());
        m_targetInfo = project->targetInfo();
        m_buildTag = project->filePath();
        LiteApi::IBuild *build = findProjectBuild(project);
        if (build) {
            m_bProjectBuild = true;
            setCurrentBuild(build);
        } else {
            currentEditorChanged(m_liteApp->editorManager()->currentEditor());
        }
    } else {
        LiteApi::IBuild *build = findProjectBuildByEditor(m_liteApp->editorManager()->currentEditor());
        if (build) {
            m_bProjectBuild = true;
        }
        setCurrentBuild(build);
    }
}

QMap<QString,QString> LiteBuild::liteideEnvMap() const
{
    QMap<QString,QString> env = m_liteAppInfo;
    QMapIterator<QString,QString> p(m_projectInfo);
    while(p.hasNext()) {
        p.next();
        env.insert(p.key(),p.value());
    }
    QMapIterator<QString,QString> e(m_editorInfo);
    while(e.hasNext()) {
        e.next();
        env.insert(e.key(),e.value());
    }
    QMapIterator<QString,QString> t(m_targetInfo);
    while(t.hasNext()) {
        t.next();
        env.insert(t.key(),t.value());
    }
    return env;
}

LiteApi::TargetInfo LiteBuild::getTargetInfo()
{
    LiteApi::TargetInfo info;
    if (!m_build) {
        return info;
    }
    QList<BuildTarget*> lists = m_build->targetList();
    if (!lists.isEmpty()) {
        BuildTarget *target = lists.first();

        QMap<QString,QString> env = buildEnvMap(m_build,m_buildTag);
        QProcessEnvironment sysenv = LiteApi::getGoEnvironment(m_liteApp);
        info.cmd = this->envToValue(target->cmd(),env,sysenv);
        info.args = this->envToValue(target->args(),env,sysenv);
        info.workDir = this->envToValue(target->work(),env,sysenv);
    }
    return info;
}

QMap<QString,QString> LiteBuild::buildEnvMap(LiteApi::IBuild *build, const QString &buildFilePath) const
{
    QMap<QString,QString> env = liteideEnvMap();
    if (!build) {
        return env;
    }
    QString customkey;
    if (!buildFilePath.isEmpty()) {
        customkey = "litebuild-custom/"+buildFilePath;
    }
    QString configkey = "litebuild-config/"+build->id();
    foreach(LiteApi::BuildConfig *cf, build->configList()) {
        QString name = cf->name();
        QString value = cf->value();
        if (!configkey.isEmpty()) {
            value = m_liteApp->settings()->value(configkey+"#"+name,value).toString();
        }
        QMapIterator<QString,QString> m(env);
        while(m.hasNext()) {
            m.next();
            value.replace("$("+m.key()+")",m.value());
        }
        env.insert(name,value);
    }
    foreach(LiteApi::BuildCustom *cf, build->customList()) {
        QString name = cf->name();
        QString value = cf->value();
        if (!customkey.isEmpty()) {
            value = m_liteApp->settings()->value(customkey+"#"+name,value).toString();
        }
        QMapIterator<QString,QString> m(env);
        while(m.hasNext()) {
            m.next();
            value.replace("$("+m.key()+")",m.value());
        }
        env.insert(name,value);
    }
    return env;
}

QMap<QString,QString> LiteBuild::buildEnvMap() const
{
    return buildEnvMap(m_build,m_buildTag);
    /*
    LiteApi::IBuild *build = m_build;
    QString buildFilePath = m_buildFilePath;
    if (!build) {
        LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
        if (editor && !editor->filePath().isEmpty()) {
            build = m_manager->findBuild(editor->mimeType());
            buildFilePath = QFileInfo(editor->filePath()).path();
        }
    }
    return buildEnvMap(build,buildFilePath);
    */
    /*
    QMap<QString,QString> env = liteideEnvMap();
    QMapIterator<QString,QString> i(m_configMap);
    while(i.hasNext()) {
        i.next();
        QString k = i.key();
        QString v = i.value();
        QMapIterator<QString,QString> m(env);
        while(m.hasNext()) {
            m.next();
            v.replace("$("+m.key()+")",m.value());
        }
        env.insert(k,v);
    }
    QMapIterator<QString,QString> c(m_customMap);
    while(c.hasNext()) {
        c.next();
        QString k = c.key();
        QString v = c.value();
        QMapIterator<QString,QString> m(env);
        while(m.hasNext()) {
            m.next();
            v.replace("$("+m.key()+")",m.value());
        }
        env.insert(k,v);
    }    
    QMapIterator<QString,QString> p(m_projectInfo);
    while(p.hasNext()) {
        p.next();
        env.insert(p.key(),p.value());
    }
    QMapIterator<QString,QString> e(m_editorInfo);
    while(e.hasNext()) {
        e.next();
        env.insert(e.key(),e.value());
    }
    QMapIterator<QString,QString> t(m_targetInfo);
    while(t.hasNext()) {
        t.next();
        env.insert(t.key(),t.value());
    }
    return env;
    */
}

void LiteBuild::updateBuildConfig(IBuild *build)
{
    m_liteideModel->removeRows(0,m_liteideModel->rowCount());
    QMapIterator<QString,QString> i(this->liteideEnvMap());
    while (i.hasNext()) {
        i.next();
        m_liteideModel->appendRow(QList<QStandardItem*>()
                                 << new QStandardItem(i.key())
                                 << new QStandardItem(i.value()));
    }
    if (build) {
        m_configModel->removeRows(0,m_configModel->rowCount());
        m_customModel->removeRows(0,m_customModel->rowCount());
        QString customkey;
        if (!m_buildTag.isEmpty()) {
            customkey = "litebuild-custom/"+m_buildTag;
        }
        QString configkey = "litebuild-config/"+build->id();
        foreach(LiteApi::BuildCustom *cf, build->customList()) {
            QString name = cf->name();
            QString value = cf->value();
            if (!customkey.isEmpty()) {
                value = m_liteApp->settings()->value(customkey+"#"+name,value).toString();
            }
            m_customModel->appendRow(QList<QStandardItem*>()
                                     << new QStandardItem(name)
                                     << new QStandardItem(value));
        }
        foreach(LiteApi::BuildConfig *cf, build->configList()) {
            QString name = cf->name();
            QString value = cf->value();
            if (!configkey.isEmpty()) {
                value = m_liteApp->settings()->value(configkey+"#"+name,value).toString();
            }
            m_configModel->appendRow(QList<QStandardItem*>()
                                     << new QStandardItem(name)
                                     << new QStandardItem(value));
        }
    }
}

void LiteBuild::setCurrentBuild(LiteApi::IBuild *build)
{
    //update buildconfig
    if (build) {
        updateBuildConfig(build);
    }

    if (m_build == build) {
         return;
    }

    m_build = build;
    m_buildManager->setCurrentBuild(build);

    m_outputRegex.clear();
}

void LiteBuild::loadEditorInfo(const QString &filePath)
{
    m_editorInfo.clear();
    if (filePath.isEmpty()) {
        return;
    }
    //m_editorInfo = editor->editorInfo();
    QFileInfo info(filePath);
    /*
EDITOR_BASENAME
EDITOR_NAME
EDITOR_SUFFIX
EDITOR_PATH
EDITOR_DIR
EDITOR_DIRNAME
EDITOR_TARGETNAME
EDITOR_TARGETATH
    */
    m_buildTag = info.path();
    m_editorInfo.insert("EDITOR_BASENAME",info.baseName());
    m_editorInfo.insert("EDITOR_NAME",info.fileName());
    m_editorInfo.insert("EDITOR_SUFFIX",info.suffix());
    m_editorInfo.insert("EDITOR_PATH",info.filePath());
    m_editorInfo.insert("EDITOR_DIR",info.path());
    m_editorInfo.insert("EDITOR_DIRNAME",QFileInfo(info.path()).fileName());
    m_editorInfo.insert("EDITOR_DIRNAME_GO",QFileInfo(info.path()).fileName().replace(" ","_"));
//    QString target = info.fileName();
//    if (!info.suffix().isEmpty()) {
//        target = target.left(target.length()-info.suffix().length()-1);
//    }
//#ifdef Q_OS_WIN
//    m_editorInfo.insert("EDITOR_TARGETNAME",target+".exe");
//    m_editorInfo.insert("EDITOR_TARGETPATH",QFileInfo(info.path(),target+".exe").filePath());
//#else
//    m_editorInfo.insert("EDITOR_TARGETNAME",target);
//    m_editorInfo.insert("EDITOR_TARGETPATH",QFileInfo(info.path(),target).filePath());
//#endif
}

void LiteBuild::loadTargetInfo(LiteApi::IBuild *build)
{
    m_targetInfo.clear();
    if (!build) {
        return;
    }
    QList<BuildTarget*> lists = build->targetList();
    if (!lists.isEmpty()) {
        BuildTarget *target = lists.first();
        QString cmd = this->envValue(build,target->cmd());
        QString args = this->envValue(build,target->args());
        QString work = this->envValue(build,target->work());
        m_targetInfo.insert("TARGET_CMD",cmd);
        m_targetInfo.insert("TARGET_ARGS",args);
        m_targetInfo.insert("TARGET_WORK",work);
    }
}

LiteApi::IBuild *LiteBuild::findProjectBuildByEditor(IEditor *editor)
{
    m_buildTag.clear();
    m_projectInfo.clear();
    m_targetInfo.clear();

    if (!editor) {
        return 0;
    }
    QString filePath = editor->filePath();
    if (filePath.isEmpty()) {
        return 0;
    }
    QString workDir = QFileInfo(filePath).path();
    LiteApi::IBuild *build = m_buildManager->findBuild(editor->mimeType());
    LiteApi::IBuild *projectBuild = 0;
    QString projectPath;
    if (build != 0) {
        foreach (LiteApi::BuildLookup *lookup,build->lookupList()) {
            QDir dir(workDir);
            for (int i = 0; i <= lookup->top(); i++) {
                QFileInfoList infos = dir.entryInfoList(QStringList() << lookup->file(),QDir::Files);
                if (infos.size() >= 1) {
                    projectBuild = m_buildManager->findBuild(lookup->mimeType());
                    if (projectBuild != 0) {
                        projectPath = infos.at(0).filePath();
                        m_buildTag = projectPath;
                        break;
                    }
                }
                dir.cdUp();
            }
        }
    }
    if (!projectBuild) {
        return 0;
    }
    loadProjectInfo(projectPath);
    QMap<QString,QString> targetInfo;
    if (m_liteApp->fileManager()->findProjectTargetInfo(projectPath,targetInfo)) {
        m_targetInfo = targetInfo;
    }
    return projectBuild;
}

void LiteBuild::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    IBuild *build = m_buildManager->findBuild(editor->mimeType());
    if (!build) {
        return;
    }
    QToolBar *toolBar = LiteApi::findExtensionObject<QToolBar*>(editor,"LiteApi.QToolBar");
    if (!toolBar) {
        return;
    }
    QAction *spacer = LiteApi::findExtensionObject<QAction*>(editor,"LiteApi.QToolBar.Spacer");
    QList<QAction*> actions = build->actions();
    toolBar->insertSeparator(spacer);
    toolBar->insertAction(spacer,m_configAct);
    toolBar->insertSeparator(spacer);
    toolBar->insertActions(spacer,actions);

    QMenu *menu = new QMenu(editor->widget());

    menu->addAction(m_configAct);
    menu->addSeparator();
    menu->addAction(m_stopAct);
    menu->addAction(m_clearAct);
    menu->addSeparator();

    foreach (QAction *act, actions) {
        QMenu *subMenu = act->menu();
        if (subMenu) {
            if (!menu->isEmpty())
                menu->addSeparator();
            menu->addActions(subMenu->actions());
        } else {
            menu->addAction(act);
        }
    }

    editor->extension()->addObject("LiteApi.IBuild",build);
    editor->extension()->addObject("LiteApi.Menu.Build",menu);
}

void LiteBuild::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (!editor) {
        m_buildMenu->menuAction()->setMenu(m_nullMenu);
        m_buildMenu->menuAction()->setEnabled(false);
        setCurrentBuild(0);
        return;
    }
    LiteApi::IBuild *build = LiteApi::findExtensionObject<LiteApi::IBuild*>(editor,"LiteApi.IBuild");
    QMenu *menu = LiteApi::findExtensionObject<QMenu*>(editor,"LiteApi.Menu.Build");    
    if (menu) {
        m_buildMenu->menuAction()->setMenu(menu);
    } else {
        m_buildMenu->menuAction()->setMenu(m_nullMenu);
    }
    m_buildMenu->setEnabled(menu != 0);

    loadEditorInfo(editor->filePath());

    setCurrentBuild(build);
}

void LiteBuild::extOutput(const QByteArray &data, bool /*bError*/)
{
    if (data.isEmpty()) {
        return;
    }
    //m_liteApp->outputManager()->setCurrentOutput(m_output);
    m_outputAct->setChecked(true);

    QString codecName = m_process->userData(2).toString();
    QTextCodec *codec = QTextCodec::codecForLocale();
    if (!codecName.isEmpty()) {
        codec = QTextCodec::codecForName(codecName.toLatin1());
    }
    m_output->append(codec->toUnicode(data));
}

void LiteBuild::extFinish(bool error,int /*exitCode*/, QString msg)
{
    m_output->setReadOnly(true);

    // exitCode != 0 does not mean error, example "gofmt --help"
    //error = error || (exitCode != 0);

    if (error) {
        m_output->appendTag(tr("Error: %1.").arg(msg)+"\n",true);
    } else {
        m_output->appendTag(tr("Success: %1.").arg(msg)+"\n");
    }

    if (!error) {
        QStringList task = m_process->userData(ID_TASKLIST).toStringList();
        if (!task.isEmpty()) {
            QString id = task.takeFirst();
            QString mime = m_process->userData(ID_MIMETYPE).toString();
            m_process->setUserData(ID_TASKLIST,task);
            execAction(mime,id);
        }
    } else {
        m_process->setUserData(ID_TASKLIST,QStringList());
    }
}

void LiteBuild::stopAction()
{
    if (m_process->isRunning()) {
        if (!m_process->waitForFinished(100)) {
            m_process->kill();
        }
    }
}

void LiteBuild::executeCommand(const QString &cmd1, const QString &args, const QString &workDir, bool updateExistsTextColor)
{
    if (updateExistsTextColor) {
        m_output->updateExistsTextColor();
    }
    m_outputAct->setChecked(true);
    if (m_process->isRunning()) {
        m_output->append(tr("A process is currently running.  Stop the current action first.")+"\n",Qt::red);
        return;
    }
    QProcessEnvironment sysenv = LiteApi::getGoEnvironment(m_liteApp);
    QString cmd = cmd1.trimmed();
    m_output->setReadOnly(false);
    m_process->setEnvironment(sysenv.toStringList());
    m_process->setUserData(0,cmd);
    m_process->setUserData(1,args);
    m_process->setUserData(2,"utf-8");

    QString shell = FileUtil::lookPathInDir(cmd,workDir);
    if (shell.isEmpty()) {
        shell = FileUtil::lookPath(cmd,sysenv,false);
    }
    if (!shell.isEmpty()) {
        cmd = shell;
    }

    m_process->setWorkingDirectory(workDir);
    m_output->appendTag(QString("%1 %2 [%3]\n")
                         .arg(cmd).arg(args).arg(workDir));
#ifdef Q_OS_WIN
    m_process->setNativeArguments(args);
    if (cmd.indexOf(" ")) {
        m_process->start("\""+cmd+"\"");
    } else {
        m_process->start(cmd);
    }
#else
    m_process->start(cmd+" "+args);
#endif
}

void LiteBuild::buildAction(LiteApi::IBuild* build,LiteApi::BuildAction* ba)
{  
    m_outputAct->setChecked(true);
    if (m_process->isRunning()) {        
        if (ba->isKillOld()) {
            m_output->append(tr("Killing current process...")+"\n");
            m_process->kill();
            if (!m_process->waitForFinished(1000)) {
                m_output->append(tr("Failed to terminate the existing process!")+"\n",Qt::red);
                return;
            }
        } else {
            m_output->append(tr("A process is currently running.  Stop the current action first.")+"\n",Qt::red);
            return;
        }
    }

    QString mime = build->mimeType();
    QString id = ba->id();
    QString editor;
    LiteApi::IEditor *ed = m_liteApp->editorManager()->currentEditor();
    if (ed) {
        editor = ed->filePath();
    }

    m_output->updateExistsTextColor();
    m_process->setUserData(ID_MIMETYPE,mime);
    m_process->setUserData(ID_EDITOR,editor);
    if (ba->task().isEmpty()) {
        execAction(mime,id);
    } else {
        QStringList task = ba->task();
        QString id = task.takeFirst();
        m_process->setUserData(ID_TASKLIST,task);
        execAction(mime,id);
    }
}

void LiteBuild::execAction(const QString &mime, const QString &id)
{
    if (m_process->isRunning()) {
        return;
    }

    LiteApi::IBuild *build = m_buildManager->findBuild(mime);
    if (!build) {
        return;
    }

    LiteApi::BuildAction *ba = build->findAction(id);
    if (!ba) {
        return;
    }

    QString codec = ba->codec();
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (ba->save() == "project") {
        if (editor && editor->isModified()) {
            m_liteApp->editorManager()->saveEditor();
        }
        m_liteApp->projectManager()->saveProject();
    } else if(ba->save() == "editor") {
        if (editor && editor->isModified()) {
            m_liteApp->editorManager()->saveEditor();
        }
    } else if (ba->save() == "all") {
        m_liteApp->editorManager()->saveAllEditors();
    }

    QString editorPath = m_process->userData(ID_EDITOR).toString();
    QString buildFilePath;
    if (!editorPath.isEmpty()) {
        buildFilePath = QFileInfo(editorPath).path();
    } else {
        buildFilePath = m_buildTag;
    }

    QMap<QString,QString> env = buildEnvMap(build,buildFilePath);

    QProcessEnvironment sysenv = LiteApi::getGoEnvironment(m_liteApp);

    QString cmd = this->envToValue(ba->cmd(),env,sysenv);
    QString args = this->envToValue(ba->args(),env,sysenv);

    m_workDir = this->envToValue(build->work(),env,sysenv);
    QString work = ba->work();
    if (!work.isEmpty()) {
        m_workDir = this->envToValue(work,env,sysenv);
    }

//    if (!QFileInfo(cmd).exists()) {
//        QString findCmd = FileUtil::lookPathInDir(cmd,m_workDir);
//        if (!findCmd.isEmpty()) {
//            cmd = findCmd;
//        }
//    }
    QString shell = FileUtil::lookPathInDir(cmd,m_workDir);
    if (shell.isEmpty()) {
        shell = FileUtil::lookPath(cmd,sysenv,false);
    }
    if (!shell.isEmpty()) {
        cmd = shell;
    }

    if (cmd.indexOf("$(") >= 0 || args.indexOf("$(") >= 0 || m_workDir.isEmpty()) {
        m_output->appendTag(tr("> Could not parse action '%1'").arg(ba->id())+"\n");
        m_process->setUserData(3,QStringList());
        return;
    }


    if (!ba->regex().isEmpty()) {
        m_outputRegex = this->envToValue(ba->regex(),env,sysenv);
    }

    if (ba->isOutput() && ba->isReadline()) {
        m_output->setReadOnly(false);
    } else {
        m_output->setReadOnly(true);
    }

//    if (ba->func() == "debug") {
//        LiteApi::ILiteDebug *debug = LiteApi::getLiteDebug(m_liteApp);
//        if (debug) {
//            debug->startDebug(cmd,args,work);
//        }
//        return;
//    }

    m_process->setEnvironment(sysenv.toStringList());
    if (!ba->isOutput()) {
        bool b = QProcess::startDetached(cmd,args.split(" "),m_workDir);
        m_output->appendTag(QString("%1 %2 [%3]\n")
                             .arg(QDir::cleanPath(cmd)).arg(args).arg(m_workDir));
        m_output->appendTag(b?tr("Started process successfully"):tr("Failed to start process")+"\n");
    } else {
        m_process->setUserData(0,cmd);
        m_process->setUserData(1,args);
        m_process->setUserData(2,codec);

        m_process->setWorkingDirectory(m_workDir);        
        m_output->appendTag(QString("%1 %2 [%3]\n")
                             .arg(QDir::cleanPath(cmd)).arg(args).arg(m_workDir));
#ifdef Q_OS_WIN
        m_process->setNativeArguments(args);
        m_process->start("\""+cmd+"\"");
#else
        m_process->start(cmd + " " + args);
#endif
    }
}

void LiteBuild::enterTextBuildOutput(QString text)
{
    if (!m_process->isRunning()) {
        return;
    }
    QTextCodec *codec = QTextCodec::codecForLocale();
    QString codecName = m_process->userData(2).toString();
    if (!codecName.isEmpty()) {
        codec = QTextCodec::codecForName(codecName.toLatin1());
    }
    if (codec) {
        m_process->write(codec->fromUnicode(text));
    } else {
        m_process->write(text.toLatin1());
    }
}

void LiteBuild::dbclickBuildOutput(const QTextCursor &cur)
{
    if (m_outputRegex.isEmpty()) {
        m_outputRegex = "([\\w\\d_\\\\/\\.]+):(\\d+):";
    }
    QRegExp rep(m_outputRegex);//"([\\w\\d:_\\\\/\\.]+):(\\d+)");

    int index = rep.indexIn(cur.block().text());
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

    QDir dir(m_workDir);
    QString filePath = dir.filePath(fileName);
    if (QFile::exists(filePath)) {
        fileName = filePath;
    } else {
        foreach(QFileInfo info,dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot)) {
            QString filePath = info.absoluteDir().filePath(fileName);
            if (QFile::exists(filePath)) {
                fileName = filePath;
                break;
            }
        }
    }
    /*
    LiteApi::IProject *project = m_liteApp->projectManager()->currentProject();
    if (project) {
        fileName = project->fileNameToFullPath(fileName);
    } else {
        QDir dir(m_workDir);
        QString filePath = dir.filePath(fileName);
        if (QFile::exists(filePath)) {
            fileName = filePath;
        } else {
            foreach(QFileInfo info,dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot)) {
                QString filePath = info.absoluteDir().filePath(fileName);
                if (QFile::exists(filePath)) {
                    fileName = filePath;
                    break;
                }
            }
        }
    }
    */
    LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(fileName);
    if (editor) {
        QTextCursor lineCur = cur;
        lineCur.select(QTextCursor::LineUnderCursor);
        m_output->setTextCursor(lineCur);
        editor->widget()->setFocus();
        LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
        if (textEditor) {
            textEditor->gotoLine(line-1,0,true);
        }
    }
}
