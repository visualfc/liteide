/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
// Module: astwidget.cpp
// Creator: visualfc <visualfc@gmail.com>



#include "astwidget.h"
#include "golangastitem.h"
#include "golangasticon.h"

#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QFont>
#include <QVBoxLayout>
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


AstWidget::AstWidget(bool outline, LiteApi::IApplication *app, QWidget *parent) :
    QWidget(parent),
    m_bOutline(outline),
    m_liteApp(app)
{
    m_bFirst = true;
    QVBoxLayout *layout = new QVBoxLayout;
    m_tree = new SymbolTreeView;
    m_filterEdit = new Utils::FilterLineEdit(200);

    m_model = new QStandardItemModel(this);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSourceModel(m_model);

    layout->addWidget(m_filterEdit);
    layout->addWidget(m_tree);

    this->setLayout(layout);

    m_tree->setModel(proxyModel);
    m_tree->setExpandsOnDoubleClick(false);

    connect(m_tree,SIGNAL(doubleClicked(QModelIndex)),this,SIGNAL(doubleClicked(QModelIndex)));
    connect(m_filterEdit,SIGNAL(filterChanged(QString)),this,SLOT(filterChanged(QString)));
}

void AstWidget::clear()
{
    m_model->clear();
    m_filterEdit->clear();
}

static QModelIndexList filterModelList(QString filter, QAbstractItemModel *model, QModelIndex parent)
{
    QModelIndexList list;
    for (int i = 0; i < model->rowCount(parent); i++) {
        QModelIndex index = model->index(i,0,parent);
        if (index.data().toString().indexOf(filter,0,Qt::CaseInsensitive) >= 0) {
            list.append(index);
        }
        list.append(filterModelList(filter,model,index));
    }
    return list;
}

void  AstWidget::clearFilter(QModelIndex parent)
{
    for (int i = 0; i < m_model->rowCount(parent); i++) {
        QModelIndex index = m_model->index(i,0,parent);
        clearFilter(index);
        GolangAstItem *item = (GolangAstItem*)m_model->itemFromIndex(index);
        if (!item) {
            continue;
        }
        if (item->tagName().indexOf("+") >= 0) {
            continue;
        }
        QFont font = item->font();
        font.setBold(false);
        item->setFont(font);
    }
}

bool AstWidget::filterModel(QString filter, QModelIndex parent, QModelIndex &first)
{
    bool b = false;
    for (int i = 0; i < proxyModel->rowCount(parent); i++) {
        QModelIndex index = proxyModel->index(i,0,parent);
        GolangAstItem *item = astItemFromIndex(index);
        if (!item) {
            continue;
        }
        if (item->tagName().indexOf("+") < 0) {
            QFont font = item->font();
            if (index.data().toString().indexOf(filter,0,Qt::CaseInsensitive) >= 0) {
                font.setBold(true);
                if (!b) {
                    b = true;
                    if (!first.isValid()) {
                        first = index;
                    }
                }
            } else {
                font.setBold(false);
            }
            item->setFont(font);
        }
        if (filterModel(filter,index,first)) {
            m_tree->expand(index);
            b = true;
        } else {
            //if (!m_bOutline) {
            m_tree->collapse(index);
            //}
        }
    }
    return b;
}

void AstWidget::filterChanged(QString filter)
{
    if (filter.isEmpty()) {
        clearFilter(m_tree->rootIndex());
        //m_tree->expandToDepth(0);
        for(int i = 0; i < proxyModel->rowCount(); i++) {
            QModelIndex index = proxyModel->index(i,0);
            m_tree->expand(index);
            if (m_bOutline) {
                for (int j = 0; j < proxyModel->rowCount(index);j++) {
                    m_tree->expand(proxyModel->index(j,0,index));
                }
            }
        }
    } else {
        QModelIndex first;
        filterModel(filter,m_tree->rootIndex(),first);
        if (first.isValid()) {
            m_tree->scrollTo(first);
        }
    }
}

GolangAstItem *AstWidget::astItemFromIndex(QModelIndex index)
{
    QModelIndex i = proxyModel->mapToSource(index);
    if (!i.isValid()) {
        return 0;
    }
    return (GolangAstItem*)m_model->itemFromIndex(i);
}

static QString tagName(const QString &tag)
{
    /*
    tools/goastview/packageview.go
    const (
            tag_package      = "p"
            tag_type         = "t"
            tag_struct       = "s"
            tag_interface    = "i"
            tag_value        = "v"
            tag_const        = "c"
            tag_func         = "f"
            tag_value_folder = "+v"
            tag_const_folder = "+c"
            tag_func_folder  = "+f"
            tag_type_method  = "tm"
            tag_type_factor  = "tf"
            tag_type_value   = "tv"
    )
    */
    if (tag == "p") {
        return "package";
    } else if (tag == "t") {
        return "type";
    } else if (tag == "s") {
        return "struct";
    } else if (tag == "i") {
        return "interface";
    } else if (tag == "v") {
        return "value";
    } else if (tag == "c") {
        return "const";
    } else if (tag == "f") {
        return "func";
    } else if (tag == "+v") {
        return "value folder";
    } else if (tag == "+c") {
        return "const folder";
    } else if (tag == "+f") {
        return "func folder";
    } else if (tag == "tm") {
        return "method";
    } else if (tag == "tf") {
        return "factory";
    } else if (tag == "tv") {
        return "field";
    }
    return QString();
}

// level,tag,name,index,x,y
void AstWidget::updateModel(const QByteArray &data)
{
    //save state
    SymbolTreeState state;
    m_tree->saveState(&state);

    m_model->clear();

    QList<QByteArray> array = data.split('\n');
    QMap<int,QStandardItem*> items;
    QStringList indexFiles;
    bool ok = false;
    bool bmain = false;
    QMap<QString,GolangAstItem*> level1NameItemMap;
    foreach (QByteArray line, array) {
        QList<QByteArray> info = line.split(',');
        if (info.size() == 2 && info.at(0) == "@") {
            indexFiles.append(info.at(1));
        }
        if (info.size() < 3) {
            continue;
        }
        int level = info[0].toInt(&ok);
        if (!ok) {
            continue;
        }
        QString tag = info[1];
        QString name = info[2];
        if (name.isEmpty() || tag.isEmpty()) {
            continue;
        }
        if (level == 0) {
            level1NameItemMap.clear();
        }
        if (tag == "p") {
            if (name == "main") {
                bmain = true;
            } else {
                bmain = false;
            }
            if (name == "documentation") {
                continue;
            }
        }
        GolangAstItem *item = 0;
        if (level == 1) {
            item = level1NameItemMap.value(name);
            if (item != 0) {
                items[level] = item;
                continue;
            }
        }
        item = new GolangAstItem;
        if (level == 1) {
            level1NameItemMap.insert(name,item);
        }
        item->setTagName(tag);
        item->setText(name);
        if (!bmain && (name.at(0).isLower() || name.at(0) == '_')) {
            item->setIcon(GolangAstIcon::instance()->iconFromTag(tag,false));
        } else {
            item->setIcon(GolangAstIcon::instance()->iconFromTag(tag));
        }
        if (tag.at(0) == '+') {
            item->setToolTip(QString("%1").arg(tagName(tag)));
        } else {
            item->setToolTip(QString("%1 : %2").arg(tagName(tag)).arg(name));
        }
        if (info.size() >= 6) {
            int index = info[3].toInt(&ok);
            if (ok && index >= 0 && index < indexFiles.size()) {
                item->setFileName(indexFiles.at(index));
            }
            int line = info[4].toInt(&ok);
            if (ok) {
                item->setLine(line);
            }
            int col = info[5].toInt(&ok);
            if (ok) {
                item->setCol(col);
            }
        }
        QStandardItem *parent = items.value(level-1,0);
        if (parent ) {
            parent->appendRow(item);
        } else {
            m_model->appendRow(item);
        }
        items[level] = item;
    }

    //load state
    m_tree->loadState(proxyModel,&state);

    if (m_bOutline && m_bFirst) {
        //m_tree->expandToDepth(1);
        for(int i = 0; i < proxyModel->rowCount(); i++) {
            QModelIndex index = proxyModel->index(i,0);
            m_tree->expand(index);
            for (int j = 0; j < proxyModel->rowCount(index);j++) {
                m_tree->expand(proxyModel->index(j,0,index));
            }
        }
        m_bFirst = false;
    }
    QString text = m_filterEdit->text().trimmed();
    if (!text.isEmpty()) {
        this->filterChanged(text);
    }
}
