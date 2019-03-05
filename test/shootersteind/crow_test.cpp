#include "crow.h"
#include <thread>
#include "testing.h"

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

int main(int argc, char* argv[])
{
    T_ADD(test_should_run_server);
    
    return T_RUN(argc, argv);
}
