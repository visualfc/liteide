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
// Module: filtermanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "filtermanager.h"
#include "liteapp_global.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSortFilterProxyModel>
#include <QTreeView>
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

FilterManager::FilterManager()
{

}

FilterManager::~FilterManager()
{
    qDeleteAll(m_filterMap);
}

bool FilterManager::initWithApp(IApplication *app)
{
    if (!IFilterManager::initWithApp(app)) {
        return false;
    }
    m_widget = new QuickOpenWidget(m_liteApp,m_liteApp->mainWindow());
    connect(m_widget,SIGNAL(filterChanged(QString)),this,SLOT(filterChanged(QString)));
    connect(m_widget,SIGNAL(selected()),this,SLOT(selected()));

    m_filesFilter = new FilesFilter(app,this);

    setCurrentFilter(m_filesFilter);

    return true;
}

void FilterManager::createActions()
{
    m_quickOpenAct = new QAction(tr("Quick Open"),this);

    LiteApi::IActionContext *context = m_liteApp->actionManager()->getActionContext(m_liteApp,"App");
    context->regAction(m_quickOpenAct,"QuickOpen","CTRL+P");

    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuBrowserPos,m_quickOpenAct);

    connect(m_quickOpenAct,SIGNAL(triggered(bool)),this,SLOT(showQuickOpen()));
}

void FilterManager::addFilter(const QString &sym, IFilter *filter)
{
    m_filterMap.insert(sym,filter);
}

void FilterManager::removeFilter(IFilter *filter)
{
    QMutableMapIterator<QString,IFilter*> i(m_filterMap);
    while (i.hasNext()) {
        i.next();
        if (i.value() == filter) {
            m_filterMap.remove(i.key());
            break;
        }
    }
}

QList<IFilter *> FilterManager::filterList() const
{
    return m_filterMap.values();
}

void FilterManager::setCurrentFilter(IFilter *filter)
{
    if (m_currentFilter == filter) {
        return;
    }
    m_currentFilter = filter;
    m_sym = m_filterMap.key(filter);
}

IFilter *FilterManager::currentFilter() const
{
    return m_currentFilter;
}

void FilterManager::showQuickOpen()
{
    if (m_currentFilter) {
        m_currentFilter->updateModel();
        m_widget->setModel(m_currentFilter->model());
        m_widget->view()->resizeColumnToContents(0);
    }
    m_widget->editor()->clear();
    m_widget->showPopup();
}

void FilterManager::hideQuickOpen()
{
    m_widget->close();
}

void FilterManager::filterChanged(const QString &text)
{
    if (text.isEmpty()) {
        setCurrentFilter(m_filesFilter);
    }
    if (!m_sym.isEmpty() && !text.startsWith(m_sym)) {
        if (text.size() == 1 || text.endsWith(" ")) {
            QString key = text.trimmed();
            if (key.isEmpty()) {
                return;
            }
            QMapIterator<QString,IFilter*> i(m_filterMap);
            while(i.hasNext()) {
                i.next();
                if (i.key() == key) {
                    this->setCurrentFilter(i.value());
                    return;
                }
            }
        }
    }
    if (m_currentFilter) {
        QModelIndex index = m_currentFilter->filter(text.mid(m_sym.size()));
        m_widget->view()->setCurrentIndex(index);
    }
}

void FilterManager::selected()
{
    QModelIndex index = m_widget->view()->currentIndex();
    if (index.isValid()) {
        if (m_currentFilter) {
            m_currentFilter->selected(index);
        }
    }
    this->hideQuickOpen();
}

FilesFilter::FilesFilter(IApplication *app, QObject *parent)
    : LiteApi::IFilter(parent), m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
}

QString FilesFilter::id() const
{
    return "filter/editor";
}

QAbstractItemModel *FilesFilter::model() const
{
    return m_proxyModel;
}

void updateFolder(QString folder, QStringList extFilter, QStandardItemModel *model, int maxcount)
{
    if (model->rowCount() > maxcount) {
        return;
    }
    QDir dir(folder);
    foreach (QFileInfo info, dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot)) {
        if (info.isDir()) {
            updateFolder(info.filePath(),extFilter,model,maxcount);
        } else if (info.isFile()) {
            if (extFilter.contains(info.suffix())) {
                model->appendRow(QList<QStandardItem*>() << new QStandardItem("f") << new QStandardItem(info.fileName()) << new QStandardItem(info.filePath()));
            }
        }
    }
}

void FilesFilter::updateModel()
{
    m_model->clear();
    m_proxyModel->setFilterFixedString("");
    m_proxyModel->setFilterKeyColumn(2);

    foreach(LiteApi::IEditor *editor, m_liteApp->editorManager()->editorList()) {
        if (editor->filePath().isEmpty()) {
            continue;
        }
        m_model->appendRow(QList<QStandardItem*>() << new QStandardItem("*") << new QStandardItem(editor->name()) << new QStandardItem(editor->filePath()) );
    }

    QStringList extFilter;
    foreach(LiteApi::IMimeType* type, m_liteApp->mimeTypeManager()->mimeTypeList()) {
        foreach (QString ext, type->globPatterns()) {
            if (ext.startsWith(".")) {
                extFilter << ext.mid(1);
            } else if (ext.startsWith("*.")) {
                extFilter << ext.mid(2);
            }
        }
    }

    int maxcount = m_liteApp->settings()->value(LITEAPP_FILESFILTER_MAXCOUNT,100000).toInt();
    foreach(QString folder, m_liteApp->fileManager()->folderList()) {
        updateFolder(folder,extFilter,m_model,maxcount);
    }
    //m_proxyModel->sort(0);
}

QModelIndex FilesFilter::filter(const QString &text)
{
    m_proxyModel->setFilterFixedString(text);

    for(int i = 0; i < m_proxyModel->rowCount(); i++) {
        QModelIndex index = m_proxyModel->index(i,1);
        QString name = index.data().toString();
        if (name.contains(text)) {
            return index;
        }
    }
    return QModelIndex();
}

void FilesFilter::selected(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    QString filePath = m_proxyModel->index(index.row(),2).data().toString();
    m_liteApp->fileManager()->openFile(filePath);
}
