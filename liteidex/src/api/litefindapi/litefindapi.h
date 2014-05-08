/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
// Module: litefindapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef __LITEFINDAPI_H__
#define __LITEFINDAPI_H__

#include "liteapi/liteapi.h"

namespace LiteApi {

class FileSearchResult
{
public:
    FileSearchResult()
        : line(0), col(0), len(0) {}

    FileSearchResult(const QString &path, const QString &lineText, int line, int col, int len)
        : path(path), lineText(lineText), line(line), col(col), len(len) {}

public:
    QString path;
    QString lineText;
    int line;
    int col;
    int len;
};

class IFileSearch : public QObject
{
    Q_OBJECT
public:
    IFileSearch(QObject *parent) : QObject(parent) {}
    virtual QString mimeType() const = 0;
    virtual QString displayName() const = 0;
    virtual QWidget* widget() const = 0;
    virtual void start() = 0;
    virtual void cancel() = 0;
    virtual void activate() = 0;
    virtual QString searchText() const = 0;
    virtual bool replaceMode() const = 0;
signals:
    void findStarted();
    void findFinished(bool b = true);
    void findResult(const LiteApi::FileSearchResult &result);
};

class IFileSearchManager : public IManager
{
    Q_OBJECT
public:
    IFileSearchManager(QObject *parent = 0) : IManager(parent) {}
    virtual void addFileSearch(IFileSearch* search) = 0;
    virtual IFileSearch *findFileSearch(const QString &mime) = 0;
    virtual QList<IFileSearch*> fileSearchList() const = 0;
    virtual void setCurrentSearch(LiteApi::IFileSearch *search) = 0;
public slots:
    virtual void newSearch() = 0;
};

inline IFileSearchManager* getFileSearchManager(LiteApi::IApplication *app)
{
    return LiteApi::findExtensionObject<IFileSearchManager*>(app,"LiteApi.IFileSearchManager");
}

} //namespace LiteApi

#endif //__LITEFINDAPI_H__

