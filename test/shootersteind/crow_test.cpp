#include <thread>
#include "testing.h"

#ifndef LEAK_SANITIZER
#ifndef ADDRESS_SANITIZER
#ifndef THREAD_SANITIZER
#include "crow.h"
void test_should_run_server() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([&app](){
        return "Hello world";
    });

    std::thread stopper([&app]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        app.stop();
    });

    std::thread runner([&app]() {
        app.port(18080).multithreaded().run();
    });

    runner.join();
    stopper.join();
}
#endif
#endif
#endif

int main(int argc, char* argv[])
{
#ifndef LEAK_SANITIZER
#ifndef ADDRESS_SANITIZER
#ifndef THREAD_SANITIZER
    T_ADD(test_should_run_server);
#endif
#endif
#endif
    
    return T_RUN(argc, argv);
}
