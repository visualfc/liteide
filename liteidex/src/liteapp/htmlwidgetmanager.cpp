/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
// Module: htmlwidgetmanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "htmlwidgetmanager.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

HtmlWidgetManager::HtmlWidgetManager(QObject *parent) :
    IHtmlWidgetManager(parent)
{
}

QStringList HtmlWidgetManager::classNameList() const
{
    QStringList names;
    foreach(IHtmlWidgetFactory *factory, m_factoryList) {
        names.append(factory->className());
    }
    return names;
}

void HtmlWidgetManager::addFactory(IHtmlWidgetFactory *factory)
{
    m_factoryList.append(factory);
    if (m_factoryList.size() == 1) {
        m_defaultClassName = factory->className();
    }
}

QList<IHtmlWidgetFactory *> HtmlWidgetManager::factoryList() const
{
    return m_factoryList;
}

IHtmlWidget *HtmlWidgetManager::create(QObject *parent)
{
    return createByName(parent,m_defaultClassName);
}

IHtmlWidget *HtmlWidgetManager::createByName(QObject *parent, const QString &className)
{
    foreach(IHtmlWidgetFactory *factory, m_factoryList) {
        if (factory->className() == className) {
            return factory->create(parent);
        }
    }
    return 0;
}

IHtmlDocument *HtmlWidgetManager::createDocument(QObject *parent)
{
   return createDocumentByName(parent,m_defaultClassName);
}

IHtmlDocument *HtmlWidgetManager::createDocumentByName(QObject *parent, const QString &className)
{
    foreach(IHtmlWidgetFactory *factory, m_factoryList) {
        if (factory->className() == className) {
            return factory->createDocument(parent);
        }
    }
    return 0;
}

bool HtmlWidgetManager::setDefaultClassName(const QString &className)
{
    foreach(IHtmlWidgetFactory *factory, m_factoryList) {
        if (factory->className() == className) {
            m_defaultClassName = className;
            return true;
        }
    }
    return false;
}

QString HtmlWidgetManager::defaultClassName() const
{
    return m_defaultClassName;
}
