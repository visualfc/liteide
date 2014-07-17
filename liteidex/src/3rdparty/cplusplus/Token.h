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

#ifndef CPLUSPLUS_TOKEN_H
#define CPLUSPLUS_TOKEN_H

namespace CPlusPlus {

enum Kind {
    T_EOF_SYMBOL = 0,
    T_ERROR,

    T_CPP_COMMENT,
    T_CPP_DOXY_COMMENT,
    T_COMMENT,
    T_DOXY_COMMENT,
    T_IDENTIFIER,

    T_FIRST_LITERAL,
    T_NUMERIC_LITERAL = T_FIRST_LITERAL,
    T_FIRST_CHAR_LITERAL,
    T_CHAR_LITERAL = T_FIRST_CHAR_LITERAL,
    T_WIDE_CHAR_LITERAL,
    T_UTF16_CHAR_LITERAL,
    T_UTF32_CHAR_LITERAL,
    T_LAST_CHAR_LITERAL = T_UTF32_CHAR_LITERAL,
    T_FIRST_STRING_LITERAL,
    T_STRING_LITERAL = T_FIRST_STRING_LITERAL,
    T_WIDE_STRING_LITERAL,
    T_UTF8_STRING_LITERAL,
    T_UTF16_STRING_LITERAL,
    T_UTF32_STRING_LITERAL,
    T_RAW_STRING_LITERAL,
    T_RAW_WIDE_STRING_LITERAL,
    T_RAW_UTF8_STRING_LITERAL,
    T_RAW_UTF16_STRING_LITERAL,
    T_RAW_UTF32_STRING_LITERAL,
    T_AT_STRING_LITERAL,
    T_ANGLE_STRING_LITERAL,
    T_LAST_STRING_LITERAL = T_ANGLE_STRING_LITERAL,
    T_LAST_LITERAL = T_ANGLE_STRING_LITERAL,

    T_FIRST_OPERATOR,
    T_AMPER = T_FIRST_OPERATOR,
    T_AMPER_AMPER,
    T_AMPER_EQUAL,
    T_ARROW,
    T_ARROW_STAR,
    T_CARET,
    T_CARET_EQUAL,
    T_COLON,
    T_COLON_EQUAL,
    T_COLON_COLON,
    T_COMMA,
    T_SLASH,
    T_SLASH_EQUAL,
    T_DOT,
    T_DOT_DOT_DOT,
    T_DOT_STAR,
    T_EQUAL,
    T_EQUAL_EQUAL,
    T_EXCLAIM,
    T_EXCLAIM_EQUAL,
    T_GREATER,
    T_GREATER_EQUAL,
    T_GREATER_GREATER,
    T_GREATER_GREATER_EQUAL,
    T_LBRACE,
    T_LBRACKET,
    T_LESS,
    T_LESS_EQUAL,
    T_LESS_LESS,
    T_LESS_LESS_EQUAL,
    T_LPAREN,
    T_MINUS,
    T_MINUS_EQUAL,
    T_MINUS_MINUS,
    T_PERCENT,
    T_PERCENT_EQUAL,
    T_PIPE,
    T_PIPE_EQUAL,
    T_PIPE_PIPE,
    T_PLUS,
    T_PLUS_EQUAL,
    T_PLUS_PLUS,
    T_POUND,
    T_POUND_POUND,
    T_QUESTION,
    T_RBRACE,
    T_RBRACKET,
    T_RPAREN,
    T_SEMICOLON,
    T_STAR,
    T_STAR_EQUAL,
    T_TILDE,
    T_TILDE_EQUAL,
    T_LAST_OPERATOR = T_TILDE_EQUAL,

    T_FIRST_KEYWORD,
    T_ALIGNAS = T_FIRST_KEYWORD,
    T_ALIGNOF,
    T_ASM,
    T_AUTO,
    T_BOOL,
    T_BREAK,
    T_CASE,
    T_CATCH,
    T_CHAR,
    T_CHAR16_T,
    T_CHAR32_T,
    T_CLASS,
    T_CONST,
    T_CONST_CAST,
    T_CONSTEXPR,
    T_CONTINUE,
    T_DECLTYPE,
    T_DEFAULT,
    T_DELETE,
    T_DO,
    T_DOUBLE,
    T_DYNAMIC_CAST,
    T_ELSE,
    T_ENUM,
    T_EXPLICIT,
    T_EXPORT,
    T_EXTERN,
    T_FALSE,
    T_FLOAT,
    T_FOR,
    T_FRIEND,
    T_GOTO,
    T_IF,
    T_INLINE,
    T_INT,
    T_LONG,
    T_MUTABLE,
    T_NAMESPACE,
    T_NEW,
    T_NOEXCEPT,
    T_NULLPTR,
    T_OPERATOR,
    T_PRIVATE,
    T_PROTECTED,
    T_PUBLIC,
    T_REGISTER,
    T_REINTERPRET_CAST,
    T_RETURN,
    T_SHORT,
    T_SIGNED,
    T_SIZEOF,
    T_STATIC,
    T_STATIC_ASSERT,
    T_STATIC_CAST,
    T_STRUCT,
    T_SWITCH,
    T_TEMPLATE,
    T_THIS,
    T_THREAD_LOCAL,
    T_THROW,
    T_TRUE,
    T_TRY,
    T_TYPEDEF,
    T_TYPEID,
    T_TYPENAME,
    T_UNION,
    T_UNSIGNED,
    T_USING,
    T_VIRTUAL,
    T_VOID,
    T_VOLATILE,
    T_WCHAR_T,
    T_WHILE,

    T___ATTRIBUTE__,
    T___THREAD,
    T___TYPEOF__,

    // obj c++ @ keywords
    T_FIRST_OBJC_AT_KEYWORD,

    T_AT_CATCH = T_FIRST_OBJC_AT_KEYWORD,
    T_AT_CLASS,
    T_AT_COMPATIBILITY_ALIAS,
    T_AT_DEFS,
    T_AT_DYNAMIC,
    T_AT_ENCODE,
    T_AT_END,
    T_AT_FINALLY,
    T_AT_IMPLEMENTATION,
    T_AT_INTERFACE,
    T_AT_NOT_KEYWORD,
    T_AT_OPTIONAL,
    T_AT_PACKAGE,
    T_AT_PRIVATE,
    T_AT_PROPERTY,
    T_AT_PROTECTED,
    T_AT_PROTOCOL,
    T_AT_PUBLIC,
    T_AT_REQUIRED,
    T_AT_SELECTOR,
    T_AT_SYNCHRONIZED,
    T_AT_SYNTHESIZE,
    T_AT_THROW,
    T_AT_TRY,

    T_LAST_OBJC_AT_KEYWORD = T_AT_TRY,

    T_FIRST_QT_KEYWORD,

    // Qt keywords
    T_EMIT = T_FIRST_QT_KEYWORD,
    T_SIGNAL,
    T_SLOT,
    T_Q_SIGNAL,
    T_Q_SLOT,
    T_Q_SIGNALS,
    T_Q_SLOTS,
    T_Q_FOREACH,
    T_Q_D,
    T_Q_Q,
    T_Q_INVOKABLE,
    T_Q_PROPERTY,
    T_Q_PRIVATE_PROPERTY,
    T_Q_INTERFACES,
    T_Q_EMIT,
    T_Q_ENUMS,
    T_Q_FLAGS,
    T_Q_PRIVATE_SLOT,
    T_Q_DECLARE_INTERFACE,
    T_Q_OBJECT,
    T_Q_GADGET,
    T_LAST_KEYWORD = T_Q_GADGET,

    T_FIRST_GO_KEYWORD,
    T_GO_BREAK = T_FIRST_GO_KEYWORD,
    T_GO_DEFAULT,
    T_GO_FUNC,
    T_GO_INTERFACE,
    T_GO_SELECT,
    T_GO_CASE,
    T_GO_DEFER,
    T_GO_GO,
    T_GO_MAP,
    T_GO_STRUCT,
    T_GO_CHAN,
    T_GO_ELSE,
    T_GO_GOTO,
    T_GO_PACKAGE,
    T_GO_SWITCH,
    T_GO_CONST,
    T_GO_FALLTHROUGH,
    T_GO_IF,
    T_GO_RANGE,
    T_GO_TYPE,
    T_GO_CONTINUE,
    T_GO_FOR,
    T_GO_IMPORT,
    T_GO_RETURN,
    T_GO_VAR,
    T_LAST_GO_KEYWORD = T_GO_VAR,

    T_FIRST_GO_TYPED,
    T_GO_BOOL = T_FIRST_GO_TYPED,
    T_GO_UINT8,
    T_GO_UINT16,
    T_GO_UINT32,
    T_GO_UINT64,
    T_GO_INT8,
    T_GO_INT16,
    T_GO_INT32,
    T_GO_INT64,
    T_GO_FLOAT32,
    T_GO_FLOAT64,
    T_GO_COMPLEX64,
    T_GO_COMPLEX128,
    T_GO_BYTE,
    T_GO_RUNE,
    T_GO_UINT,
    T_GO_INT,
    T_GO_UINTPTR,
    T_GO_TRUE,
    T_GO_FALSE,
    T_GO_NIL,
    T_GO_IOTA,
    T_LAST_GO_TYPED = T_GO_IOTA,

    // aliases
    T_OR = T_PIPE_PIPE,
    T_AND = T_AMPER_AMPER,
    T_NOT = T_EXCLAIM,
    T_XOR = T_CARET,
    T_BITOR = T_PIPE,
    T_COMPL = T_TILDE,
    T_OR_EQ = T_PIPE_EQUAL,
    T_AND_EQ = T_AMPER_EQUAL,
    T_BITAND = T_AMPER,
    T_NOT_EQ = T_EXCLAIM_EQUAL,
    T_XOR_EQ = T_CARET_EQUAL,

    T___ASM = T_ASM,
    T___ASM__ = T_ASM,

    T_TYPEOF = T___TYPEOF__,
    T___TYPEOF = T___TYPEOF__,

    T___DECLTYPE = T_DECLTYPE,

    T___INLINE = T_INLINE,
    T___INLINE__ = T_INLINE,

    T___CONST = T_CONST,
    T___CONST__ = T_CONST,

    T___VOLATILE = T_VOLATILE,
    T___VOLATILE__ = T_VOLATILE,

    T___ATTRIBUTE = T___ATTRIBUTE__
};

class Token
{
public:
    Token() : flags(0), offset(0), ptr(0) {}

    inline bool is(unsigned k) const    { return f.kind == k; }
    inline bool isNot(unsigned k) const { return f.kind != k; }
    const char *spell() const;
    void reset();

    inline unsigned kind() const { return f.kind; }
    inline bool newline() const { return f.newline; }
    inline bool whitespace() const { return f.whitespace; }
    inline bool joined() const { return f.joined; }
    inline bool expanded() const { return f.expanded; }
    inline bool generated() const { return f.generated; }
    inline unsigned length() const { return f.length; }

    inline unsigned begin() const
    { return offset; }

    inline unsigned end() const
    { return offset + f.length; }

    inline bool isLiteral() const
    { return f.kind >= T_FIRST_LITERAL && f.kind <= T_LAST_LITERAL; }

    inline bool isCharLiteral() const
    { return f.kind >= T_FIRST_CHAR_LITERAL && f.kind <= T_LAST_CHAR_LITERAL; }

    inline bool isStringLiteral() const
    { return f.kind >= T_FIRST_STRING_LITERAL && f.kind <= T_LAST_STRING_LITERAL; }

    inline bool isOperator() const
    { return f.kind >= T_FIRST_OPERATOR && f.kind <= T_LAST_OPERATOR; }

    inline bool isKeyword() const
    { return f.kind >= T_FIRST_KEYWORD && f.kind < T_FIRST_QT_KEYWORD; }

    inline bool isComment() const
    { return f.kind == T_COMMENT || f.kind == T_DOXY_COMMENT ||
      f.kind == T_CPP_COMMENT || f.kind == T_CPP_DOXY_COMMENT; }

    inline bool isObjCAtKeyword() const
    { return f.kind >= T_FIRST_OBJC_AT_KEYWORD && f.kind <= T_LAST_OBJC_AT_KEYWORD; }

    inline bool isGoKeyword() const
    { return f.kind >= T_FIRST_GO_KEYWORD && f.kind <= T_LAST_GO_KEYWORD; }

    inline bool isGoTyped() const
    { return f.kind >= T_FIRST_GO_TYPED && f.kind <= T_LAST_GO_TYPED; }

    static const char *name(int kind);

public:
    struct Flags {
        // The token kind.
        unsigned kind       : 8;
        // The token starts a new line.
        unsigned newline    : 1;
        // The token is preceded by whitespace(s).
        unsigned whitespace : 1;
        // The token is joined with the previous one.
        unsigned joined     : 1;
        // The token originates from a macro expansion.
        unsigned expanded   : 1;
        // The token originates from a macro expansion and does not correspond to an
        // argument that went through substitution. Notice the example:
        //
        // #define FOO(a, b) a + b;
        // FOO(1, 2)
        //
        // After preprocessing we would expect the following tokens: 1 + 2;
        // Tokens '1', '+', '2', and ';' are all expanded. However only tokens '+' and ';'
        // are generated.
        unsigned generated  : 1;
        // Unused...
        unsigned pad        : 3;
        // The token length.
        unsigned length     : 16;
    };
    union {
        unsigned flags;
        Flags f;
    };

    unsigned offset;

    union {
        void *ptr;
        //const Literal *literal;
        //const NumericLiteral *number;
        //const StringLiteral *string;
        //const Identifier *identifier;
        //unsigned close_brace;
        //unsigned lineno;
    };
};

struct LanguageFeatures
{
    LanguageFeatures() : flags(0) {}

    union {
        unsigned int flags;
        struct {
            unsigned int qtEnabled : 1; // If Qt is used.
            unsigned int qtMocRunEnabled : 1;
            unsigned int qtKeywordsEnabled : 1; // If Qt is used but QT_NO_KEYWORDS defined
            unsigned int cxx11Enabled : 1;
            unsigned int objCEnabled : 1;
            unsigned int golangEnable : 1;
        };
    };
};

} // namespace CPlusPlus


#endif // CPLUSPLUS_TOKEN_H
