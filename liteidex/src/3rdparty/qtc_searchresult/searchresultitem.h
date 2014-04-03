/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef SEARCHRESULTITEM_H
#define SEARCHRESULTITEM_H

#include <QStringList>
#include <QVariant>
#include <QIcon>

namespace Find {

enum AddMode {
    AddSorted,
    AddOrdered
};

class SearchResultItem
{
public:
    SearchResultItem()
        : textMarkPos(-1),
        textMarkLength(0),
        lineNumber(-1),
        useTextEditorFont(false)
    {
    }

    SearchResultItem(const SearchResultItem &other)
        : path(other.path),
        text(other.text),
        textMarkPos(other.textMarkPos),
        textMarkLength(other.textMarkLength),
        icon(other.icon),
        lineNumber(other.lineNumber),
        useTextEditorFont(other.useTextEditorFont),
        userData(other.userData)
    {
    }

    QStringList path; // hierarchy to the parent item of this item
    QString text; // text to show for the item itself
    int textMarkPos; // 0-based starting position for a mark (-1 for no mark)
    int textMarkLength; // length of the mark (0 for no mark)
    QIcon icon; // icon to show in front of the item (by be null icon to hide)
    int lineNumber; // (0 or -1 for no line number)
    bool useTextEditorFont;
    QVariant userData; // user data for identification of the item
};

//inline SearchResultItem makeResult(const QString &fileName, int lineNumber, const QString &rowText,
//    int searchTermStart, int searchTermLength, const QVariant &userData)
//{
//    SearchResultItem item;
//    item.path = QStringList() << QDir::toNativeSeparators(fileName);
//    item.lineNumber = lineNumber;
//    item.text = rowText;
//    item.textMarkPos = searchTermStart;
//    item.textMarkLength = searchTermLength;
//    item.useTextEditorFont = true;
//    item.userData = userData;
//    return item;
//}

} // namespace Find

Q_DECLARE_METATYPE(Find::SearchResultItem)

#endif //SEARCHRESULTITEM_H
