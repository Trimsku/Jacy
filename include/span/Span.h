#ifndef JACY_SPAN_H
#define JACY_SPAN_H

#include <cstdint>
#include <iostream>
#include <algorithm>

#include "common/Logger.h"

namespace jc::span {
    using span_pos_t = uint32_t;
    using span_len_t = uint16_t;
    using file_id_t = size_t;

    struct Span {
        Span() = default;
        explicit Span(span_pos_t lowBound, span_pos_t highBound, file_id_t fileId) {
            pos = lowBound;
            len = static_cast<span_len_t>(highBound - lowBound);
            this->fileId = fileId;
        }

        explicit Span(span_pos_t pos, span_len_t len, file_id_t fileId)
            : pos(pos), len(len), fileId(fileId) {}

        span_pos_t pos{0};
        span_len_t len{0};
        file_id_t fileId{0}; // TODO: Context

        std::string toString() const {
            return std::to_string(pos) + "; len = " + std::to_string(len);
        }

        span_pos_t getHighBound() const {
            return pos + len;
        }

        Span to(const Span & end) const {
            if (end.fileId != fileId) {
                common::Logger::devPanic("Called `Span::to` with spans from different files");
            }
            // FIXME: Here may be problems with different lines
            // FIXME: This does not work
            return Span(std::min(pos, end.pos), std::max(getHighBound(), end.getHighBound()), fileId);
        }
    };
}

#endif // JACY_SPAN_H
