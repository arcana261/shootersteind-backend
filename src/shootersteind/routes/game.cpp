#include "routes/game.h"
#include "handlers/game.h"
#include "context.h"
#include <memory>
#include "loop.h"
#include <iostream>

namespace shooterstein { namespace routes {

    void register_game(crow::SimpleApp& app) {
        app.route_dynamic("/game")
            .websocket()
            .onopen([&](crow::websocket::connection& conn){
                ::std::cout << "[WS client connected]"  << ::std::endl;

                ::std::shared_ptr<::shooterstein::Client> client(new ::shooterstein::Client(&conn));
                client->start();
                auto ctx = new ::shooterstein::Context;
                ctx->client = client;
                //ctx->loop_id = loop_id;
                ctx->loop_id = -1;

                conn.userdata(ctx);
            })
            .onclose([&](crow::websocket::connection& conn, const std::string& reason){
                ::std::cout << "[WS client disconnected]"  << ::std::endl;

                auto ctx = ((::shooterstein::Context*)conn.userdata());
                ctx->player->move(Coordinate(0,0));
                if (ctx->loop_id >= 0) {
                    ::shooterstein::get_loop()->unregister_client(ctx->loop_id);
                }
                ctx->client->stop();
                delete ctx;
            })
            .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary){
                ::std::cout << "[WS client message](" << data << ")" << ::std::endl;

                auto request = crow::json::load(data);

                auto response = ::shooterstein::handlers::game(
                    *((::shooterstein::Context*)conn.userdata()),
                    request);

                auto response_text = crow::json::dump(response);

                ::std::cout << "[WS response](" << response_text << ")" << ::std::endl;

                conn.send_text(response_text);
            });
    }

} }
