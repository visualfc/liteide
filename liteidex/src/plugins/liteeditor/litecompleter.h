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
// Module: litecompleter.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITECOMPLETER_H
#define LITECOMPLETER_H

#include <QObject>
#include <QModelIndex>
#include "liteeditorapi/liteeditorapi.h"

class CodeCompleter;
class CodeCompleterEx;
class QPlainTextEdit;
class QStandardItemModel;

class LiteCompleter : public LiteApi::ICompleter
{
    Q_OBJECT
public:
    explicit LiteCompleter(QObject *parent = 0);
    virtual ~LiteCompleter();
    virtual void setEditor(QPlainTextEdit *editor);
    virtual QStandardItem *findRoot(const QString &name);
    virtual void clearChildItem(QStandardItem *root);
    virtual void appendChildItem(QStandardItem *root,QString name,const QString &kind, const QString &info,const QIcon &icon, bool temp);
    virtual bool appendItem(const QString &name,const QIcon &icon, bool temp);
    virtual bool appendItemEx(const QString &name,const QString &kind, const QString &info,const QIcon &icon, bool temp);
    virtual void appendItems(QStringList nameList,const QString &kind, const QString &info,const QIcon &icon, bool temp);
    virtual void clearItemChilds(const QString &name);
    virtual void clear();
    virtual void clearTemp();
    virtual void setSearchSeparator(bool b);
    virtual bool searchSeparator() const;
    virtual void setExternalMode(bool b);
    virtual bool externalMode() const;
    virtual void showPopup();
    virtual void hidePopup();
    virtual bool isShowPopup();
    virtual void setCaseSensitivity(Qt::CaseSensitivity caseSensitivity);
    virtual void setCompletionPrefix(const QString &prefix);
    virtual QString completionPrefix() const;
    virtual void setCompletionContext(LiteApi::CompletionContext ctx);
    virtual LiteApi::CompletionContext completionContext() const;
    virtual QAbstractItemView *popup() const;
    virtual QModelIndex currentIndex() const;
    virtual QString currentCompletion() const;
    virtual QAbstractItemModel *model() const;
    virtual QAbstractItemModel *completionModel() const;
    virtual bool startCompleter(const QString &completionPrefix);
    virtual void updateCompleterModel();
    virtual void setImportList(const QStringList &importList);
public slots:
    virtual void completionPrefixChanged(QString,bool force);
    virtual void insertCompletion(QModelIndex);
    virtual void updateCompleteInfo(QModelIndex index);
protected:
    CodeCompleterEx    *m_completer;
    QStandardItemModel *m_model;
    QPlainTextEdit *m_editor;
    QTimer         *m_timer;
    QTextCursor     m_lastTextCursor;
    QString         m_lastPrefix;
    QChar           m_stop;
    bool            m_bSearchSeparator;
    bool            m_bExternalMode;
};

#endif // LITECOMPLETER_H
