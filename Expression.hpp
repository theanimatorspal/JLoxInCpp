#pragma once
#include "include.hpp"
#include "Token.hpp"

namespace Birali {

struct Expr;
struct Binary;
struct Unary;
struct Grouping;
struct Literal;
using atype = var<up<Expr*>, s>;

struct Visitor {
    virtual atype Visit(Binary& inBinary)     = 0;
    virtual atype Visit(Grouping& inGrouping) = 0;
    virtual atype Visit(Unary& inUnary)       = 0;
    virtual atype Visit(Literal& inLiteral)   = 0;
    virtual ~Visitor()                        = default;
};

struct Expr {
    virtual atype Accept(Visitor& inVisitor) = 0;
    virtual ~Expr()                          = default;
};

struct Binary : public Expr {
    up<Expr> mLeft;
    Token mOperator;
    up<Expr> mRight;
    Binary(up<Expr> left, Token op, up<Expr> right)
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

struct AstPrinter : public Visitor {
    s Print(Expr& inExpr) { return std::get<s>(inExpr.Accept(*this)); };
    virtual atype Visit(Binary& inBinary) override;
    virtual atype Visit(Grouping& inGrouping) override;
    virtual atype Visit(Unary& inUnary) override;
    virtual atype Visit(Literal& inLiteral) override;
    template <typename... Expression> s Parenthesize(sv inName, Expression&... inExprs) {
        std::stringstream ss;
        ss << "(" << inName;
        (
             [&] {
                 ss << " ";
                 ss << std::get<s>(inExprs.Accept(*this));
             }(),
             ...);
        ss << ")";
        return ss.str();
    }
};

} // namespace Birali