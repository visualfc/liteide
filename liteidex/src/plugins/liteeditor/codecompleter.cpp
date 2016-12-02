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
// Module: codecompleter.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "codecompleter.h"
#include "faketooltip.h"
#include <QApplication>
#include <QListView>
#include <QStandardItemModel>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QDesktopWidget>
#include <QItemDelegate>
#include <QLabel>
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

CodeCompleter::CodeCompleter(QObject *parent) :
    QCompleter(parent)
{
    this->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    this->setWrapAround(true);
    m_popup = new QListView;
    m_popup->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_popup->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_popup->setSelectionMode(QAbstractItemView::SingleSelection);
    m_popup->setModelColumn(0);
    this->setPopup(m_popup);
    m_proxy = new QSortFilterProxyModel(this);
}

CodeCompleter::~CodeCompleter()
{
}

void CodeCompleter::setModel(QAbstractItemModel *c)
{
    m_proxy->setSourceModel(c);
    QCompleter::setModel(m_proxy);
}

void CodeCompleter::setSeparator(const QString &separator)
{
    m_seperator = separator;
}

void CodeCompleter::setCompletionPrefix(const QString &prefix)
{
    QCompleter::setCompletionPrefix(prefix);
}

QString CodeCompleter::completionPrefix() const
{
    return QCompleter::completionPrefix();
}

void CodeCompleter::updateFilter()
{

}

bool CodeCompleter::eventFilter(QObject *o, QEvent *e)
{
    switch (e->type()) {
    case QEvent::KeyPress: {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        switch (ke->key()) {
        case Qt::Key_Up:
            if ( this->popup() && this->popup()->isVisible()) {
                QModelIndex index = this->popup()->currentIndex();
                if (index.isValid() && (index.row() == 0)) {
                    this->popup()->setCurrentIndex(this->popup()->model()->index(this->popup()->model()->rowCount()-1,0));
                    return true;
                }
            }
            break;
        case Qt::Key_Down:
            if (this->popup() && this->popup()->isVisible()) {
                QModelIndex index = this->popup()->currentIndex();
                if (index.isValid() && (index.row() == this->popup()->model()->rowCount()-1)) {
                    this->popup()->setCurrentIndex(this->popup()->model()->index(0,0));
                    return true;
                }
            }
            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
    return QCompleter::eventFilter(o,e);
}

QString CodeCompleter::separator() const
{
    return m_seperator;
}

QStringList CodeCompleter::splitPath(const QString &path) const
{
    if (m_seperator.isNull()) {
        return QCompleter::splitPath(path);
    }
    return path.split(m_seperator);
}

QString CodeCompleter::pathFromIndex(const QModelIndex &index) const
{
    if (m_seperator.isNull()) {
        return QCompleter::pathFromIndex(index);
    }

    // navigate up and accumulate data
    QStringList dataList;
    for (QModelIndex i = index; i.isValid(); i = i.parent()) {
        dataList.prepend(model()->data(i, completionRole()).toString());
    }
    return dataList.join(m_seperator);
}

class CodeCompleterInfo : public FakeToolTip
{
public:
    CodeCompleterInfo(QWidget *parent = 0)
        : FakeToolTip(parent), m_label(new QLabel(this))
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setMargin(0);
        layout->setSpacing(0);
        layout->addWidget(m_label);

        // Limit horizontal width
        m_label->setSizePolicy(QSizePolicy::Fixed, m_label->sizePolicy().verticalPolicy());

        m_label->setForegroundRole(QPalette::ToolTipText);
        m_label->setBackgroundRole(QPalette::ToolTipBase);
    }

    void setText(const QString &text)
    {
        m_label->setText(text);
    }

    // Workaround QTCREATORBUG-11653
    void calculateMaximumWidth()
    {
        const QDesktopWidget *desktopWidget = QApplication::desktop();
        const int desktopWidth = desktopWidget->isVirtualDesktop()
                ? desktopWidget->width()
                : desktopWidget->availableGeometry(desktopWidget->primaryScreen()).width();
        const QMargins widgetMargins = contentsMargins();
        const QMargins layoutMargins = layout()->contentsMargins();
        const int margins = widgetMargins.left() + widgetMargins.right()
                + layoutMargins.left() + layoutMargins.right();
        m_label->setMaximumWidth(desktopWidth - this->pos().x() - margins);
    }

private:
    QLabel *m_label;
};

CodeCompleterListView::CodeCompleterListView(QWidget *parent)
    : QListView(parent)
{
    setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    m_infoFrame = new CodeCompleterInfo(this);
    m_infoTimer.setInterval(100);
    m_infoTimer.setSingleShot(true);
    connect(&m_infoTimer, SIGNAL(timeout()), SLOT(maybeShowInfoTip()));
}

void CodeCompleterListView::setModel(QAbstractItemModel *model)
{
    QListView::setModel(model);
    connect(this->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),&m_infoTimer,SLOT(start()));
}

QSize CodeCompleterListView::calculateSize() const
{
    static const int maxVisibleItems = 10;

    // Determine size by calculating the space of the visible items
    const int visibleItems = qMin(model()->rowCount(), maxVisibleItems);
    const int firstVisibleRow = verticalScrollBar()->value();

    const QStyleOptionViewItem &option = viewOptions();
    QSize shint;
    for (int i = 0; i < visibleItems; ++i) {
        QSize tmp = itemDelegate()->sizeHint(option, model()->index(i + firstVisibleRow, 0));
        if (shint.width() < tmp.width())
            shint = tmp;
    }
    shint.rheight() *= visibleItems;
    return shint;
}

QPoint CodeCompleterListView::infoFramePos() const
{
    const QRect &r = rectForIndex(currentIndex());
    int xoffset = this->frameWidth()+3;
    int yoffset = this->frameWidth()-verticalOffset();
    QScrollBar *vsb = this->verticalScrollBar();
    if (vsb && vsb->isVisible())
        xoffset += this->horizontalScrollBar()->sizeHint().height();
    QPoint pt = this->mapToGlobal(r.topRight());
    pt.rx() += xoffset;
    pt.ry() += yoffset;
    return pt;
}

void CodeCompleterListView::maybeShowInfoTip()
{
    const QModelIndex &current = this->currentIndex();
    if (!current.isValid())
        return;

    if (!this->isVisible()) {
        if (m_infoFrame->isVisible()) {
            m_infoFrame->hide();
        }
        m_infoTimer.setInterval(100);
        return;
    }

    const QString &infoTip = current.data(Qt::ToolTipRole).toString();
    if (infoTip.isEmpty()) {
        m_infoFrame->hide();
        return;
    }
    m_infoFrame->move(this->infoFramePos());
    m_infoFrame->setText(infoTip);
    m_infoFrame->calculateMaximumWidth();
    m_infoFrame->adjustSize();
    m_infoFrame->show();
    m_infoFrame->raise();
    m_infoTimer.setInterval(0);
}

void CodeCompleterListView::hideEvent(QHideEvent *e)
{
    m_infoFrame->hide();
    QListView::hideEvent(e);
}

class CodeCompleterItemDelegate : public QItemDelegate
{
public:
    CodeCompleterItemDelegate(QAbstractItemView *view)
        : QItemDelegate(view), view(view) { }
    void paint(QPainter *p, const QStyleOptionViewItem& opt, const QModelIndex& idx) const {
        QStyleOptionViewItem optCopy = opt;
        optCopy.showDecorationSelected = true;
        if (view->currentIndex() == idx)
            optCopy.state |= QStyle::State_HasFocus;
        QItemDelegate::paint(p, optCopy, idx);
    }

private:
    QAbstractItemView *view;
};

CodeCompleterProxyModel::CodeCompleterProxyModel(QObject *parent)
    : QAbstractListModel(parent),m_model(0)
{
    m_seperator = "::";
    m_fuzzy = false;
}

CodeCompleterProxyModel::~CodeCompleterProxyModel()
{
    clearItems();
}

int CodeCompleterProxyModel::rowCount(const QModelIndex &) const
{
    return m_items.size();
}

QVariant CodeCompleterProxyModel::data(const QModelIndex &index, int role) const
{
    if (index.row() >= m_items.size())
        return QVariant();
    QStandardItem *item = m_items[index.row()];
    if (role == Qt::DisplayRole) {
        return item->text();
    }
    return item->data(role);
}

void CodeCompleterProxyModel::setSourceModel(QStandardItemModel *model)
{
    m_model = model;
}

void CodeCompleterProxyModel::setImportList(const QStringList &importList)
{
    m_importList = importList;
}

QStandardItemModel *CodeCompleterProxyModel::sourceModel() const
{
    return m_model;
}

QStandardItem *CodeCompleterProxyModel::item(const QModelIndex &index) const
{
    if (index.row() >= m_items.size())
        return 0;
    return m_items[index.row()];
}

bool CodeCompleterProxyModel::splitFilter(const QString &filter, QModelIndex &parent, QString &prefix, const QString &sep)
{
    if (filter.isEmpty()) {
        parent = QModelIndex();
        prefix = filter;
        return true;
    }
    QStringList filterList = filter.split(sep);
    if (filterList.size() == 1) {
        parent = QModelIndex();
        prefix = filter;
        return true;
    }
    prefix = filterList.last();
    filterList.removeLast();
    QStandardItem *root = 0;
    QStandardItem *item = 0;
    foreach (QString word, filterList) {
        item = 0;
        QModelIndex parent = m_model->indexFromItem(root);
        for (int i = 0; i < m_model->rowCount(parent); i++) {
            QModelIndex index = m_model->index(i,0,parent);
            QStandardItem *tmp = m_model->itemFromIndex(index);
            if (tmp->text() == word) {
                item = tmp;
                break;
            }
        }
        if (item == 0) {
            break;
        }
        root = item;
    }
    if (!item) {
        return false;
    }
    parent = m_model->indexFromItem(item);
    return true;
}

void CodeCompleterProxyModel::clearItems()
{
    for (int i = 0; i < m_items.size(); i++) {
        delete m_items[i];
    }
    m_items.clear();
}

//copy ContentLessThan from QtCreator source
struct ContentLessThan
{
    ContentLessThan(const QString &prefix)
        : m_prefix(prefix)
    {}

    bool operator()(const QStandardItem *a, const QStandardItem *b)
    {
        // The order is case-insensitive in principle, but case-sensitive when this
        // would otherwise mean equality
        const QString &lowera = a->text().toLower();
        const QString &lowerb = b->text().toLower();

        if (!m_prefix.isEmpty()) {
            const QString &lowerprefix = m_prefix.toLower();

            // All continuations should go before all fuzzy matches
            if (int diff = lowera.startsWith(lowerprefix) - lowerb.startsWith(lowerprefix))
                return diff > 0;
            if (int diff = a->text().startsWith(m_prefix) - b->text().startsWith(m_prefix))
                return diff > 0;
        }
        if (lowera == lowerb)
            return lessThan(a->text(), b->text());
        else
            return lessThan(lowera, lowerb);
    }

    bool lessThan(const QString &a, const QString &b)
    {
        QString::const_iterator pa = a.begin();
        QString::const_iterator pb = b.begin();

        CharLessThan charLessThan;
        enum { Letter, SmallerNumber, BiggerNumber } state = Letter;
        for (; pa != a.end() && pb != b.end(); ++pa, ++pb) {
            if (*pa == *pb)
                continue;
            if (state != Letter) {
                if (!pa->isDigit() || !pb->isDigit())
                    break;
            } else if (pa->isDigit() && pb->isDigit()) {
                if (charLessThan(*pa, *pb))
                    state = SmallerNumber;
                else
                    state = BiggerNumber;
            } else {
                return charLessThan(*pa, *pb);
            }
        }

        if (state == Letter)
            return pa == a.end() && pb != b.end();
        if (pa != a.end() && pa->isDigit())
            return false;                   //more digits
        if (pb != b.end() && pb->isDigit())
            return true;                    //fewer digits
        return state == SmallerNumber;      //same length, compare first different digit in the sequence
    }

    struct CharLessThan
    {
        bool operator()(const QChar &a, const QChar &b)
        {
            if (a == QLatin1Char('_'))
                return false;
            else if (b == QLatin1Char('_'))
                return true;
            else
                return a < b;
        }
    };

private:
    QString m_prefix;
};

int CodeCompleterProxyModel::filter(const QString &filter, int cs, LiteApi::CompletionContext ctx)
{
    if (!m_model) {
        return 0;
    }
    clearItems();
    if (ctx == LiteApi::CompleterImportContext) {
        QIcon icon("icon:liteeditor/images/keyword.png");
        if (filter.isEmpty()) {
            foreach (QString import, m_importList) {
                m_items.append(new QStandardItem(icon,import));
            }
            return m_items.size();
        }
        bool hasSep = filter.contains("/");
        if (hasSep) {
            foreach (QString import, m_importList) {
                if (import.startsWith(filter)) {
                    m_items.append(new QStandardItem(icon,import));
                }
            }
        } else {
            QList<QStandardItem*> items;
            foreach (QString import, m_importList) {
                if (import.contains("/")) {
                    foreach (QString path, import.split("/")) {
                        if (path.startsWith(filter)) {
                            items.append(new QStandardItem(icon,import));
                            break;
                        }
                    }
                } else if (import.startsWith(filter)) {
                    m_items.append(new QStandardItem(icon,import));
                }
            }
            m_items.append(items);
        }
        return m_items.size();
    }
    QModelIndex parentIndex;
    QString prefix;
    if (!splitFilter(filter,parentIndex,prefix,m_seperator)) {
        return 0;
    }
    if (prefix.isEmpty()) {
        int count = m_model->rowCount(parentIndex);
        for (int i = 0; i < count; i++) {
            QModelIndex index = m_model->index(i,0,parentIndex);
            QStandardItem *item = m_model->itemFromIndex(index);
            m_items.append(item->clone());
        }
        qStableSort(m_items.begin(), m_items.end(), ContentLessThan(prefix));
        return m_items.size();
    }

    //fuzzy completer
    if (m_fuzzy) {
        if (!prefix.isEmpty() && !( prefix[0].isLetter() || prefix[0] == '_') ) {
            return 0;
        }
        QString keyRegExp;
        if (!parentIndex.isValid()) {
            keyRegExp = "^";
        }
        foreach (const QChar &c, prefix) {
            keyRegExp += c;
            keyRegExp += "[0-9a-z_]*";
        }

        QRegExp regExp(keyRegExp);
        regExp.setCaseSensitivity(Qt::CaseInsensitive);

        int count = m_model->rowCount(parentIndex);
        QList<QStandardItem*> otherItems;
        for (int i = 0; i < count; i++) {
            QModelIndex index = m_model->index(i,0,parentIndex);
            QStandardItem *item = m_model->itemFromIndex(index);
            int n = regExp.indexIn(item->text());
            if (n == 0) {
                m_items.append(item->clone());
            } else if (n > 0) {
                otherItems.append(item->clone());
            }
        }
        m_items.append(otherItems);
        return m_items.size();
    }

    QString keyRegExp;
    keyRegExp += QLatin1Char('^');
    bool first = true;
    const QLatin1String uppercaseWordContinuation("[a-z0-9_]*");
    const QLatin1String lowercaseWordContinuation("(?:[a-zA-Z0-9]*_)?");
    foreach (const QChar &c, prefix) {
        if (cs == LiteApi::CaseInsensitive ||
                (cs == LiteApi::FirstLetterCaseSensitive && !first)) {

            keyRegExp += QLatin1String("(?:");
            if (!first)
                keyRegExp += uppercaseWordContinuation;
            keyRegExp += QRegExp::escape(c.toUpper());
            keyRegExp += QLatin1Char('|');
            if (!first)
                keyRegExp += lowercaseWordContinuation;
            keyRegExp += QRegExp::escape(c.toLower());
            keyRegExp += QLatin1Char(')');
        } else {
            if (!first) {
                if (c.isUpper())
                    keyRegExp += uppercaseWordContinuation;
                else
                    keyRegExp += lowercaseWordContinuation;
            }
            keyRegExp += QRegExp::escape(c);
        }

        first = false;
    }

    QRegExp regExp(keyRegExp);

    int count = m_model->rowCount(parentIndex);
    for (int i = 0; i < count; i++) {
        QModelIndex index = m_model->index(i,0,parentIndex);
        QStandardItem *item = m_model->itemFromIndex(index);
        if (regExp.indexIn(item->text()) == 0) {
            m_items.append(item->clone());
        }
    }
    qStableSort(m_items.begin(), m_items.end(), ContentLessThan(prefix));

    return m_items.size();
}

void CodeCompleterProxyModel::setSeparator(const QString &separator)
{
    m_seperator = separator;
}

QString CodeCompleterProxyModel::separator() const
{
    return m_seperator;
}

void CodeCompleterProxyModel::setFuzzy(bool b)
{
    m_fuzzy = b;
}

bool CodeCompleterProxyModel::isFuzzy() const
{
    return m_fuzzy;
}

CodeCompleterEx::CodeCompleterEx(QObject *parent)
    : QObject(parent), m_widget(0)
{
    maxVisibleItems = 10;
    m_eatFocusOut = true;
    m_hiddenBecauseNoMatch = false;
    m_cs = Qt::CaseInsensitive;
    m_ctx = LiteApi::CompleterCodeContext;
    m_wrap = true;
    m_popup = new CodeCompleterListView;
    m_popup->setUniformItemSizes(true);
    m_popup->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_popup->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_popup->setSelectionMode(QAbstractItemView::SingleSelection);
    m_popup->setItemDelegate(new CodeCompleterItemDelegate(m_popup));
    m_popup->setModelColumn(0);

    m_popup->setParent(0, Qt::Popup);
    m_popup->setFocusPolicy(Qt::NoFocus);

    m_popup->installEventFilter(this);

    m_proxy = new CodeCompleterProxyModel(this);
    m_popup->setModel(m_proxy);

    QObject::connect(m_popup, SIGNAL(clicked(QModelIndex)),
                     this, SLOT(completerActivated(QModelIndex)));
    QObject::connect(m_popup, SIGNAL(activated(QModelIndex)),
                     m_popup, SLOT(hide()));

}

CodeCompleterEx::~CodeCompleterEx()
{
    if (m_popup) {
        delete m_popup;
    }
}

void CodeCompleterEx::setModel(QStandardItemModel *c)
{
    m_proxy->setSourceModel(c);
}

QAbstractItemModel *CodeCompleterEx::model() const
{
    return m_proxy->sourceModel();
}

void CodeCompleterEx::setImportList(const QStringList &importList)
{
    m_proxy->setImportList(importList);
}

void CodeCompleterEx::setSeparator(const QString &separator)
{
    m_proxy->setSeparator(separator);
}

QString CodeCompleterEx::separator() const
{
    return m_proxy->separator();
}

void CodeCompleterEx::setCompletionPrefix(const QString &prefix)
{
    m_prefix = prefix;
    if (m_proxy->filter(prefix,m_cs,m_ctx) <= 0) {
        if (m_popup->isVisible()) {
            m_popup->close();
        }
        return;
    }
    m_popup->reset();
}

QString CodeCompleterEx::completionPrefix() const
{
    return m_prefix;
}

void CodeCompleterEx::setCompletionContext(LiteApi::CompletionContext ctx)
{
    m_ctx = ctx;
}

LiteApi::CompletionContext CodeCompleterEx::completionContext() const
{
    return m_ctx;
}

void CodeCompleterEx::updateFilter()
{
    if (m_proxy->filter(m_prefix,m_cs) <= 0) {
        if (m_popup->isVisible()) {
            m_popup->close();
        }
        return;
    }
    m_popup->reset();
}

void CodeCompleterEx::complete(const QRect &rect)
{
    if (m_proxy->rowCount() == 0) {
        return;
    }

    const QRect screen = QApplication::desktop()->availableGeometry(m_widget);
    Qt::LayoutDirection dir = m_widget->layoutDirection();
    QPoint pos;
    int rh, w;
    int h = (m_popup->sizeHintForRow(0) * qMin(maxVisibleItems, m_popup->model()->rowCount()) + 3)+3;
    QScrollBar *hsb = m_popup->horizontalScrollBar();
    if (hsb && hsb->isVisible())
        h += m_popup->horizontalScrollBar()->sizeHint().height();

    if (rect.isValid()) {
        rh = rect.height();
        w = rect.width();
        pos = m_widget->mapToGlobal(dir == Qt::RightToLeft ? rect.bottomRight() : rect.bottomLeft());
    } else {
        rh = m_widget->height();
        pos = m_widget->mapToGlobal(QPoint(0, m_widget->height() - 2));
        w = m_widget->width();
    }

    if (w > screen.width())
        w = screen.width();
    if ((pos.x() + w) > (screen.x() + screen.width()))
        pos.setX(screen.x() + screen.width() - w);
    if (pos.x() < screen.x())
        pos.setX(screen.x());

    int top = pos.y() - rh - screen.top() + 2;
    int bottom = screen.bottom() - pos.y();
    h = qMax(h, m_popup->minimumHeight());
    if (h > bottom) {
        h = qMin(qMax(top, bottom), h);

        if (top > bottom)
            pos.setY(pos.y() - h - rh + 2);
    }
    w = qMax(w,200);
    m_popup->setGeometry(pos.x(), pos.y(), w, h);

    if (!m_popup->isVisible())
        m_popup->show();
}

QWidget *CodeCompleterEx::widget() const
{
    return m_widget;
}

void CodeCompleterEx::setWidget(QWidget *widget)
{
    if (m_widget == widget) {
        return;
    }
    if (m_widget) {
        m_widget->removeEventFilter(this);
    }
    m_widget = widget;
//    Qt::FocusPolicy origPolicy = Qt::NoFocus;
//    if (widget)
//        origPolicy = widget->focusPolicy();
//    m_popup->setParent(0, Qt::Popup);
//    m_popup->setFocusPolicy(Qt::NoFocus);
//    if (widget)
//        widget->setFocusPolicy(origPolicy);
    if (m_widget) {
        m_widget->installEventFilter(this);
        m_popup->setFocusProxy(m_widget);
    }
}

QModelIndex CodeCompleterEx::currentIndex() const
{
    return m_popup->currentIndex();
}

QString CodeCompleterEx::currentCompletion() const
{
    QModelIndex index = m_popup->currentIndex();
    if (index.isValid()) {
        QStandardItem *item = m_proxy->item(index);
        if (item) {
            return item->text();
        }
    }
    return QString();
}

void CodeCompleterEx::setCaseSensitivity(Qt::CaseSensitivity cs)
{
    m_cs = cs;
}

Qt::CaseSensitivity CodeCompleterEx::caseSensitivity() const
{
    return m_cs;
}

QAbstractItemView *CodeCompleterEx::popup() const
{
    return m_popup;
}

QAbstractItemModel *CodeCompleterEx::completionModel() const
{
    return m_proxy;
}

void CodeCompleterEx::setWrapAround(bool wrap)
{
    m_wrap = wrap;
}

bool CodeCompleterEx::wrapAround() const
{
    return m_wrap;
}

void CodeCompleterEx::setFuzzy(bool b)
{
    m_proxy->setFuzzy(b);
}

bool CodeCompleterEx::isFuzzy() const
{
    return m_proxy->isFuzzy();
}

void CodeCompleterEx::completerActivated(QModelIndex index)
{
    if (m_popup->isVisible()) {
        m_popup->close();
    }
    emit activated(index);
}

bool CodeCompleterEx::eventFilter(QObject *o, QEvent *e)
{
    if (m_eatFocusOut && o == m_widget && e->type() == QEvent::FocusOut) {
        m_hiddenBecauseNoMatch = false;
        if (m_popup && m_popup->isVisible())
            return true;
    }

    if (o != m_popup)
        return QObject::eventFilter(o, e);

    switch (e->type()) {
    case QEvent::KeyPress: {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);

        QModelIndex curIndex = m_popup->currentIndex();
        //QModelIndexList selList = m_popup->selectionModel()->selectedIndexes();

        const int key = ke->key();
        // In UnFilteredPopup mode, select the current item
//        if ((key == Qt::Key_Up || key == Qt::Key_Down) && selList.isEmpty() && curIndex.isValid()
//            && m_mode == QCompleter::UnfilteredPopupCompletion) {
//            m_popup->setCurrentIndex(curIndex);
//              return true;
//        }

        // Handle popup navigation keys. These are hardcoded because up/down might make the
        // widget do something else (lineedit cursor moves to home/end on mac, for instance)
        switch (key) {
        case Qt::Key_End:
        case Qt::Key_Home:
            if (ke->modifiers() & Qt::ControlModifier)
                return false;
            break;
        case Qt::Key_N:
        case Qt::Key_P:
            // select next/previous completion
            if (ke->modifiers() == Qt::ControlModifier) {
                int change = (ke->key() == Qt::Key_N) ? 1 : -1;
                int nrows = m_proxy->rowCount();
                int row = m_popup->currentIndex().row();
                int newRow = (row + change + nrows) % nrows;
                if (newRow == row + change || !ke->isAutoRepeat()) {
                    QModelIndex index = m_proxy->index(newRow, 0);
                    m_popup->setCurrentIndex(index);
                }
                return true;
            }
            break;

        case Qt::Key_Up:
            if (curIndex.row() == 0) {
                if (m_wrap) {
                    int rowCount = m_proxy->rowCount();
                    QModelIndex lastIndex = m_proxy->index(rowCount - 1, 0);
                    m_popup->setCurrentIndex(lastIndex);
                }
                return true;
            }
            return false;

        case Qt::Key_Down:
            if (curIndex.row() == m_proxy->rowCount() - 1) {
                if (m_wrap) {
                    QModelIndex firstIndex = m_proxy->index(0, 0);
                    m_popup->setCurrentIndex(firstIndex);
                }
                return true;
            }
            return false;

        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            return false;
        }

        // Send the event to the widget. If the widget accepted the event, do nothing
        // If the widget did not accept the event, provide a default implementation
        m_eatFocusOut = false;
        (static_cast<QObject *>(m_widget))->event(ke);
        m_eatFocusOut = true;
        if (!m_widget || e->isAccepted() || !m_popup->isVisible()) {
            // widget lost focus, hide the popup
            if (m_widget && (!m_widget->hasFocus()
#ifdef QT_KEYPAD_NAVIGATION
                || (QApplication::keypadNavigationEnabled() && !m_widget->hasEditFocus())
#endif
                ))
                m_popup->hide();
            if (e->isAccepted())
                return true;
        }

        // default implementation for keys not handled by the widget when popup is open
        switch (key) {
#ifdef QT_KEYPAD_NAVIGATION
        case Qt::Key_Select:
            if (!QApplication::keypadNavigationEnabled())
                break;
#endif
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Tab:
            m_popup->hide();
            if (curIndex.isValid())
                //m__q_complete(curIndex);
                this->completerActivated(curIndex);
            break;

        case Qt::Key_F4:
            if (ke->modifiers() & Qt::AltModifier)
                m_popup->hide();
            break;

        case Qt::Key_Backtab:
        case Qt::Key_Escape:
            m_popup->hide();
            break;

        default:
            break;
        }

        return true;
    }

#ifdef QT_KEYPAD_NAVIGATION
    case QEvent::KeyRelease: {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        if (QApplication::keypadNavigationEnabled() && ke->key() == Qt::Key_Back) {
            // Send the event to the 'widget'. This is what we did for KeyPress, so we need
            // to do the same for KeyRelease, in case the widget's KeyPress event set
            // up something (such as a timer) that is relying on also receiving the
            // key release. I see this as a bug in Qt, and should really set it up for all
            // the affected keys. However, it is difficult to tell how this will affect
            // existing code, and I can't test for every combination!
            m_eatFocusOut = false;
            static_cast<QObject *>(m_widget)->event(ke);
            m_eatFocusOut = true;
        }
        break;
    }
#endif
    case QEvent::MouseButtonPress: {
#ifdef QT_KEYPAD_NAVIGATION
        if (QApplication::keypadNavigationEnabled()) {
            // if we've clicked in the widget (or its descendant), let it handle the click
            QWidget *source = qobject_cast<QWidget *>(o);
            if (source) {
                QPoint pos = source->mapToGlobal((static_cast<QMouseEvent *>(e))->pos());
                QWidget *target = QApplication::widgetAt(pos);
                if (target && (m_widget->isAncestorOf(target) ||
                    target == m_widget)) {
                    m_eatFocusOut = false;
                    static_cast<QObject *>(target)->event(e);
                    m_eatFocusOut = true;
                    return true;
                }
            }
        }
#endif
        if (!m_popup->underMouse()) {
            m_popup->hide();
            return true;
        }
        }
        return false;

    case QEvent::InputMethod:
        QApplication::sendEvent(m_widget, e);
        break;
    case QEvent::ShortcutOverride:    {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        switch (ke->key()) {
        case Qt::Key_N:
        case Qt::Key_P:
            if (ke->modifiers() == Qt::ControlModifier) {
                e->accept();
                return true;
            }
        }
        QApplication::sendEvent(m_widget, e);
        break;
        }
    default:
        return false;
    }
    return false;
}
