#include "parser/Lexer.h"

namespace jc::parser {
    Lexer::Lexer() {
        loc.line = 1;
        loc.col = 1;
        loc.offset = 0;
    }

    void Lexer::addToken(Token && t, uint16_t len) {
        t.loc = loc;
        t.loc.len = len;
        tokens.emplace_back(t);
    }

    void Lexer::addToken(TokenType type, const std::string & val) {
        addToken(Token(type, val, loc), val.size());
    }

    char Lexer::peek() {
        if (eof()) {
            return 0;
        }
        return source.at(loc.offset);
    }

    char Lexer::lookup(int distance) {
        if (loc.offset + distance >= source.size()) {
            return 0;
        }
        return source.at(loc.offset + distance);
    }

    char Lexer::advance(int distance) {
        for (int i = 0; i < distance; i++) {
            if (isNL()) {
                loc.line++;
                loc.col = 1;
            } else {
                loc.col++;
            }
            loc.offset++;
        }
        return peek();
    }

    char Lexer::forward() {
        const auto cur = peek();
        advance();
        return cur;
    }

    //
    bool Lexer::eof() {
        return loc.offset >= source.size();
    }

    bool Lexer::hidden() {
        return hidden(peek());
    }

    bool Lexer::hidden(char c) {
        return c == '\t' || c == ' ' || c == '\r';
    }

    bool Lexer::isNL() {
        return peek() == '\n';
    }

    bool Lexer::isDigit() {
        return isDigit(peek());
    }

    bool Lexer::isDigit(char c) {
        return c >= '0' and c <= '9';
    }

    bool Lexer::isBinDigit() {
        return peek() == '0' or peek() == '1';
    }

    bool Lexer::isOctDigit() {
        return peek() >= '0' and peek() <= '7';
    }

    bool Lexer::isHexDigit() {
        return peek() >= '0' and peek() <= '9' or peek() >= 'a' and peek() >= 'z' or peek() >= 'A' and peek() <= 'Z';
    }

    bool Lexer::isAlpha(char c) {
        return c >= 'a' and c <= 'z' or c >= 'A' and c <= 'Z';
    }

    bool Lexer::isExpSign() {
        return peek() == 'e' or peek() == 'E';
    }

    bool Lexer::isIdFirst() {
        return isIdFirst(peek());
    }

    bool Lexer::isIdFirst(char c) {
        return isAlpha(c) || isDigit(c);
    }

    bool Lexer::isIdPart() {
        return isIdFirst() || isDigit();
    }

    bool Lexer::isQuote() {
        return peek() == '\'' || peek() == '"';
    }

    // Lexers //
    void Lexer::lexNumber() {
        std::string num;
        const bool allowBase = forward() == '0';
        bool baseUpperCase;

        if (allowBase) {
            switch (peek()) {
                case 'b':
                case 'B': {
                    baseUpperCase = peek() == 'B';
                    lexBinLiteral(baseUpperCase);
                    return;
                }
                case 'o':
                case 'O': {
                    baseUpperCase = peek() == 'O';
                    lexOctLiteral(baseUpperCase);
                    return;
                }
                case 'x':
                case 'X': {
                    baseUpperCase = peek() == 'X';
                    lexHexLiteral(baseUpperCase);
                    return;
                }
            }
            num = '0';
        }

        while (isDigit()) {
            num += forward();
        }

        if (peek() == '.') {
            if (!isDigit(lookup())) {
                addToken(TokenType::DecLiteral, num);
                return;
            }

            lexFloatLiteral(num);
        } else {
            addToken(TokenType::DecLiteral, num);
        }
    }

    void Lexer::lexBinLiteral(bool upperCase) {
        std::string num = "0";
        num += upperCase ? "B" : "b";

        while (isBinDigit()) {
            num += forward();
        }

        addToken(TokenType::BinLiteral, num);
    }

    void Lexer::lexOctLiteral(bool upperCase) {
        std::string num = "0";
        num += upperCase ? "O" : "o";

        while (isOctDigit()) {
            num += forward();
        }

        addToken(TokenType::OctLiteral, num);
    }

    void Lexer::lexHexLiteral(bool upperCase) {
        std::string num = "0";
        num += upperCase ? "X" : "x";

        while (isHexDigit()) {
            num += forward();
        }

        addToken(TokenType::HexLiteral, num);
    }

    void Lexer::lexFloatLiteral(const std::string & start) {
        std::string num = start;

        while (isDigit()) {
            num += forward();
        }

        // TODO: Exponents

        addToken(TokenType::FloatLiteral, num);
    }

    void Lexer::lexId() {
        std::string id(1, forward());

        while (!eof() and isIdPart()) {
            id += forward();
        }

        if (id == "as" and peek() == '?') {
            addToken(TokenType::AsQM);
            return;
        }

        const auto kw = Token::keywords.find(id);
        if (kw != Token::keywords.end()) {
            addToken(kw->second);
        } else {
            addToken(TokenType::Id, id);
        }
    }

    void Lexer::lexString() {
        const auto quote = forward();
        std::string str;

        // TODO: Cover to function `isSingleQuote` or something, to avoid hard-coding
        const auto tokenType = quote == '"' ? TokenType::DQStringLiteral : TokenType::SQStringLiteral;

        // TODO: String templates
        while (!eof() and peek() != quote) {
            str += forward();
        }

        if (peek() != quote) {
            unexpectedEof();
        }

        advance();

        addToken(tokenType, str);
    }

    void Lexer::lexOp() {
        switch (peek()) {
            case '=': {
                if (lookup() == '>') {
                    addToken(TokenType::DoubleArrow);
                    advance(2);
                } else if (lookup() == '=') {
                    if (lookup(2) == '=') {
                        addToken(TokenType::RefEq);
                        advance(3);
                    } else {
                        addToken(TokenType::Eq);
                        advance(2);
                    }
                } else {
                    addToken(TokenType::Assign);
                    advance();
                }
            } break;
            case '+': {
                if (lookup() == '=') {
                    addToken(TokenType::AddAssign);
                    advance(2);
                } else if (lookup() == '+') {
                    addToken(TokenType::Inc);
                    advance(2);
                } else {
                    addToken(TokenType::Add);
                    advance();
                }
            } break;
            case '-': {
                if (lookup() == '=') {
                    addToken(TokenType::SubAssign);
                    advance(2);
                } else if (lookup() == '-') {
                    addToken(TokenType::Dec);
                    advance(2);
                } else if (lookup() == '>') {
                    addToken(TokenType::Arrow);
                    advance(2);
                } else {
                    addToken(TokenType::Sub);
                    advance();
                }
            } break;
            case '*': {
                if (lookup() == '*') {
                    if (lookup(2) == '=') {
                        addToken(TokenType::PowerAssign);
                        advance(3);
                    } else {
                        addToken(TokenType::Power);
                        advance(2);
                    }
                } else if (lookup() == '=') {
                    addToken(TokenType::MulAssign);
                    advance(2);
                } else {
                    addToken(TokenType::Mul);
                    advance();
                }
            } break;
            case '/': {
                if (lookup() == '/') {
                    while (!eof()) {
                        advance();
                        if (isNL()) {
                            break;
                        }
                    }
                } else if (lookup() == '*') {
                    while (!eof()) {
                        advance();
                        if (peek() == '*' and lookup() == '/') {
                            break;
                        }
                    }
                    advance(2);
                } else if (lookup() == '=') {
                    addToken(TokenType::DivAssign);
                    advance(2);
                } else {
                    addToken(TokenType::Div);
                    advance();
                }
            } break;
            case '%': {
                if (lookup() == '=') {
                    addToken(TokenType::ModAssign);
                    advance(2);
                } else {
                    addToken(TokenType::Mod);
                    advance();
                }
            } break;
            case ';': {
                addToken(TokenType::Semi);
                advance();
            } break;
            case '(': {
                addToken(TokenType::LParen);
                advance();
            } break;
            case ')': {
                addToken(TokenType::RParen);
                advance();
            } break;
            case '{': {
                addToken(TokenType::LBrace);
                advance();
            } break;
            case '}': {
                addToken(TokenType::RBrace);
                advance();
            } break;
            case '[': {
                addToken(TokenType::LBracket);
                advance();
            } break;
            case ']': {
                addToken(TokenType::RBracket);
                advance();
            } break;
            case ',': {
                addToken(TokenType::Comma);
                advance();
            } break;
            case ':': {
                addToken(TokenType::Colon);
                advance();
            } break;
            case '.': {
                if (isDigit(lookup())) {
                    lexFloatLiteral(".");
                } else if (lookup() == '.') {
                    if (lookup(2) == '.') {
                        addToken(TokenType::Spread);
                        advance(3);
                    } else if (lookup(2) == '<') {
                        addToken(TokenType::RangeRE);
                        advance(3);
                    } else {
                        addToken(TokenType::Range);
                        advance(2);
                    }
                } else {
                    addToken(TokenType::Dot);
                    advance();
                }
            } break;
            case '&': {
                if (lookup() == '&') {
                    addToken(TokenType::And);
                    advance(2);
                } else if (lookup() == '=') {
                    addToken(TokenType::BitAndAssign);
                    advance(2);
                } else {
                    addToken(TokenType::BitAnd);
                    advance();
                }
            } break;
            case '!': {
                if (lookup() == '=') {
                    if (lookup(2) == '=') {
                        addToken(TokenType::RefNotEq);
                        advance(3);
                    } else {
                        addToken(TokenType::NotEq);
                        advance(2);
                    }
                } else if (lookup() == 'i' and lookup(2) == 's') {
                    // `!is` operator
                    addToken(TokenType::NotIs);
                    advance(3);
                } else if (lookup() == 'i' and lookup(2) == 'n') {
                    addToken(TokenType::NotIn);
                    advance(3);
                } else {
                    addToken(TokenType::Not);
                    advance();
                }
            } break;
            case '|': {
                if (lookup() == '|') {
                    addToken(TokenType::Or);
                    advance(2);
                } else if (lookup() == '>') {
                    addToken(TokenType::Pipe);
                    advance(2);
                } else if (lookup() == '=') {
                    addToken(TokenType::BitOrAssign);
                    advance(2);
                } else {
                    addToken(TokenType::BitOr);
                    advance();
                }
            } break;
            case '<': {
                if (lookup() == '=') {
                    if (lookup(2) == '>') {
                        addToken(TokenType::Spaceship);
                        advance(3);
                    } else {
                        addToken(TokenType::LE);
                        advance(2);
                    }
                } else if (lookup() == '<') {
                    if (lookup(2) == '=') {
                        addToken(TokenType::ShlAssign);
                        advance(3);
                    } else {
                        addToken(TokenType::Shl);
                        advance(2);
                    }
                } else {
                    addToken(TokenType::LAngle);
                    advance();
                }
            } break;
            case '>': {
                if (lookup() == '=') {
                    addToken(TokenType::GE);
                    advance(2);
                } else if (lookup() == '.') {
                    if (lookup(2) == '.') {
                        addToken(TokenType::RangeLE);
                        advance(3);
                    } else if (lookup(2) == '<') {
                        addToken(TokenType::RangeBothE);
                        advance(3);
                    } else {
                        unexpectedTokenError();
                    }
                } else if (lookup() == '>') {
                    if (lookup(2) == '=') {
                        addToken(TokenType::ShrAssign);
                        advance(3);
                    } else {
                        addToken(TokenType::Shr);
                        advance(2);
                    }
                } else {
                    addToken(TokenType::RAngle);
                    advance();
                }
            } break;
            case '^': {
                if (lookup() == '=') {
                    addToken(TokenType::XorAssign);
                    advance(2);
                } else {
                    addToken(TokenType::Xor);
                    advance();
                }
            } break;
            case '~': {
                addToken(TokenType::Inv);
                advance();
            } break;
            case '?': {
                if (lookup() == '?') {
                    if (lookup(2) == '=') {
                        addToken(TokenType::NullishAssign);
                        advance(3);
                    } else {
                        addToken(TokenType::NullCoalesce);
                        advance(2);
                    }
                } else {
                    if (lookup() == '.') {
                        addToken(TokenType::SafeCall);
                        advance(2);
                    } else {
                        addToken(TokenType::Quest);
                        advance();
                    }
                }
            } break;
            case '$': {
                addToken(TokenType::Dollar);
            } break;
            case '@': {
                if (!hidden()) {
                    addToken(TokenType::At_WWS);
                } else {
                    addToken(TokenType::At);
                }
            } break;
            default: {
                unexpectedTokenError();
            }
        }
    }

    //

    token_list Lexer::lex(const std::string & source) {
        log.dev("Tokenize...");

        this->source = source;

        sess::source_t sourceLines;
        std::string line;

        while (!eof()) {
            line += peek();
            if (hidden()) {
                advance();
            } else if (isNL()) {
                addToken(TokenType::Nl);
                advance();
                sourceLines.push_back(line);
                line = "";
            } else if (isDigit()) {
                lexNumber();
            } else if (isIdFirst()) {
                lexId();
            } else if (isQuote()) {
                lexString();
            } else {
                lexOp();
            }
        }

        addToken(TokenType::Eof);

        log.debug("Source lines:");
        for (size_t i = 0; i < sourceLines.size(); i++) {
            log.raw(i, "|", sourceLines.at(i));
        }

        sess::sourceMap.addSource(std::move(sourceLines));

        return tokens;
    }


    void Lexer::error(const std::string & msg) {
        throw LexerError(msg);
    }

    void Lexer::unexpectedTokenError() {
        throw common::UnexpectedTokenError(std::to_string(peek()));
    }

    void Lexer::unexpectedEof() {
        throw common::UnexpectedEof();
    }
}
