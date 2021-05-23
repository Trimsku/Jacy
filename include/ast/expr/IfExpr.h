#ifndef JACY_AST_EXPR_IFEXPR_H
#define JACY_AST_EXPR_IFEXPR_H

#include "ast/expr/Expr.h"

#include <utility>
#include "Block.h"

namespace jc::ast {
    struct IfExpr : Expr {
        IfExpr(
            expr_ptr condition,
            opt_block_ptr ifBranch,
            opt_block_ptr elseBranch,
            const Span & span
        ) : condition(std::move(condition)),
            ifBranch(std::move(ifBranch)),
            elseBranch(std::move(elseBranch)),
            Expr(span, ExprKind::If) {}

        expr_ptr condition;
        opt_block_ptr ifBranch;
        opt_block_ptr elseBranch;


        void accept(BaseVisitor & visitor) override {
            return visitor.visit(*this);
        }

        void accept(ConstVisitor & visitor) const override {
            return visitor.visit(*this);
        }
    };
}

#endif // JACY_AST_EXPR_IFEXPR_H
