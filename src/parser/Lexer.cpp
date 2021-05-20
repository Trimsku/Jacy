#include "parser/Lexer.h"

namespace jc::parser {
    void Lexer::addToken(Token && t, span::span_len_t len) {
        tokenLoc.len = len;
        t.loc = tokenLoc;
        tokens.emplace_back(t);
    }

    void Lexer::addToken(TokenType type, const std::string & val) {
        addToken(Token(type, val, tokenLoc), val.size());
    }

    void Lexer::addToken(TokenType type, span::span_len_t len) {
        addToken(Token(type, "", tokenLoc), len);
    }

    char Lexer::peek() {
        if (eof()) {
            return 0;
        }
        return source.at(index);
    }

    char Lexer::lookup(int distance) {
        if (index + distance >= source.size()) {
            return 0;
        }
        return source.at(index + distance);
    }

    char Lexer::advance(int distance) {
        for (int i = 0; i < distance; i++) {
            line += peek();
            if (isNL()) {
                sourceLines.push_back(line);
                line = "";
                lexerLine++;
                lexerCol = 0;
            } else {
                lexerCol++;
            }
            index++;
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
        return index >= source.size();
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
        const bool allowBase = peek() == '0';

        if (allowBase) {
            switch (peek()) {
                case 'b':
                case 'B': {
                    lexBinLiteral();
                    return;
                }
                case 'o':
                case 'O': {
                    lexOctLiteral();
                    return;
                }
                case 'x':
                case 'X': {
                    lexHexLiteral();
                    return;
                }
            }
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

    void Lexer::lexBinLiteral() {
        std::string num(1, peek());
        num += advance();

        while (isBinDigit()) {
            num += forward();
        }

        addToken(TokenType::BinLiteral, num);
    }

    void Lexer::lexOctLiteral() {
        std::string num(1, peek());
        num += advance();

        while (isOctDigit()) {
            num += forward();
        }

        addToken(TokenType::OctLiteral, num);
    }

    void Lexer::lexHexLiteral() {
        std::string num(1, peek());
        num += advance();

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

        const auto kw = Token::keywords.find(id);
        if (kw != Token::keywords.end()) {
            addToken(kw->second, kw->first.size());
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
                    addToken(TokenType::DoubleArrow, 2);
                    advance(2);
                } else if (lookup() == '=') {
                    if (lookup(2) == '=') {
                        addToken(TokenType::RefEq, 3);
                        advance(3);
                    } else {
                        addToken(TokenType::Eq, 2);
                        advance(2);
                    }
                } else {
                    addToken(TokenType::Assign, 1);
                    advance();
                }
            } break;
            case '+': {
                if (lookup() == '=') {
                    addToken(TokenType::AddAssign, 2);
                    advance(2);
                } else if (lookup() == '+') {
                    addToken(TokenType::Inc, 2);
                    advance(2);
                } else {
                    addToken(TokenType::Add, 1);
                    advance();
                }
            } break;
            case '-': {
                if (lookup() == '=') {
                    addToken(TokenType::SubAssign, 2);
                    advance(2);
                } else if (lookup() == '-') {
                    addToken(TokenType::Dec, 2);
                    advance(2);
                } else if (lookup() == '>') {
                    addToken(TokenType::Arrow, 2);
                    advance(2);
                } else {
                    addToken(TokenType::Sub, 1);
                    advance();
                }
            } break;
            case '*': {
                if (lookup() == '*') {
                    if (lookup(2) == '=') {
                        addToken(TokenType::PowerAssign, 3);
                        advance(3);
                    } else {
                        addToken(TokenType::Power, 2);
                        advance(2);
                    }
                } else if (lookup() == '=') {
                    addToken(TokenType::MulAssign, 2);
                    advance(2);
                } else {
                    addToken(TokenType::Mul, 1);
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
                    addToken(TokenType::DivAssign, 2);
                    advance(2);
                } else {
                    addToken(TokenType::Div, 1);
                    advance();
                }
            } break;
            case '%': {
                if (lookup() == '=') {
                    addToken(TokenType::ModAssign, 2);
                    advance(2);
                } else {
                    addToken(TokenType::Mod, 1);
                    advance();
                }
            } break;
            case ';': {
                addToken(TokenType::Semi, 1);
                advance();
            } break;
            case '(': {
                addToken(TokenType::LParen, 1);
                advance();
            } break;
            case ')': {
                addToken(TokenType::RParen, 1);
                advance();
            } break;
            case '{': {
                addToken(TokenType::LBrace, 1);
                advance();
            } break;
            case '}': {
                addToken(TokenType::RBrace, 1);
                advance();
            } break;
            case '[': {
                addToken(TokenType::LBracket, 1);
                advance();
            } break;
            case ']': {
                addToken(TokenType::RBracket, 1);
                advance();
            } break;
            case ',': {
                addToken(TokenType::Comma, 1);
                advance();
            } break;
            case ':': {
                if (lookup() == ':') {
                    addToken(TokenType::Path, 2);
                    advance(2);
                } else {
                    addToken(TokenType::Colon, 1);
                    advance();
                }
            } break;
            case '.': {
                if (isDigit(lookup())) {
                    lexFloatLiteral(".");
                } else if (lookup() == '.') {
                    if (lookup(2) == '.') {
                        addToken(TokenType::Spread, 3);
                        advance(3);
                    } else if (lookup(2) == '<') {
                        addToken(TokenType::RangeRE, 3);
                        advance(3);
                    } else {
                        addToken(TokenType::Range, 2);
                        advance(2);
                    }
                } else {
                    addToken(TokenType::Dot, 1);
                    advance();
                }
            } break;
            case '&': {
                if (lookup() == '&') {
                    addToken(TokenType::And, 2);
                    advance(2);
                } else if (lookup() == '=') {
                    addToken(TokenType::BitAndAssign, 2);
                    advance(2);
                } else {
                    addToken(TokenType::BitAnd, 1);
                    advance();
                }
            } break;
            case '!': {
                if (lookup() == '=') {
                    if (lookup(2) == '=') {
                        addToken(TokenType::RefNotEq, 2);
                        advance(3);
                    } else {
                        addToken(TokenType::NotEq, 2);
                        advance(2);
                    }
                } else if (lookup() == 'i' and lookup(2) == 'n') {
                    addToken(TokenType::NotIn, 3);
                    advance(3);
                } else {
                    addToken(TokenType::Not, 1);
                    advance();
                }
            } break;
            case '|': {
                if (lookup() == '|') {
                    addToken(TokenType::Or, 2);
                    advance(2);
                } else if (lookup() == '>') {
                    addToken(TokenType::Pipe, 2);
                    advance(2);
                } else if (lookup() == '=') {
                    addToken(TokenType::BitOrAssign, 2);
                    advance(2);
                } else {
                    addToken(TokenType::BitOr, 1);
                    advance();
                }
            } break;
            case '<': {
                if (lookup() == '=') {
                    if (lookup(2) == '>') {
                        addToken(TokenType::Spaceship, 3);
                        advance(3);
                    } else {
                        addToken(TokenType::LE, 2);
                        advance(2);
                    }
                } else if (lookup() == '<') {
                    if (lookup(2) == '=') {
                        addToken(TokenType::ShlAssign, 3);
                        advance(3);
                    } else {
                        addToken(TokenType::Shl, 2);
                        advance(2);
                    }
                } else {
                    addToken(TokenType::LAngle, 1);
                    advance();
                }
            } break;
            case '>': {
                if (lookup() == '=') {
                    addToken(TokenType::GE, 2);
                    advance(2);
                } else if (lookup() == '.') {
                    if (lookup(2) == '.') {
                        addToken(TokenType::RangeLE, 3);
                        advance(3);
                    } else if (lookup(2) == '<') {
                        addToken(TokenType::RangeBothE, 3);
                        advance(3);
                    } else {
                        unexpectedTokenError();
                    }
                } else if (lookup() == '>') {
                    if (lookup(2) == '=') {
                        addToken(TokenType::ShrAssign, 3);
                        advance(3);
                    } else {
                        addToken(TokenType::Shr, 2);
                        advance(2);
                    }
                } else {
                    addToken(TokenType::RAngle, 1);
                    advance();
                }
            } break;
            case '^': {
                if (lookup() == '=') {
                    addToken(TokenType::XorAssign, 2);
                    advance(2);
                } else {
                    addToken(TokenType::Xor, 1);
                    advance();
                }
            } break;
            case '~': {
                addToken(TokenType::Inv, 1);
                advance();
            } break;
            case '?': {
                if (lookup() == '?') {
                    if (lookup(2) == '=') {
                        addToken(TokenType::NullishAssign, 3);
                        advance(3);
                    } else {
                        addToken(TokenType::NullCoalesce, 2);
                        advance(2);
                    }
                } else {
                    addToken(TokenType::Quest, 1);
                    advance();
                }
            } break;
            case '$': {
                addToken(TokenType::Dollar, 1);
            } break;
            case '@': {
                if (!hidden()) {
                    addToken(TokenType::At_WWS, 1);
                } else {
                    addToken(TokenType::At, 1);
                }
            } break;
            default: {
                unexpectedTokenError();
            }
        }
    }

    //

    token_list Lexer::lex(sess::sess_ptr sess, const std::string & source) {
        log.dev("Tokenize...");

        this->source = source;
        this->sess = sess;

        while (!eof()) {
            tokenLoc = {lexerLine, lexerCol};
            if (hidden()) {
                advance();
            } else if (isNL()) {
                addToken(TokenType::Nl, 1);
                advance();
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

        tokenLoc = {lexerLine, lexerCol};
        addToken(TokenType::Eof, 1);

        // Add last line, cause it ends with `EOF`
        sourceLines.push_back(line);

        sess::SourceMap::getInstance().setSource(sess, std::move(sourceLines));

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
