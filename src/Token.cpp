#include "Token.hpp"
#include "Interpreter.hpp"
using namespace Birali;

std::ostream& operator<<(std::ostream& os, const TokenType& token) {
    if (token >= 0 && token < COUNT) {
        os << TokenStrings[token];
    } else {
        os << "TOKEN_UNKNOWN";
    }
    return os;
}

s Birali::ToString(Object inObject) {
    if (not inObject.has_value()) {
        return "nil";
    } else {
        switch (inObject->index()) {
            case ObjectIndex_Callable:
                return g<sp<Callable>>(inObject.value())->ToString();
            case ObjectIndex_Number:
                return std::to_string(g<Number>(inObject.value()));
            case ObjectIndex_String:
                return g<s>(inObject.value());
            case ObjectIndex_Boolean: {
                bool boolean = g<bool>(inObject.value());
                if (boolean) {
                    return "true";
                } else {
                    return "false";
                }
            }
            case ObjectIndex_Class: {
                auto x = g<sp<ClassType>>(inObject.value());
                return "class[" + x->mName + "]";
            }
            case ObjectIndex_ClassInstance: {
                auto x = g<sp<ClassInstance>>(inObject.value());
                return x->mClassType.mName + " instance.";
            }
        }
    }
    return "nil";
}

Object ClassType::Call(Interpreter& inI, v<Object>& inObject) {
    sp<ClassInstance> Inst = mksh<ClassInstance>(*this);
    return Inst;
}

Object ClassInstance::Get(Token& inName) {
    if (mFields.contains(inName.mLexeme)) {
        return mFields[inName.mLexeme];
    }
    bool hasMethod = mClassType.mMethods.contains(inName.mLexeme);
    if (hasMethod) {
        return mClassType.mMethods[inName.mLexeme]->Bind(shared_from_this());
    }
    throw Interpreter::RuntimeError(inName, "Undefined Property '" + inName.mLexeme + "'.");
}

void ClassInstance::Set(Token& inName, Object inValue) { mFields[inName.mLexeme] = inValue; }