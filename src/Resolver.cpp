#include "Resolver.hpp"
#include "LuaBirali.hpp"

using namespace Birali;
atype Resolver::Visit(Binary& inBinary) {
    Resolve(*inBinary.mLeft);
    Resolve(*inBinary.mRight);
    return std::nullopt;
};
atype Resolver::Visit(Logical& inBinary) {
    Resolve(*inBinary.mLeft);
    Resolve(*inBinary.mRight);
    return std::nullopt;
};
atype Resolver::Visit(Grouping& inGrouping) {
    Resolve(*inGrouping.mExpr);
    return std::nullopt;
};
atype Resolver::Visit(Unary& inUnary) {
    Resolve(*inUnary.mExpr);
    return std::nullopt;
};
atype Resolver::Visit(Literal& inLiteral) { return std::nullopt; };
atype Resolver::Visit(Variable& inExpression) {
    if (not mScopes.empty() and mScopes.back()[inExpression.mName.mLexeme] and
        mScopes.back()[inExpression.mName.mLexeme] == false) {
        Error(inExpression.mName, "Can't read local variable in its own initializer");
    }
    ResolveLocal(inExpression, inExpression.mName);
    return std::nullopt;
};
atype Resolver::Visit(Assign& inExpression) {
    Resolve(*inExpression.mValue);
    ResolveLocal(inExpression, inExpression.mName);
    return std::nullopt;
};
atype Resolver::Visit(Callee& inExpression) {
    Resolve(*inExpression.mCallee);
    for (auto& arg : inExpression.mArguments) {
        Resolve(*arg);
    }
    return std::nullopt;
};

atype Resolver::Visit(ExpressionStmt& inExpression) {
    Resolve(*inExpression.mExpr);
    return std::nullopt;
};
atype Resolver::Visit(PrintStmt& inExpression) {
    Resolve(*inExpression.mExpr);
    return std::nullopt;
};
atype Resolver::Visit(VarStmt& inExpression) {
    Declare(inExpression.mToken);
    if (inExpression.mExpr != nullptr) {
        Resolve(*inExpression.mExpr);
    }
    Define(inExpression.mToken);
    return std::nullopt;
};
atype Resolver::Visit(BlockStmt& inExpression) {
    BeginScope();
    Resolve(inExpression.mStatements);
    EndScope();
    return std::nullopt;
};
atype Resolver::Visit(IfStmt& inExpression) {
    Resolve(*inExpression.mCondition);
    Resolve(*inExpression.mThenBranch);
    if (inExpression.mElseBranch != nullptr) {
        Resolve(*inExpression.mElseBranch);
    }
    return std::nullopt;
};
atype Resolver::Visit(WhileStmt& inExpression) {
    Resolve(*inExpression.mCondition);
    Resolve(*inExpression.mBody);
    return std::nullopt;
};
atype Resolver::Visit(BreakStmt& inExpression) { return std::nullopt; };
atype Resolver::Visit(FunctionStmt& inExpression) {
    Declare(inExpression.mName);
    Define(inExpression.mName);
    ResolveFunction(inExpression, FunctionType::FUNCTION);
    return std::nullopt;
};
atype Resolver::Visit(ReturnStmt& inExpression) {
    if (mCurrentFunctionType == FunctionType::NONE) {
        Error(inExpression.mKeyword, "Can't return from top level code");
    }
    if (inExpression.mValue != nullptr) {
        Resolve(*inExpression.mValue);
    }
    return std::nullopt;
};

void Resolver::Resolve(v<up<Stmt>>& inStatements) {
    for (auto& stmt : inStatements) {
        Resolve(*stmt);
    }
}

void Resolver::Resolve(Stmt& inStatement) { inStatement.Accept(*this); }
void Resolver::BeginScope() { mScopes.push_back(umap<s, bool>()); }
void Resolver::EndScope() { mScopes.pop_back(); }

void Resolver::Declare(Token inName) {
    if (mScopes.empty()) return;
    auto& Scope = mScopes.back();
    if (Scope.contains(inName.mLexeme)) {
        Error(inName, "Already a variable with this name in scope.");
    }
    Scope[inName.mLexeme] = false;
}

void Resolver::Define(Token inName) {
    if (mScopes.empty()) return;
    mScopes.back()[inName.mLexeme] = true;
}

void Resolver::ResolveLocal(Expr& inExpr, Token inName) {
    for (int i = mScopes.size() - 1; i >= 0; i--) {
        if (mScopes[i].contains(inName.mLexeme)) {
            mInterpreter.Resolve(inExpr, mScopes.size() - 1 - i);
            return;
        }
    }
}
void Resolver::ResolveFunction(FunctionStmt& inStmt, FunctionType inType) {
    FunctionType enclosingFunction = mCurrentFunctionType;
    mCurrentFunctionType           = inType;
    BeginScope();
    for (Token& param : inStmt.mParamters) {
        Declare(param);
        Define(param);
    }
    Resolve(inStmt.mBody);
    EndScope();
    mCurrentFunctionType = enclosingFunction;
}

Resolver::ResolverError Resolver::Error(Token inToken, const sv inMessage) {
    if (inToken.mType == EOF_) {
        Lua::Report(inToken.mLine, " at end", inMessage);
    } else {
        Lua::Report(inToken.mLine, "at '" + inToken.mLexeme + "' ", inMessage);
    }
    Lua::HadError();
    return ResolverError{};
}