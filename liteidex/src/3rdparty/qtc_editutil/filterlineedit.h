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

#ifndef FILTERLINEEDIT_H
#define FILTERLINEEDIT_H

#include "fancylineedit.h"

namespace Utils {

/* A fancy line edit customized for filtering purposes with a clear button. */

class FilterLineEdit : public FancyLineEdit
{
    Q_OBJECT
public:
    explicit FilterLineEdit(int dly,QWidget *parent = 0);

signals:
    void filterChanged(const QString &);

private slots:
    void slotTextChanged();
    void dlyTextChanged();

private:
    QString m_lastFilterText;
    int m_dlytimer;
    QTimer  *m_timer;
};

} // namespace Utils

#endif // FILTERLINEEDIT_H
