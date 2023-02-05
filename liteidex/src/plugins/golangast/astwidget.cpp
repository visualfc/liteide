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
// Module: astwidget.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "astwidget.h"
#include "golangastitem.h"
#include "golangasticon.h"
#include "golangdocapi/golangdocapi.h"
#include "liteenvapi/liteenvapi.h"

#include <QAction>
#include <QMenu>
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
    layout->setMargin(0);
    layout->setSpacing(0);

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
    //m_tree->setExpandsOnDoubleClick(false);
    m_tree->setContextMenuPolicy(Qt::CustomContextMenu);

    m_gotoPosAct = new QAction(tr("Go To Definition"),this);
    m_importDocAct = new QAction(tr("View Import Document"),this);
    m_contextMenu = new QMenu(this);
    m_contextMenu->addAction(m_gotoPosAct);
    m_contextMenu->addAction(m_importDocAct);

    m_contextItem = 0;
    connect(m_tree,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClicked(QModelIndex)));
    connect(m_tree,SIGNAL(enterKeyPressed(QModelIndex)),this,SLOT(enterKeyPressed(QModelIndex)));
    connect(m_filterEdit,SIGNAL(filterChanged(QString)),this,SLOT(filterChanged(QString)));
    connect(m_tree,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(treeContextMenuRequested(QPoint)));
    connect(m_gotoPosAct,SIGNAL(triggered()),this,SLOT(gotoDefinition()));
    connect(m_importDocAct,SIGNAL(triggered()),this,SLOT(viewImportDoc()));
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
        if (item->m_tagName.indexOf("+") >= 0) {
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
        if (item->m_tagName.indexOf("+") < 0) {
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
        m_tree->expandToDepth(0);
        /*
        for(int i = 0; i < proxyModel->rowCount(); i++) {
            QModelIndex index = proxyModel->index(i,0);
            m_tree->expand(index);
            if (m_bOutline) {
                for (int j = 0; j < proxyModel->rowCount(index);j++) {
                    m_tree->expand(proxyModel->index(j,0,index));
                }
            }
        }
        */
    } else {
        QModelIndex first;
        filterModel(filter,m_tree->rootIndex(),first);
        if (first.isValid()) {
            m_tree->scrollTo(first);
        }
    }
}

void AstWidget::treeContextMenuRequested(QPoint pt)
{
    QModelIndex index =  m_tree->indexAt(pt);
    if (!index.isValid()) {
        return;
    }
    GolangAstItem *item = astItemFromIndex(index);
    if (item->isFolder() || item->m_tagFlag == LiteApi::TagPackage) {
        return;
    }
    m_contextItem = item;
    m_importDocAct->setVisible(item->m_tagFlag == LiteApi::TagImport);
    m_contextMenu->popup(m_tree->mapToGlobal(pt));
}

void AstWidget::gotoDefinition()
{
    gotoItemDefinition(m_contextItem);
}

void AstWidget::viewImportDoc()
{
    QString pkg = m_contextItem->text();
    QString orgPkg = pkg;
    //check mod and vendor pkg
    QString gotools = LiteApi::getGotools(m_liteApp);
    if (!gotools.isEmpty()) {
        QProcess process(this);
        process.setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
        process.setWorkingDirectory(m_workPath);
        QStringList args;
        args << "pkgcheck" << "-pkg" << pkg;
        process.start(gotools,args);
        if (!process.waitForFinished(3000)) {
            process.kill();
        }
        QByteArray ar = process.readAllStandardOutput();
        QString pkgs = QString::fromUtf8(ar).trimmed();
        if (!pkgs.isEmpty()) {
            QStringList pkgInfo = pkgs.split(",");
            if (pkgInfo.size() == 2 && !pkgInfo[0].isEmpty()) {
                pkg = pkgInfo[0];
            }
        }
    }

    LiteApi::IGolangDoc *doc = LiteApi::getGolangDoc(m_liteApp);
    if (!doc) {
        return;
    }
    QUrl url;
    url.setScheme("pdoc");
    url.setPath(pkg);
    QString addin;
    if (pkg != orgPkg) {
        addin = orgPkg;
    }
    doc->openUrl(url,addin);
    doc->activeBrowser();

}

void AstWidget::doubleClicked(QModelIndex index)
{
    GolangAstItem *item = astItemFromIndex(index);
    if (item && !item->isFolder()) {
        gotoItemDefinition(item);
    }
}

void AstWidget::enterKeyPressed(const QModelIndex &index)
{
    GolangAstItem *item = astItemFromIndex(index);
    if (!item) {
        return;
    }
    if (item->isFolder()) {
        m_tree->setExpanded(index,!m_tree->isExpanded(index));
    } else {
        gotoItemDefinition(item);
    }
}

void AstWidget::gotoItemDefinition(GolangAstItem *item)
{
    if (item->m_posList.isEmpty()) {
         return;
    }
    AstItemPos pos = item->m_posList.at(0);
    QFileInfo info(QDir(m_workPath),pos.fileName);
    LiteApi::gotoLine(m_liteApp,info.filePath(),pos.line-1,pos.column-1,true,true);
    return;
//    LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(info.filePath());
//    if (!editor) {
//        return;
//    }
//    editor->widget()->setFocus();
//    LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
//    if (!textEditor) {
//        return;
//    }
//    textEditor->gotoLine(pos.line-1,pos.column,true);
}

GolangAstItem *AstWidget::astItemFromIndex(QModelIndex index)
{
    QModelIndex i = proxyModel->mapToSource(index);
    if (!i.isValid()) {
        return 0;
    }
    return (GolangAstItem*)m_model->itemFromIndex(i);
}

/*
tools/goastview/packageview.go
const (
    tag_package        = "p"
    tag_imports_folder = "+m"
    tag_import         = "mm"
    tag_type           = "t"
    tag_struct         = "s"
    tag_interface      = "i"
    tag_value          = "v"
    tag_const          = "c"
    tag_func           = "f"
    tag_value_folder   = "+v"
    tag_const_folder   = "+c"
    tag_func_folder    = "+f"
    tag_type_method    = "tm"
    tag_type_factor    = "tf"
    tag_type_value     = "tv"
)
*/

static QString tagInfo(const QString &tag)
{
    if (tag == "p") {
        return "package";
    } else if (tag == "+m") {
        return "imports folder";
    } else if (tag == "mm") {
        return "import";
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
        return "values folder";
    } else if (tag == "+c") {
        return "const folder";
    } else if (tag == "+f") {
        return "funcs folder";
    } else if (tag == "tm") {
        return "method";
    } else if (tag == "tf") {
        return "factory";
    } else if (tag == "tv") {
        return "field";
    }
    return QString();
}

static LiteApi::ASTTAG_ENUM toTagFlag(const QString &tag)
{
    if (tag == "p") {
        return LiteApi::TagPackage;
    } else if (tag == "+m") {
        return LiteApi::TagImportFolder;
    } else if (tag == "mm") {
        return LiteApi::TagImport;
    } else if (tag == "t") {
        return LiteApi::TagType;
    } else if (tag == "s") {
        return LiteApi::TagStruct;
    } else if (tag == "i") {
        return LiteApi::TagInterface;
    } else if (tag == "v") {
        return LiteApi::TagValue;
    } else if (tag == "c") {
        return LiteApi::TagConst;
    } else if (tag == "f") {
        return LiteApi::TagFunc;
    } else if (tag == "+v") {
        return LiteApi::TagValueFolder;
    } else if (tag == "+c") {
        return LiteApi::TagConstFolder;
    } else if (tag == "+f") {
        return LiteApi::TagFuncFolder;
    } else if (tag == "tm") {
        return LiteApi::TagTypeMethod;
    } else if (tag == "tf") {
        return LiteApi::TagTypeFactor;
    } else if (tag == "tv") {
        return LiteApi::TagTypeValue;
    } else if (tag == "b") {
        return LiteApi::TagTodo;
    } else if (tag == "+b") {
        return LiteApi::TagTodoFolder;
    }
    return LiteApi::TagNone;
}

void AstWidget::parserModel(QStandardItemModel *model, const QByteArray &data, const QByteArray &sep, bool flatMode, bool skipimport)
{
    QList<QString> array = QString::fromUtf8(data).split('\n');
    QMap<int,QStandardItem*> items;
    QStringList indexFiles;
    bool ok = false;
    bool bmain = false;
    QMap<QString,GolangAstItem*> level1NameItemMap;
    foreach (QString line, array) {
        int pos = line.indexOf('@');
        QString tip;
        if (pos == 0) {
            indexFiles.append(line.mid(1));
            continue;
        } else if (pos >= 1) {
            tip = line.mid(pos+1);
            line = line.left(pos);
        }
        line = line.trimmed();
        QList<QString> info = line.split(sep);
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
        if (flatMode && tag.startsWith("+")) {
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
            if (flatMode) {
                continue;
            }
        }
        if (skipimport && tag == "mm") {
            continue;
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
        item->m_tagName = tag;
        item->m_tagFlag = toTagFlag(tag);
        item->setText(name);
        if (!bmain && (name.at(0).isLower() || name.at(0) == '_')) {
            item->setIcon(GolangAstIcon::instance()->iconFromTag(tag,false));
        } else {
            item->setIcon(GolangAstIcon::instance()->iconFromTag(tag));
        }
        if (!tip.isEmpty()) {
            item->setToolTip(tip);
            // todo comment use tip
            if (tag == "b") {
                item->setText(tip);
                item->setToolTip(name);
            }
        } else if (tag.at(0) == '+') {
            item->setToolTip(QString("%1").arg(tagInfo(tag)));
        } else {
            item->setToolTip(QString("%1 %2").arg(tagInfo(tag)).arg(name));
        }
        if (info.size() >= 4) {
            foreach (QString pos, info[3].split(';')) {
                QList<QString> ar = pos.split(':');
                if (ar.size() == 3) {
                    bool ok = false;
                    int index = ar[0].toInt(&ok);
                    if (ok && index >= 0 && index < indexFiles.size()) {
                        bool ok1,ok2;
                        int line = ar[1].toInt(&ok1);
                        int col = ar[2].toInt(&ok2);
                        if (ok1 && ok2) {
                            AstItemPos pos = {indexFiles[index],line,col,-1,-1};
                            item->m_posList.append(pos);
                        }
                    }
                } else if (ar.size() == 5) {
                    bool ok = false;
                    int index = ar[0].toInt(&ok);
                    if (ok && index >= 0 && index < indexFiles.size()) {
                        bool ok1,ok2,ok3,ok4;
                        int line = ar[1].toInt(&ok1);
                        int col = ar[2].toInt(&ok2);
                        int endLine = ar[3].toInt(&ok3);
                        int endCol = ar[4].toInt(&ok4);
                        if (ok1 && ok2 && ok3 && ok4) {
                            AstItemPos pos = {indexFiles[index],line,col,endLine,endCol};
                            item->m_posList.append(pos);
                        }
                    }
                }
            }
        }
        QStandardItem *parent = items.value(level-1,0);
        if (parent ) {
            if (flatMode) {
                if (tag == "tv") {
                    item->setText(parent->text()+"."+item->text());
                }
                model->appendRow(item);
            } else {
                parent->appendRow(item);
            }
        } else {
            model->appendRow(item);
        }
        items[level] = item;
    }
}

bool AstWidget::trySyncIndex(const QString &filePath, int line, int column)
{
    QModelIndexList finds;
    QFileInfo info(filePath);
    findModelIndex(QModelIndex(),info.fileName(),line+1,column+1,finds);
    if (finds.isEmpty()) {
        return false;
    }
    m_tree->setCurrentIndex(finds.last());
    m_tree->scrollTo(finds.last());
    return true;
}

void AstWidget::findModelIndex(const QModelIndex &parent, const QString &fileName, int line, int column, QModelIndexList &finds)
{
    for (int i = 0; i < proxyModel->rowCount(parent); i++) {
        QModelIndex index = proxyModel->index(i,0,parent);
        GolangAstItem *item = astItemFromIndex(index);
        if (!item) {
            continue;
        }
        foreach (AstItemPos pos, item->m_posList) {
            if (pos.fileName == fileName && line >= pos.line && column >= pos.column && line <= pos.endLine) {
                if (line < pos.endLine || (line == pos.endLine && column <= pos.endColumn) ) {
                    finds.push_back(index);
                }
            }
        }
        if (item->hasChildItem()) {
            findModelIndex(index,fileName,line,column,finds);
        }
    }
}

// level,tag,name,pos,@info
void AstWidget::updateModel(const QByteArray &data, const QByteArray &sep)
{
    //save state
    SymbolTreeState state;
    m_tree->saveState(&state);

    m_model->clear();

    parserModel(m_model,data,sep,false,false);

    //load state
    if (!m_tree->isExpanded(m_tree->rootIndex())) {
        m_tree->expandToDepth(0);
    }
    m_tree->loadState(proxyModel,&state);
    /*
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
    */
    QString text = m_filterEdit->text().trimmed();
    if (!text.isEmpty()) {
        this->filterChanged(text);
    }
}
