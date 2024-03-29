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
// Module: liteapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEAPI_H
#define LITEAPI_H

#include "liteqt.h"
#include "liteobj.h"
#include "litehtml.h"

#include <QWidget>
#include <QMenu>
#include <QToolBar>
#include <QPlainTextEdit>
#include <QSettings>
#include <QMainWindow>
#include <QDockWidget>
#include <QFlags>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QDesktopServices>
#include <QTextCursor>
#include <QAbstractItemModel>

class ColorStyle;
class ColorStyleScheme;

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

struct TargetInfo1
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
    virtual IApplication* application() {
        return m_liteApp;
    }

protected:
    IApplication *m_liteApp;
};

class IMimeType
{
public:
    virtual ~IMimeType() {}

    virtual QString package() const = 0;
    virtual QString type() const = 0;
    virtual QString scheme() const = 0;
    virtual QString comment() const = 0;
    virtual QString codec() const = 0;
    virtual bool tabToSpace() const = 0;
    virtual int tabWidth() const = 0;
    virtual QStringList globPatterns() const = 0;
    virtual QStringList subClassesOf() const = 0;
    virtual void merge(const IMimeType *mimeType) = 0;
    virtual void setCustomPatterns(const QStringList &custom) = 0;
    virtual QStringList customPatterns() const = 0;
    virtual QStringList allPatterns() const = 0;
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
    virtual QString findPackageByMimeType(const QString &type) const = 0;
    virtual QString findMimeTypeByFile(const QString &fileName) const = 0;
    virtual QString findMimeTypeBySuffix(const QString &suffix) const = 0;
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
    virtual bool loadText(const QString &filePath, const QString &mimeType, QString &outText) = 0;
    virtual bool reloadText(QString &outText) = 0;
    virtual bool saveText(const QString &filePath, const QString &text) = 0;
    virtual bool isReadOnly() const = 0;
    virtual bool isBinary() const = 0;
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
    virtual QString id() const = 0;
    virtual QString displayName() const = 0;
    virtual bool testMimeType(const QString &mimeType) = 0;
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

enum FILESYSTEM_CONTEXT_FLAG {
    FILESYSTEM_ROOT = 0,
    FILESYSTEM_ROOTFOLDER,
    FILESYSTEM_FOLDER,
    FILESYSTEM_FILES
};

class IRecent : public QObject
{
    Q_OBJECT
public:
    IRecent(QObject *parent = 0) : QObject(parent) {}
    virtual QString type() const = 0;
    virtual QString displyType() const = 0;
    virtual void addRecent(const QString &name, int maxRecent) = 0;
    virtual void removeRecent(const QString &name) = 0;
    virtual QStringList recentNameList() = 0;
    virtual void clearRecentNameList() = 0;
    virtual void openRecent(const QString &name) = 0;
};

class ISettingRecent : public IRecent
{
    Q_OBJECT
public:
    ISettingRecent(QSettings *setting, QObject *parent) : IRecent(parent), m_settings(setting)
    {
    }

    virtual void addRecent(const QString &name, int maxRecent)
    {
        QString key = recentKey();
        QStringList files = m_settings->value(key).toStringList();
        files.removeAll(name);
        files.prepend(name);
        while (files.size() > maxRecent) {
            files.removeLast();
        }
        m_settings->setValue(key, files);
    }

    virtual void removeRecent(const QString &name)
    {
        QString key = recentKey();
        QStringList values = m_settings->value(key).toStringList();
        values.removeAll(name);
        m_settings->setValue(key, values);
    }

    virtual QStringList recentNameList()
    {
        QString key = recentKey();
        return m_settings->value(key).toStringList();
    }

    virtual void clearRecentNameList()
    {
        QString key = recentKey();
        m_settings->remove(key);
    }
protected:
    virtual QString recentKey() const
    {
        return QString("Recent1/%1").arg(type());
    }
protected:
    QSettings   *m_settings;
};

class IRecentManager : public IManager
{
    Q_OBJECT
public:
    IRecentManager(QObject *parent = 0) : IManager(parent) {}

    virtual void registerRecent(IRecent *recent) = 0;
    virtual QList<IRecent*> recentList() const = 0;
    virtual IRecent *findRecent(const QString &type) const = 0;
    virtual QStringList recentTypeList() const = 0;

    virtual void addRecent(const QString &name, const QString &type) = 0;
    virtual void removeRecent(const QString &name, const QString &type) = 0;
    virtual QStringList recentNameList(const QString &type) = 0;
    virtual void clearRecentNameList(const QString &type) = 0;
    virtual void openRecent(const QString &name, const QString &type) = 0;
    virtual void updateRecentMenu(const QString &type) = 0;
signals:
    void recentNameListChanged(const QString &type);
};

class IFileManager : public IManager
{
    Q_OBJECT
public:
    IFileManager(QObject *parent = 0) : IManager(parent) {}

    virtual void execFileWizard(const QString &projPath, const QString &filePath, const QString &gopath = QString()) = 0;
    virtual bool openFile(const QString &fileName) = 0;
    virtual IEditor *openEditor(const QString &fileName, bool bActive = true, bool ignoreNavigationHistory = false) = 0;
    virtual IEditor *openEditorByFactory(const QString &fileName, const QString &factoryId, bool bActive = true, bool ignoreNavigationHistory = false) = 0;
    virtual IEditor *createEditor(const QString &contents, const QString &_mimeType) = 0;
    virtual IEditor *createEditor(const QString &fileName) = 0;
    virtual IProject *openProject(const QString &fileName) = 0;
    virtual IProject *openProjectScheme(const QString &fileName, const QString &scheme) = 0;
    virtual bool findProjectTargetInfo(const QString &fileName, QMap<QString,QString>& targetInfo) const = 0;
    //virtual IApplication* openFolderEx(const QString &folder) = 0;
    virtual QStringList folderList() const = 0;
    virtual void setFolderList(const QStringList &folders) = 0;
    virtual void addFolderList(const QString &folders) = 0;
    virtual IApplication* openFolderInNewWindow(const QString &folder) = 0;
    virtual void emitAboutToShowFolderContextMenu(QMenu *menu, LiteApi::FILESYSTEM_CONTEXT_FLAG flag, const QFileInfo &info, const QString &context) = 0;
signals:
    void fileListChanged();
    void fileWizardFinished(const QString &type, const QString &scheme, const QString &location);
    void aboutToShowFolderContextMenu(QMenu *menu, LiteApi::FILESYSTEM_CONTEXT_FLAG flag, const QFileInfo &info,const QString &context);
public slots:
    virtual void newFile() = 0;
    virtual void openFiles() = 0;
    virtual void openFolder() = 0;
    virtual void openEditors() = 0;
    virtual void openProjects() = 0;
};

class IEditContext : public QObject
{
    Q_OBJECT
public:
    IEditContext(QObject *parent) : QObject(parent) {}
    virtual QWidget *focusWidget() const = 0;
    virtual QMenu   *focusMenu() const = 0;
    virtual QToolBar *focusToolBar() const = 0;
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

struct FindOption {
    QString findText;
    bool    useRegexp;
    bool    matchWord;
    bool    matchCase;
    bool    wrapAround;
    bool    backWard;
};

class ITextEditor : public IEditor
{
    Q_OBJECT
public:
    enum PositionOperation {
        Current = 1,
        EndOfLine = 2,
        StartOfLine = 3,
        Anchor = 4,
        EndOfDoc = 5
    };
    ITextEditor(QObject *parent = 0) : IEditor(parent) {}
    virtual int line() const = 0;
    virtual int column() const = 0;
    virtual int utf8Position(bool realFile = false, int pos = -1) const = 0;
    virtual QByteArray utf8Data() const = 0;
    virtual void setLineWrap(bool wrap) = 0;
    virtual bool isLineWrap() const = 0;
    virtual void gotoLine(int blockNumber, int column, bool center = false, int selection = 0) = 0;
    virtual void setFindOption(FindOption *opt) = 0;
    virtual int position(PositionOperation posOp = Current, int at = -1) const = 0;
    virtual QString textAt(int pos, int length) const = 0;
    virtual QRect cursorRect(int pos = -1) const = 0;
    virtual QTextCursor textCursor() const = 0;
    virtual QTextDocument *document() const = 0;
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

inline IEditContext *getEditContext(IObject *obj)
{
    if (obj && obj->extension()) {
        return findExtensionObject<IEditContext*>(obj->extension(),"LiteApi.IEditContext");
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

inline QToolBar *getEditToolBar(IEditor *editor) {
    if (editor && editor->extension()) {
        return findExtensionObject<QToolBar*>(editor->extension(),"LiteApi.QToolBar.Edit");
    }
    return 0;
}

inline QToolBar *getBuildToolBar(IEditor *editor) {
    if (editor && editor->extension()) {
        return findExtensionObject<QToolBar*>(editor->extension(),"LiteApi.QToolBar.Build");
    }
    return 0;
}


class IEditorManager : public IManager
{
    Q_OBJECT
public:
    IEditorManager(QObject *parent = 0) : IManager(parent) {}
    virtual IEditor *openEditor(const QString &fileName, const QString &mimeType) = 0;
    virtual IEditor *openEditorByFactory(const QString &fileName, const QString &mimeType, const QString &factoryId) = 0;
    virtual void addFactory(IEditorFactory *factory) = 0;
    virtual void removeFactory(IEditorFactory *factory) = 0;
    virtual QList<IEditorFactory*> factoryList() const = 0;
    virtual QStringList mimeTypeList() const = 0;
    virtual QWidget *widget() = 0;
    virtual IEditor *currentEditor() const = 0;
    virtual void setCurrentEditor(IEditor *editor, bool ignoreNavigationHistory = false) = 0;
    virtual IEditor *findEditor(const QString &fileName, bool canonical) const = 0;
    virtual QList<IEditor*> editorList() const = 0;
    virtual QAction *registerBrowser(IEditor *editor) = 0;
    virtual void activeBrowser(IEditor *editor) = 0;
    virtual void addNavigationHistory(IEditor *editor = 0,const QByteArray &saveState = QByteArray()) = 0;
    virtual void cutForwardNavigationHistory() = 0;
    virtual void loadColorStyleScheme(const QString &fileName) = 0;
    virtual const ColorStyleScheme *colorStyleScheme() const = 0;
    virtual void addEditContext(IEditContext *context) = 0;
    virtual void removeEditContext(IEditContext *context) = 0;
    virtual void updateEditInfo(const QString &info) = 0;
public slots:
    virtual bool saveEditor(IEditor *editor = 0, bool emitAboutSave = true) = 0;
    virtual bool saveEditorAs(IEditor *editor = 0) = 0;
    virtual bool saveAllEditors(bool emitAboutSave = true) = 0;
    virtual bool closeEditor(IEditor *editor = 0) = 0;
    virtual bool closeAllEditors() = 0;
signals:
    void currentEditorChanged(LiteApi::IEditor *editor);
    void editorCreated(LiteApi::IEditor *editor);
    void editorAboutToClose(LiteApi::IEditor *editor);
    void editorAboutToSave(LiteApi::IEditor *editor);
    void editorSaved(LiteApi::IEditor *editor);
    void editorModifyChanged(LiteApi::IEditor *editor, bool b);
    void colorStyleSchemeChanged();
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

class IWebKitBrowser : public IBrowserEditor
{
    Q_OBJECT
public:
    IWebKitBrowser(QObject *parent = 0)  : IBrowserEditor(parent) {}
    virtual void openUrl(const QUrl &url) = 0;
signals:
    void loadFinished(bool);
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
    virtual void save() = 0;
    virtual void load() = 0;
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
    virtual void emitApplyOption(const QString &mimetype) = 0;
public slots:
    virtual void exec(const QString &mimeType) = 0;
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
    virtual QAction *addToolWindow(Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split,
                                   QList<QAction*> widgetActions = QList<QAction*>(),
                                   QList<QWidget*> widgetList = QList<QWidget*>() ) = 0;
    virtual void moveToolWindow(Qt::DockWidgetArea from, Qt::DockWidgetArea to,QAction *action, bool split) = 0;
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

struct ActionInfo {
    QString label;
    QString defks;
    QString ks;
    bool    standard;
    QList<QKeySequence> keys;
    QAction *action;
};

class IActionContext {
public:
    virtual ~IActionContext() {}
    virtual QString contextName() const = 0;
    virtual void regAction(QAction *act, const QString &id, const QString &defks, bool standard = false) = 0;
    virtual void regAction(QAction *act, const QString &id, const QKeySequence::StandardKey &def) = 0;
    virtual QStringList actionKeys() const = 0;
    virtual ActionInfo *actionInfo(const QString &id) const = 0;
    virtual void setActionShortcuts(const QString &id, const QString &shortcuts) = 0;
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
    virtual void setViewMenuSeparator(const QString &sepid, bool group = false) = 0;
    virtual void insertViewMenuAction(QAction *act, const QString &sepid) = 0;    
    virtual bool insertMenuActions(const QString &idMenu, const QString &idBeforeSep, bool newGroup,  QList<QAction*> &actions) = 0;
    virtual IActionContext *getActionContext(QObject *obj, const QString &name) = 0;
    virtual QStringList actionKeys() const = 0;
    virtual ActionInfo *actionInfo(const QString &id) const = 0;
    virtual void setActionShourtcuts(const QString &id, const QString &shortcuts) = 0;
    virtual QStringList actionContextNameList() const = 0;
    virtual IActionContext *actionContextForName(const QString &name) = 0;
};

class IGoProxy : public QObject
{
    Q_OBJECT
public:
    IGoProxy(QObject *parent) : QObject(parent) {}
    virtual bool isValid() const = 0;
    virtual bool isRunning() const = 0;
    virtual QByteArray commandId() const = 0;
    virtual void writeStdin(const QByteArray &data) = 0;
signals:
    void started();
    void stdoutput(const QByteArray &data);
    void stderror(const QByteArray &data);
    void finished(int code, const QByteArray &msg);
public slots:
    virtual void call(const QByteArray &id, const QByteArray &args = QByteArray()) = 0;
};

class IPlugin;
class IApplication : public IObject
{
    Q_OBJECT
public:
    virtual ~IApplication() {}
    virtual IApplication    *newInstance(const QString &session) = 0;
    virtual QList<IApplication*> instanceList() const = 0;
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
    virtual IRecentManager *recentManager() = 0;

    virtual QMainWindow *mainWindow() const = 0;
    virtual QSettings *settings() = 0;
    virtual QMap<QString,QVariant> &globalCookie() = 0; //global cookie

    virtual QString rootPath() const = 0;
    virtual QString applicationPath() const = 0;
    virtual QString toolPath() const = 0;
    virtual QString resourcePath() const = 0;
    virtual QString pluginPath() const = 0;
    virtual QString storagePath() const = 0;

    virtual QString ideVersion() const = 0;
    virtual QString ideFullName() const = 0;
    virtual QString ideName() const = 0;
    virtual QString ideCopyright() const = 0;

    virtual QList<IPlugin*> pluginList() const = 0;

    virtual void loadSession(const QString &sessioin) = 0;
    virtual void saveSession(const QString &sessioin) = 0;
    virtual QStringList sessionList() const = 0;
    virtual QString currentSession() const = 0;

    virtual void loadState() = 0;
    virtual void saveState() = 0;

    virtual void appendLog(const QString &model, const QString &log, bool error = false) = 0;
    virtual void sendBroadcast(const QString &module, const QString &id, const QVariant &param = QVariant()) = 0;
signals:    
    void loaded();
    void aboutToQuit();
    void key_escape();
    void broadcast(QString,QString,QVariant);
    void sessionListChanged();
};

class PluginInfo
{
public:
    PluginInfo() : m_mustLoad(false)
    {}
    virtual ~PluginInfo() {}
    QString author() const { return m_author; }
    QString info() const { return m_info; }
    QString id() const { return m_id; }
    QString name() const { return m_name; }
    QString ver() const { return m_ver; }
    QStringList dependList() const { return m_dependList; }
    QString filePath() const { return m_filePath; }
    bool isMustLoad() const { return m_mustLoad; }
    void setAuthor(const QString &author) { m_author = author; }
    void setInfo(const QString &info) { m_info = info; }
    void setId(const QString &id) { m_id = id.toLower(); }
    void setName(const QString &name) { m_name = name; }
    void setVer(const QString &ver) { m_ver = ver; }
    void setFilePath(const QString &path) { m_filePath = path; }
    void setDependList(const QStringList &dependList) { m_dependList = dependList; }
    void appendDepend(const QString &depend) { m_dependList.append(depend); }
    void setMustLoad(bool b) { m_mustLoad = b; }
protected:
    bool m_mustLoad;
    QString m_author;
    QString m_info;
    QString m_id;
    QString m_name;
    QString m_filePath;
    QString m_ver;
    QStringList m_dependList;
};

class IPlugin : public IObject
{
    Q_OBJECT
public:
    virtual bool load(LiteApi::IApplication *app) = 0;
};

class IPluginFactory : public QObject
{
    Q_OBJECT
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
    Q_OBJECT
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

class IAppIdleTimer : public QObject
{
    Q_OBJECT
signals:
    void appIdle(int sec);
public:
    virtual void resetTimer() = 0;
};

inline IAppIdleTimer *GetAppIdleTimer(LiteApi::IApplication *app)
{
    return static_cast<IAppIdleTimer*>(app->extension()->findObject("LiteApi.IAppIdleTimer"));
}

inline bool gotoLine(IApplication *app, const QString &fileName, int line, int col, bool forceCenter, bool saveHistory) {
    if (saveHistory) {
        app->editorManager()->addNavigationHistory();
    }
    IEditor *cur = app->editorManager()->currentEditor();
    IEditor *edit = app->fileManager()->openEditor(fileName);
    ITextEditor *textEdit = getTextEditor(edit);
    if (textEdit) {
        if (cur == edit) {
            textEdit->gotoLine(line,col,forceCenter);
        } else {
            textEdit->gotoLine(line,col,true);
        }
        return true;
    }
    return false;
}

inline QSize getToolBarIconSize(LiteApi::IApplication *app) {
    int v = app->settings()->value("General/ToolBarIconSize",0).toInt();
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

inline IWebKitBrowser *getWebKitBrowser(LiteApi::IApplication *app)
{
    return static_cast<IWebKitBrowser*>(app->extension()->findObject("LiteApp.IWebKitBrowser"));
}

inline QString getGotools(LiteApi::IApplication *app)
{
#ifdef Q_OS_WIN
    return app->toolPath()+"/gotools.exe";
#else
    return app->toolPath()+"/gotools";
#endif
}

inline QString findPackageByMimeType(LiteApi::IApplication *app, const QString mimeType)
{
    return app->mimeTypeManager()->findPackageByMimeType(mimeType);
}

} //namespace LiteApi

Q_DECLARE_INTERFACE(LiteApi::IPluginFactory,"LiteApi.IPluginFactory.X37")


#endif //LITEAPI_H

