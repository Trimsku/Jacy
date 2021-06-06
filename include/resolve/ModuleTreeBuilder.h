#ifndef JACY_RESOLVE_MODULETREEBUILDER_H
#define JACY_RESOLVE_MODULETREEBUILDER_H

#include "ast/StubVisitor.h"

namespace jc::resolve {
    struct Module;
    using ast::node_id;
    using ns_map = std::map<std::string, node_id>;

    enum class Namespace {
        Value,
        Type,
    };

    /// Module
    /// Represents any that can
    struct Module {
        Module(Module * parent) : parent(parent) {}

        dt::Option<Module*> parent;
        std::map<std::string, Module*> children;
        ns_map valueNS;
        ns_map typeNS;

        ns_map & getNS(Namespace ns) {
            switch (ns) {
                case Namespace::Value: return valueNS;
                case Namespace::Type: return typeNS;
            }
        }
    };

    class ModuleTreeBuilder : public ast::StubVisitor {
    public:
        ModuleTreeBuilder() : StubVisitor("ScopeTreeBuilder") {}

        void visit(const ast::FileModule & fileModule) override;
        void visit(const ast::DirModule & dirModule) override;
        void visit(const ast::Mod & mod) override;
        void visit(const ast::Trait & trait) override;

//        void visit(const ast::Struct & _struct) override;



    private:
        common::Logger log{"ScopeTreeBuilder"};

        // Modules //
    private:
        Module * mod;
        void declare(Namespace ns, const std::string & name, node_id nodeId);

        void enterMod(const dt::Option<std::string> & name = dt::None);
        void exitMod();
    };
}

#endif // JACY_RESOLVE_MODULETREEBUILDER_H
