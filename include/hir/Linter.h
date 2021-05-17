#ifndef JACY_HIR_LINTER_H
#define JACY_HIR_LINTER_H

#include "common/Logger.h"
#include "ast/BaseVisitor.h"
#include "ast/nodes.h"
#include "suggest/BaseSugg.h"
#include "data_types/SuggResult.h"

namespace jc::hir {
    class Linter : public ast::BaseVisitor {
    public:
        Linter();

        dt::SuggResult<dt::none_t> lint(sess::sess_ptr sess, const ast::stmt_list & tree);

    private:
        // Statements //
        void visit(ast::EnumDecl * enumDecl) override;
        void visit(ast::ExprStmt * exprStmt) override;
        void visit(ast::ForStmt * forStmt) override;
        void visit(ast::FuncDecl * funcDecl) override;
        void visit(ast::Impl * impl) override;
        void visit(ast::Item * item) override;
        void visit(ast::Struct * _struct) override;
        void visit(ast::Trait * trait) override;
        void visit(ast::TypeAlias * typeAlias) override;
        void visit(ast::VarDecl * varDecl) override;
        void visit(ast::WhileStmt * whileStmt) override;

        // Expressions //
        void visit(ast::Assignment * assignment) override;
        void visit(ast::BreakExpr * breakExpr) override;
        void visit(ast::ContinueExpr * continueExpr) override;
        void visit(ast::Identifier * identifier) override;
        void visit(ast::IfExpr * ifExpr) override;
        void visit(ast::Infix * infix) override;
        void visit(ast::Invoke * invoke) override;
        void visit(ast::ListExpr * listExpr) override;
        void visit(ast::LiteralConstant * literalConstant) override;
        void visit(ast::LoopExpr * loopExpr) override;
        void visit(ast::ParenExpr * parenExpr) override;
        void visit(ast::Postfix * postfix) override;
        void visit(ast::Prefix * prefix) override;
        void visit(ast::ReturnExpr * returnExpr) override;
        void visit(ast::SpreadExpr * spreadExpr) override;
        void visit(ast::Subscript * subscript) override;
        void visit(ast::SuperExpr * superExpr) override;
        void visit(ast::ThisExpr * thisExpr) override;
        void visit(ast::TupleExpr * tupleExpr) override;
        void visit(ast::UnitExpr * unitExpr) override;
        void visit(ast::WhenExpr * whenExpr) override;

        // Types //
        void visit(ast::ParenType * parenType) override;
        void visit(ast::TupleType * tupleType) override;
        void visit(ast::FuncType * funcType) override;
        void visit(ast::ArrayType * listType) override;
        void visit(ast::TypePath * typePath) override;
        void visit(ast::UnitType * unitType) override;

    private:
        sess::sess_ptr sess;
        sugg::sugg_list suggestions;
        void suggest(sugg::sugg_ptr suggestion);

        common::Logger log{"linter", {}};
    };
}

#endif //JACY_HIR_LINTER_H
