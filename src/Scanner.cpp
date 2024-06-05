#include "Scanner.hpp"
#include "LuaBirali.hpp"
using namespace Birali;

static umap<sv, TokenType> KeywordsMap = {{"and", AND},
                                          {"class", CLASS},
                                          {"else", ELSE},
                                          {"false", FALSE},
                                          {"for", FOR},
                                          {"fun", FUN},
                                          {"if", IF},
                                          {"nil", NIL},
                                          {"or", OR},
                                          {"print", PRINT},
                                          {"return", RETURN},
                                          {"super", SUPER},
                                          {"this", THIS},
                                          {"true", TRUE},
                                          {"var", VAR},
                                          {"while", WHILE}};

const v<Token> Scanner::ScanTokens() {
    while (not IsAtEnd()) {
        mStart = mCurrent;
        ScanToken();
    }
    mTokens.push_back(Token(TokenType::EOF_, "", std::nullopt, mLine));
    return mTokens;
}

bool Scanner::Match(char inExpected) {
    if (IsAtEnd()) return false;
    if (mSource[mCurrent] != inExpected) return false;
    mCurrent++;
    return true;
}

char Scanner::Peek() {
    if (IsAtEnd()) return '\0';
    return mSource[mCurrent];
}

char Scanner::PeekNext() {
    if (mCurrent + 1 >= mSource.length()) return '\0';
    return mSource[mCurrent + 1];
}

void Scanner::ScanToken() {
    char c = Advance();
    switch (c) {
        case '(':
            AddToken(LEFT_PAREN);
            break;
        case ')':
            AddToken(RIGHT_PAREN);
            break;
        case '{':
            AddToken(LEFT_BRACE);
            break;
        case '}':
            AddToken(RIGHT_BRACE);
            break;
        case ',':
            AddToken(COMMA);
            break;
        case '.':
            AddToken(DOT);
            break;
        case '-':
            AddToken(MINUS);
            break;
        case '+':
            AddToken(PLUS);
            break;
        case ';':
            AddToken(SEMICOLON);
            break;
        case '*':
            AddToken(STAR);
            break;
        case '!':
            AddToken(Match('=') ? BANG_EQUAL : BANG);
            break;
        case '=':
            AddToken(Match('=') ? EQUAL_EQUAL : EQUAL);
            break;
        case '<':
            AddToken(Match('=') ? LESS_EQUAL : LESS);
            break;
        case '>':
            AddToken(Match('=') ? GREATER_EQUAL : GREATER);
            break;
        case '/':
            if (Match('/')) {
                while (Peek() != '\n' and not IsAtEnd()) {
                    Advance();
                }
            }
            AddToken(SLASH);
            break;
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            mLine++;
            break;
        case '"':
            ReadString();
            break;
        default:
            if (std::isdigit(c)) {
                ReadNumber();
            } else if (std::isalpha(c)) {
                ReadIdentifier();
            } else {
                Lua::Error(mLine, "Unexpected Character");
            }
            break;
    }
}

char Scanner::Advance() { return mSource[mCurrent++]; }

void Scanner::AddToken(TokenType inTokenType) {
    AddToken(inTokenType, std::nullopt);
} // TODO Replace 0 with sth meaningful

void Scanner::AddToken(TokenType inType, Object inLiteral) {
    const sv text = mSource.substr(mStart, mCurrent - mStart);
    mTokens.push_back(Token(inType, text, inLiteral, mLine));
}

void Scanner::ReadString() {
    while (Peek() != '"' and not IsAtEnd()) {
        if (Peek() == '\n') mLine++;
        Advance();
    }

    if (IsAtEnd()) {
        Lua::Error(mLine, "Unterminated String");
        return;
    }

    Advance(); // eat "
    sv value = mSource.substr(mStart + 1, mCurrent - 1 - (mStart + 1));
    AddToken(STRING, s(value));
}

void Scanner::ReadNumber() {
    while (std::isdigit(Peek()))
        Advance();

    if (Peek() == '.' and std::isdigit(PeekNext())) {
        // eat .
        Advance();
        while (std::isdigit(Peek()))
            Advance();
    }
    s num = s(mSource.substr(mStart, mCurrent - mStart));
    AddToken(NUMBER, std::stod(num));
}

void Scanner::ReadIdentifier() {
    while (std::isalnum(Peek()))
        Advance();

    sv text = mSource.substr(mStart, mCurrent - mStart);
    TokenType token;
    if (KeywordsMap[text]) {
        token = KeywordsMap[text];
    } else {
        token = IDENTIFIER;
    }

    AddToken(token);
}