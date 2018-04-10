#pragma once
#ifndef XL_UTILS_CODEC_JSON_H_

#include "json/json.hpp"

namespace xl {
namespace utils {

using json = nlohmann::json;
using Buffer = std::vector<uint8_t>;

static const int kLenSize = sizeof(uint16_t);

inline Buffer Encode(json &msg) {
    auto msgpack = json::to_msgpack(msg);
    auto len = static_cast<uint16_t>(msgpack.size());
    Buffer buf(len + sizeof(uint16_t), 0);
    std::memcpy(static_cast<void *>(buf.data()), static_cast<void *>(&len),
                sizeof(uint16_t));
    std::memcpy(static_cast<void *>(buf.data() + sizeof(uint16_t)),
                (const void *)(msgpack.data()), len);
    return buf;
}

inline size_t Decode(uint8_t *buf, size_t len, json &msg) {
    if (buf == nullptr || len < kLenSize) {
        return 0;
    }
    auto h = (uint16_t *)(buf);
    size_t total = kLenSize + *h;
    if (len < total) {
        return 0;
    }

    uint8_t *start = (uint8_t *)(buf + kLenSize);
    Buffer body(start, start + *h);
    msg = json::from_msgpack(body);
    return total;
}
}
}

#endif // XL_UTILS_CODEC_JSON_H_
