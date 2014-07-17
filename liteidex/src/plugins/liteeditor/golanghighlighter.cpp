/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "golanghighlighter.h"

#include "qtc_texteditor/basetextdocumentlayout.h"

#include "cplusplus/SimpleLexer.h"
#include "cplusplus/Lexer.h"

#include <QTextDocument>

using namespace TextEditor;
using namespace CPlusPlus;

GolangHighlighter::GolangHighlighter(QTextDocument *document) :
    TextEditor::SyntaxHighlighter(document)
{
}

GolangHighlighter::~GolangHighlighter()
{
}

void GolangHighlighter::highlightBlock(const QString &text)
{
    const int previousBlockState_ = previousBlockState();
    int lexerState = 0, initialBraceDepth = 0;
    if (previousBlockState_ != -1) {
        lexerState = previousBlockState_ & 0xff;
        initialBraceDepth = previousBlockState_ >> 8;
    }

    int braceDepth = initialBraceDepth;

    LanguageFeatures features;
    features.golangEnable = true;

    SimpleLexer tokenize;
    tokenize.setLanguageFeatures(features);

    int initialLexerState = lexerState;
    const QList<Token> tokens = tokenize(text, initialLexerState);
    lexerState = tokenize.state(); // refresh lexer state

    initialLexerState &= ~0x80; // discard newline expected bit
    int foldingIndent = initialBraceDepth;
    if (TextBlockUserData *userData = BaseTextDocumentLayout::testUserData(currentBlock())) {
        userData->setFoldingIndent(0);
        userData->setFoldingStartIncluded(false);
        userData->setFoldingEndIncluded(false);
    }

    if (tokens.isEmpty()) {
        setCurrentBlockState((braceDepth << 8) | lexerState);
        BaseTextDocumentLayout::clearParentheses(currentBlock());
        if (text.length())  {// the empty line can still contain whitespace
            if (initialLexerState == T_COMMENT) {
                highlightLine(text, 0, text.length(), m_creatorFormats[SyntaxHighlighter::Comment]);
            }
            else if (initialLexerState == T_DOXY_COMMENT) {
                highlightLine(text, 0, text.length(), m_creatorFormats[SyntaxHighlighter::Comment]);
            }
            else if (initialLexerState == T_RAW_STRING_LITERAL) {
                highlightLine(text, 0, text.length(), m_creatorFormats[SyntaxHighlighter::String]);
            }
            else
                setFormat(0, text.length(), m_creatorFormats[SyntaxHighlighter::VisualWhitespace]);
        }
        BaseTextDocumentLayout::setFoldingIndent(currentBlock(), foldingIndent);
        return;
    }

    const unsigned firstNonSpace = tokens.first().begin();

    Parentheses parentheses;
    parentheses.reserve(20); // assume wizard level ;-)

    bool expectPreprocessorKeyword = false;
    bool onlyHighlightComments = false;

    for (int i = 0; i < tokens.size(); ++i) {
        const Token &tk = tokens.at(i);

        unsigned previousTokenEnd = 0;
        if (i != 0) {
            // mark the whitespaces
            previousTokenEnd = tokens.at(i - 1).begin() +
                               tokens.at(i - 1).length();
        }

        if (previousTokenEnd != tk.begin())
            setFormat(previousTokenEnd, tk.begin() - previousTokenEnd, m_creatorFormats[SyntaxHighlighter::VisualWhitespace]);

        if (tk.is(T_LPAREN) || tk.is(T_LBRACE) || tk.is(T_LBRACKET)) {
            const QChar c = text.at(tk.begin());
            parentheses.append(Parenthesis(Parenthesis::Opened, c, tk.begin()));
            if (tk.is(T_LBRACE) || tk.is(T_LPAREN)) {
                ++braceDepth;

                // if a folding block opens at the beginning of a line, treat the entire line
                // as if it were inside the folding block
                if (tk.begin() == firstNonSpace) {
                    ++foldingIndent;
                    BaseTextDocumentLayout::userData(currentBlock())->setFoldingStartIncluded(true);
                }
            }
        } else if (tk.is(T_RPAREN) || tk.is(T_RBRACE) || tk.is(T_RBRACKET)) {
            const QChar c = text.at(tk.begin());
            parentheses.append(Parenthesis(Parenthesis::Closed, c, tk.begin()));
            if (tk.is(T_RBRACE) || tk.is(T_RPAREN)) {
                --braceDepth;
                if (braceDepth < foldingIndent) {
                    // unless we are at the end of the block, we reduce the folding indent
                    if (i == tokens.size()-1 || tokens.at(i+1).is(T_SEMICOLON))
                        BaseTextDocumentLayout::userData(currentBlock())->setFoldingEndIncluded(true);
                    else
                        foldingIndent = qMin(braceDepth, foldingIndent);
                }
            }
        }

        bool highlightCurrentWordAsPreprocessor = expectPreprocessorKeyword;

        if (expectPreprocessorKeyword)
            expectPreprocessorKeyword = false;

        if (onlyHighlightComments && !tk.isComment())
            continue;

        if (i == 0 && tk.is(T_POUND)) {
            highlightLine(text, tk.begin(), tk.length(), m_creatorFormats[SyntaxHighlighter::PreprocessorFormat]);
            expectPreprocessorKeyword = true;
        } else if (highlightCurrentWordAsPreprocessor &&
                   (tk.isKeyword() || tk.is(T_IDENTIFIER)) && isPPKeyword(text.midRef(tk.begin(), tk.length()))) {
            setFormat(tk.begin(), tk.length(), m_creatorFormats[SyntaxHighlighter::PreprocessorFormat]);
            const QStringRef ppKeyword = text.midRef(tk.begin(), tk.length());
            if (ppKeyword == QLatin1String("error")
                    || ppKeyword == QLatin1String("warning")
                    || ppKeyword == QLatin1String("pragma")) {
                onlyHighlightComments = true;
            }
        } else if (tk.is(T_NUMERIC_LITERAL)) {
            setFormat(tk.begin(), tk.length(), m_creatorFormats[SyntaxHighlighter::Decimal]);
        } else if (tk.is(T_RAW_STRING_LITERAL)) {
            const int startPosition = initialLexerState ? previousTokenEnd : tk.begin();
            highlightLine(text, startPosition, tk.end() - startPosition, m_creatorFormats[SyntaxHighlighter::String]);
            // we need to insert a close comment parenthesis, if
            //  - the line starts in a C Comment (initalState != 0)
            //  - the first token of the line is a T_COMMENT (i == 0 && tk.is(T_COMMENT))
            //  - is not a continuation line (tokens.size() > 1 || !state)
            if (initialLexerState && i == 0 && (tokens.size() > 1 || !lexerState)) {
                --braceDepth;
                // unless we are at the end of the block, we reduce the folding indent
                if (i == tokens.size()-1)
                    BaseTextDocumentLayout::userData(currentBlock())->setFoldingEndIncluded(true);
                else
                    foldingIndent = qMin(braceDepth, foldingIndent);
                const int tokenEnd = tk.begin() + tk.length() - 1;
                parentheses.append(Parenthesis(Parenthesis::Closed, QLatin1Char('-'), tokenEnd));
                // clear the initial state.
                initialLexerState = 0;
            }
        } else if (tk.isStringLiteral() || tk.isCharLiteral()) {
            highlightLine(text, tk.begin(), tk.length(), m_creatorFormats[SyntaxHighlighter::String]);
        } else if (tk.isComment()) {
            const int startPosition = initialLexerState ? previousTokenEnd : tk.begin();
            if (tk.is(T_COMMENT) || tk.is(T_CPP_COMMENT))
                highlightLine(text, startPosition, tk.end() - startPosition, m_creatorFormats[SyntaxHighlighter::Comment]);
            else // a doxygen comment
                highlightDoxygenComment(text, startPosition, tk.end() - startPosition);
            // we need to insert a close comment parenthesis, if
            //  - the line starts in a C Comment (initalState != 0)
            //  - the first token of the line is a T_COMMENT (i == 0 && tk.is(T_COMMENT))
            //  - is not a continuation line (tokens.size() > 1 || !state)
            if (initialLexerState && i == 0 && (tokens.size() > 1 || !lexerState)) {
                --braceDepth;
                // unless we are at the end of the block, we reduce the folding indent
                if (i == tokens.size()-1)
                    BaseTextDocumentLayout::userData(currentBlock())->setFoldingEndIncluded(true);
                else
                    foldingIndent = qMin(braceDepth, foldingIndent);
                const int tokenEnd = tk.begin() + tk.length() - 1;
                parentheses.append(Parenthesis(Parenthesis::Closed, QLatin1Char('-'), tokenEnd));

                // clear the initial state.
                initialLexerState = 0;
            }

        } else if (tk.isGoKeyword()) {
            setFormat(tk.begin(), tk.length(), m_creatorFormats[SyntaxHighlighter::Keyword]);
        } else if (tk.isGoTyped()) {
            setFormat(tk.begin(), tk.length(), m_creatorFormats[SyntaxHighlighter::DataType]);
        } else if (i == 0 && tokens.size() > 1 && tk.is(T_IDENTIFIER) && tokens.at(1).is(T_COLON)) {
            setFormat(tk.begin(), tk.length(), m_creatorFormats[SyntaxHighlighter::String]);
        } else if (tk.is(T_IDENTIFIER)) {
            highlightWord(text.midRef(tk.begin(), tk.length()), tk.begin(), tk.length());
        }
    }

    // mark the trailing white spaces
    const int lastTokenEnd = tokens.last().end();
    if (text.length() > lastTokenEnd)
        highlightLine(text, lastTokenEnd, text.length() - lastTokenEnd, m_creatorFormats[SyntaxHighlighter::VisualWhitespace]);

    if (!initialLexerState && lexerState && !tokens.isEmpty()) {
        const Token &lastToken = tokens.last();
        if (lastToken.is(T_COMMENT) || lastToken.is(T_DOXY_COMMENT) || lastToken.is(T_RAW_STRING_LITERAL)) {
            parentheses.append(Parenthesis(Parenthesis::Opened, QLatin1Char('+'),
                                           lastToken.begin()));
            ++braceDepth;
        }
    }

    BaseTextDocumentLayout::setParentheses(currentBlock(), parentheses);

    // if the block is ifdefed out, we only store the parentheses, but

    // do not adjust the brace depth.
    if (BaseTextDocumentLayout::ifdefedOut(currentBlock())) {
        braceDepth = initialBraceDepth;
        foldingIndent = initialBraceDepth;
    }

    BaseTextDocumentLayout::setFoldingIndent(currentBlock(), foldingIndent);

    // optimization: if only the brace depth changes, we adjust subsequent blocks
    // to have QSyntaxHighlighter stop the rehighlighting
    int currentState = currentBlockState();
    if (currentState != -1) {
        int oldState = currentState & 0xff;
        int oldBraceDepth = currentState >> 8;
        if (oldState == tokenize.state() && oldBraceDepth != braceDepth) {
            BaseTextDocumentLayout::FoldValidator foldValidor;
            foldValidor.setup(qobject_cast<BaseTextDocumentLayout *>(document()->documentLayout()));
            int delta = braceDepth - oldBraceDepth;
            QTextBlock block = currentBlock().next();
            while (block.isValid() && block.userState() != -1) {
                BaseTextDocumentLayout::changeBraceDepth(block, delta);
                BaseTextDocumentLayout::changeFoldingIndent(block, delta);
                foldValidor.process(block);
                block = block.next();
            }
            foldValidor.finalize();
        }
    }

    setCurrentBlockState((braceDepth << 8) | tokenize.state());
}

bool GolangHighlighter::isPPKeyword(const QStringRef &text) const
{
    switch (text.length())
    {
    case 2:
        if (text.at(0) == QLatin1Char('i') && text.at(1) == QLatin1Char('f'))
            return true;
        break;

    case 4:
        if (text.at(0) == QLatin1Char('e')
            && (text == QLatin1String("elif") || text == QLatin1String("else")))
            return true;
        break;

    case 5:
        switch (text.at(0).toLatin1()) {
        case 'i':
            if (text == QLatin1String("ifdef"))
                return true;
            break;
          case 'u':
            if (text == QLatin1String("undef"))
                return true;
            break;
        case 'e':
            if (text == QLatin1String("endif") || text == QLatin1String("error"))
                return true;
            break;
        }
        break;

    case 6:
        switch (text.at(0).toLatin1()) {
        case 'i':
            if (text == QLatin1String("ifndef") || text == QLatin1String("import"))
                return true;
            break;
        case 'd':
            if (text == QLatin1String("define"))
                return true;
            break;
        case 'p':
            if (text == QLatin1String("pragma"))
                return true;
            break;
        }
        break;

    case 7:
        switch (text.at(0).toLatin1()) {
        case 'i':
            if (text == QLatin1String("include"))
                return true;
            break;
        case 'w':
            if (text == QLatin1String("warning"))
                return true;
            break;
        }
        break;

    case 12:
        if (text.at(0) == QLatin1Char('i') && text == QLatin1String("include_next"))
            return true;
        break;

    default:
        break;
    }

    return false;
}

void GolangHighlighter::highlightLine(const QString &text, int position, int length,
                                   const QTextCharFormat &format)
{
    QTextCharFormat visualSpaceFormat = m_creatorFormats[SyntaxHighlighter::VisualWhitespace];
    visualSpaceFormat.setBackground(format.background());

    const int end = position + length;
    int index = position;

    while (index != end) {
        const bool isSpace = text.at(index).isSpace();
        const int start = index;

        do { ++index; }
        while (index != end && text.at(index).isSpace() == isSpace);

        const int tokenLength = index - start;
        if (isSpace)
            setFormat(start, tokenLength, visualSpaceFormat);
        else if (format.isValid())
            setFormat(start, tokenLength, format);
    }
}

void GolangHighlighter::highlightWord(QStringRef word, int position, int length)
{
    // try to highlight Qt 'identifiers' like QObject and Q_PROPERTY

    if (word.length() > 2 && word.at(0) == QLatin1Char('Q')) {
        if (word.at(1) == QLatin1Char('_') // Q_
            || (word.at(1) == QLatin1Char('T') && word.at(2) == QLatin1Char('_'))) { // QT_
            for (int i = 1; i < word.length(); ++i) {
                const QChar &ch = word.at(i);
                if (!(ch.isUpper() || ch == QLatin1Char('_')))
                    return;
            }

            setFormat(position, length, m_creatorFormats[SyntaxHighlighter::DataType]);
        }
    }
}

void GolangHighlighter::highlightDoxygenComment(const QString &text, int position, int)
{
//    int initial = position;

//    const QChar *uc = text.unicode();
//    const QChar *it = uc + position;

//    const QTextCharFormat &format = formatForCategory(CppDoxygenCommentFormat);
//    const QTextCharFormat &kwFormat = formatForCategory(CppDoxygenTagFormat);

//    while (!it->isNull()) {
//        if (it->unicode() == QLatin1Char('\\') ||
//            it->unicode() == QLatin1Char('@')) {
//            ++it;

//            const QChar *start = it;
//            while (it->isLetterOrNumber() || it->unicode() == '_')
//                ++it;

//            int k = CppTools::classifyDoxygenTag(start, it - start);
//            if (k != CppTools::T_DOXY_IDENTIFIER) {
//                highlightLine(text, initial, start - uc - initial, format);
//                setFormat(start - uc - 1, it - start + 1, kwFormat);
//                initial = it - uc;
//            }
//        } else
//            ++it;
//    }

//    highlightLine(text, initial, it - uc - initial, format);
}

