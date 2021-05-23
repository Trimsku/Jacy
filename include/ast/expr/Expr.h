#ifndef JACY_AST_EXPR_EXPR_H
#define JACY_AST_EXPR_EXPR_H

#include "ast/Node.h"
#include "ast/BaseVisitor.h"
#include "data_types/Option.h"
#include "ast/ConstVisitor.h"

namespace jc::ast {
    struct Expr;
    using expr_ptr = std::shared_ptr<Expr>;
    using opt_expr_ptr = dt::Option<expr_ptr>;
    using expr_list = std::vector<expr_ptr>;

    enum class ExprKind {
        Assign,
        Block,
        Borrow,
        Break,
        Continue,
        Deref,
        Id,
        If,
        Infix,
        Invoke,
        Lambda,
        List,
        LiteralConstant,
        Loop,
        MemberAccess,
        Paren,
        Path,
        Prefix,
        Quest,
        Return,
        Spread,
        Subscript,
        Super,
        This,
        Tuple,
        Unit,
        When,

        Error,
    };

    struct Expr : Node {
        Expr(const Span & span, ExprKind kind) : Node(span), kind(kind) {}

        ExprKind kind;

        bool is(ExprKind kind) const {
            return this->kind == kind;
        }

        bool isSimple() const {
            return kind == ExprKind::LiteralConstant
                   or kind == ExprKind::Id;
        }

        template<class T>
        static std::shared_ptr<T> as(expr_ptr expr) {
            return std::static_pointer_cast<T>(expr);
        }

        virtual void accept(BaseVisitor & visitor) = 0;
        virtual void accept(ConstVisitor & visitor) const = 0;
    };

    struct ErrorExpr : Expr {
        explicit ErrorExpr(const Span & span) : Expr(span, ExprKind::Error) {}


        void accept(BaseVisitor & visitor) override {
            return visitor.visit(*this);
        }

        void accept(ConstVisitor & visitor) const override {
            return visitor.visit(*this);
        }
    };
}

#endif // JACY_AST_EXPR_EXPR_H
