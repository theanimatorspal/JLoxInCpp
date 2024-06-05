#pragma once
#include "include.hpp"
#include "Token.hpp"

namespace Birali {
struct Environment {
    Environment* mEnclosing = nullptr;
    umap<s, Object> mValues;
    void Define(s inName, Object inValue) { mValues[inName] = inValue; }
    void Assign(Token inName, Object inValue);
    Object Get(Token inName);
    Environment(Environment* inEnvironment) : mEnclosing(mv(inEnvironment)) {}
};
} // namespace Birali