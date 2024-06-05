#include "Expression.hpp"

using namespace Birali;

atype AstPrinter::Visit(Binary& inBinary) {
    return Parenthesize(inBinary.mOperator.mLexeme, *inBinary.mLeft, *inBinary.mRight);
}

atype AstPrinter::Visit(Grouping& inGrouping) { return Parenthesize("group", *inGrouping.mExpr); };

atype AstPrinter::Visit(Unary& inUnary) {
    return Parenthesize(inUnary.mOperator.mLexeme, *inUnary.mExpr);
};

atype AstPrinter::Visit(Literal& inLiteral) {
    if (not inLiteral.mObject.has_value()) {
        return s("nil");
    }
    auto& LiteralValue = inLiteral.mObject.value();
    switch (LiteralValue.index()) {
        case 0:
            return std::to_string(std::get<Number>(LiteralValue));
        case 1:
            return std::get<s>(LiteralValue);
        case 2:
            if (std::get<bool>(LiteralValue)) {
                return s("true");
            } else {
                return s("false");
            }
        default:
            break;
    }
    return s("");
}