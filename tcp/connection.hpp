#pragma once

#include <boost/asio.hpp>
#include <cinttypes>
#include <cstring>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <vector>

namespace us {
namespace tcp {

using Buffer = std::vector<uint8_t>;

template <class T> class Connection {
public:
    static const int kBufferMax = 65536;
    static const int kReadSize = 4096;
    struct SendTask {
        Buffer buf;
        size_t transfer;
        size_t total;
    };
    using Ptr = std::unique_ptr<Connection<T>>;
    using SendList = std::list<SendTask>;
    using Encoder = std::function<Buffer(T &)>;
    using Decoder = std::function<size_t(uint8_t *, size_t, T &)>;
    using Handler = std::function<void(T &)>;
    using OnDisconnect = std::function<void()>;

public:
    Connection(boost::asio::io_service &ios, Encoder enc, Decoder dec)
        : socket_(ios),
          encoder_(enc),
          decoder_(dec),
          buf_(kBufferMax, 0),
          start_pos_(0),
          end_pos_(0) {}

    void Start(Handler handler, OnDisconnect ondisconn) {
        handler_ = handler;
        ondisconn_ = ondisconn;
        connected_ = true;
        StartRead();
    }

    void Send(T &msg) {
        try {
            SendTask task;
            task.buf = encoder_(msg);
            task.transfer = 0;
            task.total = task.buf.size();
            std::lock_guard<std::mutex> lock(mut_);
            send_list_.push_back(task);
            if (send_list_.size() == 1) {
                StartWrite(task.buf.data(), task.total);
            }
        } catch (const std::exception &e) {
            // LOG_ERROR("encode msg exception: {}", e.what());
        }
    }

    void Close() {
        if (connected_) {
            try {
                socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            } catch (boost::system::system_error &ec) {
                // LOG_ERROR("socket shutdown failed! errno: {}, error: {}",
                //   ec.code().value(), ec.code().message());
            }
            try {
                socket_.close();
            } catch (boost::system::system_error &ec) {
                // LOG_ERROR("socket close failed! errno: {}, error: {}",
                //   ec.code().value(), ec.code().message());
            }
            connected_ = false;
            if (ondisconn_) {
                ondisconn_();
            }
        }
    }

    boost::asio::ip::tcp::socket &socket() { return socket_; }

private:
    void StartRead() {
        if (end_pos_ == kBufferMax) {
            if (start_pos_ == 0) {
                // LOG_ERROR("connection buffer overflow");
                Close();
                return;
            }
            // move
            auto len = end_pos_ - start_pos_;
            std::memmove(static_cast<void *>(buf_.data()),
                         static_cast<void *>(buf_.data() + start_pos_), len);
            start_pos_ = 0;
            end_pos_ = len;
        }
        auto cap = kBufferMax - end_pos_;
        auto read = cap < kReadSize ? cap : kReadSize;
        socket_.async_read_some(
            boost::asio::buffer(buf_.data() + end_pos_, read),
            [this](const boost::system::error_code &ec, size_t transferred) {
                HandleRead(ec, transferred);
            });
    }

    void StartWrite(uint8_t *data, size_t size) {
        boost::asio::async_write(
            socket_, boost::asio::buffer(data, size),
            [this](const boost::system::error_code &ec, size_t transferred) {
                HandleWrite(ec, transferred);
            });
    }

    void HandleWrite(const boost::system::error_code &ec, size_t transferred) {
        if (!ec) {
            try {
                std::lock_guard<std::mutex> lock(mut_);
                SendTask *task = &(send_list_.front());
                if (task == nullptr) {
                    return;
                }
                task->transfer += transferred;
                if (task->transfer == task->total) {
                    send_list_.pop_front();
                    if (send_list_.empty()) {
                        return;
                    }
                    task = &(send_list_.front());
                    if (task == nullptr) {
                        return;
                    }
                }

                if (send_list_.size() > 1) {
                    // 合并buf
                    auto it = send_list_.begin();
                    for (++it; it != send_list_.end();) {
                        auto &t = *it;
                        std::copy(t.buf.begin(), t.buf.end(),
                                  std::back_inserter(task->buf));
                        task->total += t.total;
                        it = send_list_.erase(it);
                    }
                }

                StartWrite(task->buf.data() + task->transfer,
                           task->total - task->transfer);
            } catch (const std::exception &e) {
                // LOG_ERROR("send msg exception: {}", e.what());
                return;
            }
        } else {
            // LOG_ERROR("connection write error: {}", ec.message());
            Close();
        }
    }

    void HandleRead(const boost::system::error_code &ec, size_t transferred) {
        if (!ec) {
            if (transferred > 0) {
                try {
                    end_pos_ += transferred;
                    T ret;
                    size_t offset = 0;
                    do {
                        offset = decoder_(buf_.data() + start_pos_,
                                          end_pos_ - start_pos_, ret);
                        if (offset != 0) {
                            start_pos_ += offset;
                            offset = end_pos_ - start_pos_;
                            if (start_pos_ > end_pos_) {
                                // LOG_ERROR(
                                // "connection read error: wrong format");
                                Close();
                                return;
                            }
                            handler_(ret);
                        }
                    } while (offset != 0);
                } catch (const std::exception &e) {
                    // LOG_ERROR("decode/handle msg exception: {}", e.what());
                    Close();
                    return;
                }
            }
            StartRead();
        } else {
            // LOG_ERROR("connection read error: {}", ec.message());
            Close();
        }
    }

private:
    boost::asio::ip::tcp::socket socket_;
    Encoder encoder_;
    Decoder decoder_;
    Handler handler_;
    OnDisconnect ondisconn_;
    bool connected_;

    // send
    SendList send_list_;

    // recv
    Buffer buf_;
    size_t start_pos_;
    size_t end_pos_;

    std::mutex mut_;
};
} // namespace tcp
} // namespace us
