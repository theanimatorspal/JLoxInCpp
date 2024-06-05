#include "Interpreter.hpp"
using namespace Birali;

Object Environment::Get(Token inName) {
    if (mValues.contains(inName.mLexeme)) {
        return mValues[inName.mLexeme];
    }
    if (mEnclosing != nullptr) {
        return mEnclosing->Get(inName);
    }
    throw Interpreter::RuntimeError(inName, "Undefined variable '" + inName.mLexeme + "'.");
}

void Environment::Assign(Token inName, Object inValue) {
    if (mValues.contains(inName.mLexeme)) {
        mValues[inName.mLexeme] = inValue;
        return;
    }
    if (mEnclosing != nullptr) {
        mEnclosing->Assign(inName, inValue);
        return;
    }
    throw Interpreter::RuntimeError(inName, "Undefined variable '" + inName.mLexeme + "'.");
}