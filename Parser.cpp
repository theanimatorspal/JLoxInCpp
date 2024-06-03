#include "Parser.hpp"
#include "LuaBirali.hpp"

using namespace Birali;

Parser::ParseError Parser::Error(Token inToken, const sv inMessage) {
    if (inToken.mType == EOF_) {
        Lua::Report(inToken.mLine, " at end", inMessage);
    } else {
        Lua::Report(inToken.mLine, "at '" + inToken.mLexeme + "' ", inMessage);
    }
    return ParseError{};
}

Token Parser::Consume(TokenType inType, const sv inStr) {
    if (Check(inType)) return Advance();
    std::cout << inStr << "\n";
    throw Error(Peek(), inStr);
}

up<Expr> Parser::Expression() { return Equality(); }

up<Expr> Parser::Equality() {
    auto Exp = Comparision();
    while (Match(BANG_EQUAL, EQUAL_EQUAL)) {
        Token Operator = Previous();
        auto Right     = Comparision();
        Exp            = mu<Binary>(mv(Exp), Operator, mv(Right));
    }
    return mv(Exp);
}

up<Expr> Parser::Comparision() {
    auto Exp = Term();
    while (Match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
        Token Operator = Previous();
        auto Right     = Term();
        Exp            = mu<Binary>(mv(Exp), Operator, mv(Right));
    }
    return mv(Exp);
}

up<Expr> Parser::Term() {
    auto Exp = Factor();
    while (Match(MINUS, PLUS)) {
        Token Operator = Previous();
        auto Right     = Factor();
        Exp            = mu<Binary>(mv(Exp), Operator, mv(Right));
    }
    return mv(Exp);
}

up<Expr> Parser::Factor() {
    auto Exp = Unary();
    while (Match(SLASH, STAR)) {
        Token Operator = Previous();
        auto Right     = Unary();
        Exp            = mu<Binary>(mv(Exp), Operator, mv(Right));
    }
    return mv(Exp);
}

up<Expr> Parser::Unary() {
    if (Match(BANG, MINUS)) {
        Token Operator = Previous();
        auto Right     = Unary();
        return mu<Birali::Unary>(Operator, mv(Right));
    }
    return Primary();
}

up<Expr> Parser::Primary() {
    if (Match(FALSE)) return mu<Literal>(false);
    if (Match(TRUE)) return mu<Literal>(true);
    if (Match(NIL)) return mu<Literal>(std::nullopt);

    if (Match(NUMBER, STRING)) {
        return mu<Literal>(Previous().mLiteral);
    }

    if (Match(LEFT_PAREN)) {
        auto Exp = Expression();
        Consume(RIGHT_PAREN, "Expected ')' after expression.");
        return mu<Grouping>(mv(Exp));
    }

    throw Error(Peek(), "Expected expression.");
}