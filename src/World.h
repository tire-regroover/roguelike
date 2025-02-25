#ifndef WORLD_H_
#define WORLD_H_

#include "Monster.h"
#include "Guy.h"
#include "Tile.h"
#include "Room.h"
#include "Item.h"

#include <ncurses.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <map>

namespace Roguelike {

class Level;
class Monster;
class Guy;
class Item;

struct PathItem {
    Point pos;
    int c;
    PathItem() {}
    PathItem(int y, int x, int counter) {
        pos = Point(y, x);
        c = counter;
    }
    PathItem(Point position, int counter) {
        pos = position;
        c = counter;
    }
};

class World {
private:
    static const int DUNGEON_DEPTH = 10;

    unsigned long turns_;
    int width_;
    int height_;

    Guy* guy_;
    std::map<int, char> inventory_keys_;

    std::vector<Level*> dungeon_;
    Level* level_;
    int current_level_;

    std::vector<std::string>* status_;
    std::vector<std::string>* status_history_;

    void create_dungeon();
    void add_monsters(int level);
    void add_items(int level);
    void change_level(Level* new_level);
    void help() const;
    void examine();
    void draw_coloured_line(int y, std::string line) const;
    void paginated_redraw(int top, std::vector<std::string> &lines, std::string bottom_line) const;
    std::vector<std::string> prepare_status();
public:
    World();
    virtual ~World();

    void go();

    Guy*& guy() { return guy_; }

    int current_level() const { return current_level_; }
    int turns() const { return turns_; }

    void do_turn();

    void draw_status();

    bool is_diag(Direction::DIRECTION dir) {
        switch (dir) {
            case Direction::NORTHEAST:
            case Direction::NORTHWEST:
            case Direction::SOUTHEAST:
            case Direction::SOUTHWEST:
                return true;
            default:
                break;
        }
        return false;
    }

    bool are_adjacent(Point one, Point two, bool diags = true) const {
        if ((one.y == two.y - 1 && one.x == two.x) ||
                (one.y == two.y - 1 && one.x == two.x + 1) ||
                (one.y == two.y && one.x == two.x + 1) ||
                (one.y == two.y + 1 && one.x == two.x + 1) ||
                (one.y == two.y + 1 && one.x == two.x) ||
                (one.y == two.y + 1 && one.x == two.x - 1) ||
                (one.y == two.y && one.x == two.x - 1) ||
                (one.y == two.y - 1 && one.x == two.x - 1)) {
                    if (!diags)
                        return !are_adjacent_diagonally(one, two);
                    return true;
                }
        return false;
    }

    bool are_adjacent_diagonally(Point one, Point two) const {
        if ((one.y == two.y - 1 && one.x == two.x + 1) ||
                (one.y == two.y + 1 && one.x == two.x + 1) ||
                (one.y == two.y + 1 && one.x == two.x - 1) ||
                (one.y == two.y - 1 && one.x == two.x - 1)) {
                    return true;
                }
        return false;
    }

    Direction::DIRECTION get_relative_dir(Point start, Point dest) const {
        Direction::DIRECTION dir = Direction::NONE;
        if (start.y > dest.y)
            dir = (Direction::DIRECTION)(dir | Direction::NORTH);
        else if (start.y < dest.y)
            dir = (Direction::DIRECTION)(dir | Direction::SOUTH);
        if (start.x > dest.x)
            dir = (Direction::DIRECTION)(dir | Direction::WEST);
        else if (start.x < dest.x)
            dir = (Direction::DIRECTION)(dir | Direction::EAST);
        return dir;
    }

    Point get_adjacent_pos(Point pos, Direction::DIRECTION dir) const;

    std::vector<Point> get_adjacent_points(Point pos) const;
    std::vector<Point> get_walk_check_points(Point pos, Direction::DIRECTION ignore_dir) const;

    std::vector<Point> find_path(Monster* who, Point start, Point dest, bool diags = true, bool check_blocked = false) const;
    std::vector<Point> bresenham_path(Point start, Point end) const;

    Direction::DIRECTION get_input_dir();
    bool get_input_yes_or_no(bool default_yn = true) const;
    YesNoAllNumberQuit::YES_NO_ALL_NUMBER_QUIT get_input_yes_no_number_all_or_quit(YesNoAllNumberQuit::YES_NO_ALL_NUMBER_QUIT default_ynq = YesNoAllNumberQuit::YES) const;
    int get_input_quantity(size_t min, size_t max);
    Item* get_input_item(std::vector<Item*>& items, std::string title);
    Hand::HAND get_input_hand();

    Point get_nearest_target() const;

    void status(std::string text, bool show_now = false);
    void status(const Point pos);
    void status(int i);

    void show_paginated(std::vector<std::string>& lines, std::string title) const;
    void show_guy_inventory();
    void show_message_history();

    bool play_again() const;

    std::vector<std::string> format_items(std::vector<Item*>& items, bool do_key_map = false);
    std::string inventory_line(Item& item);

    Direction::DIRECTION opposite_dir(Direction::DIRECTION dir) const;
    Direction::DIRECTION clockwise_dir(Direction::DIRECTION dir) const;

    Direction::DIRECTION random_dir(bool diags = true, bool ns = true, bool ew = true) const {
        while (1) {
            switch (Utils::random_int(0, 7)) {
                case 0:
                    if (ns) return (Direction::NORTH);
                    break;
                case 1:
                    if (diags) return (Direction::NORTHEAST);
                    break;
                case 2:
                    if (ew) return (Direction::EAST);
                    break;
                case 3:
                    if (diags) return (Direction::SOUTHEAST);
                    break;
                case 4:
                    if (ns) return (Direction::SOUTH);
                    break;
                case 5:
                    if (diags) return (Direction::SOUTHWEST);
                    break;
                case 6:
                    if (ew) return (Direction::WEST);
                    break;
                case 7:
                    if (diags) return (Direction::NORTHWEST);
                    break;
            }
        }
        return Direction::NONE;
    }
};

}

#endif /* WORLD_H_ */
