#include "actions/move.h"
#include "engine.h"
#include "context.h"
#include "crow.h"
#include "shootersteind/config.h"
#include <cmath>

namespace shooterstein { namespace actions {

    crow::json::wvalue move(::shooterstein::Context& ctx, crow::json::rvalue const& req) {
        auto velocity = req["velocity"];
        double x = velocity["x"].d();
        double y = velocity["y"].d();

        crow::json::wvalue payload;

        if ((::std::abs(x) < EPS && ::std::abs(y) < EPS) || ::std::abs((x*x + y*y) - 1.0) < EPS) {
            ctx.player->move(Coordinate(x, y));
            payload["move"] = "done";
        } else {
            payload["move"] = "failed";
        }

        crow::json::wvalue result;
        result["type"] = "confirm";
        result["payload"] = std::move(payload);

        return result;
    }

}}

