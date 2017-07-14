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
// Module: katehighlighterfactory.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef KATEHIGHLIGHTERFACTORY_H
#define KATEHIGHLIGHTERFACTORY_H

#include "liteeditorapi/liteeditorapi.h"
#include "qtc_texteditor/katehighlighter.h"

class KateHighlighterFactory : public LiteApi::IHighlighterFactory
{
    Q_OBJECT
public:
    KateHighlighterFactory(QObject *parent);
    virtual QStringList mimeTypes() const;
    virtual TextEditor::SyntaxHighlighter* create(LiteApi::ITextEditor *editor, QTextDocument *doc, const QString &mimeType);
    void loadPath(const QString &dir);
    KateHighlighter *kate() const { return m_kate; }
protected:
    KateHighlighter *m_kate;
};

#endif // KATEHIGHLIGHTERFACTORY_H
