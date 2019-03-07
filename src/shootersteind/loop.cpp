#include "shootersteind/loop.h"
#include <functional>
#include "shootersteind/engine.h"

namespace shooterstein { namespace internal {
    Loop *_main_loop;
} }

::shooterstein::Loop::Loop(::std::chrono::milliseconds every) {
    _every = every;
    _nextid = 0;
    _worker = NULL;
}

::shooterstein::Loop::~Loop() {
    stop();
}

::shooterstein::Loop::Loop(::shooterstein::Loop const& x) {}
::shooterstein::Loop::Loop(::shooterstein::Loop&& x) {}

::shooterstein::Loop& ::shooterstein::Loop::operator = (::shooterstein::Loop const& x) {
    return *this;
}

::shooterstein::Loop& ::shooterstein::Loop::operator = (::shooterstein::Loop&& x) {
    return *this;
}

int ::shooterstein::Loop::register_client(::std::shared_ptr<::shooterstein::Client> client) {
    std::lock_guard<std::mutex> guard(_mutex);

    int id = _nextid++;
    _clients[id] = client;

    return id;
}

void ::shooterstein::Loop::unregister_client(int id) {
    std::lock_guard<std::mutex> guard(_mutex);

    _clients.erase(id);
}

void ::shooterstein::Loop::start() {
    std::lock_guard<std::mutex> guard(_mutex);

    if (_worker != NULL) {
        return;
    }

    _worker = new ::std::thread(
        ::std::bind(&::shooterstein::Loop::_thread_loop, this));
}

void ::shooterstein::Loop::stop() {
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

void ::shooterstein::Loop::_thread_loop() {
    while (!_isclosed()) {
        ::std::this_thread::sleep_for(_every);

        auto data = _next();
        auto clients = _get_clients();

        for (auto it = clients.begin(); it != clients.end(); ++it) {
            (*it)->send(data);
        }
    }
}

bool ::shooterstein::Loop::_isclosed() {
    std::lock_guard<std::mutex> guard(_mutex);
    return _exit;
}

::std::vector<::std::shared_ptr<::shooterstein::Client> > shooterstein::Loop::_get_clients() {
    std::lock_guard<std::mutex> guard(_mutex);

    ::std::vector<::std::shared_ptr<::shooterstein::Client> > result;

    for (auto it = _clients.begin(); it != _clients.end(); ++it) {
        result.push_back(it->second);
    }

    return result;
}

::crow::json::wvalue shooterstein::Loop::_next() {
    return ::shooterstein::get_engine()->map();
}

::shooterstein::Loop* ::shooterstein::get_loop() {
    return ::shooterstein::internal::_main_loop;
}

namespace shooterstein { namespace internal {
    void initialize_loop(::std::chrono::milliseconds every) {
        ::shooterstein::internal::_main_loop = new ::shooterstein::Loop(every);
        ::shooterstein::internal::_main_loop->start();
    }
}}
