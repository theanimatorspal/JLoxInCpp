#include "Interpreter.hpp"

using namespace Birali;
Object CallableFunction::Call(Interpreter& inI, v<Object>& inArguments) {
    auto Env = mksh<Environment>(mClosure);
    for (int i = 0; i < inArguments.size(); i++) {
        Env->Define(mFunctionStmt.mParamters[i].mLexeme, inArguments[i]);
    }

    try {
        inI.ExecuteBlock(mFunctionStmt.mBody, mv(Env));
    } catch (Interpreter::Return& inReturn) {
        return inReturn.mValue;
    }
    return std::nullopt;
}

struct GlobalFunctionCallable_clock : Callable {
    int mArity = 0;
    virtual Object Call(Interpreter& inI, v<Object>& inObject) {
        using namespace std;
        auto now      = chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return (double)chrono::duration_cast<chrono::milliseconds>(duration).count();
    }
    virtual s ToString() { return ""; }
};

Interpreter::Interpreter() {
    mGlobals     = mksh<Environment>(nullptr);
    mEnvironment = mksh<Environment>(nullptr);
    mGlobals->Define("clock", mksh<GlobalFunctionCallable_clock>());
    *mEnvironment = *mGlobals;
}

bool Interpreter::mHadRuntimeError = false;

void Interpreter::Interpret(v<up<Stmt>>& inStmts) {
    // try {
    for (auto& stmt : inStmts) {
        Execute(*stmt);
    }
    // } catch (const RuntimeError& inError) {
    //     std::cout << inError.what() << "\n";
    //     mHadRuntimeError = true;
    // }
}

atype Interpreter::Execute(Stmt& inStmt) { return inStmt.Accept(*this); }

atype Interpreter::Visit(Binary& inBinary) {
    auto Left  = g<Object>(Evaluate(*inBinary.mLeft)).value();
    auto Right = g<Object>(Evaluate(*inBinary.mRight)).value();
    switch (inBinary.mOperator.mType) {
        case BANG_EQUAL: {
            return not IsEqual(Left, Right);
        }
        case EQUAL_EQUAL: {
            return IsEqual(Left, Right);
        }
        case GREATER: {
            CheckNumberOperands(inBinary.mOperator, Left, Right);
            return g<Number>(Left) > g<Number>(Right);
        }
        case GREATER_EQUAL: {
            CheckNumberOperands(inBinary.mOperator, Left, Right);
            return g<Number>(Left) >= g<Number>(Right);
        }
        case LESS: {
            CheckNumberOperands(inBinary.mOperator, Left, Right);
            return g<Number>(Left) < g<Number>(Right);
        }
        case LESS_EQUAL: {
            CheckNumberOperands(inBinary.mOperator, Left, Right);
            return g<Number>(Left) <= g<Number>(Right);
        }
        case MINUS: {
            CheckNumberOperands(inBinary.mOperator, Left, Right);
            return g<Number>(Left) - g<Number>(Right);
        }
        case SLASH: {
            CheckNumberOperands(inBinary.mOperator, Left, Right);
            return g<Number>(Left) / g<Number>(Right);
        }
        case STAR: {
            CheckNumberOperands(inBinary.mOperator, Left, Right);
            return g<Number>(Left) * g<Number>(Right);
        }
        case PLUS: {
            if (Left.index() == ObjectIndex_String and Right.index() == ObjectIndex_String) {
                return g<s>(Left) + g<s>(Right);
            }
            if (Left.index() == ObjectIndex_Number and Right.index() == ObjectIndex_Number) {
                return g<Number>(Left) + g<Number>(Right);
            }
        }
        default:
            break;
    }
    return std::nullopt;
};

atype Interpreter::Visit(Logical& inLogial) {
    Object Left = g<Object>(Evaluate(*inLogial.mLeft));
    if (inLogial.mOperator.mType == OR) {
        if (IsTruthy(Left)) return Left;
    } else { // And
        if (not IsTruthy(Left)) return Left;
    }
    return Evaluate(*inLogial.mRight);
}
atype Interpreter::Visit(Grouping& inGrouping) { return Evaluate(*inGrouping.mExpr); };
atype Interpreter::Visit(Unary& inUnary) {
    atype Right = Evaluate(*inUnary.mExpr);
    switch (inUnary.mOperator.mType) {
        case MINUS:
            CheckNumberOperands(inUnary.mOperator, g<Object>(Right));
            return -g<Number>(g<Object>(Right).value());
            break;
        case BANG:
            return not IsTruthy(g<Object>(Right));
            break;
        default:
            break;
    }
    return std::nullopt;
};
atype Interpreter::Visit(Literal& inLiteral) {
    if (inLiteral.mObject.has_value()) {
        return inLiteral.mObject;
    } else {
        return std::nullopt;
    }
};

atype Interpreter::Evaluate(Expr& inExpr) { return inExpr.Accept(*this); }
bool Interpreter::IsTruthy(Object inObject) {
    if (inObject == std::nullopt) {
        return false;
    }
    if (inObject->index() == ObjectIndex_Boolean) {
        return g<bool>(inObject.value());
    }
    return true;
}
bool Interpreter::IsEqual(Object a, Object b) {
    if (a == std::nullopt and b == std::nullopt) {
        return true;
    }
    if (a == std::nullopt) {
        return false;
    }
    return a == b;
}

void Interpreter::CheckNumberOperands(Token inOperator, Object inOperand) {
    if (inOperand->index() == ObjectIndex_Number) {
        return;
    }
    throw RuntimeError(inOperator, "Operand must be a number.");
}

void Interpreter::CheckNumberOperands(Token inOperator, Object inLeft, Object inRight) {
    if (inLeft->index() == ObjectIndex_Number and inRight->index() == ObjectIndex_Number) {
        return;
    }
    throw RuntimeError(inOperator, "Operands must be a number.");
}

atype Interpreter::Visit(ExpressionStmt& inExpression) {
    Evaluate(*inExpression.mExpr);
    return std::nullopt;
}

atype Interpreter::Visit(PrintStmt& inExpression) {
    Object Value = g<Object>(Evaluate(*inExpression.mExpr));
    std::cout << ToString(Value) << "\n";
    return std::nullopt;
}

atype Interpreter::Visit(Variable& inExpression) { return mEnvironment->Get(inExpression.mName); }

atype Interpreter::Visit(Assign& inExpression) {
    Object Value = g<Object>(Evaluate(*inExpression.mValue));
    mEnvironment->Assign(inExpression.mName, Value);
    return Value;
}

atype Interpreter::Visit(VarStmt& inExpression) {
    Object Value = std::nullopt;
    if (inExpression.mExpr != nullptr) {
        Value = g<Object>(Evaluate(*inExpression.mExpr));
    }
    mEnvironment->Define(inExpression.mToken.mLexeme, Value);
    return std::nullopt;
}

atype Interpreter::Visit(BlockStmt& inStatement) {
    ExecuteBlock(inStatement.mStatements, mksh<Environment>(mEnvironment));
    return std::nullopt;
}

atype Interpreter::Visit(IfStmt& inStatement) {
    if (IsTruthy(g<Object>(Evaluate(*inStatement.mCondition)))) {
        Execute(*inStatement.mThenBranch);
    } else if (inStatement.mElseBranch) {
        Execute(*inStatement.mElseBranch);
    }
    return std::nullopt;
}

void Interpreter::ExecuteBlock(v<up<Stmt>>& inStatements, sp<Environment> inEnvironment) {
    sp<Environment> Previous = mv(this->mEnvironment);
    // auto& Enclosingref       = inEnvironment->GetEnclosingRef();
    // Enclosingref             = Previous;
    // Enclosingref->mEnclosing = nullptr;

    try {
        this->mEnvironment = mv(inEnvironment);
        for (auto& stmts : inStatements) {
            Execute(*stmts);
        }
        this->mEnvironment = mv(Previous);
    } catch (Return& inReturn) {
        this->mEnvironment = mv(Previous);
        throw Return(inReturn.mValue);
    }
}

atype Interpreter::Visit(WhileStmt& inExpression) {
    while (IsTruthy(g<Object>(Evaluate(*inExpression.mCondition)))) {
        Execute(*inExpression.mBody); // TODO implement Break statement later
    }
    return std::nullopt;
}

atype Interpreter::Visit(BreakStmt& inExpression) { return Special{.mBreak = true}; }

atype Interpreter::Visit(Callee& inCall) {
    Object callee = g<Object>(Evaluate(*inCall.mCallee));
    v<Object> Arguments;
    for (auto& arg : inCall.mArguments) {
        Arguments.push_back(g<Object>(Evaluate(*arg)));
    }

    if (not(callee->index() == ObjectIndex_Callable)) {
        throw RuntimeError(inCall.mParen, "Can only call functions and classes");
    }

    auto function = g<sp<Callable>>(callee.value());
    if (Arguments.size() != function->mArity) {
        throw RuntimeError(inCall.mParen,
                           "Expected " + std::to_string(function->mArity) + "Arguments but got " +
                                std::to_string(Arguments.size()) + " arguments.");
    }

    return function->Call(*this, Arguments);
}

atype Interpreter::Visit(FunctionStmt& inExpression) {
    auto Function = mksh<CallableFunction>(inExpression, mEnvironment);
    mEnvironment->Define(inExpression.mName.mLexeme, Function);
    return std::nullopt;
}

atype Interpreter::Visit(ReturnStmt& inExpression) {
    Object Value = std::nullopt;
    if (inExpression.mValue != nullptr) {
        Value = g<Object>(Evaluate(*inExpression.mValue));
    }
    throw Return(Value);
}