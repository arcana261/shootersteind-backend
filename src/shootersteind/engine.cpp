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

const double eps = 1e-6;
const double bullet_speed = 10;
const int bullet_damage = 30;
const double bullet_collision = 0.1;
const double player_collision = 0.5;
const double wall_collision = 0.3;


struct Coordinate {
    double x;
    double y;
};

struct Direction {
    double x;
    double y;
};

class InMapPointObject {
    public: Coordinate location;
    public: double collision;

    public:
        InMapPointObject (double x = 0, double y = 0, double collision_size = 0) {
            location.x = x;
            location.y = y;
            collision = collision_size;
        }
};

class Bullet: public InMapPointObject {
    public: Direction direction;
    public: double fuel; // how much distance can travel

    public:
        Bullet(double x = 0, double y = 0, double initial_fuel = 0, double direction_x = 0, double direction_y = 0) {
            location.x = x;
            location.y = y;
            direction.x = direction_x;
            direction.y = direction_y;
            collision = bullet_collision;
            fuel = initial_fuel;
        }
};

struct Wall {
    Coordinate location_start;
    Coordinate location_end;
    double collision;

    public:
        Wall(double x1 = 0, double y1 = 0, double x2 = 0, double y2 = 0)
        {
            location_start.x = x1;
            location_start.y = y1;
            location_end.x = x2;
            location_end.y = y2;
            collision = wall_collision;
        }
};

struct Item {
    Coordinate location;
    bool is_used;
    int type; // 0 -> Ammo, 1 -> Health Package
    int value;
    double collision;
};

class Player : public InMapPointObject{
    public: int player_id;
    public: bool is_alive;
    public: int health;
    public: int ammo;

    public:
        Player(double x = 0, double y = 0, int pid = 0, int hp = 0, int ammunation = 0) {
            location.x = x;
            location.y = y;
            player_id = pid;
            is_alive = true;
            health = hp;
            ammo = ammunation;
            collision = player_collision;
        }
};

struct locationBasedComparisonStruct {
    bool operator() (const InMapPointObject& lhs, const InMapPointObject& rhs) const
    {
        Coordinate c1 = lhs.location;
        Coordinate c2 = rhs.location;
        if (abs(c1.x - c2.x) < eps)
            return c1.y < c2.y;
        return c1.x < c2.x;
    }
};

bool point_collision(InMapPointObject o1, InMapPointObject o2) {
    double delta_x = o1.location.x - o2.location.x;
    double delta_y = o1.location.y - o2.location.y;
    
    double dist = sqrt(delta_x * delta_x + delta_y * delta_y);
    if (o1.collision + o2.collision > dist)
        return true;
    return false;
}

bool point_line_collision(InMapPointObject o1, Wall w1) {
    int wall_type = 0; // Horizontal
    if (abs(w1.location_start.x - w1.location_end.x) < eps)
        wall_type = 1; // Vertical
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

            distance = std::min(sqrt(delta_x_1 * delta_x_1 + delta_y_1 * delta_y_1), sqrt(delta_x_2 * delta_x_2 + delta_y_2 * delta_y_2));
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

            distance = std::min(sqrt(delta_x_1 * delta_x_1 + delta_y_1 * delta_y_1), sqrt(delta_x_2 * delta_x_2 + delta_y_2 * delta_y_2));
        }
    }
    if (distance < (w1.collision + o1.collision))
        return true;
    return false;
}

class Engine {
    public: int ticksPerSecond;
    protected: int gameTick;
    protected: double width, height;
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
                double len = sqrt(d.x * d.x + d.y * d.y);
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