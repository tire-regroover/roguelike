#include "Monster.h"
#include <cmath>
#include "Food.h"
#include "Weapon.h"
#include "Armor.h"
#include "World.h"

namespace Roguelike {

unsigned long Monster::top_id_ = 0;

Monster::Monster(Level* level, std::string name) : attack_dice_(1, 4) {
    id_ = top_id_++;
    level_ = level;
    name_ = name;
    pos_ = Point();

    strength_ = 1;
    dexterity_ = 1;
    hp_ = 1;
    hp_max_ = 1;
    xp_ = 1;
    lvl_ = 1;
    kill_worth_ = 1;
    base_attack_ = 0;

    breathe_every_ = 0;

    breathe_offset_ = Utils::random_int(0, 9);

    hostile_ = false;

    good_ = false;

    can_open_doors_ = false;
    chasing_guy_ = false;
    give_up_chase_turns_ = 10;
    saw_guy_turns_ago_ = give_up_chase_turns_;
    eyesight_ = 10;

    inventory_ = new std::vector<Item*>;

    left_hand_item_ = NULL;
    right_hand_item_ = NULL;
    armor_ = NULL;

    drops_corpse_ = Utils::percent_chance(50);
    corpse_ = NULL;

    thrower_ = false;
}

Monster::~Monster() {
    delete inventory_;
}

std::string Monster::verb() const {
    return "<attack>";
}

std::string Monster::verbs() const {
    return "<attack>";
}

std::string Monster::verbed() const {
    return "<attacked>";
}


void Monster::draw() {
    attron(colour_);
    mvaddch(pos_.y + 3, pos_.x + 1, symbol_);
    attroff(colour_);
    focus();
    refresh();
}

void Monster::focus() {
    move(pos_.y + 3, pos_.x + 1);
}

void Monster::set_pos(Point pos) {
    pos_ = pos;
    level_->tile_at(pos)->set_passage_blocked(true);
}

void Monster::step_toward(Point dest) {
    bool found = false;
    std::vector<Point> path = level_->world()->find_path(this, pos_, dest, true, true);
    if (path.size() > 0 && (!level_->tile_at(path.at(0))->passage_blocked() || level_->world()->guy()->pos() == path.at(0)))
        found = true;
    else {
        path = level_->world()->find_path(this, pos_, dest, true, false);
        if (path.size() > 0 && (!level_->tile_at(path.at(0))->passage_blocked() || level_->world()->guy()->pos() == path.at(0)))
            found = true;
    }
    if (found)
        step(level_->world()->get_relative_dir(pos_, path.at(0)));
    else
        step();
}

void Monster::step(bool avoid_walls) {
    int tries = 20;
    bool ok;
    do {
        ok = false;
        switch (Utils::random_int(0, 7)) {
            case 0:
                ok = step(Direction::NORTH);
                break;
            case 1:
                ok = step(Direction::NORTHEAST);
                break;
            case 2:
                ok = step(Direction::EAST);
                break;
            case 3:
                ok = step(Direction::SOUTHEAST);
                break;
            case 4:
                ok = step(Direction::SOUTH);
                break;
            case 5:
                ok = step(Direction::SOUTHWEST);
                break;
            case 6:
                ok = step(Direction::WEST);
                break;
            case 7:
                ok = step(Direction::NORTHWEST);
                break;
        }
    } while (avoid_walls && !ok && --tries > 0);
}

bool Monster::step(Direction::DIRECTION dir) {
    if (type_ != GUY) {
        if (Utils::percent_chance(20)) {
            Item* item = level_->find_top_item(pos_);
            if (item != NULL && !(item->category() == Item::FOOD && (static_cast<Food*>(item))->food_type() == Food::CORPSE)) {
                pickup_top();
                stack_inventory_items();
                return true;
            }
        }

        bool equipped = false;
        for (std::vector<Item*>::iterator it = inventory_->begin(); !equipped && it != inventory_->end(); ++it) {
            if ((*it)->category() == Item::ARMOR) {
                Armor* armor = static_cast<Armor*>((*it));
                if (armor_ == NULL) {
                    if (armor->quantity() > 1) {
                        Item* split_item = Item::copy_item(armor, armor->quantity() - 1);
                        level_->items()->push_back(split_item);
                        armor->set_quantity(1);
                        pickup_item(split_item, true);
                    }
                    set_armor(armor);
                    if (level_->world()->guy()->sees(pos_)) {
                        std::ostringstream o;
                        o << Utils::capitalize(name_) << " puts on " << armor->a_or_an() << ' ' << armor->noun() << '.';
                        level_->world()->status(o.str());
                    }
                    equipped = true;
                } else {
                    Armor* current = static_cast<Armor*>(armor_);

                    if (armor->ac() > current->ac()) {
                        armor_->set_stackable(true);
                        if (armor->quantity() > 1) {
                            Item* split_item = Item::copy_item(armor, armor->quantity() - 1);
                            level_->items()->push_back(split_item);
                            armor->set_quantity(1);
                            pickup_item(split_item, true);
                        }
                        set_armor(armor);
                        if (level_->world()->guy()->sees(pos_)) {
                            std::ostringstream o;
                            o << Utils::capitalize(name_) << " puts on " << armor->a_or_an() << ' ' << armor->noun() << '.';
                            level_->world()->status(o.str());
                        }
                        equipped = true;
                    }
                }
            }
            else if ((*it)->category() == Item::WEAPON) {
                Weapon* weapon = static_cast<Weapon*>(*it);
                if (right_hand_item_ == NULL) { //TODO:: BUG
                    if (( weapon->dice().worth() >= attack_dice_.worth() )
                            && ( ( thrower_ && weapon->is_throwable()) || ( !thrower_ && !weapon->is_throwable() ) )) {
                        if (weapon->quantity() > 1) {
                            Item* split_item = Item::copy_item(weapon, weapon->quantity() - 1);
                            level_->items()->push_back(split_item);
                            weapon->set_quantity(1);
                            pickup_item(split_item, true);
                        }
                        set_right_hand_item(weapon);
                        if (level_->world()->guy()->sees(pos_)) {
                            std::ostringstream o;
                            o << Utils::capitalize(name_) << " wields " << weapon->a_or_an() << ' ' << weapon->noun() << '.';
                            level_->world()->status(o.str());
                        }
                        equipped = true;
                    }
                } else {
                    if (( weapon->dice().worth() > static_cast<Weapon*>(right_hand_item_)->dice().worth() )
                            && ( ( thrower_ && weapon->is_throwable()) || ( !thrower_ && !weapon->is_throwable() ) )) {
                        right_hand_item_->set_stackable(true);
                        if (weapon->quantity() > 1) {
                            Item* split_item = Item::copy_item(weapon, weapon->quantity() - 1);
                            level_->items()->push_back(split_item);
                            weapon->set_quantity(1);
                            pickup_item(split_item, true);
                        }
                        set_right_hand_item(weapon);
                        if (level_->world()->guy()->sees(pos_)) {
                            std::ostringstream o;
                            o << Utils::capitalize(name_) << " wields " << weapon->a_or_an() << ' ' << weapon->noun() << '.';
                            level_->world()->status(o.str());
                        }
                        equipped = true;
                    }
                }
            } else if ((*it)->category() == Item::SHIELD) {
                Shield* shield = static_cast<Shield*>((*it));
                if (left_hand_item_ == NULL) {
                    if (shield->quantity() > 1) {
                        Item* split_item = Item::copy_item(shield, shield->quantity() - 1);
                        level_->items()->push_back(split_item);
                        shield->set_quantity(1);
                        pickup_item(split_item, true);
                    }
                    set_left_hand_item(shield);
                    if (level_->world()->guy()->sees(pos_)) {
                        std::ostringstream o;
                        o << Utils::capitalize(name_) << " wields " << shield->a_or_an() << ' ' << shield->noun() << '.';
                        level_->world()->status(o.str());
                    }
                    equipped = true;
                } else {
                    Shield* current = static_cast<Shield*>(left_hand_item_);

                    if (shield->ac() > current->ac()) {
                        left_hand_item_->set_stackable(true);
                        if (shield->quantity() > 1) {
                            Item* split_item = Item::copy_item(shield, shield->quantity() - 1);
                            level_->items()->push_back(split_item);
                            shield->set_quantity(1);
                            pickup_item(split_item, true);
                        }
                        set_left_hand_item(shield);
                        if (level_->world()->guy()->sees(pos_)) {
                            std::ostringstream o;
                            o << Utils::capitalize(name_) << " wields " << shield->a_or_an() << ' ' << shield->noun() << '.';
                            level_->world()->status(o.str());
                        }
                        equipped = true;
                    }
                }
            }
        }

        if (equipped)
            return true;
    }

    Point pos = level_->world()->get_adjacent_pos(pos_, dir);
    Tile* tile = level_->tile_at(pos);

    bool move = true;

    if (tile->passage_blocked()) {
        move = false;
        bool did_attack = false;
        for (std::vector<Monster*>::iterator it = level_->monsters()->begin(); it != level_->monsters()->end(); ++it) {
            if ((*it)->pos() == pos) {
                if (!are_allies(this, *it)) {
                    bool threw = false;
                    if (right_hand_item_ != NULL && static_cast<Weapon*>(right_hand_item_)->is_throwable()) {
                        throw_at(pos);
                        did_attack = true;
                        threw = true;
                    }

                    if (!did_attack) {
                        attack(*it);
                        did_attack = true;
                    }

                    if (!threw && (*it)->is_dead()) {
                        level_->splatter_gore(*it, dir);
                    }
                }
                break;
            }
        }

        if (type_ == GUY) {
            if (!did_attack) {
                switch (tile->type()) {
                    case Tile::WALL:
                        level_->world()->status("You run into a wall.");
                        if (Utils::percent_chance(10)) {
                            hp_ -= Utils::random_int(1, 3);
                            level_->world()->status("You injure yourself.  Klutz.");
                        }
                        break;
                    case Tile::DOOR:
                        level_->world()->status("You run into a closed door.");
                        if (Utils::percent_chance(10)) {
                            hp_ -= Utils::random_int(1, 3);
                            level_->world()->status("You injure yourself.  Try opening the door first.");
                        }
                        break;
                    default:
                        break;
                }
            }
        } else {
            if (!did_attack) {
                switch (tile->type()) {
                    case Tile::DOOR: {
                        Door* door = static_cast<Door*>(tile);
                        if (!door->is_open() && can_open_doors_) {
                            door->open();
                            if (level_->world()->guy()->sees(pos)) {
                                level_->draw_tile_at(pos);
                                level_->world()->status(Utils::capitalize(name_) + " opens the door.");
                            }
                        }
                        break;
                    }
                    default:
                        return false;
                }
            }

        }
    }

    if (move) {
        Tile* old_tile = level_->tile_at(pos_);
        old_tile->set_passage_blocked(false);

//        Room* guy_room = level_->find_room(level_->world()->guy());
//        Room* this_room = level_->find_room(this);

        if (level_->world()->guy()->sees(pos_) || old_tile->drawn()) {
//            || ( old_tile->drawn() && guy_room != NULL && this_room != NULL && *guy_room == *this_room )) {
            level_->draw_tile_at(pos_);

            if (old_tile->drawn()) {
                for (std::vector<Item*>::iterator it = level_->items()->begin(); it != level_->items()->end(); ++it) {
                    if ((*it)->seen_pos() == pos_)
                        (*it)->draw_at_seen_pos();
                }
            }
        }

        if (type_ == GUY) {
            level_->world()->guy()->look_at(pos);
        }

        set_pos(pos);
    }

    return true;
}

bool Monster::sees(Point pos, int eyesight_adjust) const {
    if (pos == pos_) return true;
    int eyesight = eyesight_ + eyesight_adjust;
    std::vector<Point> line = level_->world()->bresenham_path(pos_, pos);
    if ((int)line.size() > 0 && (int)line.size() <= eyesight + 1) {
        bool ok = true;
        for (std::vector<Point>::iterator it = line.begin(); ok && it != line.end() - 1; ++it) {
            if (!level_->is_valid_point(*it) || level_->tile_at(*it)->sight_blocked())
                ok = false;
        }
        return ok;
    } else {
        return false;
    }
}

void Monster::look_for_guy() {
    Point guy_pos = level_->world()->guy()->pos();
    chasing_guy_ = sees(guy_pos);

    if (chasing_guy_) {
        saw_guy_turns_ago_ = 0;
    } else {
        saw_guy_turns_ago_++;
    }

    if (saw_guy_turns_ago_ < give_up_chase_turns_) {
        chasing_guy_ = true;
    }
}

void Monster::heal() {
    hp_ += 1;
    if (hp_ > hp_max_)
        hp_ = hp_max_;
}


void Monster::regenerate() {
    switch (type_) {
        case ZOMBIE:
        case VAMPIRE:
            if (level_->world()->turns() % 7 == 0) {
                heal();
            }
            break;
        case GUY:
            if (level_->world()->turns() % 5 == 0) {
                heal();
                if (level_->world()->guy()->is_satiated())
                    heal();
            }
            break;
        default:
            break;
    }
}

unsigned long Monster::next_lvl_xp() const {
    int xp = 0;
    for (int level2 = 1; level2 < lvl_ + 1; ++level2) {
        xp += (int) (std::pow(2, level2 / 7.0) * 300 + level2);
    }
    return xp / 4;
}

void Monster::level_up() {
    int hp_inc = hp_max_ * 0.1;
    if (hp_inc == 0)
        hp_inc = 1;

    int strength_inc = strength_ * 0.1;
    if (strength_inc == 0)
        strength_inc = 1;

    int dexterity_inc = dexterity_ * 0.1;
    if (dexterity_inc == 0)
        dexterity_inc = 1;

    int base_attack_inc = base_attack_ * 0.1;
    if (base_attack_inc == 0)
        base_attack_inc = 1;

    hp_max_ += hp_inc;
    hp_ += hp_inc;
    if (hp_ > hp_max_)
        hp_ = hp_max_;

    strength_ += strength_inc;
    dexterity_ += dexterity_inc;
    base_attack_ += base_attack_inc;

    lvl_++;

    std::ostringstream o;
    if (type_ == GUY) {
        o << "You've advanced to level " << lvl() << '!';
        level_->world()->status(o.str());
    }
}

void Monster::attack(Monster* victim) {
    victim->take_hit(this);
    if (victim->is_dead()) {
        xp_ += victim->kill_worth();
        while (xp_ >= next_lvl_xp()) {
            level_up();
        }
    }
}

void Monster::take_hit(Monster* attacker) {
    int ar = attacker->base_attack();

    int dr = 10 + dexterity_;

    if (armor_ != NULL && armor_->category() == Item::ARMOR) {
        dr += (static_cast<Armor*>(armor_))->ac();
    }

    if (left_hand_item_ != NULL && left_hand_item_->category() == Item::SHIELD)
        dr += (static_cast<Shield*>(left_hand_item_))->ac();

    int roll = Utils::random_int(1, 20);

    Weapon* weapon = NULL;

    int damage;
    if (attacker->right_hand_item() != NULL && attacker->right_hand_item()->category() == Item::WEAPON) {
        weapon = static_cast<Weapon*>(attacker->right_hand_item());

        if (weapon->weapon_category() == Weapon::RANGED) {
            ar += attacker->dexterity();

            if (armor_ != NULL && armor_->category() == Item::ARMOR)
                ar -= (static_cast<Armor*>(armor_))->dexterity_penalty();
            if (left_hand_item_ != NULL && left_hand_item_->category() == Item::SHIELD)
                ar -= (static_cast<Shield*>(left_hand_item_))->dexterity_penalty();

        } else if (weapon->weapon_category() == Weapon::MELEE)
            ar += attacker->strength();

        damage = weapon->dice().roll();
    } else {
        ar += attacker->strength();
        damage = attacker->attack_dice().roll();
    }

    if (roll + ar > dr) {
        hp_ -= damage;

        std::ostringstream o;
        if (type_ == GUY) {
            o << ( sees(attacker->pos()) ? Utils::capitalize(attacker->name()) : "Something" ) << ' ';
            if (weapon == NULL)
                o << attacker->verbs() << " you";
            else if (weapon->is_throwable())
                o << weapon->verbs() << ' ' << weapon->a_or_an() << ' ' << weapon->noun() << " at you";
            else
                o << weapon->verbs() << " you with its " << weapon->noun();

            o << ".  <`R:You lose " << damage << " hp.`>";
            level_->world()->status(o.str());
            if (hp_ <= 0) {
                level_->world()->status("<`r:You're dead.`>");
            }
        } else if (attacker->type() == GUY) {
            o << "You ";
            if (weapon == NULL)
                o << level_->world()->guy()->verb() << ' ' << name_;
            else if (weapon->is_throwable()) {
                o << weapon->verb() << ' ' << weapon->a_or_an() << ' ' << weapon->noun() << " at " << name_;
            }
            else {
                o << weapon->verb() << ' ' << name_ << " with your " << weapon->noun();
            }
            o << ".  <`G:You deal " << damage << " damage.`>";
            level_->world()->status(o.str());
            if (hp_ <= 0) {
                o.str("");
                o << "You have killed " << name_ << ".";
                level_->world()->status(o.str());
            }
        } else {
            if (level_->world()->guy()->sees(pos_)) {
                o << Utils::capitalize(attacker->name()) << ' ';
                if (weapon == NULL)
                    o << attacker->verbs() << ' ' << name_ ;
                else if (weapon->is_throwable())
                    o << weapon->verbs() << ' ' << weapon->a_or_an() << ' ' << weapon->noun() << " at " << name_;
                else
                    o << weapon->verbs() << ' ' << name_ << ' ' << " with its " << weapon->noun();
                o << '.';
                level_->world()->status(o.str());
                if (hp_ <= 0) {
                    o.str("");
                    o << Utils::capitalize(attacker->name()) << " has killed " << name_ << ".";
                    level_->world()->status(o.str());
                }
            }
        }

    } else {
        std::ostringstream o;
        if (type_ == GUY) {
            o << Utils::capitalize(attacker->name()) << " tries to ";
            if (weapon == NULL)
                o << attacker->verb() << " you";
            else if (weapon->is_throwable())
                o << weapon->verb() << ' ' << weapon->a_or_an() << ' ' << weapon->noun() << " at you";
            else
                o << weapon->verb() << " you with its " << weapon->noun();
            o << ".  You deflect the attack.";
            level_->world()->status(o.str());
        } else if (attacker->type() == GUY) {
            if (weapon == NULL)
                o << "You try to " << attacker->verb() << ' ' << name_;
            else if (weapon->is_throwable())
                o << "You " << weapon->verb() << ' ' << weapon->a_or_an() << ' ' << weapon->noun() << " at " << name_;
            else
                o << "You try to " << weapon->verb() << ' ' << name_ << " with your " << weapon->noun();
            o << ".  Your attack is deflected.";
            level_->world()->status(o.str());
        } else {
            if (level_->world()->guy()->sees(pos_)) {
                if (weapon == NULL)
                    o << Utils::capitalize(name_) << " tries to " << attacker->verb() << ' ' << name_;
                else if (weapon->is_throwable())
                    o << Utils::capitalize(name_) << ' ' << weapon->verbs() << ' ' << weapon->a_or_an() << ' ' << weapon->noun() << " at " << name_;
                else
                    o << Utils::capitalize(name_) << " tries to " << weapon->verb() << ' ' << name_ << " with its " << weapon->noun();
                o << " but misses.";
                level_->world()->status(o.str());
            }
        }

    }
}

void Monster::pickup_item(Item* item, bool quiet) {
    if (type_ == GUY && !quiet) {
        std::ostringstream o;
        o << "You pick up ";
        o << ( item->quantity() == 1 ? item->a_or_an() : Utils::int_to_string(item->quantity()) ) << ' ';
        o << ( item->quantity() == 1 ? item->noun() : item->nouns() ) << '.';
        level_->world()->status(o.str());
    }

    if (level_->world()->guy() != NULL && level_->world()->guy()->sees(pos_)) {
        item->set_seen_pos(Point());
        if (type_ != GUY && !quiet) {
            std::ostringstream o;
            o << Utils::capitalize(name_) << " picks up ";
            o << ( item->quantity() == 1 ? item->a_or_an() : Utils::int_to_string(item->quantity()) ) << ' ';
            o << ( item->quantity() == 1 ? item->noun() : item->nouns() ) << '.';
            level_->world()->status(o.str());
        }
    }
    item->set_owner(this);
    item->set_pos(Point());
    inventory_->push_back(item);
}

void Monster::pickup_top() {
    Item* item = level_->find_top_item(pos_);
    if (item == NULL) {
        if (type_ == GUY)
            level_->world()->status("There's nothing here to pick up.");
    } else {
        pickup_item(item);
    }
}

void Monster::drop_item(Item* drop_item) {
    //re-add:
    for (std::vector<Item*>::iterator level_items_it = level_->items()->begin(); level_items_it != level_->items()->end();) {
        if (*(*level_items_it) == *drop_item) {
            Item* re_add_item = *level_items_it;
            level_->items()->erase(level_items_it);
            level_->items()->push_back(re_add_item);
            break;
        } else {
            ++level_items_it;
        }
    }

    drop_item->set_owner(NULL);
    drop_item->set_pos(pos_);
    drop_item->set_stackable(true);

    for (std::vector<Item*>::iterator inventory_it = inventory_->begin(); inventory_it != inventory_->end(); ++inventory_it) {
        if (*(*inventory_it) == *drop_item) {
            inventory_->erase(inventory_it);
            break;
        }
    }
}

void Monster::drop_all() {
    for (std::vector<Item*>::iterator it = inventory_->begin(); it != inventory_->end();)
        drop_item(*it);
    level_->stack_items(pos_);
}

void Monster::drop_corpse() {
    drop_item(corpse_);
    corpse_ = NULL;
    level_->stack_items(pos_);
}

void Monster::set_left_hand_item(Item* item) { left_hand_item_ = item; item->set_stackable(false); }
void Monster::set_right_hand_item(Item* item) { right_hand_item_ = item; item->set_stackable(false); }
void Monster::set_armor(Item* item) { armor_ = item; item->set_stackable(false); }

void Monster::throw_at(Point dest) {
    std::vector<Point> path = level_->world()->bresenham_path(pos_, dest);
    Item* throw_item = right_hand_item_;

    Monster* victim = level_->monster_at(dest);
    if (victim != NULL) {
        attack(victim);
    }

    drop_item(throw_item);
    right_hand_item_ = NULL;

    for (std::vector<Point>::iterator it = path.begin(); it != path.end(); ++it) {
        throw_item->set_pos(*it);
        level_->world()->guy()->do_fov();
        refresh();
        Utils::sleep(37);
    }

    if (victim != NULL && victim->is_dead()) {
        level_->splatter_gore(victim, level_->world()->get_relative_dir(pos_, dest));
    }

    level_->stack_items(dest);

    bool no_more = true;
    for (std::vector<Item*>::iterator it = inventory_->begin(); it != inventory_->end(); ++it) {
        if ((*it)->type() == throw_item->type()) {
            if ((*it)->quantity() > 1) {
                Item* split_item = Item::copy_item((*it), (*it)->quantity() - 1);
                level_->items()->push_back(split_item);
                (*it)->set_quantity(1);
                pickup_item(split_item, true);
            }

            set_right_hand_item((*it));

            if (type_ == GUY) {
                std::ostringstream o;
                o << "You equip another " << (*it)->noun() << '.';
                level_->world()->status(o.str());
            }

            no_more = false;
            break;
        }
    }

    if (type_ == GUY && no_more)
        level_->world()->status("You're out of ammo!");

}


void Monster::stack_inventory_items() {
    std::vector<Item*> unique_items;
    for (std::vector<Item*>::iterator it = inventory_->begin(); it != inventory_->end(); ++it) {
        if ((*it)->is_stackable()) {
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
        for (std::vector<Item*>::iterator it = inventory_->begin(); it != inventory_->end();) {
            if ((*it)->is_stackable() && *(*it) != *(*unique_it) && (*unique_it)->noun() == (*it)->noun()) {
                (*unique_it)->set_quantity((*unique_it)->quantity() + (*it)->quantity());
                inventory_->erase(it);
            } else {
                ++it;
            }
        }
    }
}

void Monster::add_fresh_inventory(Item* item) {
    level_->items()->push_back(item);
    pickup_item(item, true);
}

}
