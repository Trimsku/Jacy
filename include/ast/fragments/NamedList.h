#ifndef JACY_AST_FRAGMENTS_NAMEDLIST_H
#define JACY_AST_FRAGMENTS_NAMEDLIST_H

#include "ast/expr/Expr.h"
#include "ast/fragments/Identifier.h"

namespace jc::ast {
    struct NamedElement;
    using named_el_ptr = std::shared_ptr<NamedElement>;
    using named_list = std::vector<named_el_ptr>;

    struct NamedElement : Node {
        NamedElement(opt_id_ptr name, opt_expr_ptr value, const Span & span)
            : Node(span), name(std::move(name)), value(std::move(value)) {}

        opt_id_ptr name;
        opt_expr_ptr value;

        void accept(BaseVisitor & visitor) const override {
            return visitor.visit(*this);
        }
    };
}

#endif // JACY_AST_FRAGMENTS_NAMEDLIST_H
