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
// Module: gopathproject.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOPATHPROJECT_H
#define GOPATHPROJECT_H

#include "liteapi/liteapi.h"
#include "liteenvapi/liteenvapi.h"
#include <QModelIndex>

class QTreeView;
class GopathModel;
class GopathBrowser;
class GopathProject : public LiteApi::IProject
{
    Q_OBJECT
public:
    explicit GopathProject(GopathBrowser *browser);
    ~GopathProject();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QString filePath() const;
    virtual QString mimeType() const;
    virtual QStringList fileNameList() const;
    virtual QStringList filePathList() const;
    virtual QString fileNameToFullPath(const QString &filePath);
    virtual QMap<QString,QString> projectInfo() const;
    virtual QMap<QString,QString> targetInfo() const;
    virtual void load();
    GopathBrowser *browser();
public slots:
    void startPathChanged(const QString &path);
protected:
    GopathBrowser *m_browser;
    QString m_path;
};

#endif // GOPATHPROJECT_H
