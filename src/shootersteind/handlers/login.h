#ifndef _SHOOTERSTEIN_HANDLERS_LOGIN_H__
#define _SHOOTERSTEIN_HANDLERS_LOGIN_H__

#include "crow.h"

namespace shooterstein { namespace handlers {

    crow::response login(crow::request const& req);

} }

#endif
