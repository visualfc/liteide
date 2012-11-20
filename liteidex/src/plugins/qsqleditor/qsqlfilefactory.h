/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: qsqlfilefactory.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-25
// $Id: qsqlfilefactory.h,v 1.0 2011-7-25 visualfc Exp $

#ifndef QSQLFILEFACTORY_H
#define QSQLFILEFACTORY_H

#include "liteapi.h"

class QSqlFileFactory : public LiteApi::IEditorFactory
{
public:
    QSqlFileFactory(LiteApi::IApplication *app, QObject *parent);

    virtual QStringList mimeTypes() const;
    virtual LiteApi::IEditor *open(const QString &fileName, const QString &mimeType);
    virtual bool targetInfo(const QString &fileName, const QString &mimetype, QString &target, QString &targetPath, QString &workPath) const;
protected:
    LiteApi::IApplication *m_liteApp;
};

#endif // QSQLFILEFACTORY_H
