#include "Guy.h"
#include "Food.h"
#include <sstream>
#include "World.h"
#include "Weapon.h"

namespace Roguelike {

Guy::Guy(Level* level, std::string name) : Monster(level, name) {
    if (name == "")
        name_ = "player";
    type_ = GUY;
    symbol_ = '@';
    strength_ = 10;
    dexterity_ = 3;
    hp_max_ = 17;
    colour_ = Colour::WHITE_ON_BLACK;
    hp_ = hp_max_;
    xp_ = 0;
    lvl_ = 1;
    base_attack_ = 0;
    attack_dice_ = Dice(1, 4);

    breathe_every_ = 0;

    good_ = true;

    can_open_doors_ = true;
    eyesight_ = 4;

    satiation_ = 800;

    drops_corpse_ = false;
}

Guy::~Guy() {}

std::string Guy::verb() const {
    std::vector<std::string> possible_verbs;
    possible_verbs.push_back("strangle");
    possible_verbs.push_back("strangulate");
    possible_verbs.push_back("choke");
    possible_verbs.push_back("throttle");
    possible_verbs.push_back("punch");
    possible_verbs.push_back("beat up");
    possible_verbs.push_back("hit");
    possible_verbs.push_back("scratch");
    possible_verbs.push_back("slap");
    possible_verbs.push_back("shove");
    possible_verbs.push_back("whup");
    return possible_verbs.at(Utils::random_int(0, possible_verbs.size() - 1));
}

std::string Guy::verbs() const {
    std::ostringstream o;
    std::string verb = this->verb();
    if (verb == "punch" || verb == "scratch")
        o << verb << "es";
    else if (verb == "beat up")
        o << "beats up";
    else
        o << verb << 's';
    return o.str();
}

std::string Guy::verbed() const {
    std::ostringstream o;
    std::string verb = this->verb();
    if (verb == "punch" || verb == "scratch")
        o << verb << "ed";
    else if (verb == "slap" || verb == "whup")
        o << verb << "ped";
    else if (verb == "beat up" || verb == "hit")
        o << verb;
    else
        o << verb << 'd';
    return o.str();
}

void Guy::rest() {
    level_->world()->status("Time passes.");
}

void Guy::open() {
    open_or_close(true);
}

void Guy::close() {
    open_or_close(false);
}

void Guy::open_or_close(Point pos, bool open) {
    Tile* tile = level_->tile_at(pos);
    Door* door = static_cast<Door*>(tile);
    if (pos == pos_) {
        level_->world()->status("You're standing in the way.");
    } else if (door->is_open() && (tile->passage_blocked() || level_->find_top_item(pos) != NULL)) {
        level_->world()->status("There's something in the way.");
    } else {
        if (open) {
            if (door->is_locked()) {
                level_->world()->status("The door is locked.");
            } else {
                door->open();
                level_->world()->status("You open the door.");
            }
        } else {
            door->close();
            level_->world()->status("You close the door.");
        }

        attron(door->colour());
        mvaddch(pos.y + 3, pos.x + 1, door->symbol());
        attroff(door->colour());
    }
}

void Guy::open_or_close(bool open) {
    std::vector<Point> points;
    std::vector<Direction::DIRECTION> dirs;
    for (int y = pos_.y - 1; y <= pos_.y + 1; y++) {
        for (int x = pos_.x - 1; x <= pos_.x + 1; x++) {
            Point pos(y, x);
            if (level_->is_valid_point(pos)) {
                Tile* tile = level_->tile_at(pos);
                if (tile->type() == Tile::DOOR) {
                    Door* door = static_cast<Door*>(tile);
                    if (open && !door->is_open()) {
                        points.push_back(pos);
                        dirs.push_back(level_->world()->get_relative_dir(pos_, pos));
                    } else if (!open && door->is_open()) {
                        points.push_back(pos);
                        dirs.push_back(level_->world()->get_relative_dir(pos_, pos));
                    }
                }
            }
        }
    }
    if (points.size() > 1) {
        level_->world()->status("Direction: ", true);
        Direction::DIRECTION input_dir = level_->world()->get_input_dir();
        if (input_dir != Direction::NONE) {
            Point pos;
            bool dir_ok = false;
            for (size_t i = 0; i < points.size(); i++) {
                pos = points.at(i);
                if (dirs.at(i) == input_dir) {
                    dir_ok = true;
                    break;
                }
            }
            if (dir_ok) {
                open_or_close(pos, open);
            } else {
                std::ostringstream o;
                o << "There's nothing to " << (open ? "open" : "close") << " in that direction.";
                level_->world()->status(o.str());
            }
        }
    } else if (points.size() == 1) {
        open_or_close(points.back(), open);
    } else {
        std::ostringstream o;
        o << "There's nothing here to " << (open ? "open." : "close.");
        level_->world()->status(o.str());
    }
}

void Guy::do_fov() {
    int h = eyesight_ * 2 + 1;
    int w = eyesight_ * 2 + 1;
    Rect rect(pos_.y - h / 2, pos_.x - w / 2, h, w);
    for (int y = rect.pos().y; y < rect.pos().y + h; y++) {
        for (int x = rect.pos().x; x < rect.pos().x + w; x++) {
            Point pos(y, x);
            if (pos != pos_ && sees(pos)) {
                level_->draw_tile_at(pos);
            }
        }
    }

    Room* guy_room = level_->find_room(this);

    if (guy_room != NULL) {
        for (int y = guy_room->pos().y; y < guy_room->pos().y + guy_room->height(); y++) {
            for (int x = guy_room->pos().x; x < guy_room->pos().x + guy_room->width(); x++) {
                Point pos(y, x);
                if (pos != pos_ && level_->tile_at(pos)->drawn()) {
                    level_->draw_tile_at(pos);
                }
            }
        }
    }

    for (std::vector<Item*>::iterator it = level_->items()->begin(); it != level_->items()->end(); ++it) {
        if (!(*it)->held()) {
            Room* item_room = level_->find_room(*it);
            if ((( level_->tile_at((*it)->pos())->drawn() && item_room != NULL && guy_room != NULL && *item_room == *guy_room ) || sees((*it)->pos()) )) {
                (*it)->draw();
                (*it)->set_seen_pos((*it)->pos());
            }
        } else if (sees((*it)->pos())) {
            (*it)->set_seen_pos(Point());
        }
    }

    for (std::vector<Monster*>::iterator it = level_->monsters()->begin(); it != level_->monsters()->end(); ++it) {
        if (*(*it) != *(level_->world()->guy())) {
//            Room* monster_room = level_->find_room(*it);
            if (sees((*it)->pos())/* || ( level_->tile_at((*it)->pos())->drawn() && monster_room != NULL && guy_room != NULL && *monster_room == *guy_room ) */) {
                (*it)->draw();
            }
        }
    }

    draw();
}

void Guy::walk(Direction::DIRECTION dir) {
    bool stop = false;
    int satiation_lvl = 3;

    if (is_satiated()) {
        satiation_lvl = 4;
    } else if (is_hungry()) {
        satiation_lvl = 2;
    } else if (is_very_hungry()) {
        satiation_lvl = 1;
    } else if (is_starving()) {
        satiation_lvl = 0;
    }

    if (satiation_lvl == 0) {
        level_->world()->status("You're too hungry!", true);
        level_->world()->guy()->focus();
        return;
    }

    do {
        step(dir);
        level_->world()->do_turn();
        int satiation_lvl_prev = satiation_lvl;
        satiation_lvl = 3;

        if (is_satiated()) {
            satiation_lvl = 4;
        } else if (is_hungry()) {
            satiation_lvl = 2;
        } else if (is_very_hungry()) {
            satiation_lvl = 1;
        } else if (is_starving()) {
            satiation_lvl = 0;
        }

        if (satiation_lvl < 3 && satiation_lvl != satiation_lvl_prev) {
            stop = true;
        } else {
            Point pos = level_->world()->get_adjacent_pos(pos_, dir);
            if (level_->tile_at(pos)->type() != Tile::FLOOR) {
                stop = true;
            } else {
                std::vector<Point> points = level_->world()->get_walk_check_points(pos_, level_->world()->opposite_dir(dir));
                points.push_back(pos_);
                for (std::vector<Point>::iterator it = points.begin(); it != points.end(); ++it) {
                    Tile::TYPE type = level_->tile_at(*it)->type();
                    if (type != Tile::FLOOR && type != Tile::WALL)
                        stop = true;
                    for (std::vector<Monster*>::iterator monster_it = level_->monsters()->begin(); !stop && monster_it != level_->monsters()->end(); ++monster_it)
                        if (*(*monster_it) != *(level_->world()->guy()) && sees((*monster_it)->pos()))
                            stop = true;
                    for (std::vector<Item*>::iterator item_it = level_->items()->begin(); !stop && item_it != level_->items()->end(); ++item_it)
                        if (!(*item_it)->held() && (*item_it)->pos() == *it)
                            stop = true;
                }
            }
        }
    } while (!stop);
}

int Guy::wealth() const {
    int w = 0;
    for (std::vector<Item*>::iterator it = inventory_->begin(); it != inventory_->end(); ++it) {
        if ((*it)->type() == Item::GOLD)
            w += (*it)->quantity();
    }
    return w;
}

void Guy::look_at(Point pos, bool detailed) {
    //TODO: look_at monster


    //items:
    bool many = false;
    Item* item = NULL;
    for (std::vector<Item*>::iterator it = level_->items()->begin(); !many && it != level_->items()->end(); ++it) {
        if ((*it)->pos() == pos && !(*it)->held()) {
            if (item != NULL)
                many = true;
            item = *it;
        }
    }

    if (item != NULL) {
        if (many) {
            if (detailed) {
                std::vector<Item*> items = level_->find_items(pos);
                std::vector<std::string> lines = level_->world()->format_items(items);
                level_->world()->show_paginated(lines, "Items");
            } else {
                level_->world()->status("There are several different items here.");
            }
        } else {
            std::ostringstream o;
            o << "There ";
            o << (item->quantity() == 1 ? "is " : "are ");
            o << (item->quantity() == 1 ? item->a_or_an() : Utils::int_to_string(item->quantity())) << ' ';
            o << (item->quantity() == 1 ? item->noun() : item->nouns());
            o << " here.";
            level_->world()->status(o.str(), detailed);
        }
    } else if (detailed) {
        //tile:
        Tile* tile = level_->tile_at(pos);
        switch (tile->type()) {
            case Tile::DOOR: {
                Door* door = static_cast<Door*>(tile);
                Key key(Point(), 1, door->key_type());
                std::ostringstream o;
                o << "There is " << ( door->is_open() ? "an open " : "a closed " ) << "door here.  ";
                o << "The lock takes " << key.a_or_an() << ' ' << key.noun() << '.';

                level_->world()->status(o.str(), true);
                break;
            }
            default:
                level_->world()->status("There's nothing interesting here.", true);
                break;
        }
    }

    level_->world()->guy()->focus();
}

void Guy::pickup_all() {
    std::vector<Item*> items = level_->find_items(pos_);
    for (size_t i = 0; i < items.size(); i++) {
        pickup_top();
        level_->world()->do_turn();
    }
}

void Guy::pickup_items() {
    std::vector<Item*> items = level_->find_items(pos_);
    if (items.size() > 1 || (items.size() == 1 && items.back()->quantity() > 1 && items.back()->category() != Item::MONEY)) {
        int input;
        do {
            Item* item = items.back();
            std::ostringstream q;
            q << "Pick up " << ( item->quantity() == 1 ? item->a_or_an() : Utils::int_to_string(item->quantity()) ) << ' ';
            q << ( item->quantity() == 1 ? item->noun() : item->nouns() ) << "? ([y]/n/#/a/q):";
            level_->world()->status(q.str(), true);
            input = level_->world()->get_input_yes_no_number_all_or_quit();
            switch (input) {
                case (YesNoAllNumberQuit::YES):
                    pickup_item(item);
                    break;
                case (YesNoAllNumberQuit::NUMBER): {
                    int quant = level_->world()->get_input_quantity(0, item->quantity());
                    if (quant == item->quantity())
                        pickup_item(item);
                    else if (quant > 0) {
                        Item* split_item = Item::copy_item(item, item->quantity() - quant);
                        split_item->set_pos(pos_);
                        split_item->set_owner(NULL);
                        level_->items()->push_back(split_item);
                        item->set_quantity(quant);
                        pickup_item(item);
                    }
                    break;
                }
                case (YesNoAllNumberQuit::ALL):
                    pickup_all();
                    return;
            }
            items.pop_back();
            if (input != YesNoAllNumberQuit::NO && input != YesNoAllNumberQuit::QUIT) {
                level_->world()->do_turn();
            }
        } while (items.size() > 0 && input != YesNoAllNumberQuit::QUIT);
    } else {
        pickup_top();
        level_->world()->do_turn();
    }
    level_->world()->guy()->focus();
}

void Guy::drop_inventory_item() {
    stack_inventory_items();
    Item* item = level_->world()->get_input_item(*inventory_, "Drop item");
    if (item != NULL) {
        if (item->quantity() > 1) {
            int quant = level_->world()->get_input_quantity(0, item->quantity());
            if (quant == item->quantity()) {
                drop_item(item);
            } else if (quant > 0) {
                Item* split_item = Item::copy_item(item, item->quantity() - quant);
                level_->items()->push_back(split_item);
                item->set_quantity(quant);
                pickup_item(split_item, true);
                drop_item(item);
            }

            if (quant > 0) {
                std::ostringstream o;
                o << "You drop ";
                o << (quant == 1 ? item->a_or_an() : Utils::int_to_string(quant)) << ' ';
                o << (quant == 1 ? item->noun() : item->nouns()) << '.';
                level_->world()->status(o.str());
            }
        } else {
            drop_item(item);
            std::ostringstream o;
            o << "You drop " << item->a_or_an() << ' ' << item->noun() << '.';
            level_->world()->status(o.str());
        }
        level_->stack_items(pos_);
        level_->world()->guy()->focus();
    }
}

void Guy::equip_item() {
    stack_inventory_items();
    std::vector<Item*> items;
    for (std::vector<Item*>::iterator it = inventory_->begin(); it != inventory_->end(); ++it)
        if ((*it)->is_equipable())
            items.push_back(*it);
    if (items.size() > 0) {
        Item* item = level_->world()->get_input_item(items, "Equip item");

        if (item != NULL) {

            if (left_hand_item_ != NULL && *left_hand_item_ == *item) {
                sheath_left();
                return;
            } else if (right_hand_item_ != NULL && *right_hand_item_ == *item) {
                sheath_right();
                return;
            } else if (armor_ != NULL && *armor_ == *item) {
                take_off_armor();
                return;
            }

            if (item->quantity() > 1) {
                Item* split_item = Item::copy_item(item, item->quantity() - 1);
                level_->items()->push_back(split_item);
                item->set_quantity(1);
                pickup_item(split_item, true);
            }

            item->set_stackable(false);

            std::ostringstream o;
            switch (item->category()) {
                case Item::SHIELD:
                    if (left_hand_item_ != NULL)
                        left_hand_item_->set_stackable(true);
                    left_hand_item_ = item;
                    o << "You wield " << item->a_or_an() << ' ' << item->noun() << " in your left hand.";
                    break;
                case Item::WEAPON:
                    if (right_hand_item_ != NULL)
                        right_hand_item_->set_stackable(true);
                    right_hand_item_ = item;
                    o << "You wield " << item->a_or_an() << ' ' << item->noun() << " in your right hand.";
                    break;
                case Item::ARMOR:
                    if (armor_ != NULL)
                        armor_->set_stackable(true);
                    armor_ = item;
                    o << "You put on " << item->a_or_an() << ' ' << item->noun() << ".";
                    break;
                default:
                    break;
            }
            level_->world()->status(o.str());
        }
    } else {
        level_->world()->status("You have nothing to equip!");
        level_->world()->guy()->focus();
    }
}

void Guy::sheath_left() {
    std::ostringstream o;
    o << "You put away the " << left_hand_item_->noun() << '.';
    level_->world()->status(o.str());
    left_hand_item_->set_stackable(true);
    left_hand_item_ = NULL;
}

void Guy::sheath_right() {
    std::ostringstream o;
    o << "You put away the " << right_hand_item_->noun() << '.';
    level_->world()->status(o.str());
    right_hand_item_->set_stackable(true);
    right_hand_item_ = NULL;
}

void Guy::take_off_armor() {
    std::ostringstream o;
    o << "You take off the " << armor_->noun() << '.';
    level_->world()->status(o.str());
    armor_->set_stackable(true);
    armor_ = NULL;
}

void Guy::hunger() {
    satiation_ -= 1;
    if (satiation_ < 0) {
        hp_ = 0;
        level_->world()->status("You're dead from starvation.");
    }
}

bool Guy::is_satiated() const {
    if (satiation_ > 1000)
        return true;
    return false;
}

bool Guy::is_hungry() const {
    if (satiation_ > 400 && satiation_ <= 600)
        return true;
    return false;
}

bool Guy::is_very_hungry() const {
    if (satiation_ > 200 && satiation_ <= 400)
        return true;
    return false;
}

bool Guy::is_starving() const {
    if (satiation_ > 0 && satiation_ <= 200)
        return true;
    return false;
}

void Guy::eat_item() {
    stack_inventory_items();
    level_->stack_items(pos_);

    std::vector<Item*> items;
    Item* item = NULL;
    std::vector<Item*> floor_items = level_->find_items(pos_);
    bool eat_off_ground = false;

    if (floor_items.size() > 0) {
        for (std::vector<Item*>::iterator it = floor_items.begin(); it != floor_items.end(); ++it)
            if ((*it)->category() == Item::FOOD)
                items.push_back(*it);
        if (items.size() > 0) {
            level_->world()->status("There's some food on the ground here.  Eat off the ground? ([y]/n):", true);
            eat_off_ground = level_->world()->get_input_yes_or_no();
            if (eat_off_ground)
                item = level_->world()->get_input_item(items, "Eat item off ground");
        }
        items.clear();
    }
    if (item == NULL && !eat_off_ground) {
        for (std::vector<Item*>::iterator it = inventory_->begin(); it != inventory_->end(); ++it)
            if ((*it)->category() == Item::FOOD)
                items.push_back(*it);
        if (items.size() > 0) {
            item = level_->world()->get_input_item(items, "Eat item from inventory");
        } else {
            level_->world()->status("You have no food!");
        }
    }

    if (item != NULL) {
        if (item->quantity() > 1) {
            Item* split_item = Item::copy_item(item, item->quantity() - 1);
            split_item->set_pos(pos_);
            level_->items()->push_back(split_item);
            item->set_quantity(1);
            if (!eat_off_ground)
                pickup_item(split_item, true);
        }

        Food* food = static_cast<Food*>(item);

        satiation_ += food->filling();

        if (satiation_ > 1500)
            satiation_ = 1500;

        std::ostringstream o;
        o << "You consume " << food->a_or_an() << ' ' << food->noun() << ".  " << food->taste();

        for (std::vector<Item*>::iterator it = inventory_->begin(); it != inventory_->end(); ++it) {
            if (*(*it) == *item) {
                inventory_->erase(it);
                break;
            }
        }

        for (std::vector<Item*>::iterator it = level_->items()->begin(); it != level_->items()->end(); ++it) {
            if (*(*it) == *item) {
                level_->items()->erase(it);
                break;
            }
        }
        delete item;
        level_->world()->status(o.str());
    }
    level_->world()->guy()->focus();
}

bool Guy::throw_or_shoot() {
    stack_inventory_items();

    bool not_equipped = false;
    if (right_hand_item_ != NULL && right_hand_item_->category() == Item::WEAPON) {
        Weapon* weapon = static_cast<Weapon*>(right_hand_item_);
        if (weapon->weapon_category() == Weapon::RANGED) {
            level_->world()->status("ni", true);
            level_->world()->guy()->focus();
        } else if (weapon->is_throwable()) {
            Point pos = level_->world()->get_nearest_target();
            if (level_->is_valid_point(pos)) {
                throw_at(pos);
                return true;
            } else {
                std::ostringstream o;
                o << "No target!";
                level_->world()->status(o.str(), true);
                level_->world()->guy()->focus();
            }
        } else {
            not_equipped = true;
        }
    } else {
        not_equipped = true;
    }
    if (not_equipped) {
        level_->world()->status("You're not wielding a ranged weapon!");
        return true;
    }
    return false;
}

void Guy::use_item() {
    std::vector<Item*> tools;
    for (std::vector<Item*>::iterator it = inventory_->begin(); it != inventory_->end(); ++it) {
        if ((*it)->category() == Item::TOOL)
            tools.push_back(*it);
    }

    if (tools.size() > 0) {
        Item* item = level_->world()->get_input_item(tools, "Use item");
        if (item != NULL) {
            switch (item->type()) {
                case Item::KEY:
                    use_key(static_cast<Key*>(item));
                    break;
                default:
                    break;
            }
        }
    } else {
        level_->world()->status("You're not carrying any tools!");
    }
}

void Guy::use_key(Key* key) {
    std::vector<Point> points;
    std::vector<Direction::DIRECTION> dirs;
    for (int y = pos_.y - 1; y <= pos_.y + 1; y++) {
        for (int x = pos_.x - 1; x <= pos_.x + 1; x++) {
            Point pos(y, x);
            if (level_->is_valid_point(pos)) {
                Tile* tile = level_->tile_at(pos);
                if (tile->type() == Tile::DOOR) {
                    points.push_back(pos);
                    dirs.push_back(level_->world()->get_relative_dir(pos_, pos));
                }
            }
        }
    }
    if (points.size() > 1) {
        level_->world()->status("Direction: ", true);
        Direction::DIRECTION input_dir = level_->world()->get_input_dir();
        if (input_dir != Direction::NONE) {
            Point pos;
            bool dir_ok = false;
            for (size_t i = 0; i < points.size(); i++) {
                pos = points.at(i);
                if (dirs.at(i) == input_dir) {
                    dir_ok = true;
                    break;
                }
            }
            if (dir_ok) {
                lock_or_unlock(pos, key);
            } else {
                level_->world()->status("There's nothing with a lock in that direction.");
            }
        }
    } else if (points.size() == 1) {
        lock_or_unlock(points.back(), key);
    } else {
        level_->world()->status("There's nothing here with a lock.");
    }
}

void Guy::lock_or_unlock(Point pos, Key* key) {
    Tile* tile = level_->tile_at(pos);
    Door* door = static_cast<Door*>(tile);
    if (pos == pos_) {
        level_->world()->status("You're standing in the way.");
    } else if (door->is_open() && ( tile->passage_blocked() || level_->find_top_item(pos) != NULL )) {
        level_->world()->status("There's something in the way.");
    } else {
        if (key->key_type() == Key::SKELETON || key->key_type() == door->key_type()) {
            if (door->is_open()) {
                level_->world()->status("Try closing the door first.");
            } else {
                if (door->is_locked())
                    door->unlock();
                else
                    door->lock();

                std::ostringstream o;
                o << "You " << (door->is_locked() ? "lock" : "unlock") << " the door.";
                level_->world()->status(o.str());
            }
        } else
            level_->world()->status("This key doesn't fit the lock.");
    }
}

}
