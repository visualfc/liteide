/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
#include "codecompleter.h"

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
    }
    enum {
        KindRole = Qt::UserRole+2,
        TempRole,
        SnippetRole,
    };
    void setKind(const QString &kind)
    {
        this->setData(kind,KindRole);
    }
    QString kind() const
    {
        return QStandardItem::data(KindRole).toString();
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
    m_completer( new CodeCompleterEx(this)),
    m_model(new QStandardItemModel(this)),
    m_bSearchSeparator(true),
    m_bExternalMode(false)
{
    m_completer->setModel(m_model);
    m_completer->setCaseSensitivity(Qt::CaseSensitive);
    m_completer->setSeparator(".");
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
            if (index.data().toString() == word) {
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
            if (index.data().toString() == name) {
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
            item->setToolTip(info);
            item->setTemp(temp);
            item->setIcon(icon);
        }
   } else {
        WordItem *item = new WordItem(name);
        root->appendRow(item);
        item->setKind(kind);
        item->setToolTip(info);
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

void LiteCompleter::showPopup()
{
    if (!m_editor) {
        return;
    }
    //m_completer->model()->sort(0);
    m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    QTextCursor cursor = m_editor->textCursor();
    int offset = m_completer->completionPrefix().length();
    int pos = m_completer->completionPrefix().indexOf(m_completer->separator());
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

void LiteCompleter::hidePopup()
{
    if (m_completer->popup()->isVisible()) {
        m_completer->popup()->hide();
    }
}

bool LiteCompleter::isShowPopup()
{
    return m_completer->popup()->isVisible();
}

void LiteCompleter::setCaseSensitivity(Qt::CaseSensitivity caseSensitivity)
{
    m_completer->setCaseSensitivity(caseSensitivity);
}

void LiteCompleter::setCompletionPrefix(const QString &prefix)
{
    m_completer->setCompletionPrefix(prefix);
}

QString LiteCompleter::completionPrefix() const
{
    return m_completer->completionPrefix();
}

void LiteCompleter::setCompletionContext(LiteApi::CompletionContext ctx)
{
    m_completer->setCompletionContext(ctx);
}

LiteApi::CompletionContext LiteCompleter::completionContext() const
{
    return m_completer->completionContext();
}

void LiteCompleter::setSeparator(const QString &sep)
{
    m_completer->setSeparator(sep);
}

QString LiteCompleter::separator() const
{
    return m_completer->separator();
}

QAbstractItemView *LiteCompleter::popup() const
{
    return m_completer->popup();
}

QModelIndex LiteCompleter::currentIndex() const
{
    return m_completer->currentIndex();
}

QString LiteCompleter::currentCompletion() const
{
    return m_completer->currentCompletion();
}

QAbstractItemModel *LiteCompleter::model() const
{
    return m_model;
}

QAbstractItemModel *LiteCompleter::completionModel() const
{
    return m_completer->completionModel();
}

bool LiteCompleter::startCompleter(const QString &completionPrefix)
{
    if (completionPrefix != this->completionPrefix()) {
        this->setCompletionPrefix(completionPrefix);
        this->popup()->setCurrentIndex(this->completionModel()->index(0, 0));
    }
    if (!completionPrefix.isEmpty() && this->currentCompletion() == completionPrefix) {
        this->popup()->hide();
        return false;
    }
    this->showPopup();
    return true;
}

void LiteCompleter::updateCompleterModel()
{
    this->m_completer->updateFilter();
}

void LiteCompleter::setImportList(const QStringList &importList)
{
    this->m_completer->setImportList(importList);
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

void LiteCompleter::appendSnippetItem(const QString &name, const QString &info, const QString &content)
{   
    WordItem *item = new WordItem(name);
    item->setKind("snippet");
    item->setToolTip(info);
    item->setIcon(QIcon(":liteeditor/images/snippet.png"));
    item->setData(content,WordItem::SnippetRole);
    m_model->appendRow(item);
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
            if (index.data().toString() == word) {
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
            if (index.data().toString() == word) {
                item = static_cast<WordItem*>(m_model->itemFromIndex(index));
                break;
            }
        }
        if (item == 0) {
            item = new WordItem(word);
            item->setIcon(icon);
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
        item->setToolTip(info);
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

    QString text = index.data().toString();
    QString kind = index.data(WordItem::KindRole).toString();
    QString info = index.data(Qt::ToolTipRole).toString();
    QString prefix = m_completer->completionPrefix();
    //IsAbs r.URL.
    int length = prefix.length();
    if (m_completer->completionContext() == LiteApi::CompleterCodeContext) {
        int pos = prefix.lastIndexOf(m_completer->separator());
        if (pos != -1) {
            length = prefix.length()-pos-m_completer->separator().length();
        }
    }
    QString extra = text;
    QString wordText = text;
    QTextCursor tc = m_editor->textCursor();
    tc.beginEditBlock();
    while (length--) {
         tc.deletePreviousChar();
    }
    extra = text;
    wordText = text;

    if (kind == "func") {
        if (tc.block().text().at(tc.positionInBlock()) != '(') {
            extra += "()";
            tc.insertText(extra);
            if (!info.startsWith("func()")) {
                tc.movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,1);
            }
        } else {
            tc.insertText(extra);
            kind.clear();
        }
    } else if (kind == "snippet") {
        QString content = index.data(WordItem::SnippetRole).toString();
        QString text = m_editor->textCursor().block().text();
        int space = 0;
        for (int i = 0; i < text.size(); i++) {
            if (text.at(i).isSpace()) {
                space++;
            } else {
                break;
            }
        }
        QString head = text.left(space);
        QStringList srcList = content.split("\n");
        QStringList targetList;
        for (int i = 0; i < srcList.size(); i++) {
            if (i == 0) {
                targetList.append(srcList[i]);
            } else {
                targetList.append(head+srcList[i]);
            }
        }
        QString target = targetList.join("\n");
        int startpos = tc.position();
        int pos1 = target.indexOf("$");
        int pos2 = -1;
        if (pos1 > 0) {
            pos2 = target.indexOf("$",pos1+1);
        }
        target.replace("$","");
        extra = target;
        tc.insertText(extra);
        if (pos1 >= 0) {
            tc.setPosition(startpos+pos1);
            if (pos2 > 0) {
                tc.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,pos2-pos1-1);
            }
        }
    } else {
        tc.insertText(extra);
    }
    tc.endEditBlock();
    m_editor->setTextCursor(tc);
    emit wordCompleted(wordText,kind,info);
}

void LiteCompleter::updateCompleteInfo(QModelIndex index)
{
    if (!m_editor) {
        return;
    }
    if (m_completer->widget() != m_editor)
        return;
    if (!index.isValid()) {
        return;
    }

    QString text = index.data().toString();
    QString kind = index.data(WordItem::KindRole).toString();
    QString info = index.data(Qt::ToolTipRole).toString();
    emit wordCompleted(text,kind,info);
}
