#pragma once
#include "include.hpp"

namespace Birali {

enum TokenType : int {
    // SINGLE-CHARACTER TOKENS.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // ONE OR TWO CHARACTER TOKENS.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // LITERALS.
    IDENTIFIER,
    STRING,
    NUMBER,

    // KEYWORDS.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    EOF_,
    COUNT
};

const Birali::sv TokenStrings[COUNT] = {
     "LEFT_PAREN",  // (
     "RIGHT_PAREN", // )
     "LEFT_BRACE",  // {
     "RIGHT_BRACE", // }
     "COMMA",       // ,
     "DOT",         // .
     "MINUS",       // -
     "PLUS",        // +
     "SEMICOLON",   // ;
     "SLASH",       // /
     "STAR",        // *

     "BANG",          // !
     "BANG_EQUAL",    // !=
     "EQUAL",         // =
     "EQUAL_EQUAL",   // ==
     "GREATER",       // >
     "GREATER_EQUAL", // >=
     "LESS",          // <
     "LESS_EQUAL",    // <=

     "IDENTIFIER", // identifier
     "STRING",     // string literal
     "NUMBER",     // numeric literal

     "AND",    // and
     "CLASS",  // class
     "ELSE",   // else
     "FALSE",  // false
     "FUN",    // fun
     "FOR",    // for
     "IF",     // if
     "NIL",    // nil
     "OR",     // or
     "PRINT",  // print
     "RETURN", // return
     "SUPER",  // super
     "THIS",   // this
     "TRUE",   // true
     "VAR",    // var
     "WHILE",  // while
     "EOF_"    // eof
};

std::ostream& operator<<(std::ostream& os, const TokenType& token);
using Object = opt<var<Number, s, bool>>;

struct Token {
    TokenType mType;
    s mLexeme;
    Object mLiteral;
    int mLine;
    Token() = default;
    Token(TokenType inType, const sv inLexeme, Object inLiteral, int inLine)
        : mType(inType), mLexeme(inLexeme), mLiteral(inLiteral), mLine(inLine) {}

    s ToString() {
        return s(TokenStrings[mType]) + " " + mLexeme + " "
             //+
             // std::to_string(mLiteral)
             ; // TODO
    }
};
} // namespace Birali
