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

    LiteApi::IEditorMarkTypeManager *manager = LiteApi::getEditorMarkTypeManager(app);
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
    connect(mark,SIGNAL(markListChanged(int)),this,SLOT(markListChanged(int)));
    m_editorMarkList.push_back(mark);

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
    foreach(QString bp, m_liteApp->settings()->value(key).toStringList()) {
        int i = bp.toInt(&ok);
        if (ok) {
            mark->addMark(i,BookMarkType);
        }
    }
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
    m_editorMarkList.removeAll(mark);
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
}

void BookmarkManager::markListChanged(int type)
{
    //qDebug() << type;
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
