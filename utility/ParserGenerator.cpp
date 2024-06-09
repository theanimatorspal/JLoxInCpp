#include "ParserGenerator.hpp"

using namespace Utils;

static umap<sv, Scanner::TokenType> OperatorsToken = {
     {"->", Scanner::TokenType::ARROW},
     {"*", Scanner::TokenType::STAR},
     {"|", Scanner::TokenType::OR},
     {"?", Scanner::TokenType::OPTIONAL},
     {";", Scanner::TokenType::SEMICOLON},
};

v<Scanner::Token> Scanner::Scan() {
    while (not IsAtEnd()) {
        mStart = mCurrent;
        ScanToken();
    }
    mTokens.push_back(Token{.mType = TokenType::EOF_, .mLine = mLine});
    return mTokens;
}

char Scanner::Advance() { return mSource[mCurrent++]; }

bool Scanner::Match(char inChar) {
    if (IsAtEnd()) return false;
    if (mSource[mCurrent] != inChar) return false;
    mCurrent++;
    return true;
}

void Scanner::ScanToken() {
    char c = Advance();
    switch (c) {
        case '-':
            if (Match('>')) {
                mTokens.push_back(Token{.mType = ARROW, .mLine = mLine});
            }
            return;
        case '(':
            mTokens.push_back(Token{.mType = LEFT_PAREN, .mLine = mLine});
            return;
        case ')':
            mTokens.push_back(Token{.mType = RIGHT_PAREN, .mLine = mLine});
            return;
        case '*':
            return;
        case '|':
            mTokens.push_back(Token{.mType = OR, .mLine = mLine});
            return;
        case ';':
            mTokens.push_back(Token{.mType = SEMICOLON, .mLine = mLine});
            return;
        case '\n':
            mLine++;
            return;

        case '\r':
        case ' ':
        case '\t':
            return;
            break;
        case '"':
            while (Peek() != '"' and not IsAtEnd()) {
                if (Peek() == '\n') mLine++;
                Advance();
            }
            if (IsAtEnd()) {
                std::cout << "Error: Unterminated String";
                return;
            }
            Advance();
            mTokens.push_back(
                 Token{.mType           = LITERAL,
                       .mLine           = mLine,
                       .mIdentifierName = s(mSource.substr(mStart + 1, mCurrent - mStart - 2))});
            return;
        default:
            if (std::isalpha(c)) {
                while (std::isalnum(Peek())) {
                    Advance();
                }
                mTokens.push_back(
                     Token{.mType           = IDENTIFIER,
                           .mLine           = mLine,
                           .mIdentifierName = s(mSource.substr(mStart, mCurrent - mStart))});
            } else {
                std::cout << "Unexpected Character" << '\n';
            }
            return;
    }
}

char Scanner::Peek() {
    if (IsAtEnd()) return '\0';
    return mSource[mCurrent];
}

v<up<Expr>> Parser::Parse() {
    v<up<Expr>> mExpressions;
    while (not IsAtEnd()) {
        mExpressions.push_back(mv(Statement()));
    }
    return mv(mExpressions);
}

up<Expr> Parser::Primary() {
    if (Check(TokenType::LEFT_PAREN)) {
        Advance();
        return Grouping();
    } else if (Check(TokenType::IDENTIFIER)) {
        Token token = Advance();
        return mu<struct Identifier>(token);
    } else if (Check(TokenType::LITERAL)) {
        Token token = Advance();
        return mu<struct Literal>(token);
    }
    return nullptr;
}

up<Expr> Parser::Unary() {
    up<Expr> Expr = Primary();
    if (Check(TokenType::STAR) or Check(TokenType::OPTIONAL)) {
        Token Type = Advance();
        return mu<struct Unary>(mv(Expr), Type);
    }
    return mv(Expr);
}

up<Expr> Parser::Binary() {
    up<Expr> left = Unary();
    while (Check(TokenType::OR)) {
        Consume(TokenType::OR, "Expected '|'");
        up<Expr> right = Unary();
        left           = mu<struct Binary>(mv(left), mv(right));
    }
    return mv(left);
}
up<Expr> Parser::Grouping() {
    up<Expr> expr = Expression();
    Consume(TokenType::RIGHT_PAREN, "Parenthesis should be closed");
    return mu<struct Grouping>(mv(expr));
}

up<Expr> Parser::Expression() {
    v<up<Expr>> Expressions;
    while ((Peek().mType != TokenType::RIGHT_PAREN) and (Peek().mType != TokenType::SEMICOLON)) {
        Expressions.push_back(mv(Binary()));
    }
    return mu<struct Expression>(mv(Expressions));
}

up<Expr> Parser::Statement() {
    if (MatchAndAdvance(TokenType::IDENTIFIER)) {
        Token Identifier = Previous();
        Consume(TokenType::ARROW, "Expected Arrow");
        up<Expr> expr = Expression();
        Consume(TokenType::SEMICOLON, "Expected Semicolon");
        return mu<Stmt>(Identifier, mv(expr));
    } else {
        std::cout << "ERROR: Identifier Expected";
    }
    return nullptr;
}

Token Parser::Consume(TokenType inType, const sv inStr) {
    if (Check(inType)) return Advance();
    std::cout << inStr << "\n";
    return {};
}

int main() {
    std::string Filename = s("main.parser");
    std::ifstream File(Filename);
    std::stringstream buffer;
    buffer << File.rdbuf();
    s str = buffer.str();
    Scanner Scanner(str);
    auto tokens = Scanner.Scan();
    Parser Parser(tokens);
    auto exprs = Parser.Parse();
    return 0;
}