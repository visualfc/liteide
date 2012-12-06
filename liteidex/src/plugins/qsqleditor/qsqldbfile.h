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
// Module: qsqldbfile.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-25
// $Id: qsqldbfile.h,v 1.0 2011-7-25 visualfc Exp $

#ifndef QSQLDBFILE_H
#define QSQLDBFILE_H

#include "liteapi.h"
#include <QSqlDatabase>

class QSqlDbFile : public LiteApi::IFile
{
public:
    QSqlDbFile(LiteApi::IApplication *app, QObject *parent);
    ~QSqlDbFile();
public:
    virtual bool open(const QString &fileName, const QString &mimeType);
    virtual bool reload();
    virtual bool save(const QString &fileName);
    virtual bool isReadOnly() const;
    virtual QString fileName() const;
    virtual QString mimeType() const;
public:
    QSqlDatabase & db();
protected:
    LiteApi::IApplication *m_liteApp;
    QSqlDatabase    m_db;
};

#endif // QSQLDBFILE_H
