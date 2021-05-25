#include "core/Interface.h"

namespace jc::core {
    Interface::Interface() = default;

    void Interface::compile() {
        init();


    }

    void Interface::init() {
        sess = std::make_shared<sess::Session>();
    }

    void Interface::scanSources() {
        // Hard-coded single file
        // FIXME
        const auto & rootFile = common::Config::getInstance().getRootFile();
    }

    void Interface::buildSourceMap() {
        for (const auto & path : filesToCompile) {
            auto fileId = sess->sourceMap.addSource(path);

            std::fstream file(path);

            if (!file.is_open()) {
                throw common::FileNotFound(path);
            }

            std::stringstream ss;
            ss << file.rdbuf();
            std::string data = ss.str();
            file.close();

            auto lexerResult = std::move(lexer.lex(data));
            filesTokenStreams.emplace_back(std::move(lexerResult.tokens));
            sess->sourceMap.setSourceLines(fileId, std::move(lexerResult.sourceLines));
        }
    }
}
