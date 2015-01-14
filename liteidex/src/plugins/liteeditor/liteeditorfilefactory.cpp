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
// Module: liteeditorfilefactory.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "liteeditorfilefactory.h"
#include "liteeditor.h"
#include "liteeditorwidget.h"
#include "litewordcompleter.h"
#include "editorapimanager.h"
#include "liteeditormark.h"
#include "liteeditor_global.h"
#include "katehighlighterfactory.h"
#include "katetextlexer.h"
#include <QDir>
#include <QFileInfo>
#include "mimetype/mimetype.h"
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


LiteEditorFileFactory::LiteEditorFileFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IEditorFactory(parent),
      m_liteApp(app)
{
    m_mimeTypes.append("text/x-gosrc");
    m_mimeTypes.append("text/x-lua");
    m_mimeTypes.append("liteide/default.editor");

    m_highlighterManager = new HighlighterManager(this);
    if (m_highlighterManager->initWithApp(app)) {
        m_liteApp->extension()->addObject("LiteApi.IHighlighterManager",m_highlighterManager);
        QDir dir(m_liteApp->resourcePath()+"/liteeditor/kate");
        if (dir.exists()) {
            KateHighlighterFactory *factory = new KateHighlighterFactory(this);
            factory->loadPath(dir.absolutePath());
            m_highlighterManager->addFactory(factory);

            KateHighlighter *kate = factory->kate();
            foreach (QString type, kate->mimeTypes()) {
                if (!m_liteApp->mimeTypeManager()->findMimeType(type)) {
                    MimeType *mimeType = new MimeType;
                    mimeType->setType(type);
                    foreach(QString pattern, kate->mimeTypePatterns(type)) {
                        mimeType->appendGlobPatterns(pattern);
                    }
                    mimeType->setComment(kate->mimeTypeName(type));
                    m_liteApp->mimeTypeManager()->addMimeType(mimeType);
                }
                m_mimeTypes.append(type);
            }
        }
    }
    m_mimeTypes.removeDuplicates();

    m_wordApiManager = new EditorApiManager(this);
    if (m_wordApiManager->initWithApp(app)) {
        m_liteApp->extension()->addObject("LiteApi.IWordApiManager",m_wordApiManager);
        m_wordApiManager->load(m_liteApp->resourcePath()+"/packages");
    }
    m_markTypeManager = new LiteEditorMarkTypeManager(this);
    if (m_markTypeManager->initWithApp(app)) {
        m_liteApp->extension()->addObject("LiteApi.IEditorMarkTypeManager",m_markTypeManager);
    }
}

QStringList LiteEditorFileFactory::mimeTypes() const
{
    return m_mimeTypes;
}

void LiteEditorFileFactory::colorStyleChanged()
{
    LiteEditor *editor = static_cast<LiteEditor *>(sender());
    if (!editor) {
        return;
    }
    TextEditor::SyntaxHighlighter *h = editor->syntaxHighlighter();
    if (h) {
        m_highlighterManager->setColorStyle(h,m_liteApp->editorManager()->colorStyleScheme());
    }
}

void LiteEditorFileFactory::tabSettingChanged(int tabSize)
{
    LiteEditor *editor = static_cast<LiteEditor *>(sender());
    if (!editor) {
        return;
    }
    TextEditor::SyntaxHighlighter *h = editor->syntaxHighlighter();
    if (h) {
        m_highlighterManager->setTabSize(h,tabSize);
    }
}

LiteApi::IEditor *LiteEditorFileFactory::open(const QString &fileName, const QString &mimeType)
{
   // m_liteApp->editorManager()->cutForwardNavigationHistory();
    //m_liteApp->editorManager()->addNavigationHistory();
    LiteEditor *editor = new LiteEditor(m_liteApp);
    editor->setEditorMark(new LiteEditorMark(m_markTypeManager,editor->document(),editor));
    if (!editor->open(fileName,mimeType)) {
        delete editor;
        return 0;
    }
    return setupEditor(editor,mimeType);
}

LiteApi::IEditor *LiteEditorFileFactory::create(const QString &contents, const QString &mimeType)
{
    LiteEditor *editor = new LiteEditor(m_liteApp);
    editor->setEditorMark(new LiteEditorMark(m_markTypeManager,editor->document(),editor));
    if (!editor->createNew(contents,mimeType)) {
        delete editor;
        return 0;
    }

    return setupEditor(editor,mimeType);
}

LiteApi::IEditor *LiteEditorFileFactory::setupEditor(LiteEditor *editor, const QString &mimeType)
{
    QTextDocument *doc = editor->document();
    LiteApi::IHighlighterFactory *factory = m_highlighterManager->findFactory(mimeType);
    if (factory) {
        TextEditor::SyntaxHighlighter *h = factory->create(doc,mimeType);
        if (h) {
            editor->setSyntaxHighlighter(h);
            editor->setTextLexer(new KateTextLexer(editor));
            editor->setTabOption(4,true);
            connect(editor,SIGNAL(colorStyleChanged()),this,SLOT(colorStyleChanged()));
            connect(editor,SIGNAL(tabSettingChanged(int)),this,SLOT(tabSettingChanged(int)));
            connect(h,SIGNAL(foldIndentChanged(QTextBlock)),editor->editorWidget(),SLOT(foldIndentChanged(QTextBlock)));
        }
    }

    LiteWordCompleter *wordCompleter = new LiteWordCompleter(editor);
    editor->setCompleter(wordCompleter);
    if (wordCompleter) {
        LiteApi::IWordApi *wordApi = m_wordApiManager->findWordApi(mimeType);
        if (wordApi && wordApi->loadApi()) {
            QIcon icon("icon:liteeditor/images/keyword.png");
            QIcon exp("icon:liteeditor/images/findword.png");
            QIcon func("icon:liteeditor/images/func.png");
            foreach(QString item, wordApi->wordList()) {
                int pos = item.indexOf("(");
                if (pos != -1) {
                    wordCompleter->appendItemEx(item.left(pos).trimmed(),"func","func"+item.right(item.length()-pos),func,false);
                } else {
                    wordCompleter->appendItemEx(item,"keyword",QString(""),icon,false);
                }
            }
            wordCompleter->appendItems(wordApi->expList(),"","",exp,false);
            wordCompleter->model()->sort(0);
        }

    }
    LiteApi::ISnippetApi *snippetApi = m_wordApiManager->findSnippetApi(mimeType);
    if (snippetApi && snippetApi->loadApi()) {
        foreach (LiteApi::Snippet *snippet, snippetApi->snippetList()) {
            wordCompleter->appendSnippetItem(snippet->Name,snippet->Info,snippet->Text);
        }
    }
    editor->applyOption(OPTION_LITEEDITOR);
    editor->loadColorStyleScheme();
    return editor;
}
