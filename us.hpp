#pragma once
#include <cinttypes>
#include "dep/json/json.hpp"
#include "dep/promise/async.h"

namespace us {

using json = nlohmann::json;

// rpc服务定义
class Service {};

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
// 其他连接方式通过sidecar处理
class Server {
public:
    void AddService(Service* svc);
    void Serve();
};

}