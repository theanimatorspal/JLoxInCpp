#include "Token.hpp"

using namespace Birali;

std::ostream &operator<<(std::ostream &os, const Token &token) {
  if (token >= 0 && token < COUNT) {
    os << TokenStrings[token];
  } else {
    os << "TOKEN_UNKNOWN";
  }
  return os;
}