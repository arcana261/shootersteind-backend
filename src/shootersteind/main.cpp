#include <iostream>
#include "crow.h"
#include "routes/routes.h"

int main()  {
    crow::SimpleApp app;

    shooterstein::register_routes(app);

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
