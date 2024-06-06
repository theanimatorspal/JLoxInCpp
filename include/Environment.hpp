#pragma once
#include "include.hpp"
#include "Token.hpp"

namespace Birali {
struct Environment {
    sp<Environment> mEnclosing = nullptr;
    umap<s, Object> mValues;
    void Define(s inName, Object inValue) { mValues[inName] = inValue; }
    void Assign(Token inName, Object inValue);
    Object Get(Token inName);
    Environment(sp<Environment> inEnvironment) : mEnclosing(inEnvironment) {}
    sp<Environment>& GetEnclosingRef() {
        if (mEnclosing != nullptr) return mEnclosing->GetEnclosingRef();
        return mEnclosing;
    };
};
} // namespace Birali