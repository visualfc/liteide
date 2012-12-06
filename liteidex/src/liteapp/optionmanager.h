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
// Module: optionmanager.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: optionmanager.h,v 1.0 2011-6-27 visualfc Exp $

#ifndef OPTIONMANAGER_H
#define OPTIONMANAGER_H

#include "liteapi/liteapi.h"

using namespace LiteApi;

class OptionsBrowser;
class BrowserEditorImpl;
class OptionManager : public IOptionManager
{
    Q_OBJECT
public:
    OptionManager();
    virtual bool initWithApp(IApplication *app);
    virtual void addFactory(IOptionFactory *factory);
    virtual void removeFactory(IOptionFactory *factory);
    virtual QList<IOptionFactory*> factoryList() const;
    IEditor *browser();
    void setAction(QAction *act);
public slots:
    virtual void exec();
protected slots:
    void editorCreated(LiteApi::IEditor*);
protected:
    OptionsBrowser  *m_browser;
    QAction *m_action;
    QList<IOptionFactory*>  m_factoryList;
};

#endif // OPTIONMANAGER_H
