#include "config.h"

namespace shooterstein {
    namespace internal {
        ::std::string websocket_address;
    }

    ::std::string const& get_websocket_address() {
        if (internal::websocket_address == "") {
            internal::websocket_address = "http://127.0.0.1:8080/game";
        }

        return internal::websocket_address;
    }

    namespace internal {
        void set_websocket_address(::std::string const& address) {
            internal::websocket_address = address;
        }
    }
}
