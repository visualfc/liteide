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
// Module: codecompleter.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef CODECOMPLETER_H
#define CODECOMPLETER_H

#include <QCompleter>
#include <QTimer>
#include <QListView>
#include "liteapi/liteapi.h"
#include "liteeditorapi/liteeditorapi.h"

class QListView;
class QKeyEvent;
class QStandardItemModel;
class QStandardItem;
class QSortFilterProxyModel;
class CodeCompleter : public QCompleter
{
    Q_OBJECT
public:
    explicit CodeCompleter(QObject *parent = 0);
    virtual ~CodeCompleter();
    void setModel(QAbstractItemModel *c);
    QString separator() const;
    void setSeparator(const QString &separator);
    void setCompletionPrefix(const QString &prefix);
    QString completionPrefix() const;
    void updateFilter();
protected:
    virtual bool eventFilter(QObject *o, QEvent *e);
    virtual QStringList splitPath(const QString &path) const;
    virtual QString pathFromIndex(const QModelIndex &index) const;
private:
    QSortFilterProxyModel *m_proxy;
    QListView *m_popup;
    QString m_seperator;
    QString m_prefix;
};

namespace LiteApi {
    enum CaseSensitivity {
        CaseInsensitive,
        CaseSensitive,
        FirstLetterCaseSensitive
    };
}

class CodeCompleterInfo;
class CodeCompleterListView : public QListView
{
    Q_OBJECT
public:
    CodeCompleterListView(QWidget *parent = 0);
    virtual void setModel(QAbstractItemModel *model);
    QSize calculateSize() const;
    QPoint infoFramePos() const;
public slots:
    void maybeShowInfoTip();
protected:
    virtual void hideEvent(QHideEvent *);
    CodeCompleterInfo  *m_infoFrame;
    QTimer              m_infoTimer;
};

class CodeCompleterProxyModel : public QAbstractListModel
{
public:
    CodeCompleterProxyModel(QObject *parent = 0);
    virtual ~CodeCompleterProxyModel();
    virtual int rowCount(const QModelIndex & index = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    void setSourceModel(QStandardItemModel *sourceModel);
    void setImportList(const QStringList &importList);
    QStandardItemModel *sourceModel() const;
    QStandardItem* item(const QModelIndex &index) const;
    int filter(const QString &filter, int cs = LiteApi::CaseInsensitive, LiteApi::CompletionContext ctx = LiteApi::CompleterCodeContext);
    void setSeparator(const QString &separator);
    QString separator() const;
    void setFuzzy(bool b);
    bool isFuzzy() const;
protected:
    bool splitFilter(const QString &filter, QModelIndex &parent, QString &prefix, const QString &sep = ".");
    void clearItems();
    QList<QStandardItem*> m_items;
    QStringList         m_importList;
    QStandardItemModel *m_model;
    QString             m_seperator;
    bool                m_fuzzy;
};

class CodeCompleterEx : public QObject
{
    Q_OBJECT
public:
    CodeCompleterEx(QObject *parent = 0);
    virtual ~CodeCompleterEx();
    void setModel(QStandardItemModel *c);
    QAbstractItemModel *model() const;
    void setImportList(const QStringList &importList);
    void setSeparator(const QString &separator);
    QString separator() const;
    void setCompletionPrefix(const QString &prefix);
    QString completionPrefix() const;
    void setCompletionContext(LiteApi::CompletionContext ctx);
    LiteApi::CompletionContext completionContext() const;
    void updateFilter();
    void complete(const QRect& rect = QRect());
    QWidget *widget() const;
    void setWidget(QWidget *widget);
    QModelIndex currentIndex() const;
    QString currentCompletion() const;
    void setCaseSensitivity(Qt::CaseSensitivity cs);
    Qt::CaseSensitivity caseSensitivity() const;
    QAbstractItemView *popup() const;
    QAbstractItemModel *completionModel() const;
    void setWrapAround(bool wrap);
    bool wrapAround() const;
    void setFuzzy(bool b);
    bool isFuzzy() const;
signals:
    void activated(QModelIndex);
public slots:
    void completerActivated(QModelIndex);
protected:
    virtual bool eventFilter(QObject *o, QEvent *e);
    QWidget              *m_widget;
    CodeCompleterListView     *m_popup;
    CodeCompleterProxyModel   *m_proxy;
    Qt::CaseSensitivity m_cs;
    LiteApi::CompletionContext m_ctx;
    QString             m_prefix;
    int                 maxVisibleItems;
    bool                m_eatFocusOut;
    bool                m_hiddenBecauseNoMatch;
    bool                m_wrap;
};

#endif // CODECOMPLETER_H
