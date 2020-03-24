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
// Module: quickopenlines.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "quickopenlines.h"
#include <QStandardItemModel>
#include <QTextDocument>
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

QuickOpenLines::QuickOpenLines(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IQuickOpen(parent), m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
}

QString QuickOpenLines::id() const
{
    return "quickopen/lines";
}

QString QuickOpenLines::info() const
{
    return tr("Go to Line");
}

QString QuickOpenLines::placeholderText() const
{
    return QString();
}

void QuickOpenLines::activate()
{

}

QAbstractItemModel *QuickOpenLines::model() const
{
    return m_model;
}

QModelIndex QuickOpenLines::rootIndex() const
{
    return QModelIndex();
}

bool QuickOpenLines::update(const QString &text)
{
    m_model->clear();
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        m_model->appendRow(new QStandardItem(tr("Open a text file first to go to a line")));
        return false;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) {
        m_model->appendRow(new QStandardItem(tr("Open a text file first to go to a line")));
        return false;
    }
    int maxcount = textEditor->document()->blockCount();
    bool ok = false;
    int line = 0;
    if (!text.isEmpty()) {
        line = text.toInt(&ok);
    }
    if (line <= 0 || line > maxcount) {
        ok = false;
    }
    if (!ok) {
        m_model->appendRow(new QStandardItem(QString(tr("Type a line number between %1 and %2 to navigate to")).arg(1).arg(maxcount)));
        return false;
    }
    m_model->appendRow(new QStandardItem(QString(tr("Go to Line %1")).arg(line)));
    return true;
}

void QuickOpenLines::updateModel()
{
    update("");
}

QModelIndex QuickOpenLines::filterChanged(const QString &text)
{
    update(text);
    return m_model->index(0,0);
}

void QuickOpenLines::indexChanged(const QModelIndex &/*index*/)
{

}

bool QuickOpenLines::selected(const QString &text, const QModelIndex &/*index*/)
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return false;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) {
        return false;
    }
    int maxcount = textEditor->document()->blockCount();
    bool ok = false;
    int line = 0;
    if (!text.isEmpty()) {
        line = text.toInt(&ok);
    }
    if (!ok) {
        return false;
    }
    if (line <= 0 || line > maxcount) {
        return false;
    }
    if (line-1 != textEditor->line()) {
        m_liteApp->editorManager()->addNavigationHistory();
    }
    textEditor->gotoLine(line-1,0,true);
    return true;
}

void QuickOpenLines::cancel()
{

}
