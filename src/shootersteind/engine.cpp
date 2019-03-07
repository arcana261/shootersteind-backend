#include "engine.h"
#include "config.h"

#include <vector>
#include <map>
#include <cstring>
#include <string>
#include <set>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <cmath>
#include <cstdlib>

#define PLAYER_COLLISION 10
#define BULLET_COLLISION 2
#define WALL_COLLISION 5

namespace shooterstein {
    namespace internal {
        Engine* engine;
    }

    //const double bullet_collision = 0.1;
    //const double player_collision = 0.5;
    const double wall_collision = 0.3;

    namespace internal {
        double clamp(double value, double max_value) {
            if (value < EPS) {
                return 0.0;
            }

            if (value > max_value - EPS) {
                return max_value;
            }

            return value;
        }
    }

    Coordinate::Coordinate() {
        x = 0;
        y = 0;
    }

    Coordinate::Coordinate(double x, double y) {
        this->x = x;
        this->y = y;
    }

    void Coordinate::clamp(double max_x, double max_y) {
        x = internal::clamp(x, max_x);
        y = internal::clamp(y, max_y);
    }

    ::crow::json::wvalue Coordinate::payload() {
        ::crow::json::wvalue result;
        result["x"] = x;
        result["y"] = y;

        return result;
    }

    InMapPointObject::InMapPointObject() {
        location.x = 0;
        location.y = 0;
        collision = 0.0;
        engine = NULL;
    }

    Bullet::Bullet() {
        location.x = 0;
        location.y = 0;
        direction.x = 0;
        direction.y = 0;
        collision = BULLET_COLLISION;
        fuel = 0;
        engine = NULL;
        last_update = ::std::chrono::system_clock::now();
    }

    bool Bullet::update() {
        auto now = ::std::chrono::system_clock::now();
        auto dt_ms = ::std::chrono::duration_cast<::std::chrono::milliseconds>(now - last_update).count();
        double dt = ((double)dt_ms) / 1000.0;
        double dx = velocity.x * BULLET_SPEED * dt;
        double dy = velocity.y * BULLET_SPEED * dt;
        double traveled = ::std::sqrt(dx*dx + dy*dy);
        bool result = true;
        if (traveled > fuel) {
            result = false;

            dt = fuel / ::std::sqrt(velocity.x*velocity.x*BULLET_SPEED*BULLET_SPEED+velocity.y*velocity.y*BULLET_SPEED*BULLET_SPEED);
            dx = velocity.x * BULLET_SPEED * dt;
            dy = velocity.y * BULLET_SPEED * dt;
        }

        location.x = reference_location.x + dx;
        location.y = reference_location.y + dy;
        location.clamp(engine->size().x, engine->size().y);

        return result;
    }

    ::crow::json::wvalue Bullet::payload() {
        ::crow::json::wvalue result;
        result["type"] = "bullet";
        result["id"] = id;
        result["position"] = location.payload();
        result["direction"] = direction.payload();
        result["speed"] = BULLET_SPEED;

        return result;
    }

    Wall::Wall() {
        location_start.x = 0.0;
        location_start.y = 0.0;
        location_end.x = 0.0;
        location_end.y = 0.0;
        collision = WALL_COLLISION;
    }

    Player::Player(::std::string const& name, Coordinate location) {
        this->location = location;
        reference_location = location;
        this->name = name;
        velocity.x = 0;
        velocity.y = 0;
        health = 100;
        ammo_count = 5;
        has_blink = true;
        direction = 0;
        last_update = ::std::chrono::system_clock::now();
        collision = PLAYER_COLLISION;
        engine = NULL;
    }

    void Player::move(Coordinate velocity) {
        update();
        this->velocity = velocity;
        last_update = ::std::chrono::system_clock::now();
        reference_location = location;
    }

    void Player::update() {
        auto now = ::std::chrono::system_clock::now();
        auto dt_ms = ::std::chrono::duration_cast<::std::chrono::milliseconds>(now - last_update).count();
        double dt = ((double)dt_ms) / 1000.0;
        location.x = reference_location.x + (velocity.x * PLAYER_SPEED * dt);
        location.y = reference_location.y + (velocity.y * PLAYER_SPEED * dt);
        location.clamp(engine->size().x, engine->size().y);
    }

    void Player::add_hp(int value) {
        health += value;

        if (health > 100) {
            health = 100;
        }
    }

    void Player::add_ammo(int value) {
        ammo_count += value;
    }

    void Player::shoot_ammo() {
        ammo_count--;
    }

    bool Player::has_ammo() {
        return ammo_count > 0;
    }

    ::crow::json::wvalue Player::payload() {
        ::crow::json::wvalue result;
        result["type"] = "player";
        result["id"] = id;
        result["hp"] = health;
        result["position"] = location.payload();
        result["velocity"] = velocity.payload();
        result["direction"] = direction;
        result["name"] = name;
        result["ammo"] = ammo_count;

        return result;
    }

    bool player_bullet_collision(::std::shared_ptr<Player> const& o1, ::std::shared_ptr<Bullet> const& o2) {
        double delta_x = o1->location.x - o2->location.x;
        double delta_y = o1->location.y - o2->location.y;
        
        double dist = ::std::sqrt(delta_x * delta_x + delta_y * delta_y);
        if (o1->collision + o2->collision > dist) {
            return true;
        }
        return false;
    }

    bool point_line_collision(InMapPointObject const& o1, Wall const& w1) {
        int wall_type = 0; // Horizontal
        if (abs(w1.location_start.x - w1.location_end.x) < EPS) {
            wall_type = 1; // Vertical
        }
        double distance;
        if (wall_type == 1) {
            double value = o1.location.y;
            if ((value < w1.location_start.y) != (value < w1.location_end.y)) // in the middle
            {
                distance = abs(w1.location_start.x - o1.location.x); 
            }
            else {
                double delta_x_1 = w1.location_start.x - o1.location.x;
                double delta_x_2 = w1.location_end.x - o1.location.x;

                double delta_y_1 = w1.location_start.y - o1.location.y;
                double delta_y_2 = w1.location_end.y - o1.location.y;

                distance = std::min(::std::sqrt(delta_x_1 * delta_x_1 + delta_y_1 * delta_y_1), ::std::sqrt(delta_x_2 * delta_x_2 + delta_y_2 * delta_y_2));
            }
        }
        else {
            double value = o1.location.x;
            if ((value < w1.location_start.x) != (value < w1.location_end.x)) // in the middle
            {
                distance = abs(w1.location_start.y - o1.location.y); 
            }
            else {
                double delta_x_1 = w1.location_start.x - o1.location.x;
                double delta_x_2 = w1.location_end.x - o1.location.x;

                double delta_y_1 = w1.location_start.y - o1.location.y;
                double delta_y_2 = w1.location_end.y - o1.location.y;

                distance = std::min(::std::sqrt(delta_x_1 * delta_x_1 + delta_y_1 * delta_y_1), ::std::sqrt(delta_x_2 * delta_x_2 + delta_y_2 * delta_y_2));
            }
        }
        if (distance < (w1.collision + o1.collision)) {
            return true;
        }
        return false;
    }

    Engine::Engine(Engine const& x) {}
    Engine::Engine(Engine&& x) {}
    Engine& Engine::operator = (Engine const& x) {
        return *this;
    }
    Engine& Engine::operator = (Engine&& x) {
        return *this;
    }

    Engine::Engine(::std::chrono::milliseconds every, Coordinate size) {
        _size = size;
        _worker = NULL;
        _exit = false;
        _every = every;
    }

    Engine::~Engine() {
        stop();
    }

    Coordinate Engine::size() {
        return _size;
    }

    void Engine::start() {
        std::lock_guard<std::mutex> guard(_mutex);

        if (_worker != NULL) {
            return;
        }

        _worker = new ::std::thread(
            ::std::bind(&::shooterstein::Engine::_thread_loop, this));
    }

    void Engine::stop() {
        _mutex.lock();

        if (_worker == NULL) {
            _mutex.unlock();
            return;
        }

        auto worker = _worker;
        _worker = NULL;
        _mutex.unlock();

        worker->join();
        delete worker;
    }

    void Engine::_thread_loop() {
        while (!_isclosed()) {
            ::std::this_thread::sleep_for(_every);

            _tick();
        }
    }

    void Engine::_tick() {
        _mutex.lock();
        auto players = _get_players();
        auto bullets = _get_bullets();
        _mutex.unlock();

        for (auto player : players) {
            player->update();
        }

        ::std::vector<::std::shared_ptr<Bullet> > bullets_to_remove;
        for (auto bullet : bullets) {
            if (!bullet->update()) {
                bullets_to_remove.push_back(bullet);
                continue;
            }

            bool hit = false;

            // check player collision
            for (auto player: players) {
                if (player_bullet_collision(player, bullet)) {
                    player->health -= BULLET_DAMAGE;
                    hit = true;
                    break;
                }
            }
            if (hit) {
                bullets_to_remove.push_back(bullet);
                continue;
            }
        }

        if (!bullets_to_remove.empty()) {
            _mutex.lock();
            for (auto bullet : bullets_to_remove) {
                _bullets.erase(bullet);
            }
            _mutex.unlock();
        }
    }

    ::std::vector<::std::shared_ptr<Player> > Engine::_get_players() {
        ::std::vector<::std::shared_ptr<Player> > result;

        for (auto player : _players) {
            result.push_back(player);
        }

        return result;
    }

    void Engine::remove_player(::std::shared_ptr<Player> player) {
        std::lock_guard<std::mutex> guard(_mutex);
        _players.erase(player);
    }

    ::std::vector<::std::shared_ptr<Bullet> > Engine::_get_bullets() {
        ::std::vector<::std::shared_ptr<Bullet> > result;

        for (auto bullet : _bullets) {
            result.push_back(bullet);
        }

        return result;
    }

    bool Engine::_isclosed() {
        std::lock_guard<std::mutex> guard(_mutex);
        return _exit;
    }
    
    void Engine::add_player(::std::shared_ptr<Player> player) {
        std::lock_guard<std::mutex> guard(_mutex);
        player->engine = this;
        player->id = _nextid++;
        _players.insert(player);
    }

    void Engine::add_bullet(::std::shared_ptr<Bullet> bullet) {
        std::lock_guard<std::mutex> guard(_mutex);
        bullet->engine = this;
        bullet->id = _nextid++;
        _bullets.insert(bullet);
    }

    ::crow::json::wvalue Engine::map() {
        _mutex.lock();
        auto players = _get_players();
        auto bullets = _get_bullets();
        _mutex.unlock();

        ::crow::json::wvalue result;

        result["type"] = "update";
        ::crow::json::wvalue& payload = result["payload"];
        for (int i = 0; i < (int)players.size(); i++) {
            payload[i] = players[i]->payload();
        }
        int offset = (int)players.size();
        for (int i = 0; i < (int)bullets.size(); i++) {
            payload[offset+i] = bullets[i]->payload();
        }

        return result;
    }

    Engine* get_engine() {
        return internal::engine;
    }

    namespace internal {
        void initialize_engine(::std::chrono::milliseconds every, Coordinate size) {
            auto engine = new Engine(every, size);
            engine->start();
            internal::engine = engine;
        }
    }
}

//const double bullet_speed = 10;
//const int bullet_damage = 30;


/*


class Engine {
    public: int ticksPerSecond;
    protected: int gameTick;
    protected: std::set<Player, locationBasedComparisonStruct> players;
    protected: std::set<Bullet, locationBasedComparisonStruct> bullets;
    protected: std::vector<Wall> walls;
    protected: std::set<Item, locationBasedComparisonStruct> items;

    public:
        Engine(int tps, double w, double h) {
            ticksPerSecond = tps;
            gameTick = 0;
            width = w;
            height = h;
        }
        void add_bullet(double x, double y, double fuel, double d1, double d2)
        {
            // std::cout << "ADDING BULLET \n";
            bullets.insert(Bullet(x, y, fuel + eps, d1, d2));
        }
        void add_player(double x, double y, int pid, int hp, int ammo)
        {
            players.insert(Player(x, y, pid, hp, ammo));
        }
        void add_wall(double x1, double y1, double x2, double y2)
        {
            walls.push_back(Wall(x1, y1, x2, y2));
        }
        void tick() {
            gameTick++;
            std::set<Bullet, locationBasedComparisonStruct> new_bullets;
            for (auto bullet: bullets) {
                Coordinate c = bullet.location;
                Direction d = bullet.direction;
                double len = ::std::sqrt(d.x * d.x + d.y * d.y);
                double d1 = d.x / len;
                double d2 = d.y / len;
                d1 *= bullet_speed / ticksPerSecond;
                d2 *= bullet_speed / ticksPerSecond;
                bullet.fuel -= bullet_speed / ticksPerSecond;
                bullet.location.x += d1;
                bullet.location.y += d2;
                
                if (bullet.fuel < 0)
                    continue;

                bool hit = false;

                // check player collision
                for (auto player: players) {
                    if (point_collision(bullet, player)) {
                        players.erase(player);
                        player.health -= bullet_damage;
                        players.insert(player);
                        hit = true;
                        break;
                    }
                }
                if (hit)
                    continue;
                for (auto wall: walls) {
                    if (point_line_collision(bullet, wall)) {
                        hit = true;
                        break;
                    }
                }
                if (hit)
                    continue;

                new_bullets.insert(bullet);
            }
            bullets = new_bullets;
        }
        void printAll() {
            std::cout << bullets.size() << " bullets ----------------\n";
            for (auto bullet: bullets) {
                Coordinate c = bullet.location;
                std::cout << c.x << ' ' << c.y << '\n';
            }

            std::cout << players.size() << " players ----------------\n";
            for (auto player: players) {
                Coordinate c = player.location;
                std::cout << c.x << ' ' << c.y << ' ' << player.health <<  '\n';
            }

            std::cout << walls.size() << " walls ----------------\n";
            for (auto wall: walls) {
                Coordinate c1 = wall.location_start;
                Coordinate c2 = wall.location_end;
                
                std::cout << c1.x << ' ' << c1.y << ' ' << c2.x << ' ' << c2.y <<  '\n';
            }
            std::cout << "end ----------------\n";
            
        }
};

Engine engine = Engine(1000, 1000, 1000);
bool running = false;

void run_engine_tick() {
    engine.tick();
}

void schedule(std::function<void(void)> func, unsigned int interval) {

    std::thread([func, interval]() {
        std::cout << "Scheduled for " << interval << '\n';
        while (true)
        {
            if (running) {
                auto start = std::chrono::high_resolution_clock::now();
                func();
                auto elapsed = std::chrono::high_resolution_clock::now() - start;
                long long elapsed_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                long long should_wait = interval - elapsed_microseconds;
                if (should_wait > 0)
                    std::this_thread::sleep_for(std::chrono::microseconds(should_wait));
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }).detach();
}

void start_server() {
    schedule(run_engine_tick, 1000 * 1000/engine.ticksPerSecond);
}


int main() {
    start_server();    
    std::string command;
    double x, y, fuel, d1, d2;
    int hp, ammo;
    while (true) {
        std::cin >> command;
        if (command == "b") {
            std::cin >> x >> y >> fuel >> d1 >> d2;
            engine.add_bullet(x, y, fuel, d1, d2);
        }
        if (command == "p") {
            std::cin >> x >> y >> hp >> ammo;
            engine.add_player(x, y, 1, hp, ammo);
        }
        if (command == "w") {
            std::cin >> x >> y >> d1 >> d2;
            engine.add_wall(x, y, d1, d2);
        }
        else if (command == "run") {
            running = true;
        }
        else if (command == "stop") {
            running = false;
        }
        else if (command == "print") {
            engine.printAll();
        }
    }
    return 0;
}
*/
