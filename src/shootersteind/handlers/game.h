#ifndef _SHOOTERSTEIN_HANDLERS_GAME_H__
#define _SHOOTERSTEIN_HANDLERS_GAME_H__

#include "crow.h"
#include "context.h"

namespace shooterstein { namespace handlers {

    crow::json::wvalue game(::shooterstein::Context& ctx, crow::json::rvalue const& req);

} }

#endif // _SHOOTERSTEIN_HANDLERS_GAME_H__
