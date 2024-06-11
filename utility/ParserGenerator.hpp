#pragma once
#include "include.hpp"

namespace Utils {
using namespace Birali;

class Scanner {
    public:
    const sv mSource;
    Scanner(const sv inSource) : mSource(inSource) {}
    enum TokenType : int {
        IDENTIFIER,
        LEFT_PAREN,
        RIGHT_PAREN,
        ARROW,
        LITERAL,
        OR,
        STAR,
        OPTIONAL,
        SEMICOLON,
        EOF_
    };
    struct Token {
        TokenType mType;
        int mLine         = 0;
        s mIdentifierName = "";
    };

    v<Token> Scan();

    private:
    v<Token> mTokens;
    int mStart   = 0;
    int mCurrent = mStart;
    int mLine    = 1;

    bool IsAtEnd() { return mCurrent >= mSource.length(); }
    void ScanToken();
    char Advance();
    char Peek();
    bool Match(char inChar);
};

using atype     = opt<var<s, v<s>>>;
using Token     = Scanner::Token;
using TokenType = Scanner::TokenType;

struct Expr;
struct Stmt;
struct Grouping;
struct Unary;
struct Binary;
struct Literal;
struct Visitor;
struct Expression;
struct Identifier;

struct Visitor {
    virtual atype Visit(Stmt& inExp)       = 0;
    virtual atype Visit(Grouping& inExp)   = 0;
    virtual atype Visit(Literal& inExp)    = 0;
    virtual atype Visit(Unary& inExp)      = 0;
    virtual atype Visit(Binary& inExp)     = 0;
    virtual atype Visit(Expression& inExp) = 0;
    virtual atype Visit(Identifier& inExp) = 0;
    virtual ~Visitor()                     = default;
};

struct Expr {
    virtual atype Accept(Visitor& inVisitor) = 0;
    virtual ~Expr()                          = default;
};

struct Literal : public Expr {
    Token mLiteralString;
    Literal(Token& inLiteralString) : mLiteralString(inLiteralString) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); };
};

struct Identifier : public Expr {
    Token mIdentifierString;
    Identifier(Token& inLiteralString) : mIdentifierString(inLiteralString) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); };
};

struct Stmt : public Expr {
    Token mIdentifierName;
    up<Expr> mExpression;
    Stmt(Token& inIdentifierName, up<Expr> inExpression)
        : mIdentifierName(inIdentifierName), mExpression(mv(inExpression)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); };
};

struct Binary : public Expr {
    up<Expr> mLeft;
    up<Expr> mRight;
    Binary(up<Expr> inLeft, up<Expr> inRight) : mLeft(mv(inLeft)), mRight(mv(inRight)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); };
};

struct Unary : public Expr {
    up<Expr> mLeft;
    Token mToken;
    Unary(up<Expr> inLeft, Token& inToken) : mLeft(mv(inLeft)), mToken(inToken) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); };
};

struct Grouping : public Expr {
    up<Expr> mExpr;
    Grouping(up<Expr> inExpr) : mExpr(mv(inExpr)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); };
};

struct Expression : public Expr {
    v<up<Expr>> mExpr;
    Expression(v<up<Expr>> inExpr) : mExpr(mv(inExpr)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); };
};

struct Parser {
    Parser(v<Token>& inTokens) : mTokens(inTokens) {}
    v<up<Expr>> Parse();
    up<Expr> Primary();
    up<Expr> Unary();
    up<Expr> Binary();
    up<Expr> Grouping();
    up<Expr> Statement();
    up<Expr> Expression();
    bool Check(TokenType inType) {
        if (IsAtEnd()) return false;
        return Peek().mType == inType;
    }

    Token Advance() {
        if (not IsAtEnd()) {
            mCurrent++;
        }
        return Previous();
    }
    Token Consume(TokenType inType, const sv inStr);
    bool IsAtEnd() { return Peek().mType == TokenType::EOF_; }
    Token Peek() { return mTokens[mCurrent]; }
    Token Previous() { return mTokens[mCurrent - 1]; }

    private:
    int mCurrent = 0;
    v<Token> mTokens;
    template <typename... T> bool MatchAndAdvance(T... inTokenType) {
        bool retval = false;
        (
             [&]() {
                 if (Check(inTokenType)) {
                     if (retval == false) {
                         Advance();
                         retval = true;
                     }
                 }
             }(),
             ...);
        return retval;
    }
};

} // namespace Utils