#include "ParserGenerator.hpp"
#include "Strings.hpp"

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

static std::stringstream gCppOutput;

template <class... T> void gOut(std::string_view inFormat, T&&... inT) {
    gCppOutput << std::vformat(inFormat, std::make_format_args(std::forward<T>(inT)...));
}

bool hasDuplicates(auto& ar1, auto& ar2) {
    return std::any_of(ar1.begin(), ar1.end(), [&](const auto& inS) {
        return std::any_of(ar2.begin(), ar2.end(), [&](const auto& inS1) { return inS == inS1; });
    });
}

void StartNameSpace(sv inNamespace) { gOut(str::namespaceStart, inNamespace); }
void EndNameSpace() { gOut(str::scopeEnd); }

struct GeneratorPass1 : public Visitor {
    umap<s, up<Expr>> mIdentifiers;
    virtual atype Visit(Stmt& inExp) {
        mIdentifiers[inExp.mIdentifierName.mIdentifierName] = mv(inExp.mExpression);
        gOut(str::declareStruct, inExp.mIdentifierName.mIdentifierName);
        return std::nullopt;
    }
    virtual atype Visit(Grouping& inExp) { return std::nullopt; }
    virtual atype Visit(Literal& inExp) { return std::nullopt; }
    virtual atype Visit(Unary& inExp) { return std::nullopt; }
    virtual atype Visit(Binary& inExp) { return std::nullopt; }
    virtual atype Visit(Expression& inExp) { return std::nullopt; }
    virtual atype Visit(Identifier& inExp) { return std::nullopt; }
};

struct GeneratorPass2 : public Visitor {
    GeneratorPass1& mPass1;
    GeneratorPass2(GeneratorPass1& inPass1) : mPass1(inPass1) {

        // VISITOR CLASS
        gOut(str::structStart, "Visitor");
        for (auto& statement : inPass1.mIdentifiers) {
            gOut("virtual atype Visit({0}& in_{0});\n", statement.first);
        }
        gOut(str::scopeEndWithSemiCoon);

        // EXPRESSIONS
        for (auto& statment : inPass1.mIdentifiers) {
            gOut(str::structStart, statment.first);
            auto exps = g<v<s>>(statment.second->Accept(*this).value());
            gOut("{0}(", statment.first);
            int expCount = 0;
            for (int i = 0; i < exps.size(); ++i) {
                if (exps[i] != "__literal__") {
                    gOut("up<{0}> in_{0}", exps[i]);
                    if (not(i == exps.size() - 1)) {
                        gOut(", ");
                    }
                    expCount++;
                }
            }
            gOut(")");

            if (expCount > 0) {
                gOut(" : ");
            }

            for (int i = 0; i < exps.size(); ++i) {
                if (exps[i] != "__literal__") {
                    gOut("m_{0}(mv(in_{0}))", exps[i]);
                    if (not(i == exps.size() - 1)) {
                        gOut(", ");
                    }
                }
            }

            gOut("{{}}\n");

            gOut(R"""(
void Accept(Visitor& inVisitor)
{{
    inVisitor.Visit(*this);
}}
            )""");

            gOut(str::scopeEndWithSemiCoon);
        }
    }

    virtual atype Visit(Stmt& inExp) { return inExp.mExpression->Accept(*this); }
    virtual atype Visit(Grouping& inExp) { return inExp.mExpr->Accept(*this); }
    virtual atype Visit(Literal& inExp) { return v<s>{"__literal__"}; }
    virtual atype Visit(Unary& inExp) { return inExp.mLeft->Accept(*this); }
    virtual atype Visit(Binary& inExp) {
        auto exp1 = g<v<s>>(inExp.mLeft->Accept(*this).value());
        auto exp2 = g<v<s>>(inExp.mRight->Accept(*this).value());
        v<s> expOut;
        for (auto& x : exp1)
            expOut.push_back(x);
        for (auto& x : exp2)
            expOut.push_back(x);
        return expOut;
    }
    virtual atype Visit(Expression& inExp) {
        v<s> exps;
        int uniquer = 0;
        for (auto& exp : inExp.mExpr) {
            auto expstr = g<v<s>>(exp->Accept(*this).value());
            for (auto& x : expstr)
                exps.push_back(x);
        }
        return exps;
    }
    virtual atype Visit(Identifier& inExp) {
        gOut(str::declareUp, inExp.mIdentifierString.mIdentifierName);
        return v<s>{inExp.mIdentifierString.mIdentifierName};
    }
};

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

    gOut(str::includeAll);

    StartNameSpace("lbirali");
    GeneratorPass1 Gen;
    for (auto& exp : exprs) {
        exp->Accept(Gen);
    }
    GeneratorPass2 Gen2(Gen);
    EndNameSpace();

    std::ofstream FileOut("out.cpp");
    FileOut << gCppOutput.str();
    std::cout << gCppOutput.str();
    return 0;
}