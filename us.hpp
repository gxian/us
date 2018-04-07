#pragma once
#include <cinttypes>
#include "dep/json/json.hpp"
#include "dep/pistache-promise/async.h"

namespace us {

using json = nlohmann::json;

// rpc客户端
// 实现客户端侧的负载均衡(非对等节点的路由方式需要考虑)
class Client {
public:
    template <class T>
    using Promise = Pistache::Async::Promise<T>;
    // promised Connect();
    template <class Request, class Response>
    Promise<Response> Call(Request req) {
        json r = req;
        return Promise<Response>();
    }
};

// rpc服务端
// 兼容restful接口
// 其他连接方式通过sidecar处理
class Server {
public:
};

// rpc通信通道定义
class Channel {};

// rpc服务定义
class Service {};
};  // namespace us
