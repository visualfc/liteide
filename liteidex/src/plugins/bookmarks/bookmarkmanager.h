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
// Module: bookmarkmanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef BOOKMARKMANAGER_H
#define BOOKMARKMANAGER_H

#include "liteapi/liteapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "symboltreeview/symboltreeview.h"
#include "bookmarkmodel.h"

#include <QSortFilterProxyModel>

enum BOOKMARK_EDITOR_MARKTYPE {
    BookMarkType = 1000
};

class QTreeView;
class BookmarkManager : public LiteApi::IManager
{
    Q_OBJECT
public:
    BookmarkManager(QObject *parent = 0);
    virtual bool initWithApp(LiteApi::IApplication *app);
public slots:
    void editorCreated(LiteApi::IEditor *editor);
    void editorAboutToClose(LiteApi::IEditor *editor);
    void toggledBookmark();
    void editorMarkListChanged(LiteApi::IEditorMark *mark, int type);
    void editorMarkNodeCreated(LiteApi::IEditorMark *mark,LiteApi::IEditorMarkNode *node);
    void editorMarkNodeRemoved(LiteApi::IEditorMark *mark,LiteApi::IEditorMarkNode *node);
    void editorMarkNodeChanged(LiteApi::IEditorMark *mark,LiteApi::IEditorMarkNode *node);
    void gotoBookmark(const QModelIndex &index);
protected:
    QAction *m_toggleBookmarkAct;
    BookmarkModel *m_bookmarkModel;
    //BookmarkSortProxyModel *m_proxyModel;
    SymbolTreeView *m_treeView;
};



#endif // BOOKMARKMANAGER_H
