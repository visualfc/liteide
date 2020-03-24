/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE. All rights reserved.
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
// Module: gopathprojectfactor.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOPATHPROJECTFACTOR_H
#define GOPATHPROJECTFACTOR_H

#include "liteapi/liteapi.h"

class GopathBrowser;
class GopathProjectFactor : public LiteApi::IProjectFactory
{
    Q_OBJECT
public:
    explicit GopathProjectFactor(LiteApi::IApplication *app, QObject *parent = 0);
    ~GopathProjectFactor();
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IProject *open(const QString &fileName, const QString &mimeType);
    virtual bool findProjectInfo(const QString &fileName, const QString &mimetype, QMap<QString,QString>& projectInfo, QMap<QString,QString>& findProjectInfo) const;
public slots:
    void importGopath();
    void loadApp();
protected:
    LiteApi::IApplication *m_liteApp;
    GopathBrowser *m_browser;
    QStringList m_mimeTypes;
};

#endif // GOPATHPROJECTFACTOR_H
