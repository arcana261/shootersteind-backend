#include "actions/move.h"
#include "engine.h"
#include "context.h"
#include "crow.h"
#include "shootersteind/config.h"
#include <cmath>

namespace shooterstein { namespace actions {

    crow::json::wvalue shoot(::shooterstein::Context& ctx, crow::json::rvalue const& req) {
        auto direction = req["direction"];
        double x = direction["x"].d();
        double y = direction["y"].d();

        crow::json::wvalue payload;

        if (ctx.player->ammo_count > 0) {
            ::std::shared_ptr<Bullet> bullet(new Bullet());
            bullet->reference_location.x = ctx.player->location.x;
            bullet->reference_location.y = ctx.player->location.y;
            bullet->direction.x = x;
            bullet->direction.y = y;
            bullet->fuel = BULLET_FUEL;
            bullet->location.x = ctx.player->location.x;
            bullet->location.y = ctx.player->location.y;

            get_engine()->add_bullet(bullet);

            payload["shoot"] = "done";
        } else {
            payload["shoot"] = "failed";
        }

        crow::json::wvalue result;
        result["type"] = "confirm";
        result["payload"] = std::move(payload);

        return result;
    }

}}

