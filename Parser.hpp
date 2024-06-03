#pragma once
#include "Scanner.hpp"
#include "Expression.hpp"

namespace Birali {
class Parser {
    public:
    class ParseError : public std::exception {};
    Parser(const v<Token>& inToken) : mTokens(inToken) {}
    up<Expr> Expression();
    up<Expr> Equality();
    up<Expr> Comparision();
    up<Expr> Term();
    up<Expr> Factor();
    up<Expr> Unary();
    up<Expr> Primary();
    up<Expr> Parse() {
        try {
            return Expression();
        } catch (ParseError& inError) {
            return nullptr;
        }
    }

    private:
    template <typename... T> bool Match(T... inTokenType) {
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

    void Synchronize() {
        Advance();
        while (not IsAtEnd()) {
            if (Previous().mType == SEMICOLON) return;
            switch (Peek().mType) {
                case CLASS:
                case FUN:
                case VAR:
                case FOR:
                case IF:
                case WHILE:
                case PRINT:
                case RETURN:
                    return;
                default:
                    break;
            }
            Advance();
        }
    }
    Parser::ParseError Error(Token inToken, const sv inMessage);
    Token Consume(TokenType inType, const sv inStr);
    bool IsAtEnd() { return Peek().mType == EOF; }
    Token Peek() { return mTokens[mCurrent]; }
    Token Previous() { return mTokens[mCurrent - 1]; }
    const v<Token> mTokens;
    int mCurrent = 0;
};
} // namespace Birali