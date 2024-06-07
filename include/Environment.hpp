#pragma once
#include "include.hpp"
#include "Token.hpp"

namespace Birali {
struct Environment : std::enable_shared_from_this<Environment> {
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

    Object GetAt(int inDistance, s inName);
    sp<Environment> Anchester(int inDistance);
    void AssignAt(int inDistance, Token inName, Object inValue);
    void DefineAt(int inDistance, s inName, Object inValue);
    // void PutAtEnd(sp<Environment> inEnv) {
    //     GetEnclosingRef() = inEnv;
    //     }
};
} // namespace Birali