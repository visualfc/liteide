/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
// Module: openeditorswidget.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef OPENEDITORSWIDGET_H
#define OPENEDITORSWIDGET_H

#include <liteapi/liteapi.h>
#include <qtc_itemview/opendocumentstreeview.h>
#include <qtc_itemview/proxymodel.h>

class QSortFilterProxyModel;
class OpenEditorsWidget : public Core::OpenDocumentsTreeView
{
    Q_OBJECT
public:
    OpenEditorsWidget(LiteApi::IApplication *app);
    void setEditorModel(QAbstractItemModel *model);
public slots:
    void handleActivated(const QModelIndex &index);
    void updateCurrentItem(LiteApi::IEditor *editor);
    void contextMenuRequested(QPoint);
    void activateEditor(const QModelIndex &index);
    void closeDocument(const QModelIndex &index);
protected:
    LiteApi::IEditor *editorFormIndex(const QModelIndex &index);
    QModelIndex indexFromEditor(LiteApi::IEditor *editor);
protected:
    LiteApi::IApplication *m_liteApp;
    QSortFilterProxyModel      *m_model;
};

#endif // OPENEDITORSWIDGET_H
