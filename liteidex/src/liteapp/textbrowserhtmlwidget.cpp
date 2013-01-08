/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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
// Module: textbrowserhtmlwidget.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "textbrowserhtmlwidget.h"
#include <QTextBrowser>
#include <QScrollBar>
#include <QFileInfo>
#include <QFile>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

TextBrowserHtmlWidget::TextBrowserHtmlWidget(QObject *parent) :
    IHtmlWidget(parent)
{
    m_widget = new QTextBrowser;
    m_widget->setOpenLinks(false);
    m_widget->setOpenExternalLinks(false);
    connect(m_widget,SIGNAL(anchorClicked(QUrl)),this,SIGNAL(linkClicked(QUrl)));
    connect(m_widget,SIGNAL(highlighted(QUrl)),this,SIGNAL(linkHovered(QUrl)));
}

QWidget *TextBrowserHtmlWidget::widget() const
{
    return m_widget;
}

QString TextBrowserHtmlWidget::className() const
{
    return QLatin1String("QTextBrowser");
}

void TextBrowserHtmlWidget::setSerachPaths(const QStringList &paths)
{
    m_widget->setSearchPaths(paths);
}

void TextBrowserHtmlWidget::setHtml(const QString &html, const QUrl &url)
{
    m_widget->setHtml(html);
    m_url = url;
    if (!url.isEmpty()) {
        QString file = url.toLocalFile();
        if (!file.isEmpty()) {
            QFileInfo info(file);
            QStringList paths = m_widget->searchPaths();
            paths.append(info.path());
            paths.removeDuplicates();
            m_widget->setSearchPaths(paths);
        }
    }
    emit loadFinished(true);
}

QUrl TextBrowserHtmlWidget::url() const
{
    return m_url;
}

void TextBrowserHtmlWidget::clear()
{
    m_widget->clear();
}

void TextBrowserHtmlWidget::scroolToAnchor(const QString &anchor)
{
    m_widget->scrollToAnchor(anchor);
}

void TextBrowserHtmlWidget::setScrollBarValue(Qt::Orientation orientation, int value)
{
    if (orientation == Qt::Horizontal) {
        m_widget->horizontalScrollBar()->setValue(value);
    } else {
        m_widget->verticalScrollBar()->setValue(value);
    }
}

int TextBrowserHtmlWidget::scrollBarValue(Qt::Orientation orientation) const
{
    if (orientation == Qt::Horizontal) {
        return m_widget->horizontalScrollBar()->value();
    }
    return m_widget->verticalScrollBar()->value();
}

int TextBrowserHtmlWidget::scrollBarMinimum(Qt::Orientation orientation) const
{
    if (orientation == Qt::Horizontal) {
        return m_widget->horizontalScrollBar()->minimum();
    }
    return m_widget->verticalScrollBar()->minimum();

}

int TextBrowserHtmlWidget::scrollBarMaximum(Qt::Orientation orientation) const
{
    if (orientation == Qt::Horizontal) {
        return m_widget->horizontalScrollBar()->maximum();
    }
    return m_widget->verticalScrollBar()->maximum();
}

QString TextBrowserHtmlWidget::selectedText() const
{
    return m_widget->textCursor().selectedText();
}

bool TextBrowserHtmlWidget::findText(const QString &exp, QTextDocument::FindFlags options)
{
    return m_widget->find(exp,options);
}

#ifndef QT_NO_PRINTER
void TextBrowserHtmlWidget::print(QPrinter *printer)
{
    m_widget->print(printer);
}
#endif

TextBrowserHtmlWidgetFactory::TextBrowserHtmlWidgetFactory(QObject *parent)
    : IHtmlWidgetFactory(parent)
{
}

QString TextBrowserHtmlWidgetFactory::className() const
{
    return "QTextBrowser";
}

IHtmlWidget *TextBrowserHtmlWidgetFactory::create(QObject *parent)
{
    return new TextBrowserHtmlWidget(parent);
}

IHtmlDocument *TextBrowserHtmlWidgetFactory::createDocument(QObject *parent)
{
    return new TextBrowserHtmlDocument(parent);
}


TextBrowserHtmlDocument::TextBrowserHtmlDocument(QObject *parent) :
    IHtmlDocument(parent)
{
    m_doc = new QTextBrowser;
    m_doc->setVisible(false);
}

TextBrowserHtmlDocument::~TextBrowserHtmlDocument()
{
    delete m_doc;
}

void TextBrowserHtmlDocument::setHtml(const QString &html, const QUrl &url)
{
    m_doc->setHtml(html);
    if (!url.isEmpty()) {
        QString file = url.toLocalFile();
        if (!file.isEmpty()) {
            QFileInfo info(file);
            QStringList paths = m_doc->searchPaths();
            paths.append(info.path());
            paths.removeDuplicates();
            m_doc->setSearchPaths(paths);
        }
    }
    emit loadFinished(true);
}

#ifndef QT_NO_PRINTER
void TextBrowserHtmlDocument::print(QPrinter *printer)
{
    m_doc->print(printer);
}
#endif

QString TextBrowserHtmlDocument::toHtml() const
{
    return m_doc->toHtml();
}

QString TextBrowserHtmlDocument::toPlainText() const
{
    return m_doc->toPlainText();
}
