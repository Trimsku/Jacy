#ifndef JACY_PARSER_PARSESESS_H
#define JACY_PARSER_PARSESESS_H

#include "session/Session.h"

namespace jc::parser {
    struct ParseSess;
    using parse_sess_ptr = std::shared_ptr<ParseSess>;

    struct ParseSess {
        explicit ParseSess(sess::file_id_t fileId) : fileId(fileId) {}

        sess::file_id_t fileId;
    };
}

#endif // JACY_PARSER_PARSESESS_H
