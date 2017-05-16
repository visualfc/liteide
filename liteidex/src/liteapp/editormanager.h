/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: editormanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include "liteapi/liteapi.h"
#include "colorstyle/colorstyle.h"
#include <QPointer>

using namespace LiteApi;

class LiteTabWidget;
class QStackedWidget;
class QToolButton;
class QLabel;
class QStandardItemModel;
class OpenEditorsWidget;
class QTreeView;

struct EditLocation {
    QString filePath;
    QByteArray state;
};

class EditorManager : public IEditorManager
{
    Q_OBJECT
public:
    ~EditorManager();
    virtual bool initWithApp(IApplication *app);
    void createActions();
public:
    virtual IEditor *openEditor(const QString &fileName, const QString &mimeType);
    virtual void addFactory(IEditorFactory *factory);
    virtual void removeFactory(IEditorFactory *factory);
    virtual QList<IEditorFactory*>  factoryList() const;
    virtual QStringList mimeTypeList() const;
public:
    virtual QWidget *widget();
    virtual IEditor *currentEditor() const;
    virtual void setCurrentEditor(IEditor *editor, bool ignoreNavigationHistory = false);
    virtual IEditor *findEditor(const QString &fileName, bool canonical) const;
    virtual QList<IEditor*> editorList() const;
    virtual QAction *registerBrowser(IEditor *editor);
    virtual void activeBrowser(IEditor *editor);
    virtual void addNavigationHistory(IEditor *editor = 0,const QByteArray &saveState = QByteArray());
    virtual void cutForwardNavigationHistory();
    virtual void loadColorStyleScheme(const QString &file);
    virtual const ColorStyleScheme *colorStyleScheme() const;
    virtual void addEditContext(IEditContext *context);
    virtual void removeEditContext(IEditContext *context);
    virtual void updateEditInfo(const QString &info);
protected:
    void addEditor(IEditor *editor);
    bool eventFilter(QObject *target, QEvent *event);
    QString tabContextFilePath() const;
public:
    QList<IEditor*> sortedEditorList() const;
public slots:
    virtual bool saveEditor(IEditor *editor = 0, bool emitAboutSave = true);
    virtual bool saveEditorAs(IEditor *editor = 0);
    virtual bool saveAllEditors(bool emitAboutSave = true);
    virtual bool closeEditor(IEditor *editor = 0);
    virtual bool closeAllEditors();
    void tabContextClose();
    void tabContextCloseOthers();
    void tabContextCloseLefts();
    void tabContextCloseRights();
    void tabContextCloseAll();
    void tabContextCloseOtherFolderFiles();
    void tabContextCloseSameFolderFiles();
    void tabContextCopyPathToClipboard();
    void tabContextShowInExplorer();
    void tabContextOpenInShell();
    void goBack();
    void goForward();
    void updateNavigatorActions();
    void updateCurrentPositionInNavigationHistory();
    void moveToNewWindow();
    void focusChanged(QWidget *old,QWidget *now);
    void aboutToShowListMenu();
    void triggeredListAction(QAction *act);
    void applyOption(QString);
    void appIdle(int sec);
signals:
    void tabAddRequest();
    void doubleClickedTab();
protected slots:
    void editorTabChanged(int);
    void editorTabCloseRequested(int);
    void modificationChanged(bool);
    void toggleBrowserAction(bool);
protected:
    QMenu *m_nullMenu;
    QList<EditLocation> m_navigationHistory;
    int m_currentNavigationHistoryPosition;
    QWidget      *m_widget;
    LiteTabWidget *m_editorTabWidget;
    QMap<QWidget *, IEditor *> m_widgetEditorMap;
    QStandardItemModel         *m_editorModel;
    OpenEditorsWidget          *m_openEditorWidget;
    QPointer<IEditor> m_currentEditor;
    QList<IEditorFactory*>    m_factoryList;
    QMap<IEditor*,QAction*>   m_browserActionMap;
    QMap<QWidget*,IEditContext*> m_editContextMap;
    QAction     *m_goBackAct;
    QAction     *m_goForwardAct;
    QAction     *m_editToolbarAct;
    QMenu       *m_listMenu;
    QActionGroup *m_listGroup;
    QMenu       *m_editMenu;
    QMenu       *m_tabContextFileMenu;
    QMenu       *m_tabContextNofileMenu;
    ColorStyleScheme *m_colorStyleScheme;
    int          m_tabContextIndex;
    QLabel      *m_lineInfo;
    bool         m_isAutoIdleSaveDocuments;
    int          m_autoIdleSaveDocumentsTime;
};

#endif // EDITORMANAGER_H
