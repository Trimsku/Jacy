#include "hir/Linter.h"

namespace jc::hir {
    Linter::Linter() = default;

    dt::SuggResult<dt::none_t> Linter::lint(sess::sess_ptr sess, const ast::stmt_list & tree) {
        log.dev("Lint...");

        this->sess = sess;

        for (const auto & stmt : tree) {
            stmt->accept(*this);
        }

        return {dt::None, std::move(suggestions)};
    }

    void Linter::visit(ast::ErrorStmt * errorStmt) {
        Logger::devPanic("[ERROR STMT] On linter stage at", errorStmt->loc.toString());
    }

    void Linter::visit(ast::ErrorExpr * errorExpr) {
        Logger::devPanic("[ERROR EXPR] On linter stage at", errorExpr->loc.toString());
    }

    void Linter::visit(ast::ErrorType * errorType) {
        Logger::devPanic("[ERROR TYPE] On linter stage at", errorType->loc.toString());
    }

    ////////////////
    // Statements //
    ////////////////
    void Linter::visit(ast::EnumDecl * enumDecl) {
        Logger::notImplemented("Linter::visit enumDecl");
    }

    void Linter::visit(ast::ExprStmt * exprStmt) {
        exprStmt->expr->accept(*this);
    }

    void Linter::visit(ast::ForStmt * forStmt) {
        forStmt->forEntity->accept(*this);
        forStmt->inExpr->accept(*this);
        lint(forStmt->body);
    }

    void Linter::visit(ast::FuncDecl * funcDecl) {
        for (const auto & modifier : funcDecl->modifiers) {
            // TODO: Check for allowed modifiers
        }
        // Something for type params?
        funcDecl->id->accept(*this);

        for (const auto & param : funcDecl->params) {
            if (!param->id) {
                Logger::devPanic("`Linter::funcDecl` -> param.id is None");
            }
            if (param->type) {

            }
        }
    }

    void Linter::visit(ast::Impl * impl) {

    }

    void Linter::visit(ast::Item * item) {

    }

    void Linter::visit(ast::Struct * _struct) {

    }

    void Linter::visit(ast::Trait * trait) {

    }

    void Linter::visit(ast::TypeAlias * typeAlias) {

    }

    void Linter::visit(ast::VarDecl * varDecl) {

    }

    void Linter::visit(ast::WhileStmt * whileStmt) {

    }

    /////////////////
    // Expressions //
    /////////////////
    void Linter::visit(ast::Assignment * assignment) {

    }

    void Linter::visit(ast::BreakExpr * breakExpr) {

    }

    void Linter::visit(ast::ContinueExpr * continueExpr) {

    }

    void Linter::visit(ast::Identifier * identifier) {
        if (!identifier->token) {
            Logger::devPanic("[ERROR ID] on linter stage at", identifier->loc.toString());
        }
    }

    void Linter::visit(ast::IfExpr * ifExpr) {

    }

    void Linter::visit(ast::Infix * infix) {
        log.dev("Visit infix", infix->op.toString());
        if (infix->op.is(parser::TokenType::Id)) {
            suggestErrorMsg("Custom infix operators feature is reserved, but not implemented", infix->op.span(sess));
        }
    }

    void Linter::visit(ast::Invoke * invoke) {

    }

    void Linter::visit(ast::ListExpr * listExpr) {

    }

    void Linter::visit(ast::LiteralConstant * literalConstant) {

    }

    void Linter::visit(ast::LoopExpr * loopExpr) {

    }

    void Linter::visit(ast::ParenExpr * parenExpr) {
        if (parenExpr->expr->type == ast::ExprType::Paren) {
            suggest(
                std::make_unique<sugg::MsgSugg>(
                    "Useless double-wrapped parenthesized expression",
                    parenExpr->loc.span(sess),
                    sugg::SuggKind::Warn
                )
            );
        }
        if (parenExpr->expr->isSimple()) {
            suggest(
                std::make_unique<sugg::MsgSugg>(
                    "Useless parentheses around simple expression",
                    parenExpr->loc.span(sess),
                    sugg::SuggKind::Warn
                )
            );
        }
    }

    void Linter::visit(ast::PathExpr * pathExpr) {

    }

    void Linter::visit(ast::Postfix * postfix) {

    }

    void Linter::visit(ast::Prefix * prefix) {

    }

    void Linter::visit(ast::ReturnExpr * returnExpr) {

    }

    void Linter::visit(ast::SpreadExpr * spreadExpr) {

    }

    void Linter::visit(ast::Subscript * subscript) {

    }

    void Linter::visit(ast::SuperExpr * superExpr) {

    }

    void Linter::visit(ast::ThisExpr * thisExpr) {

    }

    void Linter::visit(ast::TupleExpr * tupleExpr) {

    }

    void Linter::visit(ast::UnitExpr * unitExpr) {

    }

    void Linter::visit(ast::WhenExpr * whenExpr) {

    }

    ///////////
    // Types //
    ///////////
    void Linter::visit(ast::ParenType * parenType) {

    }

    void Linter::visit(ast::TupleType * tupleType) {

    }

    void Linter::visit(ast::FuncType * funcType) {

    }

    void Linter::visit(ast::ArrayType * listType) {

    }

    void Linter::visit(ast::TypePath * typePath) {

    }

    void Linter::visit(ast::UnitType * unitType) {

    }

    void Linter::lint(const ast::block_ptr & block) {
        for (const auto & stmt : block->stmts) {
            stmt->accept(*this);
        }
    }

    // Suggestions //
    void Linter::suggest(sugg::sugg_ptr suggestion) {
        suggestions.push_back(std::move(suggestion));
    }

    void Linter::suggestErrorMsg(const std::string & msg, const span::Span & span, sugg::eid_t eid) {
        suggest(std::make_unique<sugg::MsgSugg>(msg, span, sugg::SuggKind::Error, eid));
    }

    void Linter::suggestWarnMsg(const std::string & msg, const span::Span & span, sugg::eid_t eid) {
        suggest(std::make_unique<sugg::MsgSugg>(msg, span, sugg::SuggKind::Warn, eid));
    }
}
