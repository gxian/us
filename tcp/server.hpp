#pragma once

#include "connection.hpp"
#include <boost/bind.hpp>
#include <deque>

namespace us {
namespace tcp {

template <class T> class Server {
public:
    using Handler = std::function<void(int, T)>;
    using Encoder = typename Connection<T>::Encoder;
    using Decoder = typename Connection<T>::Decoder;
    using OnDisconnect = std::function<void(int)>;
    using ConnPtr = typename Connection<T>::Ptr;
    using ConnMap = std::map<int, ConnPtr>;
    using IDSet = std::deque<int>;

public:
    Server(boost::asio::io_service &ios, int port)
        : acceptor_(ios, boost::asio::ip::tcp::endpoint(
                             boost::asio::ip::tcp::v4(), port)),
          gen_(0) {}
    void Start(Encoder enc, Decoder dec, Handler handler,
               OnDisconnect ondisconn = nullptr) {
        encoder_ = enc;
        decoder_ = dec;
        handler_ = handler;
        ondisconn_ = ondisconn;
        acceptor_.set_option(
            boost::asio::ip::tcp::acceptor::reuse_address(true));
        StartAccept();
    }
    void Stop() { acceptor_.close(); }
    void Send(int id, T &msg) {
        auto it = connections_.find(id);
        if (it != connections_.end()) {
            auto &p = it->second;
            if (p) {
                p->Send(msg);
            }
        }
    }

private:
    void StartAccept() {
        accepting_ = std::move(ConnPtr(new Connection<T>(
            acceptor_.get_io_service(), encoder_, decoder_)));
        acceptor_.async_accept(accepting_->socket(),
                               boost::bind(&Server::HandleAccept, this,
                                           boost::asio::placeholders::error));
    }
    void HandleAccept(const boost::system::error_code &error) {
        if (!error) {
            accepting_->socket().set_option(
                boost::asio::socket_base::linger(true, 0));
            auto id = GenerateID();
            connections_[id] = std::move(accepting_);
            connections_[id]->Start([this, id](T &msg) { handler_(id, msg); },
                                    [this, id]() { HandleDisconnect(id); });

            StartAccept();
        } else {
        }
    }

    void HandleDisconnect(int id) {
        // TODO：tcp connection删除自己，如果有write事件需要先write结束
        auto it = connections_.find(id);
        if (it != connections_.end()) {
            connections_.erase(it);
            RecycleID(id);
            if (ondisconn_) {
                ondisconn_(id);
            }
        }
    }

    int GenerateID() {
        if (ids_.empty()) {
            return ++gen_;
        }
        auto ret = ids_.front();
        ids_.pop_front();
        return ret;
    }

    void RecycleID(int id) { ids_.push_back(id); }

private:
    boost::asio::ip::tcp::acceptor acceptor_;
    Handler handler_;
    Encoder encoder_;
    Decoder decoder_;
    OnDisconnect ondisconn_;
    ConnPtr accepting_;
    ConnMap connections_;
    IDSet ids_;
    int gen_;
};
}
}
