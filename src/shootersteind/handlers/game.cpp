#include "handlers/game.h"
#include "actions/join.h"
#include "actions/move.h"
#include "actions/shoot.h"
#include "actions/direction.h"
#include <string>
#include <stdexcept>
#include <iostream>

namespace shooterstein { namespace handlers {

    crow::json::wvalue game(::shooterstein::Context& ctx, crow::json::rvalue const& req) {
        try {
            ::std::string action = req["action"].s();

            if (action == "join") {
                return actions::join(ctx, req["payload"]);
            } else if (action == "move") {
                return actions::move(ctx, req["payload"]);
            } else if (action == "direction") {
                return actions::direction(ctx, req["payload"]);
            } else if (action == "shoot") {
                return actions::shoot(ctx, req["payload"]);
            } 

            return crow::json::wvalue();
        } catch(::std::exception const& err) {
            ::std::cerr << "[GAME HANDLER] " << err.what() << ::std::endl;
            return crow::json::wvalue();
        } catch(...) {
            ::std::cerr << "[GAME HANDLER] [N/A]" << ::std::endl;
            return crow::json::wvalue();
        }
    }

} }
