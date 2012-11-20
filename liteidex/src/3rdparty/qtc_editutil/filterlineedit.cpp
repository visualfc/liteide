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

#include "filterlineedit.h"
#include <QTimer>

namespace Utils {

FilterLineEdit::FilterLineEdit(int dly, QWidget *parent) :
   FancyLineEdit(parent),
   m_lastFilterText(text()),
    m_dlytimer(dly)
{
    // KDE has custom icons for this. Notice that icon namings are counter intuitive
    // If these icons are not avaiable we use the freedesktop standard name before
    // falling back to a bundled resource

    QIcon icon = QIcon::fromTheme(layoutDirection() == Qt::LeftToRight ?
                     QLatin1String("edit-clear-locationbar-rtl") :
                     QLatin1String("edit-clear-locationbar-ltr"),
                     QIcon::fromTheme(QLatin1String("edit-clear"), QIcon(QLatin1String("icon:images/editclear.png"))));

    setButtonPixmap(Right, icon.pixmap(16));
    setButtonVisible(Right, true);
    setPlaceholderText(tr("Filter"));
    setButtonToolTip(Right, tr("Clear text"));
    setAutoHideButton(Right, true);
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(dlyTextChanged()));
    connect(this, SIGNAL(rightButtonClicked()), this, SLOT(clear()));
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged()));
}

void FilterLineEdit::slotTextChanged()
{
    const QString newlyTypedText = text();
    if (newlyTypedText != m_lastFilterText) {
        m_lastFilterText = newlyTypedText;
        if (m_dlytimer == 0) {
            emit filterChanged(m_lastFilterText);
        } else {
            m_timer->start(m_dlytimer);
        }
    }
}

void FilterLineEdit::dlyTextChanged()
{
    emit filterChanged(m_lastFilterText);
}

} // namespace Utils
