/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
// Module: htmlutil.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: htmlutil.cpp,v 1.0 2011-8-12 visualfc Exp $

#include "htmlutil.h"
#include <QStringList>
#include <QDomDocument>
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

HtmlUtil::HtmlUtil()
{
}
/*
<!--{
    "Title": "Getting Started",
    "Path":  "/install/"
}-->
*/

QString HtmlUtil::findTitle(const QString &data)
{
    QRegExp reg("<!--([\\w\\s\\n{}\":/,]*)-->");
    int n = reg.indexIn(data);
    if (n < 0) {
        return QString();
    }
    QRegExp reg1("Title[\"\\s:]*([\\w\\s]*)[\\s\"]*");
    n = reg1.indexIn(reg.cap(1));
    if (n < 0) {
        return QString();
    }
    return reg1.cap(1);
}

QString HtmlUtil::docToNavdoc(const QString &data, QString &header, QString &nav)
{
    QDomDocument doc;
    QStringList srcLines = data.split("\n");
    QStringList navLines;
    QStringList dstLines;
    navLines.append("<table class=\"unruled\"><tbody><tr><td class=\"first\"><dl>");
    int index = 0;
    if (srcLines.length() >= 1) {
        header = findTitle(data);
        if (header.isEmpty()) {
            //<!-- How to Write Go Code -->
            QString line = srcLines.at(0);
            QRegExp reg("<!--([\\w\\s]*)-->");
            if (reg.indexIn(line) >= 0) {
                header = reg.cap(1);
                header.trimmed();
            }
        }
    }
    foreach(QString source, srcLines) {
        QString line = source.trimmed();
        index++;
        if (line.length() >= 10) {
            if (line.left(3) == "<h2") {
                if (doc.setContent(line)) {
                    QDomElement e = doc.firstChildElement("h2");
                    if (!e.isNull()) {
                        QString text = e.text();
                        QString id = e.attribute("id");
                        if (id.isEmpty()) {
                            id = QString("tmp_%1").arg(index);
                            e.setAttribute("id",id);
                        }
                        //<span class="navtop"><a href="#top">[Top]</a></span>
                        QDomElement span = doc.createElement("span");
                        span.setAttribute("class","navtop");
                        QDomElement a = doc.createElement("a");
                        a.setAttribute("href","#top");
                        QDomText top = doc.createTextNode("[Top]");
                        a.appendChild(top);
                        span.appendChild(a);
                        e.appendChild(span);

                        source = doc.toString();
                        navLines << QString("<dt><a href=\"#%1\">%2</a></dt>").arg(id).arg(text);
                    }
                }
            }
            else if (line.left(3) == "<h3") {
                if (doc.setContent(line)) {
                    QDomElement e = doc.firstChildElement("h3");
                    if (!e.isNull()) {
                        QString text = e.text();
                        QString id = e.attribute("id");
                        if (id.isEmpty()) {
                            id = QString("tmp_%1").arg(index);
                            e.setAttribute("id",id);
                        }
                        source = doc.toString();
                        navLines << QString("<dd><a href=\"#%1\">%2</a></dd>").arg(id).arg(text);
                    }
                }
           }
        }
        dstLines.append(source);
    }
    navLines.append("</dl></td><td><dl></dl></td><tr></tbody></table>");
    nav = navLines.join("");
    return dstLines.join("\n");
}
