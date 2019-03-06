#include "routes/game.h"
#include "handlers/game.h"
#include "context.h"
#include <memory>
#include "loop.h"

namespace shooterstein { namespace routes {

    void register_game(crow::SimpleApp& app) {
        app.route_dynamic("/game")
            .websocket()
            .onopen([&](crow::websocket::connection& conn){
                ::std::shared_ptr<::shooterstein::Client> client(new ::shooterstein::Client(&conn));
                client->start();
                int loop_id = ::shooterstein::get_loop()->register_client(client);
                auto ctx = new ::shooterstein::Context;
                ctx->client = client;
                ctx->loop_id = loop_id;

                conn.userdata(ctx);
            })
            .onclose([&](crow::websocket::connection& conn, const std::string& reason){
                auto ctx = ((::shooterstein::Context*)conn.userdata());
                ::shooterstein::get_loop()->unregister_client(ctx->loop_id);
                ctx->client->stop();
                delete ctx;
            })
            .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary){
                auto request = crow::json::load(data);

                auto response = ::shooterstein::handlers::game(
                    *((::shooterstein::Context*)conn.userdata()),
                    request);

                conn.send_text(crow::json::dump(response));
            });
    }

} }
