#ifndef JACY_BASEVISITOR_H
#define JACY_BASEVISITOR_H

namespace jc::ast {
    // Statements //
    struct Assignment;
    struct ClassDecl;
    struct EnumDecl;
    struct ExprStmt;
    struct ForStmt;
    struct FuncDecl;
    struct ObjectDecl;
    struct TypeAlias;
    struct VarDecl;
    struct WhileStmt;

    // Expressions //
    struct BreakExpr;
    struct ContinueExpr;
    struct Identifier;
    struct IfExpr;
    struct Infix;
    struct Invoke;
    struct ListExpr;
    struct LiteralConstant;
    struct LoopExpr;
    struct ParenExpr;
    struct Postfix;
    struct Prefix;
    struct ReturnExpr;
    struct SpreadExpr;
    struct Subscript;
    struct SuperExpr;
    struct ThisExpr;
    struct ThrowExpr;
    struct TryCatchExpr;
    struct TupleExpr;
    struct UnitExpr;
    struct WhenExpr;

    class BaseVisitor {
    public:
        virtual ~BaseVisitor() = default;

        // Statements //
        virtual void visit(Assignment*) = 0;
        virtual void visit(ClassDecl*) = 0;
        virtual void visit(EnumDecl*) = 0;
        virtual void visit(ExprStmt*) = 0;
        virtual void visit(ForStmt*) = 0;
        virtual void visit(FuncDecl*) = 0;
        virtual void visit(ObjectDecl*) = 0;
        virtual void visit(TypeAlias*) = 0;
        virtual void visit(VarDecl*) = 0;
        virtual void visit(WhileStmt*) = 0;

        // Expressions //
        virtual void visit(BreakExpr*) = 0;
        virtual void visit(ContinueExpr*) = 0;
        virtual void visit(Identifier*) = 0;
        virtual void visit(IfExpr*) = 0;
        virtual void visit(Infix*) = 0;
        virtual void visit(Invoke*) = 0;
        virtual void visit(ListExpr*) = 0;
        virtual void visit(LiteralConstant*) = 0;
        virtual void visit(LoopExpr*) = 0;
        virtual void visit(ParenExpr*) = 0;
        virtual void visit(Postfix*) = 0;
        virtual void visit(Prefix*) = 0;
        virtual void visit(ReturnExpr*) = 0;
        virtual void visit(SpreadExpr*) = 0;
        virtual void visit(Subscript*) = 0;
        virtual void visit(SuperExpr*) = 0;
        virtual void visit(ThisExpr*) = 0;
        virtual void visit(ThrowExpr*) = 0;
        virtual void visit(TryCatchExpr*) = 0;
        virtual void visit(TupleExpr*) = 0;
        virtual void visit(UnitExpr*) = 0;
        virtual void visit(WhenExpr*) = 0;
    };
}

#endif // JACY_BASEVISITOR_H
