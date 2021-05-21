#include "parser/Token.h"

namespace jc::parser {
    const std::map<std::string, TokenType> Token::keywords = {
        {"as",          TokenType::As},
        {"break",       TokenType::Break},
        {"const",       TokenType::Const},
        {"continue",    TokenType::Continue},
        {"elif",        TokenType::Elif},
        {"else",        TokenType::Else},
        {"enum",        TokenType::Enum},
        {"false",       TokenType::False},
        {"for",         TokenType::For},
        {"func",        TokenType::Func},
        {"if",          TokenType::If},
        {"impl",        TokenType::Impl},
        {"in",          TokenType::In},
        {"!in",         TokenType::NotIn},
        {"infix",       TokenType::Infix},
        {"init",        TokenType::Init},
        {"loop",        TokenType::Loop},
        {"move",        TokenType::Move},
        {"mut",         TokenType::Mut},
        {"return",      TokenType::Return},
        {"pri",         TokenType::Pri},
        {"pub",         TokenType::Pub},
        {"static",      TokenType::Static},
        {"struct",      TokenType::Struct},
        {"this",        TokenType::This},
        {"trait",       TokenType::Trait},
        {"true",        TokenType::True},
        {"type",        TokenType::Type},
        {"val",         TokenType::Val},
        {"var",         TokenType::Var},
        {"when",        TokenType::When},
        {"while",       TokenType::While},
    };

    const std::map<TokenType, std::string> Token::tokenTypeStrings = {
        {TokenType::Eof,             "EOF"},
        {TokenType::Nl,              "NL"},
        {TokenType::DecLiteral,      "DecLiteral"},
        {TokenType::BinLiteral,      "BinLiteral"},
        {TokenType::OctLiteral,      "OctLiteral"},
        {TokenType::HexLiteral,      "HexLiteral"},
        {TokenType::FloatLiteral,    "FloatLiteral"},
        {TokenType::SQStringLiteral, "SQStringLiteral"},
        {TokenType::Id,              "ID"},

        // Operators //
        {TokenType::Assign,          "="},
        {TokenType::AddAssign,       "+="},
        {TokenType::SubAssign,       "-="},
        {TokenType::MulAssign,       "*="},
        {TokenType::DivAssign,       "/="},
        {TokenType::ModAssign,       "%="},
        {TokenType::PowerAssign,     "**="},
        {TokenType::ShlAssign,          "<<="},
        {TokenType::ShrAssign,          ">>="},
        {TokenType::BitAndAssign,       "&="},
        {TokenType::BitOrAssign,        "|="},
        {TokenType::XorAssign,          "^="},
        {TokenType::NullishAssign,      "??="},
        {TokenType::Add,                "+"},
        {TokenType::Sub,                "-"},
        {TokenType::Mul,                "*"},
        {TokenType::Div,                "/"},
        {TokenType::Mod,                "%"},
        {TokenType::Power,              "**"},
        {TokenType::Inc,                "++"},
        {TokenType::Dec,                "--"},
        {TokenType::Or,                 "||"},
        {TokenType::And,                "&&"},
        {TokenType::NullCoalesce,       "??"},
        {TokenType::Shl,                "<<"},
        {TokenType::Shr,                ">>"},
        {TokenType::BitAnd,             "&"},
        {TokenType::BitOr,              "|"},
        {TokenType::Xor,                "^"},
        {TokenType::Inv,                "~"},
        {TokenType::Not,                "!"},
        {TokenType::Eq,                 "=="},
        {TokenType::NotEq,              "!="},
        {TokenType::LAngle,             "<"},
        {TokenType::RAngle,             ">"},
        {TokenType::LE,                 "<="},
        {TokenType::GE,                 ">="},
        {TokenType::Spaceship,          "<=>"},
        {TokenType::RefEq,              "==="},
        {TokenType::RefNotEq,           "!=="},
        {TokenType::Range,              ".."},
        {TokenType::RangeEQ,            "..="},
        {TokenType::Dot,                "."},
        {TokenType::Path,               "::"},
        {TokenType::Spread,             "..."},
        {TokenType::Pipe,               "|>"},
        {TokenType::Dollar,             "$"},
        {TokenType::At,                 "@"},
        {TokenType::At_WWS,             "@(WWS)"},

        // Punctuations //
        {TokenType::Semi,               ";"},
        {TokenType::Arrow,              "->"},
        {TokenType::DoubleArrow,        "=>"},
        {TokenType::LParen,             "("},
        {TokenType::RParen,             ")"},
        {TokenType::LBrace,             "{"},
        {TokenType::RBrace,             "}"},
        {TokenType::LBracket,           "["},
        {TokenType::RBracket,           "]"},
        {TokenType::Comma,              ","},
        {TokenType::Colon,              ":"},
        {TokenType::Quest,              "?"},
    };

    const std::vector<TokenType> Token::assignOperators = {
        TokenType::Assign,
        TokenType::AddAssign,
        TokenType::SubAssign,
        TokenType::MulAssign,
        TokenType::DivAssign,
        TokenType::ModAssign,
        TokenType::PowerAssign,
        TokenType::ShlAssign,
        TokenType::ShrAssign,
        TokenType::BitAndAssign,
        TokenType::BitOrAssign,
        TokenType::XorAssign,
        TokenType::NullishAssign,
    };

    const std::vector<TokenType> Token::literals = {
        TokenType::DecLiteral,
        TokenType::BinLiteral,
        TokenType::OctLiteral,
        TokenType::HexLiteral,
        TokenType::FloatLiteral,
        TokenType::SQStringLiteral,
        TokenType::DQStringLiteral,
    };

    bool Token::is(TokenType type) const {
        return this->type == type;
    }

    bool Token::isAssignOp() const {
        return utils::arr::has(assignOperators, type);
    }

    bool Token::isLiteral() const {
        return utils::arr::has(literals, type);
    }

//    bool Token::isModifier() const {
//        return is(TokenType::Infix) or is(TokenType::Operator);
//    }

    std::string Token::typeToString(TokenType type) {
        const auto found = tokenTypeStrings.find(type);
        if (found != tokenTypeStrings.end()) {
            return found->second;
        }

        for (const auto & kw : keywords) {
            if (kw.second == type) {
                return kw.first;
            }
        }

        return "No representation";
    }

    std::string Token::typeToString(const Token & token) {
        return typeToString(token.type);
    }

    std::string Token::typeToString() const {
        return typeToString(*this);
    }

    std::string Token::toString(bool prettyQuotes) const {
        std::string str;
        if (prettyQuotes) {
            str += "`";
        }
        switch (type) {
            case TokenType::DecLiteral:
            case TokenType::BinLiteral:
            case TokenType::OctLiteral:
            case TokenType::HexLiteral:
            case TokenType::FloatLiteral:
            case TokenType::SQStringLiteral:
            case TokenType::DQStringLiteral:
            case TokenType::Id: {
                str += val;
            } break;
            default: {
                str += typeToString();
            }
        }
        return str + "`";
    }

    std::string Token::dump(bool withLoc) const {
        std::string str = typeToString();

        switch (type) {
            case TokenType::DecLiteral:
            case TokenType::BinLiteral:
            case TokenType::OctLiteral:
            case TokenType::HexLiteral:
            case TokenType::FloatLiteral:
            case TokenType::SQStringLiteral:
            case TokenType::DQStringLiteral:
            case TokenType::Id: {
                str += ":'" + val + "'";
            }
        }

        if (withLoc) {
            str += " at " + loc.toString();
        }

        return str;
    }

    span::Span Token::span(sess::sess_ptr sess) const {
        return span::Span(loc.line, loc.col, loc.len, sess->fileId);
    }
}
