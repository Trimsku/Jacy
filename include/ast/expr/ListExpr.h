#ifndef JACY_LISTEXPR_H
#define JACY_LISTEXPR_H

#include "ast/expr/Expr.h"

namespace jc::ast {
    struct ListExpr : Expr {
        ListExpr(expr_list elements, const Span & span) : elements(elements), Expr(span, ExprType::List) {}

        expr_list elements;

        void accept(BaseVisitor & visitor) override {
            return visitor.visit(this);
        }
    };
}

#endif // JACY_LISTEXPR_H
