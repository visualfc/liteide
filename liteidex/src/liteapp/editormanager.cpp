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
// Module: editormanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "editormanager.h"
#include "liteapp_global.h"
#include <QFileInfo>
#include <QTabWidget>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QStackedWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QFileDialog>
#include <QEvent>
#include <QKeyEvent>
#include <QTabBar>
#include <QApplication>
#include <QStatusBar>
#include <QToolButton>
#include <QComboBox>
#include <QTextCodec>
#include <QClipboard>
#include <QLabel>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QDebug>
#include "litetabwidget.h"
#include "fileutil/fileutil.h"
#include "liteapp.h"
#include "openeditorswidget.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


EditorManager::~EditorManager()
{
    m_liteApp->settings()->setValue(LITEAPP_SHOWEDITTOOLBAR,m_editToolbarAct->isChecked());
    delete m_tabContextFileMenu;
    delete m_tabContextNofileMenu;
    delete m_editorTabWidget;
    delete m_listMenu;
    delete m_openEditorWidget;
    m_browserActionMap.clear();
    if (!m_nullMenu->parent()) {
        delete m_nullMenu;
    }
}

bool EditorManager::initWithApp(IApplication *app)
{
    if (!IEditorManager::initWithApp(app)) {
        return false;
    }
    m_updateMenuInFocus = true;
    m_nullMenu = new QMenu;
    m_nullMenu->setEnabled(false);
    m_currentNavigationHistoryPosition = 0;
    m_colorStyleScheme = new ColorStyleScheme(this);

    m_maxEditorCount = m_liteApp->settings()->value(LITEAPP_MAXEDITORCOUNT,64).toInt();

    applyOption(OPTION_LITEAPP);

    m_widget = new QWidget;
    //create editor tab widget
    m_editorTabWidget = new LiteTabWidget(LiteApi::getToolBarIconSize(m_liteApp));

    //create list menu
    m_listMenu = new QMenu;
    m_listGroup = new QActionGroup(this);
    m_editorTabWidget->setListMenu(m_listMenu);
    connect(m_listMenu,SIGNAL(aboutToShow()),this,SLOT(aboutToShowListMenu()));
    connect(m_listGroup,SIGNAL(triggered(QAction*)),this,SLOT(triggeredListAction(QAction*)));

    //create editor model
    m_editorModel = new QStandardItemModel(this);

    //create opne editor for model
    m_openEditorWidget = new OpenEditorsWidget(app);
    m_openEditorWidget->setEditorModel(m_editorModel);

    m_liteApp->toolWindowManager()->addToolWindow(Qt::LeftDockWidgetArea,m_openEditorWidget,"OpenEditor",tr("OpenEditor"),true);

    m_editorTabWidget->tabBar()->setTabsClosable(m_liteApp->settings()->value(LITEAPP_EDITTABSCLOSABLE,true).toBool());
    m_editorTabWidget->tabBar()->setEnableWheel(m_liteApp->settings()->value(LITEAPP_EDITTABSENABLEWHELL,true).toBool());

    //m_editorTabWidget->tabBar()->setIconSize(LiteApi::getToolBarIconSize());
#ifdef Q_OS_MAC
#if QT_VERSION >= 0x050900
    m_editorTabWidget->tabBar()->setStyleSheet("QTabBar::close-button:hover,QTabBar::close-button:selected {margin:0px; image: url(:/images/closetool.png); subcontrol-position: left;}");
#endif
#endif

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

//    QToolBar *toolBar = m_editorTabWidget->headerToolBar();
//    toolBar->setObjectName("toolbar/tabs");
//    m_liteApp->actionManager()->insertToolBar(toolBar);
//    m_editorTabWidget->headerToolBar()->setAllowedAreas(Qt::TopToolBarArea|Qt::BottomToolBarArea);
    mainLayout->addWidget(m_editorTabWidget->tabBarWidget());
    mainLayout->addWidget(m_editorTabWidget->stackedWidget());
    m_widget->setLayout(mainLayout);

    connect(m_editorTabWidget,SIGNAL(currentChanged(int)),this,SLOT(editorTabChanged(int)));
    connect(m_editorTabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(editorTabCloseRequested(int)));
    connect(m_editorTabWidget,SIGNAL(tabAddRequest()),this,SIGNAL(tabAddRequest()));

    m_editorTabWidget->stackedWidget()->installEventFilter(this);
    m_editorTabWidget->tabBar()->installEventFilter(this);

    m_tabContextFileMenu = new QMenu;
    m_tabContextNofileMenu = new QMenu;
    m_tabContextIndex = -1;
    QAction *closeAct = new QAction(tr("Close"),this);
    closeAct->setShortcut(QKeySequence("Ctrl+W"));
    QAction *closeOthersAct = new QAction(tr("Close Others"),this);
    QAction *closeAllAct = new QAction(tr("Close All"),this);
    QAction *closeLeftAct = new QAction(tr("Close Left Tabs"),this);
    QAction *closeRightAct = new QAction(tr("Close Right Tabs"),this);
    QAction *closeSameFolderFiles = new QAction(tr("Close Files in Same Folder"),this);
    QAction *closeOtherFolderFiles = new QAction(tr("Close Files in Other Folders"),this);
    QAction *copyPathToClipboard = new QAction(tr("Copy Full Path to Clipboard"),this);

#if defined(Q_OS_WIN)
    QAction *showInExplorer = new QAction(tr("Show in Explorer"),this);    
#elif defined(Q_OS_MAC)
    QAction *showInExplorer = new QAction(tr("Show in Finder"),this);
#else
    QAction *showInExplorer = new QAction(tr("Show Containing Folder"),this);
#endif

#ifdef Q_OS_WIN
    QAction *openInShell = new QAction(tr("Open Command Prompt Here"),this);
#else
    QAction *openInShell = new QAction(tr("Open Terminal Here"),this);
#endif


    QAction *moveToAct = new QAction(tr("Move to New Window"),this);

    m_tabContextFileMenu->addAction(closeAct);
    m_tabContextFileMenu->addAction(closeOthersAct);
    m_tabContextFileMenu->addAction(closeLeftAct);
    m_tabContextFileMenu->addAction(closeRightAct);
    m_tabContextFileMenu->addAction(closeAllAct);
    m_tabContextFileMenu->addSeparator();
    m_tabContextFileMenu->addAction(closeSameFolderFiles);
    m_tabContextFileMenu->addAction(closeOtherFolderFiles);
    m_tabContextFileMenu->addSeparator();
    m_tabContextFileMenu->addAction(copyPathToClipboard);
    m_tabContextFileMenu->addAction(showInExplorer);
    m_tabContextFileMenu->addAction(openInShell);
    m_tabContextFileMenu->addSeparator();
    m_tabContextFileMenu->addAction(moveToAct);

    m_tabContextNofileMenu->addAction(closeAct);
    m_tabContextNofileMenu->addAction(closeOthersAct);
    m_tabContextNofileMenu->addAction(closeLeftAct);
    m_tabContextNofileMenu->addAction(closeRightAct);
    m_tabContextNofileMenu->addAction(closeAllAct);

    connect(closeAct,SIGNAL(triggered()),this,SLOT(tabContextClose()));
    connect(closeOthersAct,SIGNAL(triggered()),this,SLOT(tabContextCloseOthers()));
    connect(closeLeftAct,SIGNAL(triggered()),this,SLOT(tabContextCloseLefts()));
    connect(closeRightAct,SIGNAL(triggered()),this,SLOT(tabContextCloseRights()));
    connect(closeAllAct,SIGNAL(triggered()),this,SLOT(tabContextCloseAll()));
    connect(closeSameFolderFiles,SIGNAL(triggered()),this,SLOT(tabContextCloseSameFolderFiles()));
    connect(closeOtherFolderFiles,SIGNAL(triggered()),this,SLOT(tabContextCloseOtherFolderFiles()));
    connect(copyPathToClipboard,SIGNAL(triggered()),this,SLOT(tabContextCopyPathToClipboard()));
    connect(showInExplorer,SIGNAL(triggered()),this,SLOT(tabContextShowInExplorer()));
    connect(openInShell,SIGNAL(triggered()),this,SLOT(tabContextOpenInShell()));
    connect(moveToAct,SIGNAL(triggered()),this,SLOT(moveToNewWindow()));
    connect(qApp,SIGNAL(focusChanged(QWidget*,QWidget*)),this,SLOT(focusChanged(QWidget*,QWidget*)));

    QStatusBar *bar = m_liteApp->mainWindow()->statusBar();

    m_lineInfo = new QLabel("000:000");
    bar->addPermanentWidget(m_lineInfo);

    LiteApi::IAppIdleTimer *idleTimer = LiteApi::GetAppIdleTimer(m_liteApp);
    if (idleTimer) {
        connect(idleTimer,SIGNAL(appIdle(int)),this,SLOT(appIdle(int)));
    }

    return true;
}

void EditorManager::createActions()
{
    m_editMenu = m_liteApp->actionManager()->loadMenu("menu/edit");
    if (!m_editMenu) {
        m_editMenu = m_liteApp->actionManager()->insertMenu("menu/edit",tr("&Edit"));
    }

    QToolBar *toolBar = m_liteApp->actionManager()->loadToolBar("toolbar/std");

    m_goBackAct = new QAction(tr("Navigate Backward"),this);
    m_goBackAct->setIcon(QIcon("icon:images/backward.png"));
    IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(m_liteApp,"App");
#ifdef Q_OS_MAC
    actionContext->regAction(m_goBackAct,"Backward","Ctrl+Alt+Left");
#else
    actionContext->regAction(m_goBackAct,"Backward","Alt+Left");
#endif

    m_goForwardAct = new QAction(tr("Navigate Forward"),this);
    m_goForwardAct->setIcon(QIcon("icon:images/forward.png"));
#ifdef Q_OS_MAC
    actionContext->regAction(m_goForwardAct,"Forward","Ctrl+Alt+Right");
#else
    actionContext->regAction(m_goForwardAct,"Forward","Alt+Right");
#endif

    m_liteApp->actionManager()->setViewMenuSeparator("sep/nav",true);
    m_liteApp->actionManager()->insertViewMenuAction(m_goBackAct,"sep/nav");
    m_liteApp->actionManager()->insertViewMenuAction(m_goForwardAct,"sep/nav");

    updateNavigatorActions();

    toolBar->addSeparator();
    toolBar->addAction(m_goBackAct);
    toolBar->addAction(m_goForwardAct);

    connect(m_goBackAct,SIGNAL(triggered()),this,SLOT(goBack()));
    connect(m_goForwardAct,SIGNAL(triggered()),this,SLOT(goForward()));

    m_editToolbarAct = new QAction(tr("Edit Toolbar"),this);
    m_editToolbarAct->setCheckable(true);
    m_editToolbarAct->setChecked(m_liteApp->settings()->value(LITEAPP_SHOWEDITTOOLBAR,true).toBool());
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuToolBarPos,m_editToolbarAct);

    connect(m_editToolbarAct,SIGNAL(triggered(bool)),this,SIGNAL(editToolbarVisibleChanged(bool)));
}

QWidget *EditorManager::widget()
{
    return m_widget;
}

void EditorManager::editorTabChanged(int /*index*/)
{
    QWidget *w = m_editorTabWidget->currentWidget();
    if (w == 0) {
        this->setCurrentEditor(0);
    } else {
        IEditor *ed = m_widgetEditorMap.value(w,0);
        this->setCurrentEditor(ed);
    }
}

void EditorManager::editorTabCloseRequested(int index)
{
    QWidget *w = m_editorTabWidget->widget(index);
    IEditor *ed = m_widgetEditorMap.value(w,0);
    closeEditor(ed);
}

QList<IEditor*> EditorManager::sortedEditorList() const
{
    QList<IEditor*> editorList;
    foreach (QWidget *w,m_editorTabWidget->widgetList()) {
        IEditor *ed = m_widgetEditorMap.value(w);
        if (ed) {
            editorList << ed;
        }
    }
    return editorList;
}

class EditorItem : public QStandardItem
{
public:
    LiteApi::IEditor    *editor;
};

void EditorManager::addEditor(IEditor *editor)
{
    QWidget *w = m_widgetEditorMap.key(editor,0);
    if (w == 0) {
        w = editor->widget();
        if (w == 0) {
            return;
        }
        m_editorTabWidget->addTab(w,QIcon(),editor->name(),editor->filePath());
        m_widgetEditorMap.insert(w,editor);
        emit editorCreated(editor);
        connect(editor,SIGNAL(modificationChanged(bool)),this,SLOT(modificationChanged(bool)));
        emit editToolbarVisibleChanged(m_editToolbarAct->isChecked());
        LiteApi::IEditContext *context = LiteApi::getEditContext(editor);
        if (context) {
            this->addEditContext(context);
        }
        if (!editor->filePath().isEmpty()) {
            QStandardItem *item = new QStandardItem(editor->name());
            item->setToolTip(editor->filePath());
            m_editorModel->appendRow(QList<QStandardItem*>() << item);
        }
    }
}

bool EditorManager::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *e = static_cast<QKeyEvent*>(event);
#ifdef Q_OS_MAC
        if ( (e->modifiers() & Qt::ALT) &&
#else
        if ( (e->modifiers() & Qt::CTRL) &&
#endif
             ( e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab) ) {
            int index = m_editorTabWidget->tabBar()->currentIndex();
            if (e->key() == Qt::Key_Tab) {
                index++;
                if (index >= m_editorTabWidget->tabBar()->count()) {
                    index = 0;
                }
            } else {
                index--;
                if (index < 0) {
                    index = m_editorTabWidget->tabBar()->count()-1;
                }
            }
            m_editorTabWidget->setCurrentIndex(index);
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonDblClick && target == m_editorTabWidget->tabBar()) {
        QMouseEvent *ev = (QMouseEvent*)event;
        if (ev->button() == Qt::LeftButton) {
            emit doubleClickedTab();
        }
    } else if (event->type() == QEvent::MouseButtonPress && target == m_editorTabWidget->tabBar()) {
        QMouseEvent *ev = (QMouseEvent*)event;
        if (ev->button() == Qt::RightButton) {
            m_tabContextIndex = m_editorTabWidget->tabBar()->tabAt(ev->pos());
            if (m_tabContextIndex >= 0) {
                QString filePath = tabContextFilePath();
                if (filePath.isEmpty()) {
                    m_tabContextNofileMenu->popup(ev->globalPos());
                } else {
                    m_tabContextFileMenu->popup(ev->globalPos());
                }
            }
        } else if (ev->button() == Qt::MiddleButton) {
            int index = m_editorTabWidget->tabBar()->tabAt(ev->pos());
            if (index >= 0) {
                editorTabCloseRequested(index);
            }
        }
    }
    return IEditorManager::eventFilter(target,event);
}

QString EditorManager::tabContextFilePath() const
{
    if (m_tabContextIndex < 0) {
        return QString();
    }
    QWidget *w = m_editorTabWidget->widget(m_tabContextIndex);
    IEditor *ed = m_widgetEditorMap.value(w,0);
    if (!ed) {
        return QString();
    }
    return ed->filePath();
}

QAction *EditorManager::registerBrowser(IEditor *editor)
{
    QAction *act = new QAction(editor->name(),this);
    act->setCheckable(true);
    act->setChecked(false);
    connect(act,SIGNAL(toggled(bool)),this,SLOT(toggleBrowserAction(bool)));
    m_browserActionMap.insert(editor,act);
    return act;
}

void EditorManager::activeBrowser(IEditor *editor)
{
    QAction *act = m_browserActionMap.value(editor);
    if (!act) {
        return;
    }
    if (!act->isChecked()) {
        act->toggle();
    }
    setCurrentEditor(editor);
}

bool EditorManager::closeEditor(IEditor *editor)
{
    if (!editor) {
        editor = m_currentEditor;
    }
    if (editor == 0) {
        return false;
    }

    if (editor->isModified() && !editor->isReadOnly()) {
        QString text = QString(tr("Save changes to %1?")).arg(editor->filePath());
        int ret = QMessageBox::question(m_widget,tr("Unsaved Modifications"),text,QMessageBox::Save | QMessageBox::No | QMessageBox::Cancel);
        if (ret == QMessageBox::Cancel) {
            return false;
        } else if (ret == QMessageBox::Save) {
            //cur->save();
            saveEditor(editor);
        }
    }

    if (!editor->filePath().isEmpty()) {
        m_liteApp->settings()->setValue(QString("state_%1").arg(editor->filePath()),editor->saveState());
    }
    int index = m_editorTabWidget->indexOf(editor->widget());
    if (index < 0) {
        return false;
    }
    emit editorAboutToClose(editor);
    m_editorTabWidget->removeTab(index);
    m_widgetEditorMap.remove(editor->widget());
    QString filePath = editor->filePath();
    if (!filePath.isEmpty()) {
        for (int i = 0; i < m_editorModel->rowCount(); i++) {
            QStandardItem *item = m_editorModel->item(i,0);
            if (item->toolTip() == filePath) {
                m_editorModel->removeRow(i);
                break;
            }
        }
    }

    QMapIterator<IEditor*,QAction*> i(m_browserActionMap);
    while (i.hasNext()) {
        i.next();
        if (i.key() == editor) {
            i.value()->blockSignals(true);
            i.value()->setChecked(false);
            i.value()->blockSignals(false);
            return true;
        }
    }
    LiteApi::IEditContext *context = LiteApi::getEditContext(editor);
    if (context) {
        this->removeEditContext(context);
    }
    editor->deleteLater();
    return true;
}

bool EditorManager::saveEditor(IEditor *editor, bool emitAboutSave)
{
    IEditor *cur = 0;
    if (editor) {
        cur = editor;
    } else {
        cur = m_currentEditor;
    }

    if (cur && cur->isModified()) {
        if (emitAboutSave) {
            emit editorAboutToSave(cur);
        }

        if (cur->save()) {
            emit editorSaved(cur);
        } else {
            m_liteApp->appendLog("Editor",QString("Failed to save %1").arg(cur->filePath()),true);
        }

        return true;
    }
    return false;
}

bool EditorManager::saveEditorAs(IEditor *editor)
{
    IEditor *cur = 0;
    if (editor) {
        cur = editor;
    } else {
        cur = m_currentEditor;
    }
    if (cur == 0) {
        return false;
    }
    QString fileName = cur->filePath();
    if (fileName.isEmpty()) {
        return false;
    }

    QFileInfo info(fileName);
    QStringList filter;
    QString ext = info.suffix();
    if (!ext.isEmpty()) {
        filter.append(QString("%1 (*.%1)").arg(ext).arg(ext));
    }
    filter.append(tr("All Files (*)"));
    QString path = info.absolutePath();
    QString saveFileName = QFileDialog::getSaveFileName(m_liteApp->mainWindow(),tr("Save As"),path,filter.join(";;"));
    if (FileUtil::compareFile(fileName,saveFileName,false)) {
        return false;
    }
    if (!cur->saveAs(saveFileName)) {
        return false;
    }
    QWidget *w = m_widgetEditorMap.key(cur,0);
    if(w) {
        int index = m_editorTabWidget->indexOf(w);
        m_editorTabWidget->setTabText(index,cur->name());
    }

    emit currentEditorChanged(cur);
    return true;
}

bool EditorManager::saveAllEditors(bool emitAboutSave)
{
    QList<IEditor*> editorList = m_widgetEditorMap.values();
    foreach (IEditor *editor, editorList) {
        saveEditor(editor,emitAboutSave);
    }
    return true;
}

bool EditorManager::closeAllEditors()
{
    bool bCloseAll = true;

    QMapIterator<QWidget *, IEditor *> i (m_widgetEditorMap);
    while (i.hasNext()) {
        i.next();
        if (!closeEditor(i.value())) {
            bCloseAll = false;
            break;
        }
    }

    return bCloseAll;
}

IEditor *EditorManager::currentEditor() const
{
    return m_currentEditor;
}

void EditorManager::updateEditorMenu(IEditContext *context)
{
    if (context && context->focusMenu()) {
#if defined(Q_OS_OSX)
        // dirty trick to show the correct edit menu at the first time on Mac OS X
        m_editMenu->setEnabled(false);
#endif
        m_editMenu->menuAction()->setMenu(context->focusMenu());
        m_editMenu->setEnabled(true);
    } else {
        m_editMenu->menuAction()->setMenu(m_nullMenu);
        m_editMenu->setEnabled(false);
    }
    if (context && context->focusToolBar()) {
        context->focusToolBar()->setEnabled(true);
    }
}

void EditorManager::setCurrentEditor(IEditor *editor, bool ignoreNavigationHistory)
{
    if (m_currentEditor == editor) {
        return;
    }
    if (editor && !ignoreNavigationHistory) {
        this->addNavigationHistory();
    }
    m_currentEditor = editor;
    this->updateEditInfo("");
    if (editor != 0) {
        m_editorTabWidget->setCurrentWidget(editor->widget());
        editor->onActive();
    }
    if (!m_updateMenuInFocus) {
        IEditContext *context = LiteApi::getEditContext(editor);
        updateEditorMenu(context);
    }

    emit currentEditorChanged(editor);
}

IEditor *EditorManager::findEditor(const QString &fileName, bool canonical) const
{
    QMapIterator<QWidget *, IEditor *> i(m_widgetEditorMap);
    while (i.hasNext()) {
        i.next();
        if (FileUtil::compareFile(i.value()->filePath(),fileName,canonical)) {
            return i.value();
        }
    }
    return 0;
}

QList<IEditor*> EditorManager::editorList() const
{
    return m_widgetEditorMap.values();
}

void EditorManager::addFactory(IEditorFactory *factory)
{
    m_factoryList.append(factory);
}

void EditorManager::removeFactory(IEditorFactory *factory)
{
    m_factoryList.removeOne(factory);
}

QList<IEditorFactory*>  EditorManager::factoryList() const
{
    return m_factoryList;
}

QStringList EditorManager::mimeTypeList() const
{
    QStringList types;
    foreach(IEditorFactory *factory, m_factoryList) {
        types.append(factory->mimeTypes());
    }
    return types;
}

IEditor *EditorManager::openEditor(const QString &fileName, const QString &mimeType)
{
    IEditor *editor = findEditor(fileName,true);
    if (editor) {
        return editor;
    }
    foreach (IEditorFactory *factory, m_factoryList) {
        if (factory->mimeTypes().contains(mimeType)) {
            editor = factory->open(fileName,mimeType);
            if (editor) {
                break;
            }
        }
    }
    if (editor == 0) {
        QString type = "liteide/default.editor";
        foreach (IEditorFactory *factory, m_factoryList) {
            if (factory->mimeTypes().contains(type)) {
                editor = factory->open(fileName,type);
                if (editor) {
                    break;
                }
            }
        }
    }   
    if (editor) {
        addEditor(editor);
        ITextEditor *textEditor = getTextEditor(editor);
        if (textEditor) {
            textEditor->restoreState(m_liteApp->settings()->value(QString("state_%1").arg(editor->filePath())).toByteArray());
        }
        while (m_editorTabWidget->tabBar()->count() > m_maxEditorCount) {
            this->closeEditorForTab(0);
        }
    }
    return editor;
}

void EditorManager::toggleBrowserAction(bool b)
{
    QAction *act = (QAction*)sender();
    if (act) {
        IEditor *editor = m_browserActionMap.key(act,0);
        if (editor) {
            if (b) {
                addEditor(editor);
                setCurrentEditor(editor);
            } else {
                closeEditor(editor);
            }
        }
    }
}

void EditorManager::modificationChanged(bool b)
{
    IEditor *editor = static_cast<IEditor*>(sender());
    if (editor) {
        QString text = editor->name();
        QString filePath = editor->filePath();
        if (b) {
            text += " *";
        }
        int index = m_editorTabWidget->indexOf(editor->widget());
        if (index >= 0) {
            m_editorTabWidget->setTabText(index,text);
        }
        for (int i = 0; i < m_editorModel->rowCount(); i++) {
            QStandardItem *item = m_editorModel->item(i,0);
            if (item->toolTip() == filePath) {
                item->setText(text);
                break;
            }
        }
        emit editorModifyChanged(editor,b);        
    }
}

void EditorManager::addNavigationHistory(IEditor *editor,const QByteArray &saveState)
{
    if (editor && editor != currentEditor()) {
        return; // we only save editor sate for the current editor, when the user interacts
    }

    if (!editor)
        editor = currentEditor();
    if (!editor)
        return;

    QString filePath = editor->filePath();
    if (filePath.isEmpty()) {
        return;
    }

    QByteArray state;
    if (saveState.isNull()) {
        state = editor->saveState();
    } else {
        state = saveState;
    }

    m_currentNavigationHistoryPosition = qMin(m_currentNavigationHistoryPosition, m_navigationHistory.size()); // paranoia    
    if (m_currentNavigationHistoryPosition > 0 && m_currentNavigationHistoryPosition <= m_navigationHistory.size()) {
        EditLocation &prev = m_navigationHistory[m_currentNavigationHistoryPosition-1];
        if (prev.filePath == filePath && prev.state == state) {
            return;
        }
    }
    EditLocation location;
    location.filePath = filePath;
    location.state = state;

    m_navigationHistory.insert(m_currentNavigationHistoryPosition, location);
    ++m_currentNavigationHistoryPosition;

    while (m_navigationHistory.size() >= 100) {
        if (m_currentNavigationHistoryPosition > 50) {
            m_navigationHistory.removeFirst();
            --m_currentNavigationHistoryPosition;
        } else {
            m_navigationHistory.removeLast();
        }
    }
    updateNavigatorActions();
}

void EditorManager::goBack()
{
    updateCurrentPositionInNavigationHistory();
    if (m_currentNavigationHistoryPosition > 0) {
        --m_currentNavigationHistoryPosition;
        EditLocation location = m_navigationHistory.at(m_currentNavigationHistoryPosition);
        IEditor *editor = m_liteApp->fileManager()->openEditor(location.filePath,true,true);
        if (editor) {
            editor->restoreState(location.state);
        } else {
            m_navigationHistory.removeAt(m_currentNavigationHistoryPosition);
        }
    }
    updateNavigatorActions();
}

void EditorManager::goForward()
{
    updateCurrentPositionInNavigationHistory();
    if (m_currentNavigationHistoryPosition >= m_navigationHistory.size()-1)
        return;
    ++m_currentNavigationHistoryPosition;
    EditLocation location = m_navigationHistory.at(m_currentNavigationHistoryPosition);
    IEditor *editor = m_liteApp->fileManager()->openEditor(location.filePath,true,true);
    if (!editor) {
        return;
    }
    editor->restoreState(location.state);
    updateNavigatorActions();
}

void EditorManager::updateNavigatorActions()
{
    m_goBackAct->setEnabled(m_currentNavigationHistoryPosition > 0);
    m_goForwardAct->setEnabled(m_currentNavigationHistoryPosition < m_navigationHistory.size()-1);
}

void EditorManager::cutForwardNavigationHistory()
{
    while (m_currentNavigationHistoryPosition < m_navigationHistory.size() - 1)
        m_navigationHistory.removeLast();
}

void EditorManager::loadColorStyleScheme(const QString &fileName)
{
    if (m_colorStyleScheme->load(fileName)) {
        emit colorStyleSchemeChanged();
    }
}

const ColorStyleScheme *EditorManager::colorStyleScheme() const
{
    return m_colorStyleScheme;
}

void EditorManager::addEditContext(IEditContext *context)
{
    m_editContextMap.insert(context->focusWidget(),context);
}

void EditorManager::removeEditContext(IEditContext *context)
{
    m_editContextMap.remove(context->focusWidget());
}

void EditorManager::updateEditInfo(const QString &info)
{
    m_lineInfo->setText(info);
}

void EditorManager::updateCurrentPositionInNavigationHistory()
{
    IEditor *editor = currentEditor();
    if (!editor)
        return;
    QString filePath = editor->filePath();
    if(filePath.isEmpty()) {
        return;
    }

    EditLocation *location;
    if (m_currentNavigationHistoryPosition < m_navigationHistory.size()) {
        location = &m_navigationHistory[m_currentNavigationHistoryPosition];
    } else {
        m_navigationHistory.append(EditLocation());
        location = &m_navigationHistory[m_navigationHistory.size()-1];
    }
    location->filePath = filePath;
    location->state = editor->saveState();
}

void EditorManager::tabContextClose()
{
    if (m_tabContextIndex < 0) {
        return;
    }
    editorTabCloseRequested(m_tabContextIndex);
}

void EditorManager::tabContextCloseOthers()
{
    if (m_tabContextIndex < 0) {
        return;
    }
    QList<IEditor*> closeList;
    for (int i = 0; i < m_editorTabWidget->tabBar()->count(); i++) {
        if (i != m_tabContextIndex) {
            QWidget *w = m_editorTabWidget->widget(i);
            IEditor *ed = m_widgetEditorMap.value(w,0);
            closeList << ed;
        }
    }
    foreach(IEditor *ed, closeList ) {
        closeEditor(ed);
    }
}

void EditorManager::tabContextCloseLefts()
{
    if (m_tabContextIndex < 0) {
        return;
    }
    QList<IEditor*> closeList;
    for (int i = 0; i < m_tabContextIndex; i++) {
        QWidget *w = m_editorTabWidget->widget(i);
        IEditor *ed = m_widgetEditorMap.value(w,0);
        closeList << ed;
    }
    foreach(IEditor *ed, closeList ) {
        closeEditor(ed);
    }
}

void EditorManager::tabContextCloseRights()
{
    if (m_tabContextIndex < 0) {
        return;
    }
    QList<IEditor*> closeList;
    for (int i = m_tabContextIndex+1; i < m_editorTabWidget->tabBar()->count(); i++) {
        QWidget *w = m_editorTabWidget->widget(i);
        IEditor *ed = m_widgetEditorMap.value(w,0);
        closeList << ed;
    }
    foreach(IEditor *ed, closeList ) {
        closeEditor(ed);
    }
}

void EditorManager::tabContextCloseAll()
{
    closeAllEditors();
}

void EditorManager::tabContextCopyPathToClipboard()
{
    QString filePath = tabContextFilePath();
    if (filePath.isEmpty()) {
        return;
    }
    qApp->clipboard()->setText(QDir::toNativeSeparators(filePath));
}

void EditorManager::tabContextShowInExplorer()
{
    QString filePath = tabContextFilePath();
    if (filePath.isEmpty()) {
        return;
    }
    FileUtil::openInExplorer(filePath);
}

void EditorManager::tabContextOpenInShell()
{
    QString filePath = tabContextFilePath();
    if (filePath.isEmpty()) {
        return;
    }
    FileUtil::openInShell(m_liteApp,filePath);
}

void EditorManager::tabContextCloseOtherFolderFiles()
{
    QString filePath = tabContextFilePath();
    if (filePath.isEmpty()) {
        return;
    }
    QFileInfo info(filePath);
    QString path = info.path();

    QList<IEditor*> closeList;
    for (int i = 0; i < m_editorTabWidget->tabBar()->count(); i++) {
        if (i != m_tabContextIndex) {
            QWidget *w = m_editorTabWidget->widget(i);
            IEditor *ed = m_widgetEditorMap.value(w,0);
            QString filePath = ed->filePath();
            if (filePath.isEmpty()) {
                continue;
            }
            QFileInfo info(filePath);
            if (info.path() != path) {
                closeList << ed;
            }
        }
    }
    foreach(IEditor *ed, closeList ) {
        closeEditor(ed);
    }
}

void EditorManager::tabContextCloseSameFolderFiles()
{
    if (m_tabContextIndex < 0) {
        return;
    }
    QWidget *w = m_editorTabWidget->widget(m_tabContextIndex);
    IEditor *ed = m_widgetEditorMap.value(w,0);
    if (!ed) {
        return;
    }
    QString filePath = ed->filePath();
    if (filePath.isEmpty()) {
        return;
    }
    QFileInfo info(filePath);
    QString path = info.path();

    QList<IEditor*> closeList;
    closeList << ed;
    for (int i = 0; i < m_editorTabWidget->tabBar()->count(); i++) {
        if (i != m_tabContextIndex) {
            QWidget *w = m_editorTabWidget->widget(i);
            IEditor *ed = m_widgetEditorMap.value(w,0);
            QString filePath = ed->filePath();
            if (filePath.isEmpty()) {
                continue;
            }
            QFileInfo info(filePath);
            if (info.path() == path) {
                closeList << ed;
            }
        }
    }
    foreach(IEditor *ed, closeList ) {
        closeEditor(ed);
    }
}

void EditorManager::moveToNewWindow()
{
    if (m_tabContextIndex < 0) {
        return;
    }
    QWidget *w = m_editorTabWidget->widget(m_tabContextIndex);
    IEditor *ed = m_widgetEditorMap.value(w,0);
    if (!ed) {
        return;
    }
    QString filePath = ed->filePath();
    if (filePath.isEmpty()) {
        return;
    }
    QString sessionName = "dir:"+QFileInfo(filePath).dir().dirName();
    IApplication *app = m_liteApp->newInstance(sessionName);
    QFileInfo info(filePath);
    if (app->fileManager()->openEditor(filePath)) {
        this->closeEditor(ed);
        app->fileManager()->addFolderList(info.path());
    }
}

void EditorManager::focusChanged(QWidget *old, QWidget *now)
{
    if (!m_updateMenuInFocus) {
        return;
    }
    IEditContext *context = m_editContextMap.value(now);
    updateEditorMenu(context);
    context = m_editContextMap.value(old);
    if (context && context->focusToolBar()) {
        context->focusToolBar()->setEnabled(false);
    }
}

void EditorManager::aboutToShowListMenu()
{
    m_listMenu->clear();
    QList<QAction*> actions = m_listGroup->actions();
    qDeleteAll(actions);

    foreach (QWidget *widget, m_editorTabWidget->widgetList()) {
        LiteApi::IEditor *editor = m_widgetEditorMap.value(widget);
        if (!editor) {
            continue;
        }
        QAction *act = new QAction(editor->name()+"\t"+editor->filePath(),m_listGroup);
        act->setCheckable(true);
        act->setToolTip(editor->filePath());
        m_listGroup->addAction(act);
        if (m_currentEditor == editor) {
            act->setChecked(true);
        }
    }
    m_listMenu->addActions(m_listGroup->actions());
}

void EditorManager::triggeredListAction(QAction *act)
{
    int index = m_listGroup->actions().indexOf(act);
    if (index < 0) {
        return;
    }
    m_editorTabWidget->setCurrentIndex(index);
}

void EditorManager::applyOption(QString id)
{
    if (id != OPTION_LITEAPP) {
        return;
    }
    m_isAutoIdleSaveDocuments = m_liteApp->settings()->value(LITEAPP_AUTOIDLESAVEDOCUMENTS,false).toBool();
    m_autoIdleSaveDocumentsTime = m_liteApp->settings()->value(LITEAPP_AUTOIDLESAVEDOCUMENTS_TIME,3).toInt();
    if (m_autoIdleSaveDocumentsTime < 1) {
        m_autoIdleSaveDocumentsTime = 1;
    }
    m_maxEditorCount = m_liteApp->settings()->value(LITEAPP_MAXEDITORCOUNT,64).toInt();
}

void EditorManager::appIdle(int sec)
{    
    if (m_isAutoIdleSaveDocuments) {
        if (sec == m_autoIdleSaveDocumentsTime) {
            this->saveAllEditors(false);
        }
    }
}

void EditorManager::closeEditorForTab(int index)
{
    QWidget *w = m_editorTabWidget->widget(index);
    IEditor *ed = m_widgetEditorMap.value(w,0);
    closeEditor(ed);
}

