#pragma once
#include "include.hpp"
#include "Token.hpp"

namespace Birali {

struct Expr;
struct Binary;
struct Logical;
struct Unary;
struct Grouping;
struct Literal;
struct Variable;
struct Assign;
struct Callee;

struct ExpressionStmt;
struct PrintStmt;
struct VarStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct BreakStmt;
struct FunctionStmt;

struct Special {
    bool mBreak = false;
};
using atype = var<up<Expr*>, Object, Special>;
enum Atype : int { Atype_Expr, Atype_String, Atype_Object, Atype_Special };

struct Visitor {
    virtual atype Visit(Binary& inBinary)             = 0;
    virtual atype Visit(Logical& inBinary)            = 0;
    virtual atype Visit(Grouping& inGrouping)         = 0;
    virtual atype Visit(Unary& inUnary)               = 0;
    virtual atype Visit(Literal& inLiteral)           = 0;
    virtual atype Visit(Variable& inExpression)       = 0;
    virtual atype Visit(Assign& inExpression)         = 0;
    virtual atype Visit(Callee& inExpression)         = 0;

    virtual atype Visit(ExpressionStmt& inExpression) = 0;
    virtual atype Visit(PrintStmt& inExpression)      = 0;
    virtual atype Visit(VarStmt& inExpression)        = 0;
    virtual atype Visit(BlockStmt& inExpression)      = 0;
    virtual atype Visit(IfStmt& inExpression)         = 0;
    virtual atype Visit(WhileStmt& inExpression)      = 0;
    virtual atype Visit(BreakStmt& inExpression)      = 0;
    virtual atype Visit(FunctionStmt& inExpression)   = 0;
    virtual ~Visitor()                                = default;
};

// struct Stmt {
//     virtual atype Accept(Visitor& inVisitor) = 0;
//     virtual ~Stmt()                          = default;
// };

struct Expr {
    virtual atype Accept(Visitor& inVisitor) = 0;
    virtual ~Expr()                          = default;
};

using Stmt = Expr; // TODO Remove this

struct Binary : public Expr {
    up<Expr> mLeft;
    Token mOperator;
    up<Expr> mRight;
    Binary(up<Expr> left, Token op, up<Expr> right)
        : mLeft(std::move(left)), mOperator(op), mRight(std::move(right)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

struct Logical : public Expr {
    up<Expr> mLeft;
    Token mOperator;
    up<Expr> mRight;
    Logical(up<Expr> left, Token op, up<Expr> right)
        : mLeft(std::move(left)), mOperator(op), mRight(std::move(right)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

struct Grouping : public Expr {
    up<Expr> mExpr;
    Grouping(up<Expr> expr) : mExpr(std::move(expr)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

struct Unary : public Expr {
    Token mOperator;
    up<Expr> mExpr;
    Unary(Token op, up<Expr> expr) : mOperator(op), mExpr(std::move(expr)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

struct Literal : public Expr {
    Object mObject;
    Literal(Object obj) : mObject(obj) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

struct Variable : public Expr {
    Token mName;
    Variable(Token inName) : mName(inName) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

struct Assign : public Expr {
    Token mName;
    up<Expr> mValue;
    Assign(Token inToken, up<Expr> inExpression) : mName(inToken), mValue(mv(inExpression)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

struct Callee : public Expr {
    up<Expr> mCallee;
    Token mParen;
    v<up<Expr>> mArguments;
    Callee(up<Expr> inCallee, Token inParen, v<up<Expr>> inArguments)
        : mCallee(mv(inCallee)), mParen(inParen), mArguments(mv(inArguments)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

struct ExpressionStmt : public Stmt {
    up<Expr> mExpr;
    ExpressionStmt(up<Expr> inExpr) : mExpr(mv(inExpr)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

struct PrintStmt : public Stmt {
    up<Expr> mExpr;
    PrintStmt(up<Expr> inExpr) : mExpr(mv(inExpr)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

struct VarStmt : public Stmt {
    Token mToken;
    up<Expr> mExpr;
    VarStmt(Token inToken, up<Expr> inExpr) : mToken(inToken), mExpr(mv(inExpr)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

struct BlockStmt : public Stmt {
    v<up<Stmt>> mStatements;
    BlockStmt(v<up<Stmt>> inStatements) : mStatements(mv(inStatements)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

struct IfStmt : public Stmt {
    up<Expr> mCondition;
    up<Stmt> mThenBranch;
    up<Stmt> mElseBranch = nullptr;
    IfStmt(up<Expr> inCondition, up<Stmt> inThenBranch, up<Stmt> inElseBranch)
        : mCondition(mv(inCondition)),
          mThenBranch(mv(inThenBranch)),
          mElseBranch(mv(inElseBranch)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

struct WhileStmt : public Stmt {
    up<Expr> mCondition;
    up<Stmt> mBody;
    WhileStmt(up<Expr> inCondition, up<Stmt> inBody)
        : mCondition(mv(inCondition)), mBody(mv(inBody)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

struct BreakStmt : public Stmt {
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

struct FunctionStmt : public Stmt {
    Token mName;
    v<Token> mParamters;
    v<up<Stmt>> mBody;
    FunctionStmt(Token inName, v<Token> inParameters, v<up<Stmt>> inBody)
        : mName(inName), mParamters(inParameters), mBody(mv(inBody)) {}
    virtual atype Accept(Visitor& inVisitor) { return inVisitor.Visit(*this); }
};

} // namespace Birali