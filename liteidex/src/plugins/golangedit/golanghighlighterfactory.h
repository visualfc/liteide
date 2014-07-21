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
// Module: golanghighlighterfactory.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGHIGHLIGHTERFACTORY_H
#define GOLANGHIGHLIGHTERFACTORY_H

#include "liteeditorapi/liteeditorapi.h"

class GolangHighlighterFactory : public LiteApi::IHighlighterFactory
{
    Q_OBJECT
public:
    explicit GolangHighlighterFactory(QObject *parent = 0);
    virtual QStringList mimeTypes() const;
    virtual TextEditor::SyntaxHighlighter* create(QTextDocument *doc, const QString &mimeType);
};

#endif // GOLANGHIGHLIGHTERFACTORY_H
