#include <iostream>
#include "crow.h"
#include "routes/routes.h"
#include "shootersteind/loop.h"
#include "shootersteind/config.h"
#include "shootersteind/engine.h"
#include <cstring>

int main(int argc, char* argv[])  {
    crow::SimpleApp app;

    for (int i = 1; i + 1 < argc; i++) {
        if (!::std::strcmp(argv[i], "--websocket-address")) {
            ::shooterstein::internal::set_websocket_address(::std::string(argv[i+1]));
            i++;
        }
    }

    shooterstein::register_routes(app);
    shooterstein::internal::initialize_loop(::std::chrono::milliseconds(100));
    shooterstein::internal::initialize_engine(
        ::std::chrono::milliseconds(100),
        shooterstein::Coordinate(MAP_WIDTH, MAP_HEIGHT));

    /*
    {
        "action": string,
        "payload": object
    }

    action = move|join|shoot|direction| .... get_map

    {
        "type": "update",
        "payload": [
            {
                "type": "player | bullet",
                "hp": [integer 0-100], //player only
                "position": {
                    "x": number,
                    "y": number
                },
                "velocity": {
                    "x": number,
                    "y": number
                },
                "direction": { //player only
                    "x": number,
                    "y": number
                },
                "name": string //player only
            }
        ]
    }
    */

    app.port(8080).multithreaded().run();

    return 0;
}
