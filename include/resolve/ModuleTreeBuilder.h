#ifndef JACY_RESOLVE_MODULETREEBUILDER_H
#define JACY_RESOLVE_MODULETREEBUILDER_H

#include "ast/StubVisitor.h"
#include "session/Session.h"
#include "suggest/SuggInterface.h"

#include "data_types/Option.h"
#include "data_types/SuggResult.h"

namespace jc::resolve {
    class ModuleTreeBuilder : public ast::StubVisitor, public sugg::SuggInterface {
    public:
        ModuleTreeBuilder() : StubVisitor("ScopeTreeBuilder") {}

        dt::SuggResult<dt::none_t> build(sess::sess_ptr sess, const ast::Party & party);

        void visit(const ast::RootModule & rootModule) override;
        void visit(const ast::FileModule & fileModule) override;
        void visit(const ast::DirModule & dirModule) override;

        void visit(const ast::Func & func) override;
        void visit(const ast::Mod & mod) override;
        void visit(const ast::Struct & _struct) override;
        void visit(const ast::Trait & trait) override;
        void visit(const ast::TypeAlias & typeAlias) override;

//        void visit(const ast::Struct & _struct) override;

    private:
        using ast::StubVisitor::visit;

    private:
        common::Logger log{"ScopeTreeBuilder"};

        // Modules //
    private:
        mod_node_ptr mod;
        void declare(Namespace ns, const ast::id_ptr & ident, node_id nodeId);

        void enterMod(const std::string & name, const dt::Option<span::Span> & nameSpan);
        void exitMod();
    };
}

#endif // JACY_RESOLVE_MODULETREEBUILDER_H
