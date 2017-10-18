/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE Team. All rights reserved.
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
// Module: bookmarkmanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "bookmarkmanager.h"
#include <QStandardItemModel>
#include <QTreeView>
#include <QHeaderView>
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

BookmarkManager::BookmarkManager(QObject *parent)
    : LiteApi::IManager(parent)
{
}

bool BookmarkManager::initWithApp(LiteApi::IApplication *app)
{
    if (!IManager::initWithApp(app)) {
        return false;
    }

    LiteApi::IEditorMarkManager *manager = LiteApi::getEditorMarkManager(app);
    if (!manager) {
        return false;
    }
    manager->registerMark(BookMarkType,QIcon("icon:bookmarks/images/bookmark16.png"));

    m_toggleBookmarkAct = new QAction(tr("Toggle Bookmark"),this);

    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"Bookmarks");
#ifdef Q_OS_MAC
    actionContext->regAction(m_toggleBookmarkAct,"ToggleBookmark","Meta+M");
#else
    actionContext->regAction(m_toggleBookmarkAct,"ToggleBookmark","Ctrl+M");
#endif

    connect(m_toggleBookmarkAct,SIGNAL(triggered()),this,SLOT(toggledBookmark()));

    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(editorAboutToClose(LiteApi::IEditor*)),this,SLOT(editorAboutToClose(LiteApi::IEditor*)));
    connect(manager,SIGNAL(editorMarkListChanged(LiteApi::IEditorMark*,int)),this,SLOT(editorMarkListChanged(LiteApi::IEditorMark*,int)));
    connect(manager,SIGNAL(editorMarkNodeCreated(LiteApi::IEditorMark*,LiteApi::IEditorMarkNode*)),this,SLOT(editorMarkNodeCreated(LiteApi::IEditorMark*,LiteApi::IEditorMarkNode*)));
    connect(manager,SIGNAL(editorMarkNodeRemoved(LiteApi::IEditorMark*,LiteApi::IEditorMarkNode*)),this,SLOT(editorMarkNodeRemoved(LiteApi::IEditorMark*,LiteApi::IEditorMarkNode*)));
    connect(manager,SIGNAL(editorMarkNodeChanged(LiteApi::IEditorMark*,LiteApi::IEditorMarkNode*)),this,SLOT(editorMarkNodeChanged(LiteApi::IEditorMark*,LiteApi::IEditorMarkNode*)));

    m_treeView = new SymbolTreeView(0,false);
    m_treeView->setHeaderHidden(true);
    m_treeView->setEditTriggers(QTreeView::NoEditTriggers);
    m_treeView->setRootIsDecorated(false);

    m_bookmarkModel = new BookmarkModel(this);
    m_proxyModel = new BookmarkSortProxyModel(this);
    m_proxyModel->setSourceModel(m_bookmarkModel);
    //m_proxyModel->sort(0);

    m_treeView->setModel(m_bookmarkModel);

    m_treeView->setItemDelegate(new BookmarkDelegate(this));
    m_treeView->setFrameStyle(QFrame::NoFrame);
    m_treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

 //   m_treeView->setFocusPolicy(Qt::NoFocus);
//    m_treeView->setSelectionModel(manager->selectionModel());
//    m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
//    m_treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
//    m_treeView->setDragEnabled(true);
//    m_treeView->setDragDropMode(QAbstractItemView::DragOnly);


    m_liteApp->toolWindowManager()->addToolWindow(Qt::LeftDockWidgetArea,m_treeView,"Bookmarks",tr("Bookmarks"),true);

    connect(m_treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(gotoBookmark(QModelIndex)));
    connect(m_treeView,SIGNAL(enterKeyPressed(QModelIndex)),this,SLOT(gotoBookmark(QModelIndex)));

    return true;
}

void BookmarkManager::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    LiteApi::IEditorMark *mark = LiteApi::getEditorMark(editor);
    if (!mark) {
        return;
    }

    QMenu *menu = LiteApi::getEditMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_toggleBookmarkAct);
    }
    menu = LiteApi::getContextMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_toggleBookmarkAct);
    }
    bool ok;
    QString key = QString("bookmarks/%1").arg(editor->filePath());
    QList<int> bpList;
    foreach(QString bp, m_liteApp->settings()->value(key).toStringList()) {
        int i = bp.toInt(&ok);
        if (ok) {
            bpList << i;
        }
    }
    mark->addMarkList(bpList,BookMarkType);
}

void BookmarkManager::editorAboutToClose(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    LiteApi::IEditorMark *mark = LiteApi::getEditorMark(editor);
    if (!mark) {
        return;
    }
    QList<int> bpList = mark->markLinesByType(BookMarkType);
    QStringList save;
    foreach(int bp, bpList) {
        save.append(QString("%1").arg(bp));
    }
    QString key = QString("bookmarks/%1").arg(editor->filePath());
    if (save.isEmpty()) {
        m_liteApp->settings()->remove(key);
    } else {
        m_liteApp->settings()->setValue(key,save);
    }
    mark->removeMarkList(bpList,BookMarkType);
}

void BookmarkManager::toggledBookmark()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    LiteApi::IEditorMark *mark = LiteApi::getEditorMark(editor);
    if (!mark) {
        return;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    int line = textEditor->line();
    QList<int> types = mark->markTypesByLine(line);
    if (types.contains(BookMarkType)) {
        mark->removeMark(line,BookMarkType);
    } else {
        mark->addMark(line,BookMarkType);
    }
}

void BookmarkManager::editorMarkListChanged(LiteApi::IEditorMark *mark, int type)
{
    if (type != BookMarkType) {
        return;
    }
}

void BookmarkManager::editorMarkNodeCreated(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node)
{
    if (node->type() != BookMarkType) {
        return;
    }
//    MarkNodeItem *item = new MarkNodeItem();
//    item->mark = mark;
//    item->node = node;
//    item->setText(QString("%1\t%2").arg(mark->fileName()).arg(node->lineNumber()));
//    item->setToolTip(QString("%1").arg(mark->filePath()));
//    m_bookmarkModel->appendRow(item);
    m_bookmarkModel->addNode(mark,node);
}

void BookmarkManager::editorMarkNodeRemoved(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node)
{
    if (node->type() != BookMarkType) {
        return;
    }
    m_bookmarkModel->removeNode(mark,node);
//    for (int i = 0; i < m_bookmarkModel->rowCount(); i++) {
//        MarkNodeItem *item = (MarkNodeItem*)m_bookmarkModel->item(i,0);
//        if (item->mark == mark && item->node == node) {
//            m_bookmarkModel->removeRow(i);
//            break;
//        }
//    }
}

void BookmarkManager::editorMarkNodeChanged(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node)
{
    if (node->type() != BookMarkType) {
        return;
    }
    m_bookmarkModel->updateNode(mark,node);
//    for (int i = 0; i < m_bookmarkModel->rowCount(); i++) {
//        MarkNodeItem *item = (MarkNodeItem*)m_bookmarkModel->item(i,0);
//        if (item->mark == mark && item->node == node) {
//            item->setText(QString("%1\t%2").arg(mark->fileName()).arg(node->lineNumber()));
//            break;
//        }
    //    }
}

void BookmarkManager::gotoBookmark(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    QString filePath = index.data(BookmarkModel::FilePath).toString();
    int lineNumber = index.data(BookmarkModel::LineNumber).toInt();
    if (!filePath.isEmpty() && (lineNumber > 0)) {
        LiteApi::gotoLine(m_liteApp,filePath,lineNumber-1,0,true,true);
    }
}

