#pragma once
#include "Expression.hpp"

namespace Birali {
class Interpreter : public Visitor {
    public:
    static bool mHadRuntimeError;
    class RuntimeError : public std::runtime_error {
        public:
        RuntimeError(Token inOperator, s inSt)
            : mOperator(inOperator),
              std::runtime_error(inSt + "\n[line " + std::to_string(inOperator.mLine) + "]") {}

        private:
        Token mOperator;
    };

    void Interpret(Expr& inExp) {
        try {
            auto Value = g<Object>(Evaluate(inExp));
            std::cout << "Output:" << ToString(Value) << "\n";
        } catch (const RuntimeError& inError) {
            std::cout << inError.what() << "\n";
        }
    }

    virtual atype Visit(Binary& inBinary) {
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
    virtual atype Visit(Grouping& inGrouping) { return Evaluate(*inGrouping.mExpr); };
    virtual atype Visit(Unary& inUnary) {
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
    virtual atype Visit(Literal& inLiteral) { return inLiteral.mObject.value(); };

    atype Evaluate(Expr& inExpr) { return inExpr.Accept(*this); }
    bool IsTruthy(Object inObject) {
        if (inObject == std::nullopt) {
            return false;
        }
        if (inObject->index() == ObjectIndex_Boolean) {
            return g<bool>(inObject.value());
        }
        return true;
    }
    bool IsEqual(Object a, Object b) {
        if (a == std::nullopt and b == std::nullopt) {
            return true;
        }
        if (a == std::nullopt) {
            return false;
        }
        return a == b;
    }

    void CheckNumberOperands(Token inOperator, Object inOperand) {
        if (inOperand->index() == ObjectIndex_Number) {
            return;
        }
        throw RuntimeError(inOperator, "Operand must be a number.");
    }

    void CheckNumberOperands(Token inOperator, Object inLeft, Object inRight) {
        if (inLeft->index() == ObjectIndex_Number and inRight->index() == ObjectIndex_Number) {
            return;
        }
        throw RuntimeError(inOperator, "Operands must be a number.");
    }
};
} // namespace Birali