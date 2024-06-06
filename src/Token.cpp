#include "Token.hpp"

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
        }
    }
    return "nil";
}