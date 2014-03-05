/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
// Module: litebuild.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEBUILD_H
#define LITEBUILD_H

#include "liteapi/liteapi.h"
#include "liteenvapi/liteenvapi.h"
#include "litebuildapi/litebuildapi.h"

#include <QTextCursor>

#define LITEBUILD_TAG "lightbuild/navtag"

class BuildManager;
class QComboBox;
class ProcessEx;
class TextOutput;
class QStandardItemModel;

class LiteBuild : public LiteApi::ILiteBuild
{
    Q_OBJECT
public:
    explicit LiteBuild(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~LiteBuild();
public:
    virtual void rebuild();
    virtual QString buildTag() const;
    virtual QMap<QString,QString> buildEnvMap() const;
    virtual QMap<QString,QString> liteideEnvMap() const;
    virtual LiteApi::TargetInfo getTargetInfo();
    virtual QString envValue(LiteApi::IBuild *build, const QString &value);
    virtual LiteApi::IBuildManager *buildManager() const;
    virtual void appendOutput(const QString &str, const QBrush &brush, bool active, bool updateExistsTextColor = true);
    virtual void executeCommand(const QString &cmd, const QString &args, const QString &workDir,bool updateExistsTextColor = true, bool activateOutputCheck = true);
    virtual bool buildTests();
    QMap<QString,QString> buildEnvMap(LiteApi::IBuild *build, const QString &buildTag) const;

public:
    QString envToValue(const QString &value,QMap<QString,QString> &liteEnv,const QProcessEnvironment &env);
    void setCurrentBuild(LiteApi::IBuild *build);
    void updateBuildConfig(LiteApi::IBuild *build);
    void loadProjectInfo(const QString &filePath);
    void loadEditorInfo(const QString &filePath);
    void loadTargetInfo(LiteApi::IBuild *build);

    LiteApi::IBuild *findProjectBuildByEditor(LiteApi::IEditor *editor);
    LiteApi::IBuild *findProjectBuild(LiteApi::IProject *project);
public slots:
    void appLoaded();
    void debugBefore();
    void currentEnvChanged(LiteApi::IEnv*);
    void currentProjectChanged(LiteApi::IProject*);
    void reloadProject();
    void editorCreated(LiteApi::IEditor *editor);
    void currentEditorChanged(LiteApi::IEditor*);
    void buildAction(LiteApi::IBuild*,LiteApi::BuildAction*);
    void execAction(const QString &mime,const QString &id);
    void extOutput(const QByteArray &output,bool bError);
    void extFinish(bool error,int exitCode, QString msg);
    void stopAction();
    void dbclickBuildOutput(const QTextCursor &cur);
    void enterTextBuildOutput(QString);
    void config();
protected:
    QMenu *m_nullMenu;
    LiteApi::IApplication   *m_liteApp;
    BuildManager    *m_buildManager;
    LiteApi::IBuild *m_build;
    LiteApi::IEnvManager *m_envManager;
    QMenu       *m_buildMenu;
    QStandardItemModel *m_liteideModel;
    QStandardItemModel *m_configModel;
    QStandardItemModel *m_customModel;
    QMap<QString,QString> m_liteAppInfo;
    QString m_workDir;
    ProcessEx *m_process;
    TextOutput *m_output;
    QAction     *m_configAct;
    QAction     *m_stopAct;
    QAction     *m_clearAct;
    QAction    *m_outputAct;
    QString     m_outputRegex;
    QString     m_buildTag;
    bool        m_bProjectBuild;
    QMap<QString,QString> m_editorInfo;
    QMap<QString,QString> m_projectInfo;
    QMap<QString,QString> m_targetInfo;
};

#endif // LITEBUILD_H
