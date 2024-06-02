#include "Token.hpp"
#include "include.hpp"

namespace Birali {
struct Scanner {
  Scanner(const sv inCommand) : mCommand(inCommand) {}
  const v<Token> ScanTokens() const { return {}; }

  const sv mCommand;
};
} // namespace Birali