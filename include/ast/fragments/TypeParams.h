#ifndef JACY_AST_FRAGMENTS_TYPEPARAMS_H
#define JACY_AST_FRAGMENTS_TYPEPARAMS_H

#include "Identifier.h"

namespace jc::ast {
    struct Type;
    struct TypeParam;
    using type_param_list = std::vector<std::shared_ptr<TypeParam>>;
    using opt_type_params = dt::Option<type_param_list>;
    using pure_type_ptr = std::shared_ptr<Type>;
    using type_ptr = PR<pure_type_ptr>;
    using opt_type_ptr = dt::Option<type_ptr>;

    enum class TypeParamKind {
        Type,
        Lifetime,
        Const,
    };

    struct TypeParam : Node {
        explicit TypeParam(TypeParamKind kind, const Span & span) : Node(span), kind(kind) {}
        virtual ~TypeParam() = default;

        TypeParamKind kind;

        virtual void accept(BaseVisitor & visitor) const = 0;
    };

    struct GenericType : TypeParam {
        GenericType(
            id_ptr name,
            opt_type_ptr type,
            const Span & span
        ) : TypeParam(TypeParamKind::Type, span),
            name(std::move(name)),
            boundType(std::move(type)) {}

        id_ptr name;
        opt_type_ptr boundType;

        void accept(BaseVisitor & visitor) const override {
            return visitor.visit(*this);
        }
    };

    struct Lifetime : TypeParam {
        Lifetime(id_ptr name, const Span & span)
            : TypeParam(TypeParamKind::Lifetime, span),
              name(std::move(name)) {}

        id_ptr name;

        void accept(BaseVisitor & visitor) const override {
            return visitor.visit(*this);
        }
    };

    struct ConstParam : TypeParam {
        ConstParam(
            id_ptr name,
            type_ptr type,
            opt_expr_ptr defaultValue,
            const Span & span
        ) : TypeParam(TypeParamKind::Const, span),
            name(std::move(name)),
            type(std::move(type)),
            defaultValue(std::move(defaultValue)) {}

        id_ptr name;
        type_ptr type;
        opt_expr_ptr defaultValue;

        void accept(BaseVisitor & visitor) const override {
            return visitor.visit(*this);
        }
    };
}

#endif // JACY_AST_FRAGMENTS_TYPEPARAMS_H


