#ifndef JACY_LISTEXPR_H
#define JACY_LISTEXPR_H

#include "ast/expr/Expr.h"

namespace jc::ast {
    struct ListExpr : Expr {
        ListExpr(expr_list elements, const Span & span)
            : elements(std::move(elements)), Expr(span, ExprKind::List) {}

        expr_list elements;

        void accept(BaseVisitor & visitor) override {
            return visitor.visit(*this);
        }

        void accept(const ConstVisitor & visitor) const override {
            return visitor.visit(*this);
        }
    };
}

#endif // JACY_LISTEXPR_H
