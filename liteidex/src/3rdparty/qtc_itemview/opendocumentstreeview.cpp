/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company.  For licensing terms and
** conditions see http://www.qt.io/terms-conditions.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, The Qt Company gives you certain additional
** rights.  These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "opendocumentstreeview.h"

#include <QApplication>
#include <QHeaderView>
#include <QPainter>
#include <QStyledItemDelegate>

#include <QDebug>

namespace Core {
namespace Internal {

class OpenDocumentsDelegate : public QStyledItemDelegate
{
public:
    explicit OpenDocumentsDelegate(QObject *parent = 0);

    void setCloseButtonVisible(bool visible);
    void handlePressed(const QModelIndex &index);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    mutable QModelIndex pressedIndex;
    bool closeButtonVisible;
};

OpenDocumentsDelegate::OpenDocumentsDelegate(QObject *parent)
 : QStyledItemDelegate(parent),
   closeButtonVisible(true)
{
}

void OpenDocumentsDelegate::setCloseButtonVisible(bool visible)
{
    closeButtonVisible = visible;
}

void OpenDocumentsDelegate::handlePressed(const QModelIndex &index)
{
    if (index.column() == 0)
        pressedIndex = index;
}

void OpenDocumentsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const
{
    if (option.state & QStyle::State_MouseOver) {
        if ((QApplication::mouseButtons() & Qt::LeftButton) == 0)
            pressedIndex = QModelIndex();
        QBrush brush = option.palette.alternateBase();
        if (index == pressedIndex)
            brush = option.palette.dark();
        painter->fillRect(option.rect, brush);
    }

    QStyledItemDelegate::paint(painter, option, index);

    if (closeButtonVisible && index.column() == 0 && option.state & QStyle::State_MouseOver) {
        //const QIcon icon(QLatin1String((option.state & QStyle::State_Selected) ?
        //                               ":/images/closebutton.png" : ":/images/darkclosebutton.png"));

        const QIcon icon("icon:images/darkclosebutton.png");

        QRect iconRect(option.rect.right() - option.rect.height(),
                       option.rect.top(),
                       option.rect.height(),
                       option.rect.height());

        icon.paint(painter, iconRect, Qt::AlignRight | Qt::AlignVCenter);
    }

}

} // namespace Internal

OpenDocumentsTreeView::OpenDocumentsTreeView(QWidget *parent) :
    Utils::TreeView(parent)
{
    m_delegate = new Internal::OpenDocumentsDelegate(this);
    setItemDelegate(m_delegate);
    setIndentation(0);
    setUniformRowHeights(true);
    setTextElideMode(Qt::ElideMiddle);
    setFrameStyle(QFrame::NoFrame);
    setAttribute(Qt::WA_MacShowFocusRect, false);
    viewport()->setAttribute(Qt::WA_Hover);

    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setActivationMode(Utils::SingleClickActivation);

    installEventFilter(this);
    viewport()->installEventFilter(this);

    connect(this,SIGNAL(pressed(QModelIndex)),this,SLOT(handlePressed(QModelIndex)));
}

void OpenDocumentsTreeView::setModel(QAbstractItemModel *model)
{
    Utils::TreeView::setModel(model);
    header()->hide();
    header()->setStretchLastSection(true);
//#if QT_VERSION >= 0x050000
//    header()->setSectionResizeMode(0, QHeaderView::Stretch);
//    //header()->setSectionResizeMode(1, QHeaderView::Fixed);
//#else
//    header()->setResizeMode(0,QHeaderView::Stretch);
//    //header()->setResizeMode(1,QHeaderView::Fixed);
//#endif
    //header()->resizeSection(1, 16);
}

void OpenDocumentsTreeView::setCloseButtonVisible(bool visible)
{
    m_delegate->setCloseButtonVisible(visible);
}

bool OpenDocumentsTreeView::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this && event->type() == QEvent::KeyPress
            && currentIndex().isValid()) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        if ((ke->key() == Qt::Key_Delete
                   || ke->key() == Qt::Key_Backspace)
                && ke->modifiers() == 0) {
            emit closeActivated(currentIndex());
        }
    } else if (obj == viewport()
             && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent * me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::MiddleButton
                && me->modifiers() == Qt::NoModifier) {
            QModelIndex index = indexAt(me->pos());
            if (index.isValid()) {
                emit closeActivated(index);
                return true;
            }
        } else if (me->button() == Qt::LeftButton
                   && me->modifiers() == Qt::NoModifier) {
            QModelIndex index = indexAt(me->pos());
            if (index.isValid()) {
                QRect rc = this->visualRect(index);
                QRect rc2 = QRect(rc.right()-rc.height(),rc.top(),rc.height(),rc.height());
                if (rc2.contains(me->pos())) {
                     emit closeActivated(index);
                     return true;
                }
            }
        }
    }
    return false;
}

void OpenDocumentsTreeView::handlePressed(const QModelIndex &index)
{
    m_delegate->handlePressed(index);
}

} // namespace Core
