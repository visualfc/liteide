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
// Module: syntaxeditor.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef SYNTAXEDITOR_H
#define SYNTAXEDITOR_H

#include "../../liteapi/liteapi.h"

#include <QPointer>

class SyntaxTextEditor;
class QTextDocument;

class TextFile : public LiteApi::IFile
{
    Q_OBJECT
public:
    TextFile(QObject *parent);

    virtual bool open(const QString &fileName, const QString &mimeType);
    virtual bool reload();
    virtual bool save(const QString &fileName);
    virtual QString fileName() const;
    virtual QString mimeType() const;
    virtual QString subType() const;
public:
    void setDocument(QTextDocument *document);
    QTextDocument  *document();
protected:
    QString        m_fileName;
    QString        m_mimeType;
    QTextDocument *m_document;
    QTextCodec    *m_codec;
};

class SyntaxEditor : public LiteApi::IEditor
{
    Q_OBJECT
public:
    SyntaxEditor(QObject *parent);

    virtual bool autoDelete() const;

    virtual QWidget *widget();
    virtual QToolBar* toolBar() const;
    virtual QString displayName() const;
    virtual QIcon icon() const;

    virtual void setDisplayName(const QString &title);
    virtual bool createNew(const QString &contents);
    virtual bool open(const QString &fileName, const QString &mimeType);
    virtual bool save();
    virtual bool isModified();
    virtual LiteApi::IFile *file();

    virtual int line() const;
    virtual int column() const;
    virtual void gotoLine(int line, int column);
public:
    SyntaxTextEditor *editor();
protected:
    SyntaxTextEditor *m_editor;
    QToolBar         *m_toolBar;
    TextFile         *m_file;
    QAction          *m_undoAct;
    QAction          *m_redoAct;
};

#endif // SYNTAXEDITOR_H
