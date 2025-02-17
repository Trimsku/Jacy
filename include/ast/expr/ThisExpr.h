#ifndef JACY_AST_EXPR_THISEXPR_H
#define JACY_AST_EXPR_THISEXPR_H

#include "ast/expr/Expr.h"

namespace jc::ast {
    struct ThisExpr : Expr {
        ThisExpr(const Span & span) : Expr(span, ExprKind::This) {}

        void accept(BaseVisitor & visitor) const override {
            return visitor.visit(*this);
        }
    };
}

#endif // JACY_AST_EXPR_THISEXPR_H
