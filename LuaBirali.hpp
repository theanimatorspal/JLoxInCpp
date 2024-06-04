#pragma once
#include "include.hpp"

namespace Birali {
struct Lua {
    void RunPrompt();
    void Run(const sv inCommand);
    void RunFile(const sv inFileName);
    static void Error(int inLine, const sv inMessage);
    static void Report(int inLine, const sv inWhere, const sv inMessage);

    private:
    static bool mHadError;
};
} // namespace Birali