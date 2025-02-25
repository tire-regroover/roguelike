#include "Level.h"
#include "Food.h"
#include "World.h"

namespace Roguelike {

Level::Level(World *world, Point stairs_up, bool add_stairs_up, bool add_stairs_down) {
    world_ = world;
    width_ = COLS - 2;
    height_ = LINES - 6;

    grid_ = new std::vector<Tile*>;
    grid_->resize(width_ * height_);

    memory_grid_ = new std::vector<chtype>;
    memory_grid_->resize(width_ * height_);
    for (size_t i = 0; i < memory_grid_->size(); i++) {
        memory_grid_->at(i) = ' ' | Colour::GREY_ON_BLACK;
    }

    rooms_ = new std::vector<Room*>;
    monsters_ = new std::vector<Monster*>;
    items_ = new std::vector<Item*>;
    stairs_up_ = stairs_up;
    create_level(add_stairs_up, add_stairs_down);
}

Level::~Level() {
    for (std::vector<Monster*>::iterator monster_it = monsters_->begin(); monster_it != monsters_->end(); ++monster_it) {
        for (std::vector<Item*>::iterator item_it = (*monster_it)->inventory()->begin(); item_it != (*monster_it)->inventory()->end();) {
            (*monster_it)->inventory()->erase(item_it);
        }
    }
    for (std::vector<Item*>::iterator it = items_->begin(); it != items_->end();) {
        delete *it;
        items_->erase(it);
    }
    delete items_;
    for (std::vector<Monster*>::iterator it = monsters_->begin(); it != monsters_->end();) {
        if (*(*it) != *(world_->guy()))
            delete *it;
        monsters_->erase(it);
    }
    delete monsters_;
    for (std::vector<Room*>::iterator it = rooms_->begin(); it != rooms_->end();) {
        delete *it;
        rooms_->erase(it);
    }
    delete rooms_;
    for (std::vector<Tile*>::iterator it = grid_->begin(); it != grid_->end();) {
        delete *it;
        grid_->erase(it);
    }
    delete grid_;
    delete memory_grid_;
    endwin();
}

void Level::draw_from_memory() {
    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            mvaddch(y + 3, x + 1, memory_grid_->at(index(y, x)));
        }
    }
    world_->draw_status();
    world_->guy()->focus();
    refresh();
}

Point Level::get_random_room_pos() const {
    Room* room;
    Point pos;
    do {
        room = rooms_->at(Utils::random_int(0, rooms_->size() - 1));
        pos = Point(room->pos().y + Utils::random_int(0, room->height() - 1),
                    room->pos().x + Utils::random_int(0, room->width() - 1));
    } while (pos == stairs_up_ || pos == stairs_down_ || room->type() != Room::HALL || tile_at(pos)->passage_blocked());
    return pos;
}

void Level::add_items(std::vector<Item*> items) {
    for (std::vector<Item*>::iterator it = items.begin(); it != items.end(); ++it) {
        items_->push_back(*it);
    }
    for (std::vector<Item*>::iterator it = items_->begin(); it != items_->end(); ++it) {
        stack_items((*it)->pos());
    }
}

void Level::add_monsters(std::vector<Monster*> monsters) {
    for (std::vector<Monster*>::iterator it = monsters.begin(); it != monsters.end(); ++it) {
        Point pos = get_random_room_pos();
        (*it)->set_pos(pos);
        monsters_->push_back(*it);
    }
}

void Level::create_level(bool add_stairs_up, bool add_stairs_down) {
    LayoutError err("no error");
    int dungeon_tries = 500;
    bool dungeon_ok;
    do {
        dungeon_ok = false;
        brick_all();

        for (std::vector<Room*>::iterator it = rooms_->begin(); !rooms_->empty();) {
            delete (*it);
            rooms_->erase(it);
        }
        rooms_->clear();

        int h = Utils::random_int(ROOM_HEIGHT_MIN, ROOM_HEIGHT_MAX);
        int w = Utils::random_int(ROOM_WIDTH_MIN, ROOM_WIDTH_MAX);
        int y = stairs_up_.y - h / 2;
        int x = stairs_up_.x - w / 2;

        try {
            add_room(Rect(y, x, h, w));

            if (add_stairs_up) {
                set_tile(stairs_up_, new StairsUp());
            }

            while (!is_enough()) {
                build_room();
            }
            dungeon_ok = true;
        } catch (LayoutError &x) {
            err = x;
        }
    } while (--dungeon_tries > 0 && !dungeon_ok);

    if (!dungeon_ok) {
        endwin();
        throw err;
        return;
    }

    remove_stragglers();
    connect_more_rooms();

    if (add_stairs_down) {
        Room* room = rooms_->at(Utils::random_int(1, rooms_->size() - 1));
        while (room->type() != Room::HALL)
            room = rooms_->at(Utils::random_int(1, rooms_->size() - 1));
        set_tile(room->center(), new StairsDown());
        stairs_down_ = room->center();
    }
}

void Level::connect_more_rooms() {
    for (int i = 0; i < (int)rooms_->size() * 0.2; i++) {
        int tries = 100;
        bool again;
        do {
            again = false;
            bool hit_edge = false;
            Room* room = rooms_->at(Utils::random_int(0, rooms_->size() - 1));
            bool vertical = room->height() > room->width();
            Direction::DIRECTION dir = world_->random_dir(false, !vertical, vertical);
            Point start;

            int wall_tries = 100;
            do {
                start = get_random_wall_pos(room, dir);
            } while (--wall_tries > 0 && tile_at(start)->type() != Tile::WALL);
            if (wall_tries == 0) {
                again = true;
                continue;
            }
            Point pos = world_->get_adjacent_pos(start, world_->opposite_dir(dir));
            int len = -1;
            bool stop = false;
            while (!stop) {
                len++;
                pos = world_->get_adjacent_pos(pos, dir);
                if (!is_valid_point(pos)) {
                    stop = true;
                    hit_edge = true;
                }
                else {
                    std::vector<Point> points = world_->get_walk_check_points(pos, world_->opposite_dir(dir));
                    for (std::vector<Point>::iterator it = points.begin(); !stop && it != points.end(); it++) {
                        if (is_valid_point(*it)) {
                            if (tile_at(*it)->type() != Tile::WALL)
                                stop = true;
                        } else
                            hit_edge = true;
                    }
                }
            }
            bool ok = false;
            if (!hit_edge && len > 1) {
                ok = true;
                std::vector<Point> points = world_->get_walk_check_points(pos, dir);
                for (std::vector<Point>::iterator it = points.begin(); ok && it != points.end(); it++) {
                    if (tile_at(*it)->type() != Tile::WALL)
                        ok = false;
                }
                if (tile_at(world_->get_adjacent_pos(pos, dir))->type() == Tile::DOOR)
                    ok = false;
                else if (tile_at(world_->get_adjacent_pos(start, world_->opposite_dir(dir)))->type() == Tile::DOOR)
                    ok = false;
            }
            if (ok) {
                add_corridor(start, dir, len, false);
                set_tile(start, new Door(false));
                set_tile(pos, new Door(false));
            } else
                again = true;
        } while (--tries > 0 && again);

        if (tries == 0)
            throw LayoutError("connect_more_rooms() bad");
    }
}

Point Level::get_random_wall_pos(Room* room, Direction::DIRECTION dir) const {
    switch (dir) {
        case Direction::NORTH:
            return Point(room->pos().y - 1, Utils::random_int(room->pos().x, room->pos().x + room->width() - 1));
        case Direction::EAST:
            return Point(Utils::random_int(room->pos().y, room->pos().y + room->height() - 1), room->pos().x + room->width());
        case Direction::SOUTH:
            return Point(room->pos().y + room->height(), Utils::random_int(room->pos().x, room->pos().x + room->width() - 1));
        case Direction::WEST:
            return Point(Utils::random_int(room->pos().y, room->pos().y + room->height() - 1), room->pos().x - 1);
        default:
            break;
    }
    return Point();
}

void Level::remove_stragglers() {
    for (std::vector<Room*>::iterator it = rooms_->begin(); it != rooms_->end();) {
        bool del = false;
        if ((*it)->type() == Room::CORRIDOR) {
            if ((*it)->height() > (*it)->width()) { //vertical
                int x = (*it)->pos().x;
                //north:
                for (int y = (*it)->pos().y; y < (*it)->pos().y + (*it)->height(); y++) {
                    if (tile_at(y - 1, x)->type() != Tile::DOOR
                            && tile_at(y, x - 1)->type() != Tile::DOOR
                            && tile_at(y, x + 1)->type() != Tile::DOOR) {
                        brick(Point(y, x));
                        (*it)->pos().y += 1;
                        (*it)->set_height((*it)->height() - 1);
                    } else
                        break;
                }
                if ((*it)->height() == 0) {
                    del = true;
                    brick((*it)->pos());
                }
                //south:
                for (int y = (*it)->pos().y + (*it)->height() - 1; y >= (*it)->pos().y; y--) {
                    if (tile_at(y + 1, x)->type() != Tile::DOOR
                            && tile_at(y, x - 1)->type() != Tile::DOOR
                            && tile_at(y, x + 1)->type() != Tile::DOOR) {
                        brick(Point(y, x));
                        (*it)->set_height((*it)->height() - 1);
                    } else
                        break;
                }
                if ((*it)->height() == 0) {
                    del = true;
                    brick(Point((*it)->pos().y - 1, (*it)->pos().x));
                }
            } else { //horizontal
                int y = (*it)->pos().y;
                //east:
                for (int x = (*it)->pos().x + (*it)->width() - 1; x >= (*it)->pos().x; x--) {
                    if (tile_at(y, x + 1)->type() != Tile::DOOR
                            && tile_at(y + 1, x)->type() != Tile::DOOR
                            && tile_at(y - 1, x)->type() != Tile::DOOR) {
                        brick(Point(y, x));
                        (*it)->set_width((*it)->width() - 1);
                    } else
                        break;
                }
                if ((*it)->width() == 0) {
                    del = true;
                    brick(Point((*it)->pos().y, (*it)->pos().x - 1));
                }
                //west:
                for (int x = (*it)->pos().x; x < (*it)->pos().x + (*it)->width(); x++) {
                    if (tile_at(y, x - 1)->type() != Tile::DOOR
                            && tile_at(y + 1, x)->type() != Tile::DOOR
                            && tile_at(y - 1, x)->type() != Tile::DOOR) {
                        brick(Point(y, x));
                        (*it)->pos().x += 1;
                        (*it)->set_width((*it)->width() - 1);
                    } else
                        break;
                }
                if ((*it)->width() == 0) {
                    del = true;
                    brick((*it)->pos());
                }
            }
        }

        if (del) {
            delete *it;
            rooms_->erase(it);
        } else {
            ++it;
        }
    }
}

void Level::build_room() {
    int room_tries = 100;
    bool room_ok;
    do{
        room_ok = false;
        try {
            Room* room = rooms_->at(Utils::random_int(0, rooms_->size() - 1));
            Direction::DIRECTION dir = world_->random_dir(false);
            Point pos = get_random_wall_pos(room, dir);
            switch (room->type()) {
                case Room::HALL:
                    add_corridor(pos, dir, Utils::random_int(4, dir == Direction::NORTH || dir == Direction::SOUTH ? height_ * 0.75 : width_ * 0.75));
                    break;
                case Room::CORRIDOR:
                    add_hall(pos, dir, Utils::random_int(ROOM_HEIGHT_MIN, ROOM_HEIGHT_MAX), Utils::random_int(ROOM_WIDTH_MIN, ROOM_WIDTH_MAX));
                    break;
            }
            set_tile(pos, new Door(false));
            room_ok = true;
        }
        catch (LayoutError &x) { }
    } while(--room_tries > 0 && !room_ok);

    if (!room_ok) {
        throw LayoutError("!room_ok");
    }
}

bool Level::is_enough() {
    int area = 0;
    for (std::vector<Room*>::iterator it = rooms_->begin(); it != rooms_->end(); ++it) {
        area += (*it)->height() * (*it)->width();
    }
    if (area > height_ * width_ * 0.3)
        return true;
    return false;
}

bool Level::is_valid_room(Rect rect) {
    if (!is_in_bounds(rect))
        return false;
    for (std::vector<Room*>::iterator it = rooms_->begin(); it != rooms_->end(); ++it) {
        if (does_collide(rect, *(*it))) {
            return false;
        }
    }
    return true;
}

void Level::add_hall(Point pos, Direction::DIRECTION dir, int height, int width) {
    Rect rect;
    switch (dir) {
        case Direction::NORTH:
            rect = Rect(pos.y - height, pos.x - width / 2, height, width);
            break;
        case Direction::EAST:
            rect = Rect(pos.y- height / 2, pos.x + 1, height, width);
            break;
        case Direction::SOUTH:
            rect = Rect(pos.y + 1, pos.x - width / 2, height, width);
            break;
        case Direction::WEST:
            rect = Rect(pos.y- height / 2, pos.x - width, height, width);
            break;
        default:
            break;
    }

    if (!is_valid_room(rect)) {
        throw LayoutError("hall collide");
    }

    rooms_->push_back(new Hall(rect));
    fill_rect(rect);
}

void Level::add_corridor(Point pos, Direction::DIRECTION dir, int length, bool validate) {
    Rect rect;
    switch (dir) {
        case Direction::NORTH:
            rect = Rect(pos.y - length, pos.x, length, 1);
            break;
        case Direction::EAST:
            rect = Rect(pos.y, pos.x + 1, 1, length);
            break;
        case Direction::SOUTH:
            rect = Rect(pos.y + 1, pos.x, length, 1);
            break;
        case Direction::WEST:
            rect = Rect(pos.y, pos.x - length, 1, length);
            break;
        default:
            break;
    }

    if (validate && !is_valid_room(rect)) {
        throw LayoutError("corridor collide");
    }

    rooms_->push_back(new Corridor(rect));
    fill_rect(rect);
}

void Level::fill_rect(Rect rect) {
    for (int y = rect.pos().y; y < rect.pos().y + rect.height(); y++) {
        for (int x = rect.pos().x; x < rect.pos().x + rect.width(); x++) {
            Floor* tile = new Floor();
            set_tile(y, x, tile);
        }
    }
}

void Level::add_room(Rect rect) {
    if (!is_valid_room(rect)) {
        throw LayoutError("add_room(Rect rect) !is_valid_room");
    }

    rooms_->push_back(new Hall(rect));

    fill_rect(rect);
}

void Level::add_room() {
    int tries = 1000;
    bool ok;
    do {
        ok = false;

        int rheight = Utils::random_int(ROOM_HEIGHT_MIN, ROOM_HEIGHT_MAX);
        int rwidth = Utils::random_int(ROOM_WIDTH_MIN, ROOM_WIDTH_MAX);
        int room_y = Utils::random_int(1, height_ - rheight - 3);
        int room_x = Utils::random_int(1, width_ - rwidth - 3);

        try {
            add_room(Rect(room_y, room_x, rheight, rwidth));
            ok = true;
        }
        catch (LayoutError &) {}
    } while (--tries >= 0 && !ok);

    if (!ok)
        throw LayoutError("add_room() fail");
}

void Level::tunnel(Point pos) {
    Floor* tile = new Floor();
    set_tile(pos, tile);
}

void Level::brick(Point pos) {
    Wall* tile = new Wall();
    set_tile(pos, tile);
}

void Level::brick(Rect rect) {
    for (int y = 0; y < rect.height(); y++) {
        for (int x = 0; x < rect.width(); x++) {
            brick(Point(rect.pos().y + y, rect.pos().x + x));
        }
    }
}

void Level::brick_all() {
    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            brick(Point(y, x));
        }
    }
}

void Level::draw_tile_at(Point pos) const {
    Tile* tile = tile_at(pos);
    attron(tile->colour());
    mvaddch(pos.y + 3, pos.x + 1, tile->symbol());
    attroff(tile->colour());
    tile->set_drawn(true);
}

void Level::draw_all() const {
    clear();
    for (std::vector<Room*>::iterator it = rooms_->begin(); it != rooms_->end(); ++it) {
        for (int y = (*it)->pos().y - 1; y <= (*it)->pos().y + (*it)->height(); y++)
            draw_tile_at(Point(y, (*it)->pos().x - 1));
        for (int y = (*it)->pos().y - 1; y <= (*it)->pos().y + (*it)->height(); y++)
            draw_tile_at(Point(y, (*it)->pos().x + (*it)->width()));
        for (int x = (*it)->pos().x; x <= (*it)->pos().x + (*it)->width() - 1; x++)
            draw_tile_at(Point((*it)->pos().y - 1, x));
        for (int x = (*it)->pos().x; x <= (*it)->pos().x + (*it)->width() - 1; x++)
            draw_tile_at(Point((*it)->pos().y + (*it)->height(), x));
    }
    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            if (tile_at(y, x)->type() != Tile::WALL)
                draw_tile_at(Point(y, x));
        }
    }

    for (std::vector<Item*>::iterator it = items_->begin(); it != items_->end(); ++it) {
        if (!(*it)->held())
            (*it)->draw();
    }
    for (std::vector<Monster*>::iterator it = monsters_->begin(); it != monsters_->end(); ++it) {
        if (*(*it) != *world_->guy()) {
            (*it)->draw();
        }
    }
    refresh();
}

Room* Level::find_room(Monster* monster) const {
    for (std::vector<Room*>::iterator it = rooms_->begin(); it != rooms_->end(); ++it) {
        if ((*it)->contains(monster->pos()))
            return *it;
    }
    Room* r = NULL;
    return r;
}

Room* Level::find_room(Item* item) const {
    for (std::vector<Room*>::iterator it = rooms_->begin(); it != rooms_->end(); ++it) {
        if ((*it)->contains(item->pos()))
            return *it;
    }
    Room* r = NULL;
    return r;
}

Room* Level::find_room(Point pos) const {
    for (std::vector<Room*>::iterator it = rooms_->begin(); it != rooms_->end(); ++it) {
        if ((*it)->contains(pos))
            return *it;
    }
    Room* r = NULL;
    return r;
}

Item* Level::find_top_item(Point pos) {
    for (int i = items_->size() - 1; i >= 0; i--) {
        Item* item = items_->at(i);
        if (item->pos() == pos && !item->held()) {
            return item;
        }
    }
    Item* i = NULL;
    return i;
}

std::vector<Item*> Level::find_items(Point pos) {
    std::vector<Item*> items;;
    for (size_t i = 0; i < items_->size(); i++) {
        Item* item = items_->at(i);
        if (item->pos() == pos && !item->held()) {
            items.push_back(item);
        }
    }
    return items;
}

void Level::remember() {
    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            memory_grid_->at(index(y, x)) = mvinch(y + 3, x + 1);
        }
    }
}

void Level::stack_items(Point pos) {
    std::vector<Item*> unique_items;
    for (std::vector<Item*>::iterator it = items_->begin(); it != items_->end(); ++it) {
        if ((*it)->is_stackable() && !(*it)->held() && (*it)->pos() == pos) {

            bool exists = false;
            for (std::vector<Item*>::iterator unique_it = unique_items.begin(); !exists && unique_it != unique_items.end(); ++unique_it) {
                if ((*unique_it)->noun() == (*it)->noun())
                    exists = true;
            }
            if (!exists)
                unique_items.push_back(*it);
        }
    }

    for (std::vector<Item*>::iterator unique_it = unique_items.begin(); unique_it != unique_items.end(); ++unique_it) {
        for (std::vector<Item*>::iterator it = items_->begin(); it != items_->end();) {
            if ((*it)->is_stackable() && !(*it)->held() && *(*it) != *(*unique_it) && (*it)->pos() == pos && (*unique_it)->noun() == (*it)->noun()) {
                (*unique_it)->set_quantity((*unique_it)->quantity() + (*it)->quantity());
                delete *it;
                items_->erase(it);
            } else {
                ++it;
            }
        }
    }
}

void Level::splatter_gore(Monster* victim, Direction::DIRECTION dir) {
    Point pos = victim->pos();
    Tile* tile = tile_at(pos);
    if (victim->sees(pos) && (tile->type() == Tile::FLOOR || tile->type() == Tile::WALL || tile->type() == Tile::DOOR)) {
        if (tile->type() == Tile::FLOOR && Utils::percent_chance(30))
            tile->set_colour(Colour::LIGHTRED_ON_BLACK);
        else
            tile->set_colour(Colour::RED_ON_BLACK);

        if (tile->type() == Tile::FLOOR && Utils::percent_chance(4)) {
            Entrails* entrails = new Entrails(pos, 1);
            items_->push_back(entrails);
        }
    }

    if (world_->is_diag(dir)) {
        pos = world_->get_adjacent_pos(pos, dir);
        pos = world_->get_adjacent_pos(pos, dir);
        dir = world_->clockwise_dir(world_->clockwise_dir(world_->clockwise_dir(dir)));
        if (is_valid_point(pos)) {
            Tile* tile = tile_at(pos);
            if (victim->sees(pos) && (tile->type() == Tile::FLOOR || tile->type() == Tile::WALL || tile->type() == Tile::DOOR)) {
                if (tile->type() == Tile::FLOOR && Utils::percent_chance(30))
                    tile->set_colour(Colour::LIGHTRED_ON_BLACK);
                else
                    tile->set_colour(Colour::RED_ON_BLACK);

                if (tile->type() == Tile::FLOOR && Utils::percent_chance(4)) {
                    Entrails* entrails = new Entrails(pos, 1);
                    items_->push_back(entrails);
                }
            }
        }
    }

    pos = world_->get_adjacent_pos(pos, dir);
    dir = world_->clockwise_dir(world_->clockwise_dir(dir));
    pos = world_->get_adjacent_pos(pos, dir);
    dir = world_->opposite_dir(dir);

    for (int i = 0; i < 3; i++) {
        if (!is_valid_point(pos)) continue;
        tile = tile_at(pos);
        if (victim->sees(pos) && (tile->type() == Tile::FLOOR || tile->type() == Tile::WALL || tile->type() == Tile::DOOR)) {
            if (tile->type() == Tile::FLOOR && Utils::percent_chance(30))
                tile->set_colour(Colour::LIGHTRED_ON_BLACK);
            else
                tile->set_colour(Colour::RED_ON_BLACK);

            if (tile->type() == Tile::FLOOR && Utils::percent_chance(4)) {
                Entrails* entrails = new Entrails(pos, 1);
                items_->push_back(entrails);
            }
        }
        pos = world_->get_adjacent_pos(pos, dir);
    }

    dir = world_->clockwise_dir(world_->clockwise_dir(dir));
    pos = world_->get_adjacent_pos(pos, dir);
    dir = world_->clockwise_dir(world_->clockwise_dir(dir));

    for (int i = 0; i < 5; i++) {
        if (!is_valid_point(pos)) continue;
        tile = tile_at(pos);
        if (victim->sees(pos) && (tile->type() == Tile::FLOOR || tile->type() == Tile::WALL || tile->type() == Tile::DOOR)) {
            if (tile->type() == Tile::FLOOR && Utils::percent_chance(30))
                tile->set_colour(Colour::LIGHTRED_ON_BLACK);
            else
                tile->set_colour(Colour::RED_ON_BLACK);

            if (tile->type() == Tile::FLOOR && Utils::percent_chance(4)) {
                Entrails* entrails = new Entrails(pos, 1);
                items_->push_back(entrails);
            }
        }
        pos = world_->get_adjacent_pos(pos, dir);
    }
}

Monster* Level::monster_at(Point pos) const {
    Monster* monster = NULL;
    for (std::vector<Monster*>::iterator it = monsters_->begin(); it != monsters_->end(); ++it) {
        if ((*it)->pos() == pos)
            return *it;
    }
    return monster;
}

std::vector<Point> Level::door_points() const {
    std::vector<Point> doors;
    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            Point pos(y, x);
            if (tile_at(pos)->type() == Tile::DOOR)
                doors.push_back(pos);
        }
    }
    return doors;
}

}
