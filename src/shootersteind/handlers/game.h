#ifndef _SHOOTERSTEIN_HANDLERS_GAME_H__
#define _SHOOTERSTEIN_HANDLERS_GAME_H__

#include "crow.h"

namespace shooterstein { namespace handlers {

    crow::json::wvalue game(crow::json::wvalue& context, crow::json::rvalue const& req);

} }

#endif // _SHOOTERSTEIN_HANDLERS_GAME_H__
