#include "LuaBirali.hpp"
#include "Scanner.hpp"

using namespace Birali;
bool Lua::mHadError = false;

void Lua::Error(int inLine, const sv inMessage) {
  Report(inLine, "", inMessage);
}

void Lua::Report(int inLine, const sv inWhere, const sv inMessage) {
  std::cout << "[Line " << inLine << "] Error " << inWhere << ": " << inMessage;
  mHadError = true;
}

void Lua::RunPrompt() {
  while (true) {
    std::cout << "[LuaBirali]> ";
    std::string command;
    std::cin >> command;
    if (command == "") {
      break;
    }
    Run(command);
    mHadError = false;
  }
}

void Lua::Run(const sv inCommand) {
  Scanner scanner(inCommand);
  auto Tokens = scanner.ScanTokens();
  for (Token token : Tokens) {
    std::cout << token;
  }
}

void Lua::RunFile(const sv inFileName) {
  std::string Filename = s(inFileName);
  std::ifstream File(Filename);
  std::stringstream buffer;
  buffer << File.rdbuf();
  Run(buffer.str());
  if (mHadError) {
    exit(-1);
  }
}

int main(int argc, char **argv) {
  Birali::Lua lua;
  if (argc == 2) {
    lua.RunFile(argv[1]);
  } else if (argc > 2) {
    std::cout << "LuaBirali [fileName.lb]\n";
  } else {
    lua.RunPrompt();
  }
}