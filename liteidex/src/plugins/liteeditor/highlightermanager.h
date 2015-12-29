/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: highlightermanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef HIGHLIGHTERMANAGER_H
#define HIGHLIGHTERMANAGER_H

#include "liteeditorapi/liteeditorapi.h"

class HighlighterManager : public LiteApi::IHighlighterManager
{
public:
    HighlighterManager(QObject *parent);
    virtual void addFactory(LiteApi::IHighlighterFactory *factory);
    virtual void removeFactory(LiteApi::IHighlighterFactory *factory);
    virtual QList<LiteApi::IHighlighterFactory*> factoryList() const;
    virtual QStringList mimeTypeList() const;
    virtual LiteApi::IHighlighterFactory *findFactory(const QString &mimeType) const;
public:
    void setColorStyle(TextEditor::SyntaxHighlighter *highlighter,const ColorStyleScheme *scheme);
    void setTabSize(TextEditor::SyntaxHighlighter *highlighter, int tabSize);
protected:
    QList<LiteApi::IHighlighterFactory*> m_factoryList;
};

#endif // HIGHLIGHTERMANAGER_H
