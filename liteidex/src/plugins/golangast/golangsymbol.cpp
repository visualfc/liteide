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
// Module: golangsymbol.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangsymbol.h"
#include "astwidget.h"
#include "golangastitem.h"
#include "golangast_global.h"
#include <liteenvapi/liteenvapi.h>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QProcess>
#include <QFileInfo>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GolangSymbol::GolangSymbol(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IQuickOpen(parent), m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
    m_proxy = new QSortFilterProxyModel(this);
    m_proxy->setSourceModel(m_model);
    m_process = new QProcess(this);
    connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));
    m_matchCase = Qt::CaseInsensitive;
    m_importPath = true;
}

QString GolangSymbol::id() const
{
    return "quickopen/golangsymbol";
}

QString GolangSymbol::info() const
{
    return tr("Quick Open Symbol by Name");
}

QString GolangSymbol::placeholderText() const
{
    return QString();
}

void GolangSymbol::activate()
{
   // m_liteApp->editorManager()->addNavigationHistory();
}

QAbstractItemModel *GolangSymbol::model() const
{
    return m_proxy;
}

QModelIndex GolangSymbol::rootIndex() const
{
    return QModelIndex();
}

void GolangSymbol::updateModel()
{
    m_matchCase = m_liteApp->settings()->value(GOLANGAST_QUICKOPEN_SYMBOL_MATCHCASE,false).toBool()?Qt::CaseSensitive:Qt::CaseInsensitive;
    m_importPath = m_liteApp->settings()->value(GOLANGAST_QUICKOPNE_SYMBOL_IMPORTPATH,true).toBool();

    m_model->clear();
    m_proxy->setFilterCaseSensitivity(m_matchCase);

    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    QString filePath = editor->filePath();
    if (filePath.isEmpty()) {
        return;
    }
    QFileInfo info(filePath);

    QString cmd = LiteApi::getGotools(m_liteApp);
    QStringList args;
    args << "astview";
    args << info.fileName();
    m_process->setWorkingDirectory(info.path());
    m_process->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_process->start(cmd,args);
}

QModelIndex GolangSymbol::filterChanged(const QString &text)
{
    m_proxy->setFilterFixedString(text);

    for (int i = 0; i < m_proxy->rowCount(); i++) {
        QModelIndex index = m_proxy->index(i,0);
        if (index.data().toString().startsWith(text,m_matchCase)) {
            gotoIndex(index,false);
            return index;
        }
    }
    return m_proxy->index(0,0);
}

bool GolangSymbol::gotoIndex(const QModelIndex &index,bool saveHistroy)
{
    QModelIndex i = m_proxy->mapToSource(index);
    if (!i.isValid()) {
        return false;
    }
    GolangAstItem* item = (GolangAstItem*)m_model->itemFromIndex(i);
    if (item->m_posList.isEmpty()) {
         return false;
    }
    AstItemPos pos = item->m_posList.at(0);
    QFileInfo info(QDir(m_process->workingDirectory()),pos.fileName);
    LiteApi::gotoLine(m_liteApp,info.filePath(),pos.line-1,pos.column-1,true,saveHistroy);
    return true;
}

void GolangSymbol::indexChanged(const QModelIndex &index)
{
    gotoIndex(index,false);
}

bool GolangSymbol::selected(const QString &/*text*/, const QModelIndex &index)
{
    return gotoIndex(index,false);
}

void GolangSymbol::cancel()
{

}

void GolangSymbol::finished(int code, QProcess::ExitStatus status)
{
    if (code == 0 && status == QProcess::NormalExit) {
        QByteArray ar = m_process->readAll();
        AstWidget::parserModel(m_model,ar,true,!m_importPath);
        LiteApi::IQuickOpenManager *mgr = LiteApi::getQuickOpenManager(m_liteApp);
        if (mgr) {
            mgr->modelView()->expandAll();
        }
    }
}

GolangSymbolFactory::GolangSymbolFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IQuickOpenAdapter(parent), m_liteApp(app)
{
    m_symbol = new GolangSymbol(app,this);
}

QStringList GolangSymbolFactory::mimeTypes() const
{
    return QStringList() << "text/x-gosrc";
}

LiteApi::IQuickOpen *GolangSymbolFactory::load(const QString &mimeType)
{
    if (mimeType == "text/x-gosrc") {
        return m_symbol;
    }
    return 0;
}
