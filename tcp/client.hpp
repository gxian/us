#pragma once

#include "connection.hpp"

namespace us {
namespace tcp {

template <class T> class Client {
public:
    using Handler = typename Connection<T>::Handler;
    using Encoder = typename Connection<T>::Encoder;
    using Decoder = typename Connection<T>::Decoder;
    using ConnPtr = typename Connection<T>::Ptr;
    using OnDisconnect = typename Connection<T>::OnDisconnect;

public:
    Client(boost::asio::io_service &ios, Encoder enc, Decoder dec)
        : ios_(ios), encoder_(enc), decoder_(dec) {}

    std::string Connect(std::string host, int port, Handler handler,
                        OnDisconnect ondisconn) {
        boost::asio::ip::tcp::resolver resolver(ios_);
        boost::asio::ip::tcp::resolver::query query(host, std::to_string(port));
        boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
        boost::asio::ip::tcp::resolver::iterator end;
        if (iter != end) {
            boost::system::error_code ec;
            conn_ = std::move(
                ConnPtr(new Connection<T>(ios_, encoder_, decoder_)));
            conn_->socket().connect((*iter).endpoint(), ec);
            if (ec) {
                return ec.message();
            }
            conn_->Start(handler, ondisconn);
        }
        return "";
    }

    void Disconnect() {
        if (conn_) {
            conn_->Close();
        }
    }

    void Send(T &msg) {
        if (conn_) {
            conn_->Send(msg);
        }
    }

private:
    boost::asio::io_service &ios_;
    Encoder encoder_;
    Decoder decoder_;
    ConnPtr conn_;
    int port_;
};
}
}
