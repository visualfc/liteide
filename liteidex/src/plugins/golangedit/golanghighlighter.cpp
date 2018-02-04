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
// Module: golanghighlighter.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golanghighlighter.h"

#include "qtc_texteditor/basetextdocumentlayout.h"

#include "cplusplus/SimpleLexer.h"
#include "cplusplus/Lexer.h"

#include <QTextDocument>
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

using namespace TextEditor;
using namespace CPlusPlus;

namespace {
  /**
   * Checks the "function()" formats. 
   */
  bool maybeIsFunctionCall(const QList<Token> &tks, int idx) {
      const Token& tk = tks[idx];
      if (!tk.isGoBuiltin() && !tk.is(T_IDENTIFIER)) {
          return false;
      }

      int tkCnt = tks.size();
      for (int i = idx + 1; i < tkCnt; ++i) {
          if (tks[i].isComment()) {
              continue;
          }
          return tks[i].is(T_LPAREN) || tks[i].is(T_LBRACE);
      }
      return false;
  }
}

GolangHighlighter::GolangHighlighter(LiteApi::ITextEditor *editor, QTextDocument *document) :
    TextEditor::SyntaxHighlighter(document)
{
    m_todoList = QString("TODO,BUG,FIXME,NOTE,SECBUG").split(",");
    m_todoRegexp.setPattern(QString("^(%1)([\\s\\:\\(\\,]|$)").arg(m_todoList.join("|")));
    m_gotagList = QString("+build").split(",");
    TextEditor::SyntaxComment comment;
    comment.singleLineComment = "//";
    comment.multiLineCommentStart = "/*";
    comment.multiLineCommentEnd = "*/";
    comment.isCommentAfterWhiteSpaces = false;
    setupComment(comment);
}

GolangHighlighter::~GolangHighlighter()
{
}

void GolangHighlighter::highlightBlock(const QString &text)
{
    int blockNumber = this->currentBlock().blockNumber();
    m_currentTodo.clear();
    highlightBlockHelper(text);
    QString old = m_todoInfoMap.value(blockNumber);
    if (m_currentTodo != old) {
        if (m_currentTodo.isEmpty()) {
            m_todoInfoMap.remove(blockNumber);
        } else {
            m_todoInfoMap.insert(blockNumber,m_currentTodo);
        }
    }
}


void GolangHighlighter::highlightBlockHelper(const QString &text)
{
    const int previousBlockState_ = previousBlockState();
    int lexerState = 0, initialBraceDepth = 0;
    if (previousBlockState_ != -1) {
        lexerState = previousBlockState_ & 0xff;
        initialBraceDepth = previousBlockState_ >> 8;
    }
    //fix brace not matcher
    if (initialBraceDepth < 0) {
        initialBraceDepth = 0;
    }

    int braceDepth = initialBraceDepth;


    LanguageFeatures features;
    features.golangEnable = true;
    SimpleLexer tokenize;
    tokenize.setLanguageFeatures(features);

    //hack token convert no ansi code
    QString tk_text = text;
    for (int i = 0; i < tk_text.length(); i++) {
        if (tk_text[i].unicode() > 0x7f) {
            tk_text[i] = '_';
        }
    }

    int initialLexerState = lexerState;
    QList<Token> tokens = tokenize(tk_text, initialLexerState);
    lexerState = tokenize.state(); // refresh lexer state

    initialLexerState &= ~0x80; // discard newline expected bit
    int foldingIndent = initialBraceDepth;
    if (TextBlockUserData *userData = BaseTextDocumentLayout::testUserData(currentBlock())) {
        //userData->setFoldingIndent(0);
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
        //BaseTextDocumentLayout::setFoldingIndent(currentBlock(), foldingIndent);
        setFoldingIndent(currentBlock(), foldingIndent);
        return;
    }

    const unsigned firstNonSpace = tokens.first().begin();

    Parentheses parentheses;
    parentheses.reserve(20); // assume wizard level ;-)

    //bool expectPreprocessorKeyword = false;
    //bool onlyHighlightComments = false;
    for (int i = 0; i < tokens.size(); ++i) {
        Token &tk = tokens[i];

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
                    BaseTextDocumentLayout::userData(currentBlock())->setFoldingEndIncluded(true);
                    //if (i == tokens.size()-1 || tokens.at(i+1).is(T_SEMICOLON))
                    //else
                    //    foldingIndent = qMin(braceDepth, foldingIndent);
                }
            }
        }
        /*
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
        } else */
        if (tk.is(T_NUMERIC_LITERAL)) {
            setFormat(tk.begin(), tk.length(), m_creatorFormats[SyntaxHighlighter::Decimal]);
        } else if (tk.isOperator()) {
            setFormat(tk.begin(), tk.length(), m_creatorFormats[SyntaxHighlighter::RegionMarker]);
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
            if (tk.is(T_CPP_COMMENT)) {
                highlightCommentLine(text, startPosition, tk.end() - startPosition);
            } else {
                highlightLine(text, startPosition, tk.end() - startPosition, m_creatorFormats[SyntaxHighlighter::Comment]);
            }
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
            if (tk.is(T_GO_PACKAGE)) {
                int n = i+1;
                if (n < tokens.size() && tokens[n].is(T_IDENTIFIER)) {
                    setContextData("go.package",text.mid(tokens[n].begin(),tokens[n].length()));
                }
            } else if (tk.is(T_GO_FUNC)) {
                int size = tokens.size()-1;
                int n = i+1;
                //func (sz *Size) Width()
                if ((i == 0) && (n < size)) {
                    if (tokens[n].is(T_LPAREN)) {
                        while(n++ < size) {
                            if (tokens[n].is(T_RPAREN)) {
                                setFormat(tokens[n-1].begin(), tokens[n-1].length(), m_creatorFormats[SyntaxHighlighter::DataType]);
                                n++;
                                if ((n < size) && tokens[n].is(T_IDENTIFIER)) {
                                    setFormat(tokens[n].begin(),tokens[n].length(),m_creatorFormats[SyntaxHighlighter::FuncDecl]);
//                                    n++;
//                                    if (n < size && tokens[n].is(T_LPAREN)) {
//                                        while(n++ < size) {
//                                            if (tokens[n].is(T_RPAREN) || tokens[n].is(T_COMMA)) {
//                                                setFormat(tokens[n-1].begin(), tokens[n-1].length(), m_creatorFormats[SyntaxHighlighter::DataType]);
//                                            }
//                                        }
//                                    }
                                }
                                break;
                            }
                        }
                    } else if (tokens[n].is(T_IDENTIFIER)) {
                        setFormat(tokens[n].begin(),tokens[n].length(),m_creatorFormats[SyntaxHighlighter::FuncDecl]);
//                        n++;
//                        if (n < size && tokens[n].is(T_LPAREN)) {
//                            while(n++ < size) {
//                                if (tokens[n].is(T_RPAREN) || tokens[n].is(T_COMMA)) {
//                                    setFormat(tokens[n-1].begin(), tokens[n-1].length(), m_creatorFormats[SyntaxHighlighter::DataType]);
//                                }
//                            }
//                        }
                    }
                }
            } else if (tk.is(T_GO_TYPE)) {
                int size = tokens.size()-1;
                int n = i+1;
                if (n < size && tokens[n].is(T_IDENTIFIER)) {
                    setFormat(tokens[n].begin(), tokens[n].length(), m_creatorFormats[SyntaxHighlighter::DataType]);
                }
            } else if (tk.is(T_GO_VAR)) {

            }
        } else if (tk.isGoTyped()) {
            if ((i+1 == tokens.size()) || !tokens[i+1].is(T_DOT)) {
                setFormat(tk.begin(), tk.length(), m_creatorFormats[SyntaxHighlighter::DataType]);
            }
        } else if (tk.isGoBuiltin()) {
            setFormat(tk.begin(), tk.length(), m_creatorFormats[SyntaxHighlighter::BuiltinFunc]);
        } else if (tk.isGoPredecl()) {
            setFormat(tk.begin(), tk.length(), m_creatorFormats[SyntaxHighlighter::Predeclared]);
        } else if (i == 0 && tokens.size() > 1 && tokens.at(0).begin() == 0 && tk.is(T_IDENTIFIER) && tokens.at(1).is(T_COLON)) {
            setFormat(tk.begin(), tk.length(), m_creatorFormats[SyntaxHighlighter::DataType]);
        } else if (maybeIsFunctionCall(tokens, i)) {
            setFormat(tk.begin(), tk.length(), m_creatorFormats[SyntaxHighlighter::Function]);
        } else if (tk.is(T_IDENTIFIER)) {
           // highlightWord(text.midRef(tk.begin(), tk.length()), tk.begin(), tk.length());
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
   // qDebug() << text << foldingIndent;
    //BaseTextDocumentLayout::setFoldingIndent(currentBlock(), foldingIndent);
    setFoldingIndent(currentBlock(), foldingIndent);

    // optimization: if only the brace depth changes, we adjust subsequent blocks
    // to have QSyntaxHighlighter stop the rehighlighting
//    int currentState = currentBlockState();
//    if (currentState != -1) {
//        int oldState = currentState & 0xff;
//        int oldBraceDepth = currentState >> 8;
//        if (oldState == tokenize.state() && oldBraceDepth != braceDepth) {
//            BaseTextDocumentLayout::FoldValidator foldValidor;
//            foldValidor.setup(qobject_cast<BaseTextDocumentLayout *>(document()->documentLayout()));
//            int delta = braceDepth - oldBraceDepth;
//            QTextBlock block = currentBlock().next();
//            while (block.isValid() && block.userState() != -1) {
//                BaseTextDocumentLayout::changeBraceDepth(block, delta);
//                BaseTextDocumentLayout::changeFoldingIndent(block, delta);
//                foldValidor.process(block);
//                block = block.next();
//            }
//            foldValidor.finalize();
//        }
//    }

    setCurrentBlockState((braceDepth << 8) | tokenize.state());
}

void GolangHighlighter::setFoldingIndent(const QTextBlock &block, int indent)
{
    TextBlockUserData *userData = BaseTextDocumentLayout::userData(block);
    if (userData->foldingIndent() != indent) {
        emit foldIndentChanged(block);
        userData->setFoldingIndent(qMax(0,indent));
    }
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

void GolangHighlighter::highlightCommentLine(const QString &text, int position, int length)
{
    QTextCharFormat format = m_creatorFormats[SyntaxHighlighter::Comment];
    QTextCharFormat todoFormat = m_creatorFormats[SyntaxHighlighter::ToDo];
    QTextCharFormat visualSpaceFormat = m_creatorFormats[SyntaxHighlighter::VisualWhitespace];
    visualSpaceFormat.setBackground(format.background());

    const int end = position + length;
    int index = position;

    if (text.mid(position).startsWith("//")) {
        setFormat(position, 2, format);
        index += 2;
    }

    int first = true;
    while (index != end) {
        const bool isSpace = text.at(index).isSpace();
        const int start = index;

        do { ++index; }
        while (index != end && text.at(index).isSpace() == isSpace);

        const int tokenLength = index - start;
        if (isSpace) {
            setFormat(start, tokenLength, visualSpaceFormat);
        } else if (format.isValid()) {
            if (first) {
                first = false;
                int index = m_todoRegexp.indexIn(text.mid(start,tokenLength));
                if (index == 0) {
                    int todoLen = m_todoRegexp.cap(1).length();
                    setFormat(start,todoLen,todoFormat);
                    if (todoLen < length) {
                        setFormat(start+todoLen,length-todoLen, format);
                    }
                    m_currentTodo = text.mid(start);
                    break;
                }
                if (m_gotagList.contains(text.mid(start,tokenLength))) {
                    setFormat(position,length,todoFormat);
                    break;
                }
                if (text.mid(start).startsWith("import") && text.startsWith("package")) {
                    setFormat(position,length,todoFormat);
                    break;
                }
                if (text.mid(start).startsWith("go:")) {
                    setFormat(position,length,todoFormat);
                    break;
                }
            }
            setFormat(start, tokenLength, format);
        }
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
