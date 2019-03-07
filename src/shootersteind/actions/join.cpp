#include "actions/join.h"
#include <string>
#include <memory>
#include "engine.h"
#include "shootersteind/config.h"
#include "crow.h"
#include "loop.h"

#include <iostream>

namespace shooterstein { namespace actions {

    void _log(int i) {
        ::std::cout << "[HERE " << i << "]" << ::std::endl;
    }

    crow::json::wvalue join(::shooterstein::Context& ctx, crow::json::rvalue const& req) {
        ::std::string username = req["token"].s();
        ::std::shared_ptr<Player> player(new Player(username, Coordinate(0,0)));
        get_engine()->add_player(player);

        ctx.loop_id = ::shooterstein::get_loop()->register_client(ctx.client);
        ctx.player = player;

        ::crow::json::wvalue configs;
        configs["bullet_speed"] = 0.0;
        configs["bullet_ttl"] = 0.0;
        configs["map_height"] = get_engine()->size().y;
        configs["map_width"] = get_engine()->size().x;
        configs["player_speed"] = PLAYER_SPEED;
        configs["max_blink_range"] = 0.0;

        ::crow::json::wvalue payload;
        payload["configs"] = ::std::move(configs);

        ::crow::json::wvalue result;
        result["type"] = "config";
        result["payload"] = ::std::move(payload);

        return result;
    }

}}
