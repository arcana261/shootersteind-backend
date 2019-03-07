#include "handlers/game.h"
#include "actions/join.h"
#include "actions/move.h"
#include "actions/direction.h"
#include <string>

namespace shooterstein { namespace handlers {

    crow::json::wvalue game(::shooterstein::Context& ctx, crow::json::rvalue const& req) {
        ::std::string action = req["action"].s();

        if (action == "join") {
            return actions::join(ctx, req["payload"]);
        } else if (action == "move") {
            return actions::move(ctx, req["payload"]);
        } else if (action == "direction") {
            return actions::direction(ctx, req["payload"]);
        } 

        return crow::json::wvalue();
    }

} }
