#pragma once
#include <cinttypes>
#include "dep/json/json.hpp"
#include "dep/promise/async.h"

namespace us {

using json = nlohmann::json;
using Pistache::Async::Promise;
using Handler = std::function<void()>;

struct RemoteStorate {
};

struct Framework {
    virtual ~Framework() {}
    virtual void Connect(std::string type, std::string addr,
                         std::string pw = "", std::string user = "") = 0;
    virtual void Listen(std::string type, int port) = 0;
    virtual void Register(std::string path, std::string method,
                          Handler func) = 0;
};


}  // namespace us