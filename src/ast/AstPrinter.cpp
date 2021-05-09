#include "ast/AstPrinter.h"

namespace jc::ast {
    void AstPrinter::print(const ast::stmt_list & tree) {
        for (const auto & stmt : tree) {
            stmt->accept(*this);
            log.nl();
        }
    }

    void AstPrinter::visit(Assignment * assignment) {
        assignment->lhs->accept(*this);
        log.raw(" = ");
        assignment->rhs->accept(*this);
    }

    void AstPrinter::visit(ClassDecl * classDecl) {
        print(classDecl->attributes);
        printModifiers(classDecl->modifiers);
        log.raw(" class ");
        classDecl->id->accept(*this);
        print(classDecl->typeParams);
        print(classDecl->delegations);
        print(classDecl->body);
    }

    void AstPrinter::visit(EnumDecl * enumDecl) {
        log.raw("enum ");
        enumDecl->id->accept(*this);

        for (const auto & enumEntry : enumDecl->entries) {
            enumEntry->id->accept(*this);
            if (enumEntry->value) {
                log.raw(" = ");
                enumEntry->value->accept(*this);
            }
            log.raw(",").nl();
        }

        print(enumDecl->body);
    }

    void AstPrinter::visit(ExprStmt * exprStmt) {
        exprStmt->expr->accept(*this);
    }

    void AstPrinter::visit(ForStmt * forStmt) {
        log.raw("for ");
        forStmt->forEntity->accept(*this);
        log.raw(" in ");
        forStmt->inExpr->accept(*this);;
        print(forStmt->body);
    }

    void AstPrinter::visit(FuncDecl * funcDecl) {
        print(funcDecl->attributes);
        printModifiers(funcDecl->modifiers);
        log.raw("func ");
        print(funcDecl->typeParams);
        funcDecl->id->accept(*this);

        log.raw("(");
        for (size_t i = 0; i < funcDecl->params.size(); ++i) {
            const auto & param = funcDecl->params.at(i);
            param->id->accept(*this);

            if (param->type) {
                log.raw(": ");
                print(param->type.get());
            }
            if (param->defaultValue) {
                log.raw(" = ");
                param->defaultValue->accept(*this);
            }
            if (i < funcDecl->params.size() - 1) {
                log.raw(", ");
            }
        }
        log.raw(")");

        if (funcDecl->returnType) {
            log.raw(": ");
            print(funcDecl->returnType.get());
        }

        if (funcDecl->oneLineBody) {
            log.raw(" => ");
            funcDecl->oneLineBody->accept(*this);
        } else {
            print(funcDecl->body);
        }
    }

    void AstPrinter::visit(ObjectDecl * objectDecl) {
        print(objectDecl->attributes);
        printModifiers(objectDecl->modifiers);
        log.raw("object ");
        if (objectDecl->id) {
            objectDecl->id->accept(*this);
        }
        print(objectDecl->delegations);
        print(objectDecl->body);
    }

    void AstPrinter::visit(TypeAlias * typeAlias) {
        log.raw("type ");
        typeAlias->id->accept(*this);
        log.raw(" = ");
        print(typeAlias->type.get());
    }

    void AstPrinter::visit(VarDecl * varDecl) {
        log.raw(varDecl->kind.typeToString());
        varDecl->id->accept(*this);
        if (varDecl->type) {
            log.raw(": ");
            print(varDecl->type.get());
        }
    }

    void AstPrinter::visit(WhileStmt * whileStmt) {
        log.raw("while ");
        whileStmt->condition->accept(*this);
        log.raw(" ");
        print(whileStmt->body);
    }

    void AstPrinter::visit(BreakExpr * breakExpr) {
        log.raw("break");
    }

    void AstPrinter::visit(ContinueExpr * continueExpr) {
        log.raw("continue");
    }

    void AstPrinter::visit(Identifier * identifier) {
        if (identifier->token.type == parser::TokenType::Id) {
            log.raw(identifier->token.val);
        } else {
            // TODO: Soft keywords
        }
    }

    void AstPrinter::visit(IfExpr * ifExpr) {
        log.raw("if ");
        ifExpr->condition->accept(*this);
        log.raw(" ");
        if (ifExpr->ifBranch) {
            print(ifExpr->ifBranch);
        }
        if (ifExpr->elseBranch) {
            print(ifExpr->elseBranch);
        }
    }

    void AstPrinter::visit(Infix * infix) {
        infix->lhs->accept(*this);
        if (infix->token.type == parser::TokenType::Id) {
            log.raw(infix->token.val);
        } else {
            log.raw(infix->token.typeToString());
        }
        infix->rhs->accept(*this);
    }

    void AstPrinter::visit(Invoke * invoke) {
        invoke->lhs->accept(*this);
        log.raw("(");
        print(invoke->args.get());
        log.raw(")");
    }

    void AstPrinter::visit(ListExpr * listExpr) {
        log.raw("[");
        for (const auto & el : listExpr->elements) {
            el->accept(*this);
        }
        log.raw("]");
    }

    void AstPrinter::visit(LiteralConstant * literalConstant) {
        log.raw(literalConstant->token.val);
    }

    void AstPrinter::visit(LoopExpr * loopExpr) {
        log.raw("loop ");
        print(loopExpr->body);
    }

    void AstPrinter::visit(ParenExpr * parenExpr) {
        log.raw("(");
        parenExpr->expr->accept(*this);
        log.raw(")");
    }

    void AstPrinter::visit(Postfix * postfix) {
        postfix->lhs->accept(*this);
        log.raw(postfix->token.typeToString());
    }

    void AstPrinter::visit(Prefix * prefix) {
        log.raw(prefix->token.typeToString());
        prefix->rhs->accept(*this);
    }

    void AstPrinter::visit(ReturnExpr * returnExpr) {
        log.raw("return ");
        returnExpr->expr->accept(*this);
    }

    void AstPrinter::visit(SpreadExpr * spreadExpr) {
        log.raw(spreadExpr->token.typeToString());
        spreadExpr->expr->accept(*this);
    }

    void AstPrinter::visit(Subscript * subscript) {
        subscript->lhs->accept(*this);
        for (size_t i = 0; i < subscript->indices.size(); ++i) {
            subscript->indices.at(i)->accept(*this);
            if (i < subscript->indices.size() - 1) {
                log.raw(", ");
            }
        }
    }

    void AstPrinter::visit(SuperExpr * superExpr) {
        log.raw("super");
    }

    void AstPrinter::visit(ThisExpr * thisExpr) {
        log.raw("this");
    }

    void AstPrinter::visit(ThrowExpr * throwExpr) {
        log.raw("throw ");
        throwExpr->expr->accept(*this);
    }

    void AstPrinter::visit(TryCatchExpr * tryCatchExpr) {

    }

    void AstPrinter::visit(TupleExpr * tupleExpr) {
        log.raw("(");
        print(tupleExpr->elements.get());
        log.raw(")");
    }

    void AstPrinter::visit(UnitExpr * unitExpr) {
        log.raw("()");
    }

    void AstPrinter::visit(WhenExpr * whenExpr) {
        log.raw("when ");
        whenExpr->subject->accept(*this);

        log.raw("{");
        incIndent();

        for (const auto & entry : whenExpr->entries) {
            for (size_t i = 0; i < entry->conditions.size(); ++i) {
                const auto & cond = entry->conditions.at(i);
                cond->accept(*this);
                if (i < entry->conditions.size() - 1) {
                    log.raw(", ");
                }
            }
            log.raw(" => ");

        }

        decIndent();
        log.raw("}");
    }

    void AstPrinter::printIndent() const {
        for (int i = 0; i < indent; i++) {
            std::cout << indentChar;
        }
    }

    void AstPrinter::print(const ast::attr_list & attributes) {
        for (const auto & attr : attributes) {
            log.raw("@");
            attr->id->accept(*this);
            log.raw("(");
            print(attr->params.get());
            log.raw(")").nl();
        }
    }

    void AstPrinter::printModifiers(const parser::token_list & modifiers) {
        for (const auto & mod : modifiers) {
            log.raw(mod.typeToString());
        }
    }

    void AstPrinter::print(const ast::type_param_list & typeParams) {
        if (typeParams.empty()) {
            return;
        }

        log.raw("<");
        for (const auto & typeParam : typeParams) {
            typeParam->id->accept(*this);
            if (typeParam->type) {
                log.raw(": ");
                print(typeParam->type.get());
            }
        }
        log.raw(">");
    }

    void AstPrinter::print(const ast::delegation_list & delegations) {
        if (delegations.empty()) {
            return;
        }

        log.raw(" : ");
        for (const auto & del : delegations) {
            del->id->accept(*this);
        }
    }

    void AstPrinter::print(const ast::block_ptr & block) {
        log.raw("{").nl();
        incIndent();
        print(block->stmts);
        decIndent();
        log.raw("}");
    }

    void AstPrinter::print(ast::NamedList * namedList) {
        for (const auto & namedEl : namedList->elements) {
            if (namedEl->id) {
                namedEl->id->accept(*this);
                if (namedEl->value) {
                    log.raw(" = ");
                }
            }
            if (namedEl->value) {
                namedEl->value->accept(*this);
            }
        }
    }

    void AstPrinter::print(ast::Type * type) {

    }

    void AstPrinter::incIndent() {
        ++indent;
    }

    void AstPrinter::decIndent() {
        --indent;
    }
}
