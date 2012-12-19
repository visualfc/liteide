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
// Module: liteapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef __LITEAPI_H__
#define __LITEAPI_H__

#include "liteobj.h"
#include "litehtml.h"

#include <QWidget>
#include <QMenu>
#include <QPlainTextEdit>
#include <QSettings>
#include <QMainWindow>
#include <QDockWidget>
#include <QFlags>
#include <QUrl>
#include <QDesktopServices>

namespace LiteApi {

class IApplication;
class IManager;
class IFile;
class IProject;
class IEditor;
/*
valueForKey

EDITORPATH
EDITORNAME
EDITORDIR

PROJECTPATH
PROJECTNAME
PROJECTDIR

WORKDIR

TARGETPATH
TARGETNAME
TARGETDIR
*/

struct TargetInfo
{
    QString workDir;
    QString targetPath;
    QString targetName;
    QString targetDir;
};

struct ProjectInfo
{
    QString projectPath;
    QString projectName;
    QString projectDir;
};

struct EditorInfo
{
    QString editorPath;
    QString editorName;
    QString editorDir;
};

class IManager : public QObject
{
    Q_OBJECT
public:
    IManager(QObject *parent = 0) : QObject(parent) {}
    virtual ~IManager() {}
    virtual bool initWithApp(IApplication *app) {
        m_liteApp = app;
        return true;
    }
protected:
    IApplication *m_liteApp;
};

class IMimeType
{
public:
    virtual ~IMimeType() {}

    virtual QString type() const = 0;
    virtual QString scheme() const = 0;
    virtual QString comment() const = 0;
    virtual QString codec() const = 0;
    virtual QStringList globPatterns() const = 0;
    virtual QStringList subClassesOf() const = 0;
    virtual void merge(const IMimeType *mimeType) = 0;
};

class IMimeTypeManager : public IManager
{
    Q_OBJECT
public:
    IMimeTypeManager(QObject *parent = 0) : IManager(parent) {}
    virtual bool addMimeType(IMimeType *mimeType) = 0;
    virtual void removeMimeType(IMimeType *mimeType) = 0;
    virtual QList<IMimeType*> mimeTypeList() const= 0;
    virtual IMimeType *findMimeType(const QString &type) const = 0;
    virtual QString findMimeTypeByFile(const QString &fileName) const = 0;
    virtual QString findMimeTypeByScheme(const QString &scheme) const = 0;
    virtual QStringList findAllFilesByMimeType(const QString &dir, const QString &type, int deep = 0) const = 0;
};

inline QString mimeHead(const QString &mimeType)
{
    int find = mimeType.indexOf('/');
    if (find == -1) {
        return mimeType;
    }
    return mimeType.left(find);
}

inline bool mimeIsText(const QString &mimeType)
{
    return mimeHead(mimeType) == "text";
}

inline bool mimeIsFolder(const QString &mimeType)
{
    return mimeHead(mimeType) == "folder";
}

class IFile : public QObject
{
    Q_OBJECT
public:
    IFile(QObject *parent = 0) : QObject(parent) {}
    virtual ~IFile() { }
    virtual bool open(const QString &filePath, const QString &mimeType) = 0;
    virtual bool reload() = 0;
    virtual bool save(const QString &filePath) = 0;
    virtual bool isReadOnly() const = 0;
    virtual QString filePath() const = 0;
    virtual QString mimeType() const = 0;
};

class IEditorFactory : public QObject
{
    Q_OBJECT
public:
    IEditorFactory(QObject *parent = 0) : QObject(parent) {}
    virtual QStringList mimeTypes() const = 0;
    virtual IEditor *open(const QString &fileName, const QString &mimeType) = 0;
    virtual IEditor *create(const QString &contents, const QString &mimeType) = 0;
};

class IProjectFactory : public QObject
{
    Q_OBJECT
public:
    IProjectFactory(QObject *parent = 0) : QObject(parent) {}
    virtual QStringList mimeTypes() const = 0;
    virtual IProject *open(const QString &fileName, const QString &mimeType) = 0;
    virtual bool findTargetInfo(const QString &fileName, const QString &mimetype, QMap<QString,QString>& targetInfo) const = 0;
};

class IFileManager : public IManager
{
    Q_OBJECT
public:
    IFileManager(QObject *parent = 0) : IManager(parent) {}

    virtual void execFileWizard(const QString &projPath, const QString &filePath, const QString &gopath = QString()) = 0;
    virtual bool openFile(const QString &fileName) = 0;
    virtual IEditor *openEditor(const QString &fileName, bool bActive = true) = 0;
    virtual IEditor *createEditor(const QString &contents, const QString &_mimeType) = 0;
    virtual IEditor *createEditor(const QString &fileName) = 0;
    virtual IProject *openProject(const QString &fileName) = 0;
    virtual IProject *openFolderProject(const QString &folder) = 0;
    virtual IProject *openProjectScheme(const QString &fileName, const QString &scheme) = 0;
    // recent
    virtual QStringList schemeList() const = 0;
    virtual void addRecentFile(const QString &fileName, const QString &scheme) = 0;
    virtual void removeRecentFile(const QString &fileName, const QString &scheme) = 0;
    virtual QStringList recentFiles(const QString &scheme) const = 0;

    virtual bool findProjectTargetInfo(const QString &fileName, QMap<QString,QString>& targetInfo) const = 0;
signals:
    void fileListChanged();
    void recentFilesChanged(QString);
    void fileWizardFinished(const QString &type, const QString &scheme, const QString &location);
public slots:
    virtual void newFile() = 0;
    virtual void openFiles() = 0;
    virtual void openFolder() = 0;
    virtual void openEditors() = 0;
    virtual void openProjects() = 0;
};

class IView : public IObject
{
    Q_OBJECT
public:
    IView(QObject *parent = 0) : IObject(parent) {}
    virtual QWidget *widget() = 0;
    virtual QString name() const = 0;
    virtual QIcon icon() const { return QIcon(); }
};

class IEditor : public IView
{
    Q_OBJECT
public:
    IEditor(QObject *parent = 0) : IView(parent) {}
    virtual bool open(const QString &filePath,const QString &mimeType) = 0;
    virtual bool reload() = 0;
    virtual bool save() = 0;
    virtual bool saveAs(const QString &filePath) = 0;
    virtual void setReadOnly(bool b) = 0;
    virtual bool isReadOnly() const = 0;
    virtual bool isModified() const = 0;
    virtual QString filePath() const = 0;
    virtual QString mimeType() const = 0;
    virtual QByteArray saveState() const = 0;
    virtual bool restoreState(const QByteArray &array) = 0;
    virtual void onActive() = 0;
signals:
    void modificationChanged(bool);
    void contentsChanged();
    void reloaded();
};

class ITextEditor : public IEditor
{
    Q_OBJECT
public:
    ITextEditor(QObject *parent = 0) : IEditor(parent) {}
    virtual int line() const = 0;
    virtual int column() const = 0;
    virtual int utf8Position() const = 0;
    virtual QByteArray utf8Data() const = 0;
    virtual void gotoLine(int line, int column, bool center = false) = 0;
};

inline ITextEditor *getTextEditor(IEditor *editor)
{
    if (editor && editor->extension()) {
        return findExtensionObject<ITextEditor*>(editor->extension(),"LiteApi.ITextEditor");
    }
    return 0;
}

inline QMenu *getMenu(IObject *obj, const QString &id)
{
    if (obj && obj->extension()) {
        return findExtensionObject<QMenu*>(obj->extension(),QString("LiteApi.Menu.%1").arg(id));
    }
    return 0;
}

inline QMenu *getEditMenu(IObject *obj)
{
    return getMenu(obj,"Edit");
}

inline QMenu *getContextMenu(IObject *obj)
{
    if (obj && obj->extension()) {
        return findExtensionObject<QMenu*>(obj->extension(),"LiteApi.ContextMenu");
    }
    return 0;
}

inline QPlainTextEdit *getPlainTextEdit(IEditor *editor) {
    if (editor && editor->extension()) {
        return findExtensionObject<QPlainTextEdit*>(editor->extension(),"LiteApi.QPlainTextEdit");
    }
    return 0;
}

class IEditorManager : public IManager
{
    Q_OBJECT
public:
    IEditorManager(QObject *parent = 0) : IManager(parent) {}
    virtual IEditor *openEditor(const QString &fileName, const QString &mimeType) = 0;
    virtual void addFactory(IEditorFactory *factory) = 0;
    virtual void removeFactory(IEditorFactory *factory) = 0;
    virtual QList<IEditorFactory*> factoryList() const = 0;
    virtual QStringList mimeTypeList() const = 0;
    virtual QWidget *widget() = 0;
    virtual IEditor *currentEditor() const = 0;
    virtual void setCurrentEditor(IEditor *editor) = 0;
    virtual IEditor *findEditor(const QString &fileName, bool canonical) const = 0;
    virtual QList<IEditor*> editorList() const = 0;
    virtual QAction *registerBrowser(IEditor *editor) = 0;
    virtual void activeBrowser(IEditor *editor) = 0;
    virtual void addNavigationHistory(IEditor *editor = 0,const QByteArray &saveState = QByteArray()) = 0;
    virtual void cutForwardNavigationHistory() = 0;
public slots:
    virtual bool saveEditor(IEditor *editor = 0, bool emitAboutSave = true) = 0;
    virtual bool saveEditorAs(IEditor *editor = 0) = 0;
    virtual bool saveAllEditors() = 0;
    virtual bool closeEditor(IEditor *editor = 0) = 0;
    virtual bool closeAllEditors(bool autoSaveAll = false) = 0;
signals:
    void currentEditorChanged(LiteApi::IEditor *editor);
    void editorCreated(LiteApi::IEditor *editor);
    void editorAboutToClose(LiteApi::IEditor *editor);
    void editorAboutToSave(LiteApi::IEditor *editor);
    void editorSaved(LiteApi::IEditor *editor);
};

class IBrowserEditor : public IEditor
{
    Q_OBJECT
public:
    IBrowserEditor(QObject *parent = 0)  : IEditor(parent) {}
    virtual bool open(const QString &/*fileName*/,const QString &/*mimeType*/) { return false; }
    virtual bool reload() { return false; }
    virtual bool save() { return false; }
    virtual bool saveAs(const QString &/*fileName*/){ return false; }
    virtual void setReadOnly(bool /*b*/) {}
    virtual bool isReadOnly() const { return true; }
    virtual bool isModified() const { return false; }
    virtual QString filePath() const { return QString(); }
    virtual QMap<QString,QString> editorInfo() const { return QMap<QString,QString>(); }
    virtual QMap<QString,QString> targetInfo() const { return QMap<QString,QString>(); }
    virtual QByteArray saveState() const {return QByteArray(); }
    virtual bool restoreState(const QByteArray &) { return false; }
    virtual void onActive(){}
};

class IProject : public IView
{
    Q_OBJECT
public:
    virtual QString filePath() const = 0;
    virtual QString mimeType() const = 0;
    virtual QStringList folderList() const = 0;
    virtual QStringList fileNameList() const = 0;
    virtual QStringList filePathList() const = 0;
    virtual QString fileNameToFullPath(const QString &filePath) = 0;
    virtual QMap<QString,QString> targetInfo() const = 0;
    virtual void load() = 0;
signals:
    void reloaded();
};

class IFileProject : public IProject
{
    Q_OBJECT
public:
    virtual bool isFolder() const { return false; }
};

class IFolderProject : public IProject
{
    Q_OBJECT
public:
    virtual bool isFolder() const { return true; }
    virtual QStringList folderList() const = 0;
};

class IOption : public IView
{
    Q_OBJECT
public:
    IOption(QObject *parent = 0) : IView(parent) {}
    virtual QString mimeType() const = 0;
    virtual void apply() = 0;
};

class IOptionFactory : public QObject
{
    Q_OBJECT
public:
    IOptionFactory(QObject *parent = 0) : QObject(parent) {}
    virtual QStringList mimeTypes() const = 0;
    virtual IOption *create(const QString &mimeType) = 0;
};

class IOptionManager : public IManager
{
    Q_OBJECT
public:
    IOptionManager(QObject *parent = 0) : IManager(parent) {}
    virtual void addFactory(IOptionFactory *factory) = 0;
    virtual void removeFactory(IOptionFactory *factory) = 0;
    virtual QList<IOptionFactory*> factoryList() const = 0;
public slots:
    virtual void exec() = 0;
signals:
    void applyOption(QString);
};

class IProjectManager : public IManager
{
    Q_OBJECT
public:
    IProjectManager(QObject *parent = 0) : IManager(parent) {}
    virtual IFolderProject *openFolder(const QString &folderPath) = 0;
    virtual IProject *openProject(const QString &fileName, const QString &mimeType) = 0;
    virtual void addFactory(IProjectFactory *factory) = 0;
    virtual void removeFactory(IProjectFactory *factory) = 0;
    virtual QList<IProjectFactory*> factoryList() const = 0;
    virtual QStringList mimeTypeList() const = 0;
    virtual void setCurrentProject(IProject *project) = 0;
    virtual IProject *currentProject() const = 0;
    virtual QList<IEditor*> editorList(IProject *project) const = 0;
    virtual void addImportAction(QAction *act) = 0;
    virtual QWidget *widget() = 0;
public slots:
    virtual void saveProject(IProject *project = 0) = 0;
    virtual void closeProject(IProject *project = 0) = 0;
    virtual void openSchemeDialog(const QString &scheme) = 0;
signals:
    void currentProjectChanged(LiteApi::IProject *project);
    void projectAboutToClose(LiteApi::IProject *project);
};

class IToolWindowManager : public IManager
{
    Q_OBJECT
public:
    IToolWindowManager(QObject *parent = 0) : IManager(parent) {}
    virtual QAction *addToolWindow(Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split, QList<QAction*> widgetActions = QList<QAction*>()) = 0;
    virtual void moveToolWindow(Qt::DockWidgetArea area,QAction *action, bool split) = 0;
    virtual QAction *findToolWindow(QWidget *widget) = 0;
    virtual void removeToolWindow(QAction *action) = 0;
    virtual void removeToolWindow(QWidget *widget) = 0;
};

class IDockManager : public IManager
{
    Q_OBJECT
public:
    IDockManager(QObject *parent = 0) : IManager(parent) {}
    virtual QWidget *widget() = 0;
    virtual QDockWidget *addDock(QWidget *widget,
                         const QString &title,
                         Qt::DockWidgetArea ares = Qt::LeftDockWidgetArea,
                         Qt::DockWidgetAreas alowedAreas = Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea,
                         QDockWidget::DockWidgetFeatures  features = QDockWidget::AllDockWidgetFeatures) = 0;
    virtual void removeDock(QWidget *widget) = 0;
    virtual void showDock(QWidget *widget) = 0;
    virtual void hideDock(QWidget *widget) = 0;
    virtual QDockWidget *dockWidget(QWidget *widget) = 0;
};

enum VIEWMENU_ACTION_POS
{
    ViewMenuToolBarPos = 1,
    ViewMenuToolWindowPos,
    ViewMenuBrowserPos,
    ViewMenuLastPos
};

class IActionManager : public IManager
{
    Q_OBJECT
public:
    IActionManager(QObject *parent = 0) : IManager(parent) {}
    virtual QMenu *insertMenu(const QString &id, const QString &title, const QString &idBefore = QString()) = 0;
    virtual QMenu *loadMenu(const QString &id) = 0;
    virtual void removeMenu(QMenu *menu) = 0;
    virtual QList<QString>  menuList() const = 0;
    virtual QToolBar *insertToolBar(const QString &id, const QString &title, const QString &before = QString()) = 0;
    virtual void insertToolBar(QToolBar *toolBar,const QString &before = QString()) = 0;
    virtual QToolBar *loadToolBar(const QString &id) = 0;
    virtual void removeToolBar(QToolBar* toolBar) = 0;
    virtual QList<QString> toolBarList() const = 0;
    virtual void insertViewMenu(VIEWMENU_ACTION_POS pos, QAction *act) = 0;
};

class IGoProxy : public QObject
{
    Q_OBJECT
public:
    IGoProxy(QObject *parent) : QObject(parent) {}
    virtual bool isValid() const = 0;
signals:
    void error(const QByteArray &id, int err);
    void done(const QByteArray &id, const QByteArray &args);
public slots:
    virtual void call(const QByteArray &id, const QByteArray &args = QByteArray()) = 0;
};

class IPlugin;
class IApplication : public IObject
{
    Q_OBJECT
public:
    virtual ~IApplication() {}
    virtual IApplication    *newInstance(bool loadSession) = 0;
    virtual bool hasGoProxy() const = 0;
    virtual IGoProxy *createGoProxy(QObject *parent) = 0;
    virtual IProjectManager *projectManager() = 0;
    virtual IEditorManager  *editorManager() = 0;
    virtual IFileManager    *fileManager() = 0;
    virtual IActionManager  *actionManager() = 0;
    virtual IMimeTypeManager *mimeTypeManager() = 0;
    virtual IOptionManager  *optionManager() = 0;
    virtual IToolWindowManager *toolWindowManager() = 0;
    virtual IHtmlWidgetManager *htmlWidgetManager() = 0;

    virtual QMainWindow *mainWindow() const = 0;
    virtual QSettings *settings() = 0;
    virtual QMap<QString,QVariant> &globalCookie() = 0; //global cookie

    virtual QString resourcePath() const = 0;
    virtual QString applicationPath() const = 0;
    virtual QString pluginPath() const = 0;
    virtual QString storagePath() const = 0;

    virtual QList<IPlugin*> pluginList() const = 0;

    virtual void loadSession(const QString &name) = 0;
    virtual void saveSession(const QString &name) = 0;
    virtual void loadState() = 0;
    virtual void saveState() = 0;

    virtual void appendLog(const QString &model, const QString &log, bool error = false) = 0;
    virtual void sendBroadcast(const QString &module, const QString &id, const QVariant &param = QVariant()) = 0;
signals:
    void loaded();
    void broadcast(QString,QString,QVariant);
};

class PluginInfo
{
public:
    PluginInfo() : m_mustLoad(false)
    {}
    virtual ~PluginInfo() {}
    QString anchor() const { return m_anchor; }
    QString info() const { return m_info; }
    QString id() const { return m_id; }
    QString name() const { return m_name; }
    QString ver() const { return m_ver; }
    QStringList dependList() const { return m_dependList; }
    QString filePath() const { return m_filePath; }
    bool isMustLoad() const { return m_mustLoad; }
    void setAnchor(const QString &anchor) { m_anchor = anchor; }
    void setInfo(const QString &info) { m_info = info; }
    void setId(const QString &id) { m_id = id.toLower(); }
    void setName(const QString &name) { m_name = name; }
    void setVer(const QString &ver) { m_ver = ver; }
    void setFilePath(const QString &path) { m_filePath = path; }
    void setDependList(const QStringList &dependList) { m_dependList = dependList; }
    void appendDepend(const QString &depend) { m_dependList.append(depend); }
    void setMustLoad(bool b) { m_mustLoad = b; }
protected:
    QString m_anchor;
    QString m_info;
    QString m_id;
    QString m_name;
    QString m_filePath;
    QString m_ver;
    QStringList m_dependList;
    bool m_mustLoad;
};

class IPlugin : public IObject
{
    Q_OBJECT
public:
    virtual bool load(LiteApi::IApplication *app) = 0;
};

class IPluginFactory : public QObject
{
public:
    virtual ~IPluginFactory() {}
    virtual QString id() const = 0;
    virtual PluginInfo *info() const = 0;    
    virtual QStringList dependPluginList() const = 0;
    virtual void setFilePath(const QString &path) = 0;
    virtual QString filePath() const = 0;
    virtual IPlugin *createPlugin() = 0;
};

class IPluginFactoryImpl : public IPluginFactory
{
public:
    IPluginFactoryImpl() : m_info(new PluginInfo)
    {
    }
    virtual ~IPluginFactoryImpl()
    {
        delete m_info;
    }
    virtual QString id() const
    {
        return m_info->id();
    }
    virtual PluginInfo *info() const
    {
        return m_info;
    }
    virtual QStringList dependPluginList() const{
        return m_info->dependList();
    }
    virtual void setFilePath(const QString &path)
    {
        m_info->setFilePath(path);
    }
    virtual QString filePath() const
    {
        return m_info->filePath();
    }
protected:
    PluginInfo *m_info;
};

template <typename T>
class PluginFactoryT : public IPluginFactoryImpl
{
public:
    virtual IPlugin *createPlugin()
    {
        return new T;
    }
};

inline void gotoLine(IApplication *app, const QString &fileName, int line, int col) {
    app->editorManager()->addNavigationHistory();
    IEditor *edit = app->fileManager()->openEditor(fileName);
    ITextEditor *textEdit = getTextEditor(edit);
    if (textEdit) {
        textEdit->gotoLine(line,col);
    }
}

inline QSize getToolBarIconSize() {
    const QSettings settings(QSettings::IniFormat,QSettings::UserScope,"liteide","liteide");
    int v = settings.value("General/ToolBarIconSize",0).toInt();
    switch (v) {
    case 0:
        return QSize(16,16);
    case 1:
        return QSize(18,18);
    case 2:
        return QSize(20,20);
    case 3:
        return QSize(22,22);
    case 4:
        return QSize(24,24);
    }
    return QSize(16,16);
}

} //namespace LiteApi

Q_DECLARE_INTERFACE(LiteApi::IPluginFactory,"LiteApi.IPluginFactory/X15.1")


#endif //__LITEAPI_H__

