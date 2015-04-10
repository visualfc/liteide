/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
// Module: golanghighlighterfactory.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golanghighlighterfactory.h"
#include "golanghighlighter.h"
#include "memory.h"

GolangHighlighterFactory::GolangHighlighterFactory(QObject *parent) :
    LiteApi::IHighlighterFactory(parent)
{

}

QStringList GolangHighlighterFactory::mimeTypes() const
{
    return QStringList() << "text/x-gosrc";
}

TextEditor::SyntaxHighlighter *GolangHighlighterFactory::create(QTextDocument *doc, const QString &/*mimeType*/)
{
    return new GolangHighlighter(doc);
}
