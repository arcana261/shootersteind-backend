#include "handlers/login.h"
#include "shootersteind/config.h"
#include "crow.h"

namespace shooterstein { namespace handlers {

    crow::response login(crow::request const& req) {
        auto x = crow::json::load(req.body);
        if (!x) {
            return crow::response(400);
        }

        auto username = x["username"].s();

        crow::json::wvalue result;
        result["token"] = username;
        result["websocket"] = ::shooterstein::get_websocket_address();

        auto response = crow::response(result);

        response.add_header("Access-Control-Allow-Origin", "*");

        return response;
    }

} }
