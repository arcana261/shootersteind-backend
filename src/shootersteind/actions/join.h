#ifndef _SHOOTERSTEIN_ACTIONS_JOIN_H__
#define _SHOOTERSTEIN_ACTIONS_JOIN_H__

#include "crow.h"
#include "context.h"

namespace shooterstein { namespace actions {

    crow::json::wvalue join(::shooterstein::Context& ctx, crow::json::rvalue const& req);

}}

#endif // _SHOOTERSTEIN_ACTIONS_JOIN_H__
