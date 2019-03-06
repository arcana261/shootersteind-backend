#ifndef _SHOOTERSTEIN_LOOP_H__
#define _SHOOTERSTEIN_LOOP_H__

#include <chrono>
#include <map>
#include <mutex>
#include <thread>
#include "shootersteind/client.h"
#include "crow.h"

namespace shooterstein {
    class Loop {
        ::std::chrono::milliseconds _every;
        volatile int _nextid;
        ::std::mutex _mutex;
        ::std::map<int, ::shooterstein::Client*> _clients;
        ::std::thread *_worker;

        Loop(::shooterstein::Loop const& x);
        Loop(::shooterstein::Loop&& x);
        ::shooterstein::Loop& operator = (::shooterstein::Loop const& x);
        ::shooterstein::Loop& operator = (::shooterstein::Loop&& x);

    public:
        Loop(::std::chrono::milliseconds every);
        ~Loop();

        int register_client(::shooterstein::Client* client);
        void unregister_client(int id);

        void start();
        void stop();

    private:
        void _thread_loop();

    };
}

#endif // _SHOOTERSTEIN_LOOP_H__
