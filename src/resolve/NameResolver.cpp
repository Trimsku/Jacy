#include "resolve/NameResolver.h"

namespace jc::resolve {
    dt::SuggResult<rib_ptr> NameResolver::resolve(sess::sess_ptr sess, const ast::item_list & tree) {
        visitItems(tree);

        return {rib, std::move(suggestions)};
    }

    void NameResolver::visit(ast::Item * item) {
        item->stmt->accept(*this);
    }

    // Statements //
    void NameResolver::visit(ast::ExprStmt * exprStmt) {
        exprStmt->expr->accept(*this);
    }

    void NameResolver::visit(ast::WhileStmt * whileStmt) {
        whileStmt->condition->accept(*this);
        visit(whileStmt->body.get());
    }

    // Expressions //
    void NameResolver::visit(ast::Assignment * assign) {
        assign->lhs->accept(*this);
        assign->rhs->accept(*this);
    }

    void NameResolver::visit(ast::Block * block) {
        enterRib(); // -> block rib
        for (const auto & stmt : block->stmts) {
            stmt->accept(*this);
        }
        exitRib(); // <- block rib
    }

    void NameResolver::visit(ast::BorrowExpr * borrowExpr) {
        borrowExpr->expr->accept(*this);
    }

    void NameResolver::visit(ast::BreakExpr * breakExpr) {
        if (breakExpr->expr) {
            breakExpr->expr.unwrap()->accept(*this);
        }
    }

    void NameResolver::visit(ast::ContinueExpr * continueExpr) {}

    void NameResolver::visit(ast::DerefExpr * derefExpr) {
        derefExpr->expr->accept(*this);
    }

    void NameResolver::visit(ast::Identifier * identifier) {
        // Note: If we have standalone identifier like that,
        //  we 100% sure that it is inside of an expression.
        //  Just because TypeResolver and ItemResolver handles identifiers that're parts
        //  of types or items already.
        //  So, if we have identifier here...
        //  It MUST be any kind of variable or item (!)

        resolveId(identifier->getValue());
    }

    void NameResolver::visit(ast::IfExpr * ifExpr) {
        ifExpr->condition->accept(*this);
        if (ifExpr->ifBranch) {
            ifExpr->ifBranch.unwrap()->accept(*this);
        }
        if (ifExpr->elseBranch) {
            ifExpr->elseBranch.unwrap()->accept(*this);
        }
    }

    void NameResolver::visit(ast::Infix * infix) {
        infix->lhs->accept(*this);
        infix->rhs->accept(*this);
    }

    void NameResolver::visit(ast::Invoke * invoke) {
        invoke->lhs->accept(*this);
        visitNamedList(invoke->args);
    }

    void NameResolver::visit(ast::Lambda * lambdaExpr) {
        enterRib(); // -> (lambda params)

        for (const auto & param : lambdaExpr->params) {
            param->name->accept(*this);
            if (param->type) {
                param->type.unwrap()->accept(*this);
            }
        }

        if (lambdaExpr->returnType) {
            lambdaExpr->returnType.unwrap()->accept(*this);
        }

        lambdaExpr->body->accept(*this);

        exitRib(); // <- (lambda params)
    }

    void NameResolver::visit(ast::ListExpr * listExpr) {
        for (const auto & el : listExpr->elements) {
            el->accept(*this);
        }
    }

    void NameResolver::visit(ast::LiteralConstant * literalConstant) {}

    void NameResolver::visit(ast::LoopExpr * loopExpr) {
        visit(loopExpr->body.get());
    }

    void NameResolver::visit(ast::MemberAccess * memberAccess) {
        memberAccess->lhs->accept(*this);

        // TODO!!!: Resolve field
    }

    void NameResolver::visit(ast::ParenExpr * parenExpr) {
        parenExpr->expr->accept(*this);
    }

    void NameResolver::visit(ast::PathExpr * pathExpr) {
        // TODO!!!
    }

    void NameResolver::visit(ast::Prefix * prefix) {
        prefix->rhs->accept(*this);
    }

    void NameResolver::visit(ast::QuestExpr * questExpr) {
        questExpr->expr->accept(*this);
    }

    void NameResolver::visit(ast::ReturnExpr * returnExpr) {
        if (returnExpr->expr) {
            returnExpr->expr.unwrap()->accept(*this);
        }
    }

    void NameResolver::visit(ast::SpreadExpr * spreadExpr) {
        spreadExpr->expr->accept(*this);
    }

    void NameResolver::visit(ast::Subscript * subscript) {
        subscript->lhs->accept(*this);

        for (const auto & index : subscript->indices) {
            index->accept(*this);
        }
    }

    void NameResolver::visit(ast::ThisExpr * thisExpr) {}

    void NameResolver::visit(ast::TupleExpr * tupleExpr) {
        visitNamedList(tupleExpr->elements);
    }

    void NameResolver::visit(ast::UnitExpr * unitExpr) {
        // TODO MEOW?
    }

    void NameResolver::visit(ast::WhenExpr * whenExpr) {
        whenExpr->subject->accept(*this);

        for (const auto & entry : whenExpr->entries) {
            enterRib(); // -> (when entry)
            for (const auto & cond : entry->conditions) {
                cond->accept(*this);
            }
            entry->body->accept(*this);
            exitRib(); // <- (when entry)
        }
    }

    // Types //
    void NameResolver::visit(ast::ParenType * parenType) {
        parenType->type->accept(*this);
    }

    void NameResolver::visit(ast::TupleType * tupleType) {
        for (const auto & el : tupleType->elements) {
            if (el->name) {
                el->name.unwrap()->accept(*this);
            }
            if (el->type) {
                el->type.unwrap()->accept(*this);
            }
        }
    }

    void NameResolver::visit(ast::FuncType * funcType) {
        for (const auto & param : funcType->params) {
            param->accept(*this);
        }
        funcType->returnType->accept(*this);
    }

    void NameResolver::visit(ast::SliceType * listType) {
        listType->type->accept(*this);
    }

    void NameResolver::visit(ast::ArrayType * arrayType) {
        arrayType->type->accept(*this);
        arrayType->sizeExpr->accept(*this);
    }

    void NameResolver::visit(ast::TypePath * typePath) {
        // TODO: !!!
    }

    void NameResolver::visit(ast::UnitType * unitType) {
        // TODO: MEOW?
    }

    // Extended visitors //
    void NameResolver::visitItems(const ast::item_list & members) {
        enterRib(); // -> (members)

        // At first we need to forward all declarations.
        // This is the work for ItemResolver.
        for (const auto & member : members) {
            switch (member->stmt->kind) {
                case ast::StmtKind::Func: {

                } break;
            }
        }

        // Then we resolve the signatures and bodies
        // This is done here -- in NameResolver
        for (const auto & member : members) {
            member->accept(*this);
        }

        exitRib(); // <- (members)
    }

    void NameResolver::visitNamedList(const ast::named_list_ptr & namedList) {
        for (const auto & el : namedList->elements) {
            // Note: We don't visit element `name`, because it is immaterial on name-resolution stage
            if (el->value) {
                el->value.unwrap()->accept(*this);
            }
        }
    }

    // Ribs //
    void NameResolver::enterRib() {
        rib = std::make_shared<Rib>(rib);
    }

    void NameResolver::exitRib() {
        auto parent = rib->parent;
        if (!parent) {
            Logger::devPanic("NameResolver: Tried to exit top-level rib");
        }
        rib = parent.unwrap();
    }

    // Declarations //
    void NameResolver::declareItem(const std::string & name, Name::Kind kind, ast::node_id nodeId) {
        const auto & found = rib->names.find(name);
        if (found == rib->names.end()) {
            rib->names.emplace(name, std::make_shared<Name>(kind, nodeId));
            return;
        }

    }

    // Resolution //
    opt_node_id NameResolver::resolveId(const std::string & name) {
        dt::Option<rib_ptr> maybeRib = rib;
        while (maybeRib) {
            auto checkRib = maybeRib.unwrap();
            // TODO
            maybeRib = checkRib->parent;
        }
        return dt::None;
    }
}
