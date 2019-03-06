#ifndef _SHOOTERSTEIN_LOOP_H__
#define _SHOOTERSTEIN_LOOP_H__

#include <chrono>
#include <map>
#include <mutex>
#include <thread>
#include <memory>
#include "shootersteind/client.h"
#include "crow.h"

namespace shooterstein {
    class Loop {
        ::std::chrono::milliseconds _every;
        volatile int _nextid;
        ::std::mutex _mutex;
        ::std::map<int, ::std::shared_ptr<::shooterstein::Client> > _clients;
        ::std::thread *_worker;
        volatile bool _exit;

        Loop(::shooterstein::Loop const& x);
        Loop(::shooterstein::Loop&& x);
        ::shooterstein::Loop& operator = (::shooterstein::Loop const& x);
        ::shooterstein::Loop& operator = (::shooterstein::Loop&& x);

    public:
        Loop(::std::chrono::milliseconds every);
        ~Loop();

        int register_client(::std::shared_ptr<::shooterstein::Client> client);
        void unregister_client(int id);

        void start();
        void stop();

    private:
        void _thread_loop();
        bool _isclosed();
        ::std::vector<::std::shared_ptr<::shooterstein::Client> > _get_clients();
        ::crow::json::wvalue _next();
    };

    Loop* get_loop();

    namespace internal {
        void initialize_loop(::std::chrono::milliseconds every);
    }
}

#endif // _SHOOTERSTEIN_LOOP_H__
