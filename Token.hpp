#include "include.hpp"

namespace Birali {

enum Token : int {
  // SINGLE-CHARACTER TOKENS.
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  // ONE OR TWO CHARACTER TOKENS.
  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  // LITERALS.
  IDENTIFIER,
  STRING,
  NUMBER,

  // KEYWORDS.
  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,

  COUNT
};

const Birali::sv TokenStrings[COUNT] = {};

// Overload the << operator for the Token enum
std::ostream &operator<<(std::ostream &os, const Token &token);
} // namespace Birali
