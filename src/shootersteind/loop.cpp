#include "shootersteind/loop.h"

::shooterstein::Loop::Loop(::std::chrono::milliseconds every) {
    _every = every;
    _nextid = 0;
    _worker = NULL;
}

int ::shooterstein::Loop::register_client(::shooterstein::Client* client) {
    std::lock_guard<std::mutex> guard(_mutex);

    int id = _nextid++;
    _clients[id] = client;

    return id;
}

void ::shooterstein::Loop::unregister_client(int id) {
    std::lock_guard<std::mutex> guard(_mutex);

    _clients.erase(id);
}
