#ifndef JACY_ASTPRINTER_H
#define JACY_ASTPRINTER_H

#include <cstdint>
#include <iostream>

#include "ast/BaseVisitor.h"
#include "ast/nodes.h"
#include "common/Logger.h"

namespace jc::ast {
    class AstPrinter : public BaseVisitor {
    public:
        AstPrinter() = default;

        void print(const ast::stmt_list & tree);

        // Statements //
        void visit(Assignment * assignment) override;
        void visit(ClassDecl * classDecl) override;
        void visit(EnumDecl * enumDecl) override;
        void visit(ExprStmt * exprStmt) override;
        void visit(ForStmt * forStmt) override;
        void visit(FuncDecl * funcDecl) override;
        void visit(ObjectDecl * objectDecl) override;
        void visit(TypeAlias * typeAlias) override;
        void visit(VarDecl * varDecl) override;
        void visit(WhileStmt * whileStmt) override;

        // Expressions //
        void visit(BreakExpr * breakExpr) override;
        void visit(ContinueExpr * continueExpr) override;
        void visit(Identifier * identifier) override;
        void visit(IfExpr * ifExpr) override;
        void visit(Infix * infix) override;
        void visit(Invoke * invoke) override;
        void visit(ListExpr * listExpr) override;
        void visit(LiteralConstant * literalConstant) override;
        void visit(LoopExpr * loopExpr) override;
        void visit(ParenExpr * parenExpr) override;
        void visit(Postfix * postfix) override;
        void visit(Prefix * prefix) override;
        void visit(ReturnExpr * returnExpr) override;
        void visit(SpreadExpr * spreadExpr) override;
        void visit(Subscript * subscript) override;
        void visit(SuperExpr * superExpr) override;
        void visit(ThisExpr * thisExpr) override;
        void visit(ThrowExpr * throwExpr) override;
        void visit(TryCatchExpr * tryCatchExpr) override;
        void visit(TupleExpr * tupleExpr) override;
        void visit(UnitExpr * unitExpr) override;
        void visit(WhenExpr * whenExpr) override;

    private:
        common::Logger log{"ast_printer", {}};

        void printIndent() const;
        void print(const ast::attr_list & attributes);
        void printModifiers(const parser::token_list & modifiers);
        void print(const ast::type_param_list & typeParams);
        void print(const ast::delegation_list & delegations);
        void print(const ast::block_ptr & block);
        void print(ast::ArgList * namedList);
        void print(ast::Type * type);

        const std::string indentChar = "  ";
        void incIndent();
        void decIndent();
        uint64_t indent{0};
    };
}

#endif // JACY_ASTPRINTER_H
