#ifndef _SHOOTERSTEIN_CONTEXT_H__
#define _SHOOTERSTEIN_CONTEXT_H__

#include "crow.h"
#include "client.h"
#include <memory>

namespace shooterstein {

    struct Context {
        ::std::shared_ptr<::shooterstein::Client> client;
        int loop_id;
    };

}

#endif
