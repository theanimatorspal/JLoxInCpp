#pragma once
#include "Expression.hpp"
#include "Environment.hpp"

namespace Birali {
class Interpreter : public Visitor {
    public:
    Interpreter() { mEnvironment = mu<Environment>(nullptr); }
    static bool mHadRuntimeError;
    class RuntimeError : public std::runtime_error {
        public:
        RuntimeError(Token inOperator, s inSt)
            : mOperator(inOperator),
              std::runtime_error(inSt + "\n[line " + std::to_string(inOperator.mLine) + "]") {
            mHadRuntimeError = true;
        }

        private:
        Token mOperator;
    };

    void Interpret(v<up<Stmt>>& inStatements);
    virtual atype Visit(Binary& inBinary);
    virtual atype Visit(Logical& inBinary);
    virtual atype Visit(Grouping& inGrouping);
    virtual atype Visit(Unary& inUnary);
    virtual atype Visit(Literal& inLiteral);
    virtual atype Visit(Variable& inExpression);
    virtual atype Visit(Assign& inExpression);

    virtual atype Visit(ExpressionStmt& inExpression);
    virtual atype Visit(PrintStmt& inExpression);
    virtual atype Visit(VarStmt& inExpression);
    virtual atype Visit(BlockStmt& inExpression);
    virtual atype Visit(IfStmt& inExpression);
    virtual atype Visit(WhileStmt& inExpression);
    virtual atype Visit(BreakStmt& inExpression);

    atype Execute(Stmt& inStmt);
    atype Evaluate(Expr& inExpr);
    bool IsTruthy(Object inObject);
    bool IsEqual(Object a, Object b);
    void CheckNumberOperands(Token inOperator, Object inOperand);
    void CheckNumberOperands(Token inOperator, Object inLeft, Object inRight);
    void ExecuteBlock(v<up<Stmt>>& inStatements, up<Environment> inEnvironment);

    private:
    up<Environment> mEnvironment;
};
} // namespace Birali