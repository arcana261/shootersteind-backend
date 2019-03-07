#include "actions/direction.h"
#include "crow.h"
#include "context.h"

namespace shooterstein { namespace actions {

    crow::json::wvalue direction(::shooterstein::Context& ctx, crow::json::rvalue const& req) {
        ctx.player->direction = req["direction"].d();

        crow::json::wvalue payload;
        payload["move"] = "done";

        crow::json::wvalue result;
        result["type"] = "confirm";
        result["payload"] = std::move(payload);

        return result;
    }

}}
