#pragma once
#include "Token.hpp"
#include "include.hpp"

namespace Birali {
struct Scanner {
    const v<Token> ScanTokens();
    bool IsAtEnd() const { return mCurrent >= mSource.length(); }
    void ScanToken();
    char Advance();
    void AddToken(TokenType inTokenType);
    void AddToken(TokenType inType, Object inLiteral);

    Scanner(const sv inSource) : mSource(inSource) {}
    v<Token> mTokens;
    const sv mSource;

    private:
    bool Match(char inExpected);
    char Peek();
    char PeekNext();
    void ReadString();
    void ReadNumber();
    void ReadIdentifier();

    int mStart   = 0;
    int mCurrent = 0;
    int mLine    = 1;
};
} // namespace Birali