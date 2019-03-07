#ifndef _SHOOTERSTEIN_CONFIG_H__
#define _SHOOTERSTEIN_CONFIG_H__

#include <string>

#define MAP_WIDTH 100000
#define MAP_HEIGHT 100000
#define PLAYER_SPEED 320
#define BULLET_SPEED 640
#define BULLET_DAMAGE 30
#define BULLET_FUEL 5000
#define EPS 1e-6
#define TRACE
//#define PLAYER_MAX_MOVE_MS 500

namespace shooterstein {
    ::std::string const& get_websocket_address();

    namespace internal {
        void set_websocket_address(::std::string const& address);
    }
}

#endif // _SHOOTERSTEIN_CONFIG_H__
