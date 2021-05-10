#ifndef JACY_PARSER_H
#define JACY_PARSER_H

#include <tuple>

#include "common/Logger.h"
#include "Token.h"
#include "ast/nodes.h"
#include "common/common.h"
#include "common/Error.h"
#include "parser/Suggestion.h"
#include "session/Session.h"

namespace jc::parser {
    using ast::makeInfix;

    struct ParserError : common::Error {
        explicit ParserError(const std::string & msg) : Error(msg) {}
    };

    struct ExpectedError : ParserError {
        ExpectedError(const std::string & expected, const std::string & given)
            : ParserError("Expected " + expected + ", " + given + " given") {}
    };

    struct UnexpectedTokenError : ParserError {
        explicit UnexpectedTokenError(const std::string & token) : ParserError("Unexpected token " + token) {}
    };

    class Parser {
    public:
        Parser();
        virtual ~Parser() = default;

        ast::stmt_list parse(const session::Session & sess, const token_list & tokens);

    private:
        uint16_t fileId;

        common::Logger log{"parser", {}};

        token_list tokens;
        size_t index{0};

        ast::stmt_list tree;

        const Token & peek() const;
        const Token & advance();
        const Token & lookup() const;

        // Checkers //
        bool eof();
        bool is(TokenType type) const;
        bool isNL();
        bool isSemis();

        // Skippers //
        bool skipNLs(bool optional = false);
        void skipSemis();
        void skip(TokenType type, bool skipLeftNLs, bool skipRightNLs, const std::string & expected);
        bool skipOpt(TokenType type, bool skipRightNLs = false);

        // States //
        ast::attr_list attributes{};
        parser::Token lastToken; // Last non-NL token

    private:
        ast::stmt_ptr parseTopLevel();

        ast::stmt_ptr parseStmt();

        // Control-flow statements //
    private:
        ast::stmt_ptr parseWhileStmt();
        ast::stmt_ptr parseForStmt();

        // Declarations //
    private:
        ast::stmt_ptr parseDecl(bool optional = false);
        ast::stmt_list parseDeclList();

        ast::stmt_ptr parseVarDecl();
        ast::stmt_ptr parseTypeDecl();
        ast::stmt_ptr parseFuncDecl(const ast::attr_list & attributes, const parser::token_list & modifiers);
        ast::stmt_ptr parseClassDecl(const ast::attr_list & attributes, const parser::token_list & modifiers);
        ast::stmt_ptr parseObjectDecl(const ast::attr_list & attributes, const parser::token_list & modifiers);
        ast::stmt_ptr parseEnumDecl(const ast::attr_list & attributes, const parser::token_list & modifiers);

        ast::delegation_list parseDelegationList();
        ast::delegation_ptr parseDelegation();

        // Expressions //
        ast::expr_ptr parseExpr();
        ast::expr_ptr pipe();
        ast::expr_ptr disjunction();
        ast::expr_ptr conjunction();
        ast::expr_ptr bitOr();
        ast::expr_ptr Xor();
        ast::expr_ptr bitAnd();
        ast::expr_ptr equality();
        ast::expr_ptr comparison();
        ast::expr_ptr spaceship();
        ast::expr_ptr namedChecks();
        ast::expr_ptr nullishCoalesce();
        ast::expr_ptr shift();
        ast::expr_ptr infix();
        ast::expr_ptr range();
        ast::expr_ptr add();
        ast::expr_ptr mul();
        ast::expr_ptr power();
        ast::expr_ptr typeCast();
        ast::expr_ptr prefix();
        ast::expr_ptr postfix();
        ast::expr_ptr primary();

        ast::id_ptr parseId(bool skipNLs = false);
        ast::literal_ptr parseLiteral();
        ast::expr_ptr parseListExpr();
        ast::expr_ptr parseTupleOrParenExpr();

        // Control-flow expressions //
        ast::expr_ptr parseIfExpr();
        ast::expr_ptr parseLoopExpr();
        ast::expr_ptr parseWhenExpr();
        ast::when_entry_ptr parseWhenEntry();

        // Fragments //
        ast::block_ptr parseBlock();
        std::tuple<ast::block_ptr, ast::expr_ptr> parseBodyMaybeOneLine();
        ast::attr_list parseAttributes();
        ast::attr_ptr parseAttr();
        ast::named_list_ptr parseNamedList();
        parser::token_list parseModifiers();
        ast::func_param_list parseFuncParamList(bool isParen);
        ast::func_param_ptr parseFuncParam();

        // Modules //
        ast::stmt_ptr parseImportStmt();

        // Types //
        ast::type_ptr parseType();
        ast::type_ptr parseIdType();
        std::tuple<bool, ast::tuple_t_el_list> parseParenType();
        ast::type_param_list parseTypeParams();

        // Suggestions //
    private:
        bool hasErrorSuggestions{false};
        std::vector<Suggestion> suggestions;
        void suggest(Suggestion::Kind kind, const std::string & msg, uint16_t eid, const Span & span);
        void addError(const std::string & msg, uint16_t eid, const Span & span);

        // DEBUG //
        void logParse(const std::string & entity);
    };
}


#endif // JACY_PARSER_H
