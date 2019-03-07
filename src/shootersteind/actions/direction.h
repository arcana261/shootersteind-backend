#ifndef _SHOOTERSTEIN_ACTIONS_DIRECTION_H__
#define _SHOOTERSTEIN_ACTIONS_DIRECTION_H__

#include "crow.h"
#include "context.h"

namespace shooterstein { namespace actions {

    crow::json::wvalue direction(::shooterstein::Context& ctx, crow::json::rvalue const& req);

}}


#endif
