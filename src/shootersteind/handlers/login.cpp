#include "handlers/login.h"
#include "shootersteind/config.h"
#include "crow.h"

namespace shooterstein { namespace handlers {

    crow::response login(crow::request const& req) {
        try {
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
        } catch(::std::exception const& err) {
            ::std::cerr << "[LOGIN HANDLER] " << err.what() << ::std::endl;
            return crow::json::wvalue();
        } catch(...) {
            ::std::cerr << "[LOGIN HANDLER] [N/A]" << ::std::endl;
            return crow::json::wvalue();
        }
    }

} }
