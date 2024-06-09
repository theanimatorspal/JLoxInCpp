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
    BREAK,

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
     "BREAK",  // break
     "EOF_"    // eof
};

std::ostream& operator<<(std::ostream& os, const TokenType& token);

struct Callable;
struct Interpreter;
struct ClassType;
struct ClassInstance;
struct Token;
struct FunctionStmt;
using Object        = opt<var<Number, s, bool, sp<Callable>, sp<ClassType>, sp<ClassInstance>>>;
using CallType      = std::function<Object(Interpreter&, v<Object>&)>;
using ToStringFType = std::function<s()>;

struct Callable {
    int mArity = 0;
    virtual Object Call(Interpreter& inI, v<Object>& inObject) { return std::nullopt; }
    virtual s ToString() { return ""; }
    virtual ~Callable() = default;
    bool operator==(const Callable& other) const { return mArity == other.mArity; }
};

struct CallableFunction;

enum ObjectIndex : size_t {
    ObjectIndex_Number,
    ObjectIndex_String,
    ObjectIndex_Boolean,
    ObjectIndex_Callable,
    ObjectIndex_Class,
    ObjectIndex_ClassInstance,
};

struct ClassType : Callable {
    s mName;
    umap<s, sp<CallableFunction>> mMethods;
    ClassType(sv inName, umap<s, sp<CallableFunction>> inMethods)
        : mName(inName), mMethods(inMethods) {
        mArity = 0;
    }
    virtual Object Call(Interpreter& inI, v<Object>& inObject);
};

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

// TODO change take Token by ref instead of copy

struct ClassInstance : std::enable_shared_from_this<ClassInstance> {
    ClassType& mClassType;
    umap<s, Object> mFields;
    Object Get(Token& inName);
    void Set(Token& inName, Object inValue);
    ClassInstance(ClassType& inClassType) : mClassType(inClassType) {}
};
s ToString(Object inObject);
} // namespace Birali
