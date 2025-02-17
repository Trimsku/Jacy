#include "core/Interface.h"

// Strange track, but I like it: https://open.spotify.com/track/3dBKhZCi905UfyeodO8Epl?si=e36d5b4b2cad43d2

namespace jc::core {
    Interface::Interface() : config(Config::getInstance()) {}

    void Interface::compile() {
        eachStageBenchmarks = config.checkBenchmark(Config::Benchmark::EachStage);

        try {
            beginFinalBench();

            init();

            // Note: AstPrinter is a debug tool, so it allows to accept ill-formed AST
            //  thus we use it before suggestions check

            // AST Stage //
            parse();
            printDirTree();
            printAst(ast::AstPrinterMode::Parsing);
            checkSuggestions();
            lintAst();

            // Name resolution //
            resolveNames();
            printAst(ast::AstPrinterMode::Names);
            checkSuggestions();

            printBenchmarks();
            printFinalBench();
        } catch (std::exception & e) {
            log.dev("Something went wrong:", e.what());
            log.dev("Some debug info:");
            if (config.checkDev()) {
                dt::SuggResult<dt::none_t>::dump(sess, suggestions);
                printBenchmarks();
            }
        }
    }

    void Interface::init() {
        log.dev("Initialization...");
        sess = std::make_shared<sess::Session>();
    }

    // Parsing //
    void Interface::parse() {
        log.dev("Parsing...");

        const auto & rootFileName = config.getRootFile();
        const auto & rootFileEntry = fs::readfile(rootFileName);
        auto rootFile = std::move(parseFile(rootFileEntry));
        log.dev("Project directory:", rootFileEntry->getPath().parent_path());
        auto nestedModules = parseDir(
            fs::readDirRec(rootFileEntry->getPath().parent_path(), ".jc"),
            rootFileName
        );
        auto rootModule = std::make_unique<ast::RootModule>(std::move(rootFile), std::move(nestedModules));

        party = std::make_unique<ast::Party>(std::move(rootModule));
    }

    void Interface::lintAst() {
        log.dev("Linting...");

        linter.lint(*party.unwrap());
    }

    ast::dir_module_ptr Interface::parseDir(const fs::entry_ptr & dir, const std::string & ignore) {
        if (not dir->isDir()) {
            common::Logger::devPanic("Called `Interface::parseDir` on non-dir fs entry");
        }

        const auto & name = dir->getPath().filename().string();
        ast::module_list nestedModules;
        for (const auto & entry : dir->getSubModules()) {
            if (entry->isDir()) {
                nestedModules.emplace_back(parseDir(entry));
            } else if (not ignore.empty() and entry->getPath().filename() == ignore) {
                nestedModules.emplace_back(parseFile(entry));
            }
        }

        return std::make_unique<ast::DirModule>(name, std::move(nestedModules));
    }

    ast::file_module_ptr Interface::parseFile(const fs::entry_ptr & file) {
        const auto fileId = sess->sourceMap.addSource(file->getPath().string());
        sess->sourceMap.setSrc(fileId, std::move(file->extractContent()));
        const auto parseSess = std::make_shared<parser::ParseSess>(fileId);

        beginBench();
        auto lexerResult = lexer.lex(parseSess, file->extractContent());
        endBench(file->getPath().string(), BenchmarkKind::Lexing);

        log.dev("Tokenize file", file->getPath());
        auto fileTokens = std::move(std::move(lexerResult));

        printSource(fileId);
        printTokens(fileId, fileTokens);

        log.dev("Parse file", file->getPath());

        beginBench();
        auto [parsedFile, parserSuggestions] = parser.parse(sess, parseSess, fileTokens).extract();
        endBench(file->getPath().string(), BenchmarkKind::Parsing);

        collectSuggestions(std::move(parserSuggestions));

        return std::make_unique<ast::FileModule>(
            file->getPath().filename().string(),
            fileId,
            std::move(parsedFile)
        );
    }

    // Debug //
    void Interface::printDirTree() {
        if (not config.checkPrint(Config::PrintKind::DirTree)) {
            return;
        }
        log.info("Printing directory tree (`--print dir-tree`)");
        party.unwrap()->getRootModule()->accept(dirTreePrinter);
    }

    void Interface::printSource(span::file_id_t fileId) {
        if (not config.checkPrint(Config::PrintKind::Source)) {
            return;
        }
        const auto & source = sess->sourceMap.getSourceFile(fileId);
        log.info("Printing source for file", source.path, "by fileId", fileId, "(`--print source`)");

        const auto & sourceLines = source.src.unwrap("Interface::printSource");
        for (size_t i = 0; i < sourceLines.size(); i++) {
            log.raw(i + 1, "|", sourceLines.at(i));
        }
        log.nl();
    }

    void Interface::printTokens(span::file_id_t fileId, const parser::token_list & tokens) {
        if (not config.checkPrint(Config::PrintKind::Tokens)) {
            return;
        }
        const auto & filePath = sess->sourceMap.getSourceFile(fileId).path;
        common::Logger::nl();
        log.info("Printing tokens for file", filePath, "(`--print tokens`) [ Count of tokens:", tokens.size(), "]");
        for (const auto & token : tokens) {
            log.raw(token.dump(true)).nl();
        }
        common::Logger::nl();
    }

    void Interface::printAst(ast::AstPrinterMode mode) {
        if (not config.checkPrint(Config::PrintKind::Ast)) {
            return;
        }
        common::Logger::nl();
        std::string modeStr;
        if (mode == ast::AstPrinterMode::Parsing) {
            modeStr = "parsing";
        } else if (mode == ast::AstPrinterMode::Names) {
            modeStr = "name resolution";
        }
        log.info("Printing AST after", modeStr, "(`--print ast`)");
        astPrinter.print(*party.unwrap(), mode);
        common::Logger::nl();
    }

    // Name resolution //
    void Interface::resolveNames() {
        log.dev("Resolving names...");

        moduleTreeBuilder.build(sess, *party.unwrap()).unwrap(sess);
        modulePrinter.print(sess->modTreeRoot.unwrap());
        nameResolver.resolve(sess, *party.unwrap()).unwrap(sess);
    }

    // Suggestions //
    void Interface::collectSuggestions(sugg::sugg_list && additional) {
        suggestions = utils::arr::moveConcat(std::move(suggestions), std::move(additional));
    }

    void Interface::checkSuggestions() {
        if (suggestions.empty()) {
            return;
        }
        // Use `none_t` as stub
        dt::SuggResult<dt::none_t>::check(sess, suggestions);
        suggestions.clear();
    }

    // Benchmarks //
    void Interface::beginFinalBench() {
        finalBenchStart = bench();
    }

    void Interface::printFinalBench() {
        common::Logger::print(
            "Full compilation done in",
            std::chrono::duration<double, milli_ratio>(bench() - finalBenchStart).count(),
            "ms"
        );
    }

    void Interface::beginBench() {
        if (not eachStageBenchmarks) {
            return;
        }
        lastBench = bench();
    }

    void Interface::endBench(const std::string & name, BenchmarkKind kind) {
        if (not eachStageBenchmarks) {
            return;
        }
        if (not lastBench) {
            common::Logger::devPanic("Called `Interface::endBench` with None beginning bench");
        }
        std::string formatted = name + " ";
        switch (kind) {
            case BenchmarkKind::Lexing: {
                formatted += "lexing";
                break;
            }
            case BenchmarkKind::Parsing: {
                formatted += "parsing";
                break;
            }
        }
        auto end = bench();
        benchmarks.emplace(
            formatted,
            std::chrono::duration<double, milli_ratio>(end - lastBench.unwrap()).count()
        );
        lastBench = dt::None;
    }

    void Interface::printBenchmarks() noexcept {
        if (eachStageBenchmarks) {
            for (const auto & it : benchmarks) {
                common::Logger::print(it.first, "done in", it.second, "ms");
                common::Logger::nl();
            }
        }
    }
}
