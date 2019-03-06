#include "routes/routes.h"

#include "routes/login.h"
#include "routes/game.h"

namespace shooterstein {
    void register_routes(crow::SimpleApp& app) {
        ::shooterstein::routes::register_login(app);
        ::shooterstein::routes::register_game(app);
    }
}
