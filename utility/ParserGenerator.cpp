#include "ParserGenerator.hpp"

using namespace Utils;

static umap<sv, ParserGenerator::TokenType> OperatorsToken = {
     {"->", ParserGenerator::TokenType::ARROW},
     {"*", ParserGenerator::TokenType::STAR},
     {"|", ParserGenerator::TokenType::OR},
     {";", ParserGenerator::TokenType::SEMICOLON},
};

v<ParserGenerator::Token> ParserGenerator::Scanner() {
    while (not IsAtEnd()) {
        mStart = mCurrent;
        ScanToken();
    }
    mTokens.push_back(Token{TokenType::EOF_});
    return mTokens;
}

char ParserGenerator::Advance() { return mSource[mCurrent++]; }

bool ParserGenerator::Match(char inChar) {
    if (IsAtEnd()) return false;
    if (mSource[mCurrent] != inChar) return false;
    Advance();
    return true;
}

void ParserGenerator::ScanToken() {
    char c = Advance();
    switch (c) {
        case '-':
            if (Match('>')) {
                mTokens.push_back(Token{.mType = ARROW});
            }
            return;
        case '*':
            return;
        case '|':
            mTokens.push_back(Token{.mType = OR});
            return;
        case ';':
            mTokens.push_back(Token{.mType = SEMICOLON});
            return;
        case '\n':
            mLine++;
            return;

        case '\r':
        case ' ':
        case '\t':
            return;
            break;
        case '"':
            while (not Match('"')) {
                c = Advance();
            }
            mTokens.push_back(
                 Token{.mType           = LITERAL,
                       .mIdentifierName = s(mSource.substr(mStart, mCurrent - mStart))});
        default:
            if (std::isalpha(c)) {
                while (std::isalnum(c)) {
                    c = Advance();
                }
                mTokens.push_back(
                     Token{.mType           = IDENTIFIER,
                           .mIdentifierName = s(mSource.substr(mStart, mCurrent - mStart))});
                return;
            } else {
                std::cout << "Unexpected Character" << '\n';
            }
    }
    if (std::isalpha(mSource[mCurrent])) {
        while (std::isalnum(mSource[mCurrent])) {
        }
    }
}

char ParserGenerator::Peek() {
    if (IsAtEnd()) return '\0';
    return mSource[mCurrent];
}