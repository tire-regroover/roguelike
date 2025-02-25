#ifndef LEVEL_H_
#define LEVEL_H_

#include "Room.h"
#include "Tile.h"
#include <vector>
#include <stdexcept>

namespace Roguelike {

class Monster;
class World;
class Item;

class LayoutError : public std::runtime_error {
public:
    LayoutError(const std::string& text) : std::runtime_error(text) {}
};

class Level {
private:
    World* world_;
    std::vector<Monster*>* monsters_;
    std::vector<Tile*>* grid_;
    std::vector<chtype>* memory_grid_;
    std::vector<Room*>* rooms_;
    std::vector<Item*>* items_;

    int width_;
    int height_;

    Point stairs_up_;
    Point stairs_down_;

    static const int ROOM_HEIGHT_MIN = 3;
    static const int ROOM_HEIGHT_MAX = 6;
    static const int ROOM_WIDTH_MIN = 7;
    static const int ROOM_WIDTH_MAX = 15;

    void create_level(bool add_stairs_up, bool add_stairs_down);
    void connect_more_rooms();
    void remove_stragglers();
    void build_room();
    bool is_enough();
    void add_hall(Point pos, Direction::DIRECTION dir, int height, int width);
    void add_corridor(Point pos, Direction::DIRECTION dir, int length, bool validate = true);

    void add_room();
    void add_room(Rect rect);

    void fill_rect(Rect rect);

    bool is_valid_room(Rect rect);

    bool does_collide(Rect r1, Rect r2) {
        r1 = Rect(r1.pos().y - 1, r1.pos().x - 1, r1.height() + 2, r1.width() + 2);

        return ! ( r2.pos().x > r1.pos().x + r1.width() - 1
                || r2.pos().x + r2.width() - 1 < r1.pos().x
                || r2.pos().y > r1.pos().y + r1.height() - 1
                || r2.pos().y + r2.height() - 1 < r1.pos().y );
    }

    bool is_in_bounds(Rect rect) {
        return ( rect.pos().x > 0 && rect.pos().x + rect.width() - 1 < width_ - 1
                && rect.pos().y > 0 && rect.pos().y + rect.height() - 1 < height_ - 1 );
    }

    void brick(Point pos);
    void brick(Rect rect);
    void brick_all();
    void tunnel(Point pos);

    Point get_random_wall_pos(Room* room, Direction::DIRECTION dir) const;

public:
    Level(World* world, Point stairs_up, bool add_stairs_up = true, bool add_stairs_down = true);
    virtual ~Level();

    void add_items(std::vector<Item*> items);
    void add_monsters(std::vector<Monster*> monsters);

    Point get_random_room_pos() const;

    void draw_tile_at(Point pos) const;
    void draw_all() const;
    void draw_from_memory();

    void go();

    World* world() const { return world_; }

    int width() const {
        return width_;
    }

    int height() const {
        return height_;
    }

    Tile* tile_at(Point pos) const {
        return tile_at(pos.y, pos.x);
    }

    Tile* tile_at(int py, int px) const {
        return grid_->at(py * width_ + px);
    }

    void set_tile(Point pos, Tile* tile) {
        set_tile(pos.y, pos.x, tile);
    }

    void set_tile(int py, int px, Tile* tile) {
        Tile* prev = grid_->at(py * width_ + px);
        grid_->at(index(py, px)) = tile;
        delete prev;
    }

    int index(int y, int x) {
        return y * width_ + x;
    }

    Point stairs_up() const {
        return stairs_up_;
    }

    Point stairs_down() const {
        return stairs_down_;
    }

    std::vector<Monster*>*& monsters() {
        return monsters_;
    }

    std::vector<Item*>*& items() {
        return items_;
    }

    std::vector<Room*>*& rooms() {
        return rooms_;
    }

    bool is_valid_point(Point pos) const {
        return (pos.y > 0 && pos.y < height_ && pos.x > 0 && pos.x < width_);
    }

    Room* find_room(Item* item) const;
    Room* find_room(Monster* monster) const;
    Room* find_room(Point pos) const;

    Monster* monster_at(Point pos) const;

    Item* find_top_item(Point pos);
    std::vector<Item*> find_items(Point pos);

    std::vector<chtype>* memory_grid() const { return memory_grid_; }
    void remember();

    void stack_items(Point pos);

    void splatter_gore(Monster* victim, Direction::DIRECTION dir);

    std::vector<Point> door_points() const;

};

}

#endif /* LEVEL_H_ */
