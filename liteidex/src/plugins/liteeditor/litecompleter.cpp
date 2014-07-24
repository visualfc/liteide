/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
// Module: litecompleter.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "litecompleter.h"
#include "treemodelcompleter/treemodelcompleter.h"

#include <QCompleter>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QTextBlock>
#include <QTimer>
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


class WordItem : public QStandardItem
{
public:
    WordItem(const QString &text) : QStandardItem(text)
    {
        this->setName(text);
    }
    enum {
        NameRole = Qt::UserRole+2,
        KindRole,
        InfoRole,
        TempRole
    };
    virtual QVariant data(int role = Qt::UserRole + 1) const
    {
        if (role == Qt::DisplayRole) {
            if (this->info().isEmpty()) {
                return this->name();
            }
            return QString("%1\t%2").arg(this->name()).arg(this->info());
        }
        return QStandardItem::data(role);
    }
    void setName(const QString &word)
    {
        this->setData(word,NameRole);
    }
    QString name() const
    {
        return QStandardItem::data(NameRole).toString();
    }
    void setKind(const QString &kind)
    {
        this->setData(kind,KindRole);
    }
    QString kind() const
    {
        return QStandardItem::data(KindRole).toString();
    }
    void setInfo(const QString &info)
    {
        this->setData(info,InfoRole);
    }
    QString info() const
    {
        return QStandardItem::data(InfoRole).toString();
    }
    void setTemp(bool temp)
    {
        this->setData(temp,TempRole);
    }
    bool isTemp() const
    {
        return QStandardItem::data(TempRole).toBool();
    }
};

LiteCompleter::LiteCompleter(QObject *parent) :
    LiteApi::ICompleter(parent),
    m_completer( new TreeModelCompleter(this)),
    m_model(new QStandardItemModel(this)),
    m_bSearchSeparator(true),
    m_bExternalMode(false)
{
    m_completer->setModel(m_model);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseSensitive);
    m_completer->setSeparator(".");
    m_completer->setWrapAround(true);
    m_stop = '(';
    QObject::connect(m_completer, SIGNAL(activated(QModelIndex)),
                     this, SLOT(insertCompletion(QModelIndex)));
}

LiteCompleter::~LiteCompleter()
{
    delete m_completer;
    delete m_model;
}

void LiteCompleter::setEditor(QPlainTextEdit *editor)
{
    m_editor = editor;
    m_completer->setWidget(m_editor);
}

QCompleter *LiteCompleter::completer() const
{
    return m_completer;
}

QStandardItem *LiteCompleter::findRoot(const QString &name)
{
    QStringList words = name.split(m_completer->separator(),QString::SkipEmptyParts);
    WordItem *root = 0;
    WordItem *item = 0;
    foreach (QString word, words) {
        item = 0;
        QModelIndex parent = m_model->indexFromItem(root);
        for (int i = 0; i < m_model->rowCount(parent); i++) {
            QModelIndex index = m_model->index(i,0,parent);
            if (index.data(WordItem::NameRole).toString() == word) {
                item = static_cast<WordItem*>(m_model->itemFromIndex(index));
                break;
            }
        }
        if (item == 0) {
            item = new WordItem(word);
            if (root == 0) {
                m_model->appendRow(item);
            } else {
                root->appendRow(item);
            }
        }
        root = item;
    }
    return root;
}

void LiteCompleter::clearChildItem(QStandardItem *root)
{
    if (root) {
        QModelIndex index = m_model->indexFromItem(root);
        m_model->removeRows(0,m_model->rowCount(index),index);
    }
}

void LiteCompleter::appendChildItem(QStandardItem *root, QString name, const QString &kind, const QString &info, const QIcon &icon, bool temp)
{
    if (root == 0) {
        WordItem *item = 0;
        QModelIndex parent = m_model->indexFromItem(root);
        int count = m_model->rowCount(parent);
        while(count--) {
            QModelIndex index = m_model->index(count,0,parent);
            if (index.data(WordItem::NameRole).toString() == name) {
                item = static_cast<WordItem*>(m_model->itemFromIndex(index));
                break;
            }
        }
        if (item == 0) {
            item = new WordItem(name);
            if (root == 0) {
                m_model->appendRow(item);
            } else {
                root->appendRow(item);
            }
        }
        if (item && item->kind().isEmpty()) {
            item->setKind(kind);
            item->setInfo(info);
            item->setTemp(temp);
            item->setIcon(icon);
        }
   } else {
        WordItem *item = new WordItem(name);
        root->appendRow(item);
        item->setKind(kind);
        item->setInfo(info);
        item->setTemp(temp);
        item->setIcon(icon);
    }
}

void LiteCompleter::clear()
{
    m_model->clear();
}

static void clearTempIndex(QStandardItemModel *model, QModelIndex parent) {
    int i = model->rowCount(parent);
    while (i--) {
        QModelIndex index = model->index(i,0,parent);
        if (index.data(WordItem::TempRole).toBool() == true) {
            model->removeRow(i,parent);
        } else {
            clearTempIndex(model,index);
        }
    }
}

void LiteCompleter::clearTemp()
{
    clearTempIndex(m_model,QModelIndex());
}

void LiteCompleter::show()
{
    if (!m_editor) {
        return;
    }
    m_completer->model()->sort(0);
    m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    QTextCursor cursor = m_editor->textCursor();
    int offset = m_completer->completionPrefix().length();
    int pos = m_completer->completionPrefix().indexOf(".");
    if (pos != -1) {
        offset -= pos+1;
    }
    cursor.movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,offset);
    QRect cr = m_editor->cursorRect(cursor);
    cr.setLeft(cr.left()+m_editor->viewport()->x()-24);
    cr.setWidth(m_completer->popup()->sizeHintForColumn(0)
                + m_completer->popup()->verticalScrollBar()->sizeHint().width());
    m_completer->complete(cr); // popup it up!
}

void LiteCompleter::setSearchSeparator(bool b)
{
    m_bSearchSeparator = b;
}

bool LiteCompleter::searchSeparator() const
{
    return m_bSearchSeparator;
}

bool LiteCompleter::externalMode() const
{
    return m_bExternalMode;
}

void LiteCompleter::setExternalMode(bool b)
{
    m_bExternalMode = b;
}

void LiteCompleter::appendItems(QStringList items,const QString &kind, const QString &info,const QIcon &icon, bool temp)
{
    foreach(QString item,items) {
        appendItemEx(item,kind,info,icon,temp);
    }
}


bool LiteCompleter::appendItem(const QString &name, const QIcon &icon, bool temp)
{
    QString func;
    int pos = name.indexOf(m_stop);
    if (pos == -1) {
        func = name.trimmed();
    } else {
        func = name.left(pos).trimmed();
        //arg = text.right(text.length()-pos).trimmed();
    }
    return this->appendItemEx(func,"","",icon,temp);
}

void LiteCompleter::clearItemChilds(const QString &name)
{
    QStringList words = name.split(m_completer->separator(),QString::SkipEmptyParts);
    WordItem *root = 0;
    WordItem *item = 0;
    foreach (QString word, words) {
        item = 0;
        QModelIndex parent = m_model->indexFromItem(root);
        int count = m_model->rowCount(parent);
        while(count--) {
            QModelIndex index = m_model->index(count,0,parent);
            if (index.data(WordItem::NameRole).toString() == word) {
                item = static_cast<WordItem*>(m_model->itemFromIndex(index));
                break;
            }
        }
        root = item;
    }
    if (item) {
        QModelIndex index = m_model->indexFromItem(item);
        m_model->removeRows(0,m_model->rowCount(index),index);        
    }
}

bool LiteCompleter::appendItemEx(const QString &name,const QString &kind, const QString &info, const QIcon &icon, bool temp)
{
    QStringList words = name.split(m_completer->separator(),QString::SkipEmptyParts);

    WordItem *root = 0;
    WordItem *item = 0;
    bool bnew = false;    
    foreach (QString word, words) {
        item = 0;
        QModelIndex parent = m_model->indexFromItem(root);
        for (int i = 0; i < m_model->rowCount(parent); i++) {
            QModelIndex index = m_model->index(i,0,parent);
            if (index.data(WordItem::NameRole).toString() == word) {
                item = static_cast<WordItem*>(m_model->itemFromIndex(index));
                break;
            }
        }
        if (item == 0) {
            item = new WordItem(word);
            if (root == 0) {
                m_model->appendRow(item);
            } else {
                root->appendRow(item);
            }
            bnew = true;
        }
        root = item;
    }
    if (item && item->kind().isEmpty()) {
        item->setKind(kind);
        item->setInfo(info);
        item->setTemp(temp);
        item->setIcon(icon);
    }
    return bnew;
}

void LiteCompleter::completionPrefixChanged(QString prefix, bool force)
{
    if (!m_editor) {
        return;
    }
    if (m_completer->widget() != m_editor) {
        return;
    }
    emit prefixChanged(m_editor->textCursor(),prefix, force);
}

void LiteCompleter::insertCompletion(QModelIndex index)
{   
    if (!m_editor) {
        return;
    }
    if (m_completer->widget() != m_editor)
        return;
    if (!index.isValid()) {
        return;
    }

    QString text = index.data(WordItem::NameRole).toString();
    QString kind = index.data(WordItem::KindRole).toString();
    QString info = index.data(WordItem::InfoRole).toString();
    QString prefix = m_completer->completionPrefix();
    //IsAbs r.URL.
    int pos = prefix.lastIndexOf(m_completer->separator());
    int length = prefix.length();
    if (pos != -1) {
        length = prefix.length()-pos-1;
    }
    QString extra = text;
    QString wordText = text;
    QTextCursor tc = m_editor->textCursor();
    tc.beginEditBlock();
    if (m_completer->caseSensitivity() == Qt::CaseSensitive) {
        extra = text.right(text.length()-length);
        wordText = prefix+extra;
    } else {
        while (length--) {
            tc.deletePreviousChar();
        }
        extra = text;
        wordText = text;
    }

    if (kind == "func" && tc.block().text().at(tc.positionInBlock()-1) != '(') {
        extra += "()";
        tc.insertText(extra);
        if (!info.startsWith("func()")) {
            tc.movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,1);
        }
    } else {
        tc.insertText(extra);
    }
    tc.endEditBlock();
    m_editor->setTextCursor(tc);
    emit wordCompleted(wordText,kind,info);
}
