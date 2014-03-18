/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#include "fancylineedit.h"

#include <QtCore/QEvent>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QPropertyAnimation>
#include <QApplication>
#include <QMenu>
#include <QMouseEvent>
#include <QLabel>
#include <QAbstractButton>
#ifndef QT_NO_PRINTER
#include <QPainter>
#endif
#include <QStyle>
#include <QPaintEvent>

enum { margin = 6 };

#define ICONBUTTON_HEIGHT 18
#define FADE_TIME 160

namespace Utils {

// --------- FancyLineEditPrivate
class FancyLineEditPrivate : public QObject {
public:
    explicit FancyLineEditPrivate(FancyLineEdit *parent);

    virtual bool eventFilter(QObject *obj, QEvent *event);

    FancyLineEdit  *m_lineEdit;
    QPixmap m_pixmap[2];
    QMenu *m_menu[2];
    bool m_menuTabFocusTrigger[2];
    IconButton *m_iconbutton[2];
    bool m_iconEnabled[2];
};


FancyLineEditPrivate::FancyLineEditPrivate(FancyLineEdit *parent) :
    QObject(parent),
    m_lineEdit(parent)
{
    for (int i = 0; i < 2; ++i) {
        m_menu[i] = 0;
        m_menuTabFocusTrigger[i] = false;
        m_iconbutton[i] = new IconButton(parent);
        m_iconbutton[i]->installEventFilter(this);
        m_iconbutton[i]->hide();
        m_iconbutton[i]->setAutoHide(false);
        m_iconEnabled[i] = false;
    }
}

bool FancyLineEditPrivate::eventFilter(QObject *obj, QEvent *event)
{
    int buttonIndex = -1;
    for (int i = 0; i < 2; ++i) {
        if (obj == m_iconbutton[i]) {
            buttonIndex = i;
            break;
        }
    }
    if (buttonIndex == -1)
        return QObject::eventFilter(obj, event);
    switch (event->type()) {
    case QEvent::FocusIn:
        if (m_menuTabFocusTrigger[buttonIndex] && m_menu[buttonIndex]) {
            m_lineEdit->setFocus();
            m_menu[buttonIndex]->exec(m_iconbutton[buttonIndex]->mapToGlobal(
                    m_iconbutton[buttonIndex]->rect().center()));
            return true;
        }
    default:
        break;
    }
    return QObject::eventFilter(obj, event);
}


// --------- FancyLineEdit
FancyLineEdit::FancyLineEdit(QWidget *parent) :
    QLineEdit(parent),
    m_d(new FancyLineEditPrivate(this))
{
    ensurePolished();
    updateMargins();

    connect(this, SIGNAL(textChanged(QString)), this, SLOT(checkButtons(QString)));
    connect(m_d->m_iconbutton[Left], SIGNAL(clicked()), this, SLOT(iconClicked()));
    connect(m_d->m_iconbutton[Right], SIGNAL(clicked()), this, SLOT(iconClicked()));
}

void FancyLineEdit::checkButtons(const QString &text)
{
    if (m_oldText.isEmpty() || text.isEmpty()) {
        for (int i = 0; i < 2; ++i) {
            if (m_d->m_iconbutton[i]->hasAutoHide())
                m_d->m_iconbutton[i]->animateShow(!text.isEmpty());
        }
        m_oldText = text;
    }
}

FancyLineEdit::~FancyLineEdit()
{
}

void FancyLineEdit::setButtonVisible(Side side, bool visible)
{
    m_d->m_iconbutton[side]->setVisible(visible);
    m_d->m_iconEnabled[side] = visible;
    updateMargins();
    updateButtonPositions();
}

bool FancyLineEdit::isButtonVisible(Side side) const
{
    return m_d->m_iconEnabled[side];
}

void FancyLineEdit::iconClicked()
{
    IconButton *button = qobject_cast<IconButton *>(sender());
    int index = -1;
    for (int i = 0; i < 2; ++i)
        if (m_d->m_iconbutton[i] == button)
            index = i;
    if (index == -1)
        return;
    if (m_d->m_menu[index]) {
        m_d->m_menu[index]->exec(QCursor::pos());
    } else {
        emit buttonClicked((Side)index);
        if (index == Left)
            emit leftButtonClicked();
        else if (index == Right)
            emit rightButtonClicked();
    }
}

void FancyLineEdit::updateMargins()
{
    bool leftToRight = (layoutDirection() == Qt::LeftToRight);
    Side realLeft = (leftToRight ? Left : Right);
    Side realRight = (leftToRight ? Right : Left);

    int leftMargin = m_d->m_iconbutton[realLeft]->pixmap().width() + 8;
    int rightMargin = m_d->m_iconbutton[realRight]->pixmap().width() + 8;
    // Note KDE does not reserve space for the highlight color
    if (style()->inherits("OxygenStyle")) {
        leftMargin = qMax(24, leftMargin);
        rightMargin = qMax(24, rightMargin);
    }

    QMargins margins((m_d->m_iconEnabled[realLeft] ? leftMargin : 0), 0,
                     (m_d->m_iconEnabled[realRight] ? rightMargin : 0), 0);

    setTextMargins(margins);
}

void FancyLineEdit::updateButtonPositions()
{
    QRect contentRect = rect();
    for (int i = 0; i < 2; ++i) {
        Side iconpos = (Side)i;
        if (layoutDirection() == Qt::RightToLeft)
            iconpos = (iconpos == Left ? Right : Left);

        if (iconpos == FancyLineEdit::Right) {
            const int iconoffset = textMargins().right() + 4;
            m_d->m_iconbutton[i]->setGeometry(contentRect.adjusted(width() - iconoffset, 0, 0, 0));
        } else {
            const int iconoffset = textMargins().left() + 4;
            m_d->m_iconbutton[i]->setGeometry(contentRect.adjusted(0, 0, -width() + iconoffset, 0));
        }
    }
}

void FancyLineEdit::resizeEvent(QResizeEvent *)
{
    updateButtonPositions();
}

void FancyLineEdit::setButtonPixmap(Side side, const QPixmap &buttonPixmap)
{
    m_d->m_iconbutton[side]->setPixmap(buttonPixmap);
    updateMargins();
    updateButtonPositions();
    update();
}

QPixmap FancyLineEdit::buttonPixmap(Side side) const
{
    return m_d->m_pixmap[side];
}

void FancyLineEdit::setButtonMenu(Side side, QMenu *buttonMenu)
{
     m_d->m_menu[side] = buttonMenu;
     m_d->m_iconbutton[side]->setIconOpacity(1.0);
 }

QMenu *FancyLineEdit::buttonMenu(Side side) const
{
    return  m_d->m_menu[side];
}

bool FancyLineEdit::hasMenuTabFocusTrigger(Side side) const
{
    return m_d->m_menuTabFocusTrigger[side];
}

void FancyLineEdit::setMenuTabFocusTrigger(Side side, bool v)
{
    if (m_d->m_menuTabFocusTrigger[side] == v)
        return;

    m_d->m_menuTabFocusTrigger[side] = v;
    m_d->m_iconbutton[side]->setFocusPolicy(v ? Qt::TabFocus : Qt::NoFocus);
}

bool FancyLineEdit::hasAutoHideButton(Side side) const
{
    return m_d->m_iconbutton[side]->hasAutoHide();
}

void FancyLineEdit::setAutoHideButton(Side side, bool h)
{
    m_d->m_iconbutton[side]->setAutoHide(h);
    if (h)
        m_d->m_iconbutton[side]->setIconOpacity(text().isEmpty() ?  0.0 : 1.0);
    else
        m_d->m_iconbutton[side]->setIconOpacity(1.0);
}

void FancyLineEdit::setButtonToolTip(Side side, const QString &tip)
{
    m_d->m_iconbutton[side]->setToolTip(tip);
}

void FancyLineEdit::setButtonFocusPolicy(Side side, Qt::FocusPolicy policy)
{
    m_d->m_iconbutton[side]->setFocusPolicy(policy);
}

// IconButton - helper class to represent a clickable icon

IconButton::IconButton(QWidget *parent)
    : QAbstractButton(parent), m_autoHide(false)
{
    setCursor(Qt::ArrowCursor);
    setFocusPolicy(Qt::NoFocus);
}

void IconButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    // Note isDown should really use the active state but in most styles
    // this has no proper feedback
    QIcon::Mode state = QIcon::Disabled;
    if (isEnabled())
        state = isDown() ? QIcon::Selected : QIcon::Normal;
    QRect pixmapRect = QRect(0, 0, m_pixmap.width(), m_pixmap.height());
    pixmapRect.moveCenter(rect().center());

    if (m_autoHide)
        painter.setOpacity(m_iconOpacity);

    painter.drawPixmap(pixmapRect, m_pixmap);
}

void IconButton::animateShow(bool visible)
{
    if (visible) {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "iconOpacity");
        animation->setDuration(FADE_TIME);
        animation->setEndValue(1.0);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "iconOpacity");
        animation->setDuration(FADE_TIME);
        animation->setEndValue(0.0);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

} // namespace Utils
