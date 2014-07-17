// Copyright (c) 2008 Roberto Raggi <roberto.raggi@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef CPLUSPLUS_LEXER_H
#define CPLUSPLUS_LEXER_H

#include "Token.h"

namespace CPlusPlus {

class Lexer
{
    Lexer(const Lexer &other);
    void operator =(const Lexer &other);

public:
    //Lexer(TranslationUnit *unit);
    Lexer(const char *firstChar, const char *lastChar);
    ~Lexer();

    //Control *control() const { return _control; }
    //TranslationUnit *translationUnit() const;

    void scan(Token *tok);

    inline void operator()(Token *tok)
    { scan(tok); }

    unsigned tokenOffset() const;
    unsigned tokenLength() const;
    const char *tokenBegin() const;
    const char *tokenEnd() const;
    unsigned currentLine() const;

    bool scanCommentTokens() const;
    void setScanCommentTokens(bool onoff);

    bool scanKeywords() const;
    void setScanKeywords(bool onoff);

    bool scanAngleStringLiteralTokens() const;
    void setScanAngleStringLiteralTokens(bool onoff);

    void setStartWithNewline(bool enabled);

    int state() const;
    void setState(int state);

    LanguageFeatures languageFeatures() const { return _languageFeatures; }
    void setLanguageFeatures(LanguageFeatures features) { _languageFeatures = features; }

private:
    void scan_helper(Token *tok);
    void setSource(const char *firstChar, const char *lastChar);
    static int classify(const char *string, int length, LanguageFeatures features);
    static int classifyObjCAtKeyword(const char *s, int n);
    static int golangTypes(const char *s, int n);
    static int golangKeywords(const char *s, int n);
    static int classifyOperator(const char *string, int length);

    void scanStringLiteral(Token *tok, unsigned char hint = 0);
    void scanGoRawStringLiteral(Token *tok, unsigned char hint = 0);
    void scanRawStringLiteral(Token *tok, unsigned char hint = 0);
    void scanCharLiteral(Token *tok, unsigned char hint = 0);
    void scanUntilQuote(Token *tok, unsigned char quote, bool containNewLines = false);
    void scanNumericLiteral(Token *tok);
    void scanIdentifier(Token *tok, unsigned extraProcessedChars = 0);
    void scanBackslash(Kind type);
    void scanCppComment(Kind type);

    inline void yyinp()
    {
        _yychar = *++_currentChar;
        if (_yychar == '\n')
            pushLineStartOffset();
    }

    void pushLineStartOffset();

private:
    struct Flags {
        unsigned _scanCommentTokens: 1;
        unsigned _scanKeywords: 1;
        unsigned _scanAngleStringLiteralTokens: 1;
    };

    struct State {
        unsigned char _tokenKind : 7;
        unsigned char _newlineExpected : 1;
    };

    //TranslationUnit *_translationUnit;
    //Control *_control;
    const char *_firstChar;
    const char *_currentChar;
    const char *_lastChar;
    const char *_tokenStart;
    unsigned char _yychar;
    union {
        unsigned char _state;
        State s;
    };
    union {
        unsigned _flags;
        Flags f;
    };
    unsigned _currentLine;
    LanguageFeatures _languageFeatures;

};

} // namespace CPlusPlus


#endif // CPLUSPLUS_LEXER_H
