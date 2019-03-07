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
                try {
                    ::std::cout << "[WS client connected]"  << ::std::endl;

                    ::std::shared_ptr<::shooterstein::Client> client(new ::shooterstein::Client(&conn));
                    client->start();
                    auto ctx = new ::shooterstein::Context;
                    ctx->client = client;
                    //ctx->loop_id = loop_id;
                    ctx->loop_id = -1;

                    conn.userdata(ctx);
                } catch(::std::exception const& err) {
                    ::std::cerr << "[WS NEW HANDLER] " << err.what() << ::std::endl;
                } catch(...) {
                    ::std::cerr << "[WS NEW HANDLER] [N/A]" << ::std::endl;
                }
            })
            .onclose([&](crow::websocket::connection& conn, const std::string& reason){
                try {
                    ::std::cout << "[WS client disconnected]"  << ::std::endl;

                    auto ctx = ((::shooterstein::Context*)conn.userdata());
                    //ctx->player->move(Coordinate(0,0));
                    get_engine()->remove_player(ctx->player);
                    ::std::cout << "[WS client disconnected (1)]"  << ::std::endl;
                    if (ctx->loop_id >= 0) {
                        ::shooterstein::get_loop()->unregister_client(ctx->loop_id);
                    }
                    ::std::cout << "[WS client disconnected (2)]"  << ::std::endl;
                    ctx->client->stop();
                    ::std::cout << "[WS client disconnected (3)]"  << ::std::endl;
                
                    delete ctx;
                } catch(::std::exception const& err) {
                    ::std::cerr << "[WS CLOSE HANDLER] " << err.what() << ::std::endl;
                } catch(...) {
                    ::std::cerr << "[WS CLOSE HANDLER] [N/A]" << ::std::endl;
                }
            })
            .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary){
                #ifdef TRACE
                ::std::cout << "[WS client message](" << data << ")" << ::std::endl;
                #endif

                auto request = crow::json::load(data);

                auto response = ::shooterstein::handlers::game(
                    *((::shooterstein::Context*)conn.userdata()),
                    request);

                auto response_text = crow::json::dump(response);

                #ifdef TRACE
                ::std::cout << "[WS response](" << response_text << ")" << ::std::endl;
                #endif

                conn.send_text(response_text);
            });
    }

} }
