#ifndef JACY_RESOLVE_MODULETREEBUILDER_H
#define JACY_RESOLVE_MODULETREEBUILDER_H

#include "ast/StubVisitor.h"

namespace jc::resolve {
    struct ModNode;
    using ast::node_id;

    struct ModNode {
        ModNode(const std::string & name, ModNode * parent) : name(name), parent(parent) {}

        std::string name;
        dt::Option<ModNode*> parent;
        std::vector<ModNode*> children;
    };

    struct ModulePrinter {
        ModulePrinter();

        void print(ModNode * module);

    private:
        common::Logger log{"ModulePrinter"};

        void printIndent();
        uint32_t indent{0};
    };

    class ModuleTreeBuilder : public ast::StubVisitor {
    public:
        ModuleTreeBuilder() : StubVisitor("ScopeTreeBuilder") {}

        ModNode * build(const ast::Party & party);

        void visit(const ast::RootModule & rootModule) override;
        void visit(const ast::FileModule & fileModule) override;
        void visit(const ast::DirModule & dirModule) override;
        void visit(const ast::Mod & mod) override;
        void visit(const ast::Trait & trait) override;

//        void visit(const ast::Struct & _struct) override;

    private:
        common::Logger log{"ScopeTreeBuilder"};

        // Modules //
    private:
        ModNode * mod;

        void enterMod(const std::string & name);
        void exitMod();
    };
}

#endif // JACY_RESOLVE_MODULETREEBUILDER_H
