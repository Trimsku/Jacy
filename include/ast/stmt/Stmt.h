#ifndef JACY_STMT_H
#define JACY_STMT_H

#include <vector>

#include "ast/Node.h"
#include "ast/BaseVisitor.h"

namespace jc::ast {
    struct Stmt;
    using stmt_ptr = std::shared_ptr<Stmt>;
    using opt_stmt_ptr = dt::Option<stmt_ptr>;
    using stmt_list = std::vector<stmt_ptr>;

    enum class StmtKind {
        Expr,
        For,
        VarDecl,
        While,
        Item,

        Error,
    };

    struct Stmt : Node {
        Stmt(const Span & span, StmtKind kind) : Node(span), kind(kind) {}

        StmtKind kind;

        bool is(StmtKind kind) const {
            return this->kind == kind;
        }

        template<class T>
        static std::shared_ptr<T> as(stmt_ptr stmt) {
            return std::static_pointer_cast<T>(stmt);
        }

        virtual void accept(BaseVisitor & visitor) = 0;
        virtual void accept(ConstVisitor & visitor) const = 0;
    };

    struct ErrorStmt : Stmt {
        explicit ErrorStmt(const Span & span) : Stmt(span, StmtKind::Error) {}

        void accept(BaseVisitor & visitor) override {
            return visitor.visit(*this);
        }
        void accept(ConstVisitor & visitor) const override {
            return visitor.visit(*this);
        }
    };
}

#endif // JACY_STMT_H
