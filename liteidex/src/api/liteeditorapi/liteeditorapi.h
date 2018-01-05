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
// Module: liteeditorapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEEDITORAPI_H
#define LITEEDITORAPI_H

#include "liteapi/liteapi.h"
#include <QTextCursor>
#include <QTextBlock>
#include <QCompleter>
#include <QStandardItem>

namespace TextEditor {
class SyntaxHighlighter;
}

namespace LiteApi {

class ILiteEditor;
class IWordApi
{
public:
    virtual ~IWordApi() {}
    virtual QString package() const = 0;
    virtual QStringList apiFiles() const = 0;
    virtual bool loadApi() = 0;
    virtual QStringList wordList() const = 0;
    virtual QStringList expList() const = 0;
    virtual void appendExp(const QStringList &list) = 0;
};

struct Snippet
{
    QString Name;
    QString Info;
    QString Text;
};

class ISnippetApi
{
public:
    virtual ~ISnippetApi() {}
    virtual QString package() const = 0;
    virtual QStringList apiFiles() const = 0;
    virtual bool loadApi() = 0;
    virtual QList<Snippet*> snippetList() const = 0;
};

class IEditorApiManager : public IManager
{
    Q_OBJECT
public:
    IEditorApiManager(QObject *parent = 0) : IManager(parent) {}
    virtual void addWordApi(IWordApi *api) = 0;
    virtual void removeWordApi(IWordApi *api) = 0;
    virtual IWordApi *findWordApi(const QString &mimeType) = 0;
    virtual QList<IWordApi*> wordApiList() const = 0;
    virtual void addSnippetApi(ISnippetApi *api) = 0;
    virtual void removeSnippetApi(ISnippetApi *api) = 0;
    virtual ISnippetApi *findSnippetApi(const QString &mimeType) = 0;
    virtual QList<ISnippetApi*> snippetApiList() const = 0;
};

enum CompletionContext {
    CompleterCodeContext = 0,
    CompleterImportContext,
};

class ICompleter : public QObject
{
    Q_OBJECT
public:
    ICompleter(QObject *parent): QObject(parent) {}
    virtual void setEditor(QPlainTextEdit *editor) = 0;
    virtual QStandardItem *findRoot(const QString &name) = 0;
    virtual void clearChildItem(QStandardItem *root) = 0;
    virtual void appendChildItem(QStandardItem *root,QString name,const QString &kind, const QString &info,const QIcon &icon, bool temp) = 0;
    virtual bool appendItem(const QString &name,const QIcon &icon, bool temp) = 0;
    virtual bool appendItemEx(const QString &name,const QString &kind, const QString &info,const QIcon &icon, bool temp) = 0;
    virtual void appendItems(QStringList items, const QString &kind, const QString &info,const QIcon &icon, bool temp) = 0;
    virtual void appendSnippetItem(const QString &name, const QString &info, const QString &content) = 0;
    virtual void clearItemChilds(const QString &name) = 0;
    virtual void clearTemp() = 0;
    virtual void clear() = 0;
    virtual void setSearchSeparator(bool b) = 0;
    virtual bool searchSeparator() const = 0;
    virtual void setExternalMode(bool b) = 0;
    virtual bool externalMode() const = 0;
    virtual void setCaseSensitivity(Qt::CaseSensitivity caseSensitivity) = 0;
    virtual void setFuzzy(bool fuzzy) = 0;
    virtual void setCompletionPrefix(const QString &prefix) = 0;
    virtual QString completionPrefix() const = 0;
    virtual void setCompletionContext(CompletionContext ctx) = 0;
    virtual CompletionContext completionContext() const = 0;
    virtual void setSeparator(const QString &sep) = 0;
    virtual QString separator() const = 0;
    virtual void showPopup() = 0;
    virtual void hidePopup() = 0;
    virtual QAbstractItemView *popup() const = 0;
    virtual QModelIndex currentIndex() const = 0;
    virtual QString currentCompletion() const = 0;
    virtual QAbstractItemModel *completionModel() const = 0;
    virtual bool startCompleter(const QString &completionPrefix) = 0;
    virtual void updateCompleterModel() = 0;
    virtual void updateCompleteInfo(QModelIndex index) = 0;
    virtual void setImportList(const QStringList &importList) = 0;
    virtual void setPrefixMin(int min) = 0;
    virtual int prefixMin() const = 0;
signals:
    void prefixChanged(QTextCursor,QString,bool force);
    void wordCompleted(const QString &func, const QString &kind, const QString &info);
};

class IEditorMark : public QObject
{
    Q_OBJECT
public:
    IEditorMark(QObject *parent) : QObject(parent) {}
    virtual void addMark(int line, int type) = 0;
    virtual void addMarkList(const QList<int> &lines, int type) = 0;
    virtual void removeMark(int line, int type) = 0;
    virtual void removeMarkList(const QList<int> &lines, int type) = 0;
    virtual QList<int> markLinesByType(int type) const = 0;
    virtual QList<QTextBlock> markBlocksByType(int type) const = 0;
    virtual QList<int> markTypesByLine(int line) const = 0;
    virtual ILiteEditor *editor() const = 0;
    virtual QString filePath() const = 0;
signals:
    void markListChanged(int type);
};

class IEditorMarkNode : public QObject
{
    Q_OBJECT
public:
    IEditorMarkNode(QObject *parent = 0) : QObject(parent) {}
    virtual ~IEditorMarkNode() {}

    virtual int blockNumber() const = 0;
    virtual int type() const = 0;
    virtual QTextBlock block() const = 0;
};

class IEditorMarkManager : public IManager
{
    Q_OBJECT
public:
    IEditorMarkManager(QObject *parent = 0) : IManager(parent) {}
    virtual void registerMark(int type, const QIcon &icon) = 0;
    virtual QList<int> markTypeList() const = 0;
    virtual QIcon iconForType(int type) const = 0;
    virtual int indexOfType(int type) const = 0;
    virtual QList<LiteApi::IEditorMark*> editorMarkList() const = 0;
signals:
    void editorMarkCreated(LiteApi::IEditorMark *mark);
    void editorMarkRemoved(LiteApi::IEditorMark *mark);
    void editorMarkListChanged(LiteApi::IEditorMark *mark, int type);
    void editorMarkNodeCreated(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node);
    void editorMarkNodeRemoved(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node);
    void editorMarkNodeChanged(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node);
};

enum EditorNaviagteType{
    EditorNavigateNormal = 1,
    EditorNavigateWarning = 2,
    EditorNavigateError = 4,
    EditorNavigateReload = 8,
    EditorNavigateFind = 16,
    EditorNavigateSelection = 32,
    EditorNavigateBad = EditorNavigateWarning|EditorNavigateError
};

enum ExtraSelectionKind {
    CurrentLineSelection,
    ParenthesesMatchingSelection,
    LinkSelection,
};

struct Link
{
    Link(): linkTextStart(-1)
        , linkTextEnd(-1)
        , targetLine(-1)
        , targetColumn(-1)
        , showTip(false)
        , showNav(false)
    {}

    void clear()
    {
        linkTextStart = -1;
        linkTextEnd = -1;
        targetFileName.clear();
        targetInfo.clear();
        sourceInfo.clear();
        targetLine = 0;
        targetColumn = 0;
        showTip = false;
        showNav = false;
    }

    bool hasValidTarget() const
    { return !targetFileName.isEmpty(); }

    bool hasValidLinkText() const
    { return linkTextStart != linkTextEnd; }

    bool operator==(const Link &other) const
    { return linkTextStart == other.linkTextStart && linkTextEnd == other.linkTextEnd; }

    int linkTextStart;
    int linkTextEnd;
    int targetLine;
    int targetColumn;
    bool    showTip;
    bool    showNav;
    QString targetFileName;
    QString targetInfo;
    QString sourceInfo;
    QPoint cursorPos;
};

class ITextLexer : public QObject
{
public:
    ITextLexer(QObject *parent = 0) : QObject(parent)
    {}
    virtual ~ITextLexer()
    {}
    virtual bool isLangSupport() const = 0;
    virtual bool isInComment(const QTextCursor &cursor) const = 0;
    virtual bool isInString(const QTextCursor &cursor) const = 0;
    virtual bool isInEmptyString(const QTextCursor &cursor) const = 0;
    virtual bool isEndOfString(const QTextCursor &cursor) const = 0;
    virtual bool isInStringOrComment(const QTextCursor &cursor) const = 0;
    virtual bool isCanAutoCompleter(const QTextCursor &cursor) const = 0;
    virtual bool isInImport(const QTextCursor &cursor) const = 0;
    virtual int startOfFunctionCall(const QTextCursor &cursor) const = 0;
    virtual QString fetchFunctionTip(const QString &func, const QString &kind, const QString &info) = 0;
    virtual bool fetchFunctionArgs(const QString &str, int &argnr, int &parcount) = 0;
    virtual QString stringQuoteList() const = 0;
    virtual bool hasStringBackslash() const = 0;
};

class ILiteEditor : public ITextEditor
{
    Q_OBJECT
public:
    ILiteEditor(QObject *parent = 0) : ITextEditor(parent) {}
    virtual QTextDocument* document() const = 0;
    virtual void setCompleter(ICompleter *complter) = 0;
    virtual void setEditorMark(IEditorMark *mark) = 0;
    virtual void setTextLexer(ITextLexer *lexer) = 0;
    virtual void setSpellCheckZoneDontComplete(bool b) = 0;
    virtual void insertNavigateMark(int line, EditorNaviagteType type, const QString &msg, const char *tag) = 0;
    virtual void clearNavigateMarak(int line) = 0;
    virtual void clearAllNavigateMarks() = 0;
    virtual void clearAllNavigateMark(EditorNaviagteType types, const char *tag = "") = 0;
    virtual void setNavigateHead(EditorNaviagteType type, const QString &msg) = 0;
    virtual void showLink(const Link &link) = 0;
    virtual void clearLink() = 0;
    virtual void setTabOption(int tabSize, bool tabToSpace) = 0;
    virtual void setEnableAutoIndentAction(bool b) = 0;
    virtual bool isLineEndUnix() const = 0;
    virtual void setLineEndUnix(bool b) = 0;
    virtual void showToolTipInfo(const QPoint & pos, const QString & text) = 0;
    virtual void loadDiff(const QString &diff) = 0;
    virtual void loadTextUseDiff(const QString &text) = 0;
signals:
    void updateLink(const QTextCursor &cursor, const QPoint &pos, bool nav);
};

inline ILiteEditor *getLiteEditor(IEditor *editor)
{
    if (editor && editor->extension()) {
        return findExtensionObject<ILiteEditor*>(editor->extension(),"LiteApi.ILiteEditor");
    }
    return 0;
}

inline ITextLexer *getTextLexer(IEditor *editor) {
    if (editor && editor->extension()) {
        return findExtensionObject<ITextLexer*>(editor->extension(),"LiteApi.ITextLexer");
    }
    return 0;
}

inline IEditorMark *getEditorMark(IEditor *editor) {
    if (editor && editor->extension()) {
        return findExtensionObject<IEditorMark*>(editor->extension(),"LiteApi.IEditorMark");
    }
    return 0;
}

inline IEditorMarkManager *getEditorMarkManager(IApplication *app) {
    if (app && app->extension()) {
        return findExtensionObject<IEditorMarkManager*>(app->extension(),"LiteApi.IEditorMarkManager");
    }
    return 0;
}

class IHighlighterFactory : public QObject
{
    Q_OBJECT
public:
    IHighlighterFactory(QObject *parent) : QObject(parent)
    {}
    virtual QStringList mimeTypes() const = 0;
    virtual TextEditor::SyntaxHighlighter* create(ITextEditor *editor, QTextDocument *doc, const QString &mimeType) = 0;
};

class IHighlighterManager :public IManager
{
    Q_OBJECT
public:
    IHighlighterManager(QObject *parent) : IManager(parent)
    {}
    virtual void addFactory(IHighlighterFactory *factroy) = 0;
    virtual void removeFactory(IHighlighterFactory *factory) = 0;
    virtual QList<IHighlighterFactory*> factoryList() const = 0;
    virtual QStringList mimeTypeList() const = 0;
    virtual IHighlighterFactory *findFactory(const QString &mimeType) const = 0;
};

inline IHighlighterManager *getHighlighterManager(LiteApi::IApplication *app)
{
    return static_cast<IHighlighterManager*>(app->extension()->findObject("LiteApi.IHighlighterManager"));
}

inline QString wordUnderCursor(QTextCursor tc, bool *moveLeft = 0, int *selectStart = 0)
{
    QString text = tc.block().text();
    int pos = tc.positionInBlock();
    if (pos > 0 && pos < text.length()) {
        QChar ch = text.at(pos-1);
        if (ch.isLetterOrNumber() || ch == '_') {
            tc.movePosition(QTextCursor::Left);
            if (moveLeft) {
                *moveLeft = true;
            }
        }
    }
    tc.select(QTextCursor::WordUnderCursor);
    if (selectStart) {
        *selectStart = tc.selectionStart();
    }
    return tc.selectedText();
}

inline void selectWordUnderCursor(QTextCursor &tc, bool *moveLeft = 0)
{
    QString text = tc.block().text();
    int pos = tc.positionInBlock();
    if (pos > 0 && pos < text.length()) {
        QChar ch = text.at(pos-1);
        if (ch.isLetterOrNumber() || ch == '_') {
            tc.movePosition(QTextCursor::Left);
            if (moveLeft) {
                *moveLeft = true;
            }
        }
    }
    tc.select(QTextCursor::WordUnderCursor);
}

} //namespace LiteApi


#endif //LITEEDITORAPI_H

