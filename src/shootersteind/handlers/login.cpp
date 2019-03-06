#include "handlers/login.h"
#include "crow.h"

namespace shooterstein { namespace handlers {

    crow::response login(crow::request const& req) {
        auto x = crow::json::load(req.body);
        if (!x) {
            return crow::response(400);
        }

        auto username = x["username"].s();

        crow::json::wvalue result;
        result["token"] = "abcd";
        result["websocket"] = "http://127.0.0.1:8888/game";

        auto response = crow::response(result);

        response.add_header("Access-Control-Allow-Origin", "*");

        return response;
    }

} }
