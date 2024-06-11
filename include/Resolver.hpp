#pragma once
#include "Expression.hpp"

namespace Birali {
class Resolver : Visitor {
    public:
    enum class FunctionType { NONE, FUNCTION, METHOD, INITIALIZER };
    class ResolverError : public std::exception {};
    Interpreter& mInterpreter;
    Resolver(Interpreter& inInterpreter) : mInterpreter(inInterpreter) {}
    virtual atype Visit(Binary& inBinary);
    virtual atype Visit(Logical& inBinary);
    virtual atype Visit(Grouping& inGrouping);
    virtual atype Visit(Unary& inUnary);
    virtual atype Visit(Literal& inLiteral);
    virtual atype Visit(Variable& inExpression);
    virtual atype Visit(Assign& inExpression);
    virtual atype Visit(Callee& inExpression);
    virtual atype Visit(This& inExpression);
    virtual atype Visit(Super& inExpression);

    virtual atype Visit(ExpressionStmt& inExpression);
    virtual atype Visit(PrintStmt& inExpression);
    virtual atype Visit(VarStmt& inExpression);
    virtual atype Visit(BlockStmt& inExpression);
    virtual atype Visit(IfStmt& inExpression);
    virtual atype Visit(WhileStmt& inExpression);
    virtual atype Visit(BreakStmt& inExpression);
    virtual atype Visit(FunctionStmt& inExpression);
    virtual atype Visit(ReturnStmt& inExpression);
    virtual atype Visit(ClassStmt& inExpression);
    virtual atype Visit(GetStmt& inExpression);
    virtual atype Visit(SetStmt& inExpression);

    void Resolve(v<up<Stmt>>& inStatements);
    void Resolve(Stmt& inStatement);
    void BeginScope();
    void EndScope();
    void Declare(Token inName);
    void Define(Token inName);
    void ResolveLocal(Expr& inExpr, Token inName);
    void ResolveFunction(FunctionStmt& inStmt, FunctionType inType);
    ResolverError Error(Token inToken, const sv inMessage);

    private:
    v<umap<s, bool>> mScopes;
    FunctionType mCurrentFunctionType = FunctionType::NONE;
};
} // namespace Birali