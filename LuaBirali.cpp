#include "LuaBirali.hpp"
#include "Scanner.hpp"
#include "Expression.hpp"
#include "Parser.hpp"
#include "Interpreter.hpp"

using namespace Birali;
bool Lua::mHadError = false;

void Lua::Error(int inLine, const sv inMessage) { Report(inLine, "", inMessage); }

void Lua::Report(int inLine, const sv inWhere, const sv inMessage) {
    std::cout << "[Line " << inLine << "] Error " << inWhere << ": " << inMessage << "\n";
    mHadError = true;
}

void Lua::RunPrompt() {
    while (true) {
        std::cout << "[LuaBirali]> ";
        std::string command;
        if (std::getline(std::cin, command)) {
            if (command == "") {
                return;
            }
        }
        Run(command);
        mHadError = false;
    }
}

void Lua::Run(const sv inCommand) {
    Scanner scanner(inCommand);
    auto Tokens = scanner.ScanTokens();
    Parser Parser(Tokens);
    if (mHadError) return;
    // for (Token token : Tokens) {
    //     std::cout << token.ToString() << '\n';
    // }
    // std::cout << AstPrinter().Print(*Expr) << "\n";
    // mInterpreter.Interpret(*Expr);
    auto Statements = Parser.Parse();
    mInterpreter.Interpret(Statements);

    if (mInterpreter.mHadRuntimeError) {
        exit(70);
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

int main(int argc, char** argv) {
    Birali::Lua lua;
    lua.RunFile("main.luabirali");
    // if (argc == 2) {
    //     lua.RunFile(argv[1]);
    // } else if (argc > 2) {
    //     std::cout << "LuaBirali [fileName.lb]\n";
    // } else {
    //     lua.RunPrompt();
    // }

    // auto Expr = mu<Binary>(mu<Unary>(Token(MINUS, "-", std::nullopt, 1), mu<Literal>(23.4)),
    //                        Token(STAR, "*", std::nullopt, 1),
    //                        mu<Grouping>(mu<Literal>(56.43)));
    // AstPrinter Print;
    // std::cout << Print.Print(*Expr) << "\n";
    return 0;
}
