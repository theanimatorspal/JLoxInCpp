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

up<Expr> Parser::Expression() { return Assignment(); }

up<Expr> Parser::Assignment() {
    up<Expr> expr = Or();
    if (Match(EQUAL)) {
        Token Equals   = Previous();
        up<Expr> Value = Assignment();
        auto Variable  = dynamic_cast<struct Variable*>(expr.get());
        if (Variable) {
            return mu<Assign>(Variable->mName, mv(Value));
        }
        Error(Equals, "Invalid Assignment Target.");
    }
    return mv(expr);
}

up<Expr> Parser::Or() {
    up<Expr> Exprr = And();

    if (Match(OR)) {
        Token ortoken  = Previous();
        up<Expr> Right = And();
        return mu<Logical>(mv(Exprr), ortoken, mv(Right));
    }

    return Exprr;
}

up<Expr> Parser::And() {
    up<Expr> Exprr = Equality();
    if (Match(AND)) {
        Token andtoken = Previous();
        up<Expr> Right = Equality();
        return mu<Logical>(mv(Exprr), andtoken, mv(Right));
    }
    return Exprr;
}

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
    return Call();
}

up<Expr> Parser::Call() {
    up<Expr> Expr = Primary();
    while (true) {
        if (Match(LEFT_PAREN)) {
            Expr = FinishCall(mv(Expr));
        } else {
            break;
        }
    }
    return mv(Expr);
}

up<Expr> Parser::FinishCall(up<Expr> inCallee) {
    v<up<Expr>> Arguments;
    if (not Check(RIGHT_PAREN)) {
        do {
            if (Arguments.size() >= 255) {
                Error(Peek(), "Can't have more than 255 arguments");
            }
            Arguments.push_back(mv(Expression()));
        } while (Match(COMMA));
    }
    Token paren = Consume(RIGHT_PAREN, "Expected ')' after arguments.");
    return mu<Callee>(mv(inCallee), paren, mv(Arguments));
}

up<Expr> Parser::Primary() {
    if (Match(FALSE)) return mu<Literal>(false);
    if (Match(TRUE)) return mu<Literal>(true);
    if (Match(NIL)) return mu<Literal>(std::nullopt);
    if (Match(IDENTIFIER)) {
        return mu<Variable>(Previous());
    }

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

up<Stmt> Parser::Statement() {
    if (Match(PRINT)) return PrintStatement();
    if (Match(LEFT_BRACE)) return mu<BlockStmt>(BlockStatement());
    if (Match(IF)) return IfStatement();
    if (Match(WHILE)) return WhileStatement();
    if (Match(FOR)) return ForStatement();
    if (Match(BREAK)) return BreakStatement();
    if (Match(RETURN)) return ReturnStatement();
    return ExpressionStatement();
}

up<Stmt> Parser::PrintStatement() {
    up<Expr> Value = Expression();
    Consume(SEMICOLON, "Expected ';' after value.");
    return mu<Birali::PrintStmt>(mv(Value));
}

up<Stmt> Parser::ExpressionStatement() {
    up<Expr> Value = Expression();
    Consume(SEMICOLON, "Expected ';' after value.");
    return mu<Birali::ExpressionStmt>(mv(Value));
}

v<up<Stmt>> Parser::BlockStatement() {
    v<up<Stmt>> Statements;
    while (not Check(RIGHT_BRACE) and not IsAtEnd()) {
        Statements.push_back(mv(Declaration()));
    }
    Consume(RIGHT_BRACE, "Expected '}' after block.");
    return mv(Statements);
}

up<Stmt> Parser::Declaration() {
    try {
        if (Match(FUN)) return FunctionDeclaration("function");
        if (Match(VAR)) return VarDeclartion();
        return Statement();
    } catch (ParseError& inError) {
        Synchronize();
        return nullptr;
    }
}

up<Stmt> Parser::VarDeclartion() {
    Token Name           = Consume(IDENTIFIER, "Expected Variable Name.");
    up<Expr> Initializer = nullptr;
    if (Match(EQUAL)) {
        Initializer = Expression();
    }
    Consume(SEMICOLON, "Expected ';' after declaration.");
    return mu<VarStmt>(Name, mv(Initializer));
}

up<Stmt> Parser::FunctionDeclaration(sv inKind) {
    Token Name = Consume(IDENTIFIER, "Expected " + s(inKind) + "name.");
    Consume(LEFT_PAREN, "Expected '(' after " + s(inKind) + "name.");
    v<Token> Parameters;
    if (not Check(RIGHT_PAREN)) {
        do {
            if (Parameters.size() >= 255) {
                Error(Peek(), "Can't have more than 255 parameters.");
            }
            Parameters.push_back(Consume(IDENTIFIER, "Expected parameter name."));
        } while (Match(COMMA));
    }
    Consume(RIGHT_PAREN, "Expected ')' after parameters");
    Consume(LEFT_BRACE, "Expected '{' before " + s(inKind) + "body.");
    v<up<Stmt>> Body = BlockStatement();
    return mu<FunctionStmt>(Name, Parameters, mv(Body));
}

up<Stmt> Parser::IfStatement() {
    Consume(LEFT_PAREN, "Expected '(' after if statment.");
    up<Expr> Condition = Expression();
    Consume(RIGHT_PAREN, "Expected ')' after if condition.");
    up<Stmt> ThenBranch = Statement();
    up<Stmt> ElseBranch = nullptr;
    if (Match(ELSE)) {
        ElseBranch = Statement();
    }
    return mu<IfStmt>(mv(Condition), mv(ThenBranch), mv(ElseBranch));
}

up<Stmt> Parser::WhileStatement() {
    Consume(LEFT_PAREN, "Expected '(' after while statement.");
    up<Expr> Condition = Expression();
    Consume(RIGHT_PAREN, "Expected ')' after while condition.");
    up<Stmt> body = Statement();
    return mu<WhileStmt>(mv(Condition), mv(body));
}

up<Stmt> Parser::ForStatement() {
    Consume(LEFT_PAREN, "Expected '(' after 'for'.");
    up<Stmt> Initializer = nullptr;
    if (Match(SEMICOLON)) {
        Initializer = nullptr;
    } else if (Match(VAR)) {
        Initializer = VarDeclartion();
    } else {
        Initializer = ExpressionStatement();
    }

    up<Expr> Condition = nullptr;
    if (not Check(SEMICOLON)) {
        Condition = Expression();
    }
    Consume(SEMICOLON, "Expected ';' after Loop Condition");

    up<Expr> Increment = nullptr;
    if (not Check(RIGHT_PAREN)) {
        Increment = Expression();
    }
    Consume(RIGHT_PAREN, "Expect ')' after for clauses.");

    up<Stmt> Body = Statement();

    if (Increment != nullptr) {
        v<up<Stmt>> sts;
        sts.push_back(mv(Body));
        sts.push_back(mv(Increment));
        Body = mu<BlockStmt>(mv(sts));
    }

    if (Condition == nullptr) {
        Condition = mu<Literal>(true);
    } else {
        Body = mu<WhileStmt>(mv(Condition), mv(Body));
    }

    if (Initializer != nullptr) {
        v<up<Stmt>> sts;
        sts.push_back(mv(Initializer));
        sts.push_back(mv(Body));
        Body = mu<BlockStmt>(mv(sts));
    }

    return Body;
}

up<Stmt> Parser::BreakStatement() {
    Consume(SEMICOLON, "Expected ';' after break statement.");
    return mu<BreakStmt>();
}

up<Stmt> Parser::ReturnStatement() {
    Token Keyword  = Previous();
    up<Expr> Value = nullptr;
    if (not Check(SEMICOLON)) {
        Value = Expression();
    }

    Consume(SEMICOLON, "Expect ';' after return statement.");
    return mu<ReturnStmt>(Keyword, mv(Value));
}