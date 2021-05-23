#ifndef JACY_ENUMDECL_H
#define JACY_ENUMDECL_H

#include <vector>

#include "ast/item/Item.h"
#include "ast/fragments/Identifier.h"

namespace jc::ast {
    struct EnumEntry;
    using enum_entry_ptr = std::shared_ptr<EnumEntry>;
    using enum_entry_list = std::vector<enum_entry_ptr>;

    struct EnumEntry {
        id_ptr name;
        expr_ptr value;
    };

    struct EnumDecl : Item {
        EnumDecl(attr_list attributes, const Span & span) : Item(span, std::move(attributes), ItemKind::Enum) {}

        id_ptr name;
        enum_entry_list entries;
        stmt_list body;

        void accept(BaseVisitor & visitor) override {
            return visitor.visit(*this);
        }

        void accept(ConstVisitor & visitor) const override {
            return visitor.visit(*this);
        }
    };
}

#endif // JACY_ENUMDECL_H
