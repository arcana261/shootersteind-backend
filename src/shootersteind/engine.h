#ifndef _SHOOTERSTEIN_ENGINE_H__
#define _SHOOTERSTEIN_ENGINE_H__

#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <set>
#include <memory>
#include <vector>

#include "crow.h"
#include "shootersteind/config.h"

namespace shooterstein {
    class Engine;

    struct Coordinate {
        double x;
        double y;

        Coordinate();
        Coordinate(double x, double y);

        void clamp(double max_x, double max_y);
        ::crow::json::wvalue payload();
    };

    struct Direction {
        double x;
        double y;
    };

    class InMapPointObject {
        public: Coordinate location;
        public: Coordinate reference_location;
        public: Coordinate velocity;
        public: double collision;
        public: Engine *engine;

    public:
        InMapPointObject();

        virtual ::crow::json::wvalue payload() = 0;
    };

    class Bullet: public InMapPointObject {
        public: Direction direction;
        public: double fuel; // how much distance can travel

    public:
        Bullet(double x = 0, double y = 0, double initial_fuel = 0, double direction_x = 0, double direction_y = 0);
    };

    struct Wall {
    public:
        Coordinate location_start;
        Coordinate location_end;
        double collision;

    public:
        Wall();
    };

    struct Item {
        Coordinate location;
        bool is_used;
        int type; // 0 -> Ammo, 1 -> Health Package
        int value;
        double collision;
    };

    class Player : public InMapPointObject{
    public: 
        ::std::string name;
        int health;
        int ammo_count;
        bool has_blink;
        double direction;
        ::std::chrono::system_clock::time_point last_update;

        Player(::std::string const& name, Coordinate location);

        void move(Coordinate velocity);
        void update();
        void add_hp(int value);
        void add_ammo(int value);
        void shoot_ammo();
        bool has_ammo();
        ::crow::json::wvalue payload();
    };

    namespace internal {
        struct locationBasedPlayerComparisonStruct {
            bool operator() (const ::std::shared_ptr<Player>& lhs, const ::std::shared_ptr<Player>& rhs) const {
                Coordinate c1 = lhs->location;
                Coordinate c2 = rhs->location;
                if (abs(c1.x - c2.x) < EPS) {
                    return c1.y < c2.y;
                }
                return c1.x < c2.x;
            }
        };
    }

    class Engine {
        ::std::thread *_worker;
        ::std::mutex _mutex;
        volatile bool _exit;
        std::set<::std::shared_ptr<Player>, internal::locationBasedPlayerComparisonStruct> _players;
        Coordinate _size;
        ::std::chrono::milliseconds _every;

        Engine(Engine const& x);
        Engine(Engine&& x);
        Engine& operator = (Engine const& x);
        Engine& operator = (Engine&& x);

    public:
        Engine(::std::chrono::milliseconds every, Coordinate size);
        ~Engine();

        Coordinate size();
        void start();
        void stop();

        void add_player(::std::shared_ptr<Player> player);

        ::crow::json::wvalue map();

    private:
        void _thread_loop();
        void _tick();
        bool _isclosed();

        ::std::vector<::std::shared_ptr<Player> > _get_players();
    };

    Engine* get_engine();

    namespace internal {
        void initialize_engine(::std::chrono::milliseconds every, Coordinate size);
    }
}

#endif // _SHOOTERSTEIN_ENGINE_H__
