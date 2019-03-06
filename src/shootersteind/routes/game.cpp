#include "routes/game.h"
#include "handlers/game.h"

namespace shooterstein { namespace routes {

    void register_game(crow::SimpleApp& app) {
        app.route_dynamic("/game")
            .websocket()
            .onopen([&](crow::websocket::connection& conn){
                conn.userdata(new crow::json::wvalue());
            })
            .onclose([&](crow::websocket::connection& conn, const std::string& reason){
                delete ((crow::json::wvalue*)conn.userdata());
            })
            .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary){
                auto request = crow::json::load(data);

                auto response = ::shooterstein::handlers::game(
                    *((crow::json::wvalue*)conn.userdata()),
                    request);

                conn.send_text(crow::json::dump(response));
            });
    }

} }
