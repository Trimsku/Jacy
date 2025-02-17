#ifndef AST_EXPR_STRUCTEXPR_H
#define AST_EXPR_STRUCTEXPR_H

#include <variant>

#include "ast/expr/Expr.h"
#include "ast/expr/PathExpr.h"

namespace jc::ast {
    struct StructExprField;
    using struct_expr_field_ptr = PR<std::shared_ptr<StructExprField>>;
    using struct_expr_field_list = std::vector<struct_expr_field_ptr>;

    struct StructExprField : Node {
        enum class Kind {
            Raw, // {field: expr}
            Shortcut, // {field}
            Base, // {...base}
        } kind;

        StructExprField(
            id_ptr && field,
            expr_ptr && expr,
            const Span & span
        ) : Node(span),
            kind(Kind::Raw),
            name(std::move(field)),
            expr(std::move(expr)) {}

        StructExprField(
            id_ptr && field,
            const Span & span
        ) : Node(span),
            kind(Kind::Shortcut),
            name(std::move(field)) {}

        StructExprField(
            expr_ptr && expr,
            const Span & span
        ) : Node(span),
            kind(Kind::Base),
            expr(std::move(expr)) {}

        opt_id_ptr name{dt::None};
        opt_expr_ptr expr{dt::None};

        void accept(BaseVisitor & visitor) const override {
            return visitor.visit(*this);
        }
    };

    struct StructExpr : Expr {
        StructExpr(
            path_expr_ptr && path,
            struct_expr_field_list && fields,
            const Span & span
        ) : Expr(span, ExprKind::Struct),
            path(std::move(path)),
            fields(std::move(fields)) {}

        path_expr_ptr path;
        struct_expr_field_list fields;

        void accept(BaseVisitor & visitor) const override {
            return visitor.visit(*this);
        }
    };
}

#endif // AST_EXPR_STRUCTEXPR_H