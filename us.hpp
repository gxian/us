#pragma once
#include "dep/json/json.hpp"
#include "dep/pistache-promise/async.h"
#include <cinttypes>

namespace us {

using json = nlohmann::json;

// rpc客户端
template <class Request, class Response> class Client {
public:
    using Promise = Pistache::Async::Promise<Response>;

    // promised Connect();
    Promise Call(Request req);
};

// rpc服务端
class Server {
public:

};

// rpc通信通道定义
class Channel {};

// rpc服务定义
class Service {};
}; // namespace us
