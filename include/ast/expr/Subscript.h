#ifndef JACY_SUBSCRIPT_H
#define JACY_SUBSCRIPT_H

#include "ast/expr/Expr.h"

namespace jc::ast {
    struct Subscript : Expr {
        Subscript(expr_ptr lhs, expr_list indices, const Span & span)
            : lhs(lhs), indices(indices), Expr(span, ExprType::Subscript) {}

        expr_ptr lhs;
        expr_list indices;

        void accept(BaseVisitor & visitor) override {
            return visitor.visit(this);
        }
    };
}

#endif // JACY_SUBSCRIPT_H
