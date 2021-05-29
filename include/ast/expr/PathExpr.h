#ifndef JACY_AST_EXPR_PATHEXPR_H
#define JACY_AST_EXPR_PATHEXPR_H

#include "ast/expr/Expr.h"
#include "ast/fragments/Identifier.h"
#include "ast/fragments/TypeParams.h"

namespace jc::ast {
    struct PathExprSeg;
    using path_expr_ptr = std::shared_ptr<PathExprSeg>;
    using path_expr_list = std::vector<path_expr_ptr>;

    struct PathExprSeg : Node {
        PathExprSeg(id_ptr name, opt_type_params typeParams, const Span & span)
            : name(std::move(name)), typeParams(std::move(typeParams)), Node(span) {}

        id_ptr name;
        opt_type_params typeParams;
    };

    struct PathExpr : Expr {
        PathExpr(bool global, path_expr_list segments, const Span & span)
            : global(global), segments(std::move(segments)), Expr(span, ExprKind::Path) {}

        bool global;
        path_expr_list segments;

        void accept(BaseVisitor & visitor) override {
            return visitor.visit(*this);
        }

        void accept(ConstVisitor & visitor) const override {
            return visitor.visit(*this);
        }

        void setReference(node_id reference) {
            if (refersTo) {
                common::Logger::devPanic("Called `PathExpr::setReference` on node that already has reference");
            }
            refersTo = reference;
        }

        opt_node_id getReference() const {
            return refersTo;
        }

    private:
        opt_node_id refersTo;
    };
}

#endif // JACY_AST_EXPR_PATHEXPR_H
