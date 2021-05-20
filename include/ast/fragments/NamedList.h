#ifndef JACY_NAMEDLIST_H
#define JACY_NAMEDLIST_H

#include "ast/expr/Identifier.h"

namespace jc::ast {
    struct NamedElement;
    struct NamedList;
    using named_el_ptr = std::shared_ptr<NamedElement>;
    using named_list = std::vector<named_el_ptr>;
    using named_list_ptr = std::shared_ptr<NamedList>;

    struct NamedElement : Node {
        NamedElement(dt::Option<id_ptr> id, dt::Option<expr_ptr> value, const Span & span)
            : id(std::move(id)), value(std::move(value)), Node(span) {}

        dt::Option<id_ptr> id;
        dt::Option<expr_ptr> value;
    };

    struct NamedList : Node {
        NamedList(named_list elements, const Span & span) : elements(elements), Node(span) {}

        named_list elements;
    };
}

#endif // JACY_NAMEDLIST_H
