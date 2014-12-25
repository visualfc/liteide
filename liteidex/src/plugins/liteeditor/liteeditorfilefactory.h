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
// Module: liteeditorfilefactory.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEEDITORFILEFACTORY_H
#define LITEEDITORFILEFACTORY_H

#include "liteapi/liteapi.h"
#include "highlightermanager.h"

class EditorApiManager;
class LiteEditorMarkTypeManager;
class LiteEditor;

class LiteEditorFileFactory : public LiteApi::IEditorFactory
{
    Q_OBJECT
public:
    LiteEditorFileFactory(LiteApi::IApplication *app, QObject *parent);
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IEditor *open(const QString &fileName, const QString &mimeType);
    virtual LiteApi::IEditor *create(const QString &contents,const QString &mimeType);
    LiteApi::IEditor *setupEditor(LiteEditor *editor,const QString &mimeType);
public slots:
    void colorStyleChanged();
    void tabSettingChanged(int);
protected:
    LiteApi::IApplication *m_liteApp;
    EditorApiManager *m_wordApiManager;
    LiteEditorMarkTypeManager *m_markTypeManager;
    HighlighterManager *m_highlighterManager;
    QStringList m_mimeTypes;
};

#endif // LITEEDITORFILEFACTORY_H
