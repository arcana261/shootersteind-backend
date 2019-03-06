#ifndef _SHOOTERSTEIN_CLIENT_H__
#define _SHOOTERSTEIN_CLIENT_H__

#include "shootersteind/queue.h"
#include "crow.h"
#include <thread>
#include <string>
#include <mutex>

#define CLIENT_MAX_QUEUE_SIZE 10

namespace shooterstein {

    class Client {
        ::shooterstein::Queue<::std::string> _tosend;
        ::std::thread *_worker;
        ::crow::websocket::connection* _connection;
        volatile bool _exit;
        ::std::mutex _mutex;

        Client(Client const& x);
        Client(Client&& x);
        Client& operator = (Client const& x);
        Client& operator = (Client&& x);

    public:
        Client(::crow::websocket::connection* connection);
        ~Client();

        void send(::crow::json::wvalue const& value);

        void start();
        void stop();

    private:
        bool _isclosed();
    };
}

#endif // _SHOOTERSTEIN_CLIENT_H__
