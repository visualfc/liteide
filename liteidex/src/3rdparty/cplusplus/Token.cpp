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

#include "Token.h"

using namespace CPlusPlus;

const char *token_names[] = {
    (""), ("<error>"),

    ("<C++ comment>"), ("<C++ doxy comment>"),
    ("<comment>"), ("<doxy comment>"),

    ("<identifier>"),

    ("<numeric literal>"),
    ("<char literal>"), ("<wide char literal>"), ("<utf16 char literal>"), ("<utf32 char literal>"),
    ("<string literal>"), ("<wide string literal>"), ("<utf8 string literal>"),
    ("<utf16 string literal>"), ("<utf32 string literal>"),
    ("<raw string literal>"), ("<raw wide string literal>"), ("<raw utf8 string literal>"),
    ("<raw utf16 string literal>"), ("<raw utf32 string literal>"),
    ("<@string literal>"), ("<angle string literal>"),

    ("&"), ("&&"), ("&="), ("->"), ("->*"), ("^"), ("^="), (":"), (":="), ("::"),
    (","), ("/"), ("/="), ("."), ("..."), (".*"), ("="), ("=="), ("!"),
    ("!="), (">"), (">="), (">>"), (">>="), ("{"), ("["), ("<"), ("<="),
    ("<<"), ("<<="), ("("), ("-"), ("-="), ("--"), ("%"), ("%="), ("|"),
    ("|="), ("||"), ("+"), ("+="), ("++"), ("#"), ("##"), ("?"), ("}"),
    ("]"), (")"), (";"), ("*"), ("*="), ("~"), ("~="),

    ("alignas"), ("alignof"), ("asm"), ("auto"), ("bool"), ("break"), ("case"), ("catch"),
    ("char"), ("char16_t"), ("char32_t"),
    ("class"), ("const"), ("const_cast"), ("constexpr"), ("continue"),
    ("decltype"), ("default"),
    ("delete"), ("do"), ("double"), ("dynamic_cast"), ("else"), ("enum"),
    ("explicit"), ("export"), ("extern"), ("false"), ("float"), ("for"),
    ("friend"), ("goto"), ("if"), ("inline"), ("int"), ("long"),
    ("mutable"), ("namespace"), ("new"), ("noexcept"),
    ("nullptr"), ("operator"), ("private"),
    ("protected"), ("public"), ("register"), ("reinterpret_cast"),
    ("return"), ("short"), ("signed"), ("sizeof"), ("static"),("static_assert"),
    ("static_cast"), ("struct"), ("switch"), ("template"), ("this"), ("thread_local"),
    ("throw"), ("true"), ("try"), ("typedef"), ("typeid"), ("typename"),
    ("union"), ("unsigned"), ("using"), ("virtual"), ("void"),
    ("volatile"), ("wchar_t"), ("while"),

    // gnu
    ("__attribute__"), ("__thread"), ("__typeof__"),

    // objc @keywords
    ("@catch"), ("@class"), ("@compatibility_alias"), ("@defs"), ("@dynamic"),
    ("@encode"), ("@end"), ("@finally"), ("@implementation"), ("@interface"),
    ("@not_keyword"), ("@optional"), ("@package"), ("@private"), ("@property"),
    ("@protected"), ("@protocol"), ("@public"), ("@required"), ("@selector"),
    ("@synchronized"), ("@synthesize"), ("@throw"), ("@try"),

    // Qt keywords
    ("emit"), ("SIGNAL"), ("SLOT"), ("Q_SIGNAL"), ("Q_SLOT"), ("signals"), ("slots"),
    ("Q_FOREACH"), ("Q_D"), ("Q_Q"),
    ("Q_INVOKABLE"), ("Q_PROPERTY"), ("T_Q_PRIVATE_PROPERTY"),
    ("Q_INTERFACES"), ("Q_EMIT"), ("Q_ENUMS"), ("Q_FLAGS"),
    ("Q_PRIVATE_SLOT"), ("Q_DECLARE_INTERFACE"), ("Q_OBJECT"), ("Q_GADGET"),

    // Go keywords
    ("go"),("if"),("map"),("for"),("var"),("func"),
    ("case"),("chan"),("else"),("goto"),("type"),("break"),
    ("defer"),("const"),("range"),("select"),("struct"),("switch"),
    ("import"),("return"),("default"),("package"),("continue"),("interface"),
    ("fallthrough"),

    // Go typed
    ("int"),("bool"),("int8"),("byte"),("rune"),("uint"),
    ("uint8"),("int16"),("int32"),("int64"),("error"),("uint16"),
    ("uint32"),("uint64"),("string"),("float32"),("float64"),("uintptr"),
    ("complex64"),("complex128"),
    // Go predecl
    ("nil"),("true"),("iota"),("false"),
    // Go builtin
    ("cap"),("len"),("new"),("copy"),("imag"),("make"),
    ("real"),("close"),("panic"),("print"),("append"),("delete"),
    ("complex"),("println"),("recover"),
};

void Token::reset()
{
    flags = 0;
    offset = 0;
    ptr = 0;
}

const char *Token::name(int kind)
{ return token_names[kind]; }

const char *Token::spell() const
{
    switch (f.kind) {
    case T_IDENTIFIER:
        return "";//identifier->chars();

    case T_NUMERIC_LITERAL:
    case T_CHAR_LITERAL:
    case T_WIDE_CHAR_LITERAL:
    case T_UTF16_CHAR_LITERAL:
    case T_UTF32_CHAR_LITERAL:
    case T_STRING_LITERAL:
    case T_WIDE_STRING_LITERAL:
    case T_UTF8_STRING_LITERAL:
    case T_UTF16_STRING_LITERAL:
    case T_UTF32_STRING_LITERAL:
    case T_RAW_STRING_LITERAL:
    case T_RAW_WIDE_STRING_LITERAL:
    case T_RAW_UTF8_STRING_LITERAL:
    case T_RAW_UTF16_STRING_LITERAL:
    case T_RAW_UTF32_STRING_LITERAL:
    case T_AT_STRING_LITERAL:
    case T_ANGLE_STRING_LITERAL:
        return "";//literal->chars();

    default:
        return token_names[f.kind];
    } // switch
}


