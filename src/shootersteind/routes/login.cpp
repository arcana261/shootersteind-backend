#include "routes/login.h"
#include "handlers/login.h"

namespace shooterstein { namespace routes {
    void register_login(crow::SimpleApp& app) {
        app.route_dynamic("/login")
            .methods(crow::HTTPMethod::POST)
        ([](const crow::request& req){
            return ::shooterstein::handlers::login(req);
        });
    }
} }
