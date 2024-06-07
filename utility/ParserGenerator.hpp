#include "include.hpp"

namespace Utils {
using namespace Birali;

class ParserGenerator {
    public:
    const sv mSource;
    ParserGenerator(const sv inSource) : mSource(inSource) {}
    enum TokenType : int { IDENTIFIER, ARROW, LITERAL, OR, STAR, SEMICOLON, EOF_ };
    struct Token {
        TokenType mType;
        s mIdentifierName = "";
    };

    v<Token> Scanner();

    private:
    void ScanToken();
    int mStart   = 0;
    int mCurrent = mStart;
    int mLine    = 1;
    bool IsAtEnd() { return mStart >= mSource.length(); }
    v<Token> mTokens;
    char Advance();
    char Peek();
    bool Match(char inChar);
};

} // namespace Utils