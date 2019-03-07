#include "shootersteind/client.h"
#include <iostream>

::shooterstein::Client::Client(::crow::websocket::connection* connection) {
    _worker = NULL;
    _connection = connection;
    _exit = false;
}

::shooterstein::Client::~Client() {
    stop();
}

::shooterstein::Client::Client(::shooterstein::Client const& x) {}
::shooterstein::Client::Client(::shooterstein::Client&& x) {}
::shooterstein::Client& ::shooterstein::Client::operator = (::shooterstein::Client const& x) {
    return *this;
}
::shooterstein::Client& ::shooterstein::Client::operator = (::shooterstein::Client&& x) {
    return *this;
}

void ::shooterstein::Client::send(::crow::json::wvalue const& value) {
    _tosend.enqueue_with_maxsize(::crow::json::dump(value), CLIENT_MAX_QUEUE_SIZE);
}

void ::shooterstein::Client::start() {
    std::lock_guard<std::mutex> guard(_mutex);

    if (_worker != NULL) {
        return;
    }

    _worker = new ::std::thread([this](){
        while (!_isclosed()) {
            auto item = _tosend.dequeue();
            ::std::cout << "[WS SENDING->](" << item << ")" << ::std::endl;
            _connection->send_text(item);
        }
    });
}

void ::shooterstein::Client::stop() {
    _mutex.lock();

    if (_worker == NULL) {
        _mutex.unlock();
        return;
    }

    auto worker = _worker;
    _worker = NULL;
    _mutex.unlock();

    worker->join();
    delete worker;
}

bool ::shooterstein::Client::_isclosed() {
    std::lock_guard<std::mutex> guard(_mutex);

    return _exit;
}
