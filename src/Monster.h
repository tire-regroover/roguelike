#ifndef MONSTER_H_
#define MONSTER_H_

#include "Utils.h"
#include "Room.h"
#include "Level.h"
#include <vector>

namespace Roguelike {

class Level;
class Item;
class Weapon;
class Corpse;

class Monster {
public:
    enum TYPE {
        GUY,
        ZOMBIE,
        MUMMY,
        VAMPIRE,
        WOLFMAN,
        MACHINE_ELF,
        BABY_MACHINE_ELF
    };

protected:
    static unsigned long top_id_;
    unsigned long id_;

    Level* level_;

    char symbol_;
    std::string name_;
    TYPE type_;
    Colour::COLOUR colour_;
    Point pos_;

    int hp_;
    int hp_max_;
    int strength_;
    int dexterity_;
    unsigned long xp_;
    int lvl_;
    int kill_worth_;

    int base_attack_;

    bool good_;

    bool hostile_;
    bool can_open_doors_;
    bool chasing_guy_;
    int saw_guy_turns_ago_;
    int give_up_chase_turns_;
    int eyesight_;

    bool drops_corpse_;

    void eval_move(Point pos);
    void eval_move(int py, int px);
    void heal();

    std::vector<Item*>* inventory_;
    Dice attack_dice_;

    Item* left_hand_item_;
    Item* right_hand_item_;
    Item* armor_;

    Corpse* corpse_;

    int breathe_every_;
    int breathe_offset_;

    bool thrower_;

public:
    Monster(Level* level, std::string name = std::string());
    virtual ~Monster();

    void stack_inventory_items();

    bool is_thrower() const {
        return thrower_;
    }

    bool drops_corpse() const {
        return drops_corpse_;
    }

    void drop_corpse();

    const Dice& attack_dice() const { return attack_dice_; }

    Item* left_hand_item() const { return left_hand_item_; }
    Item* right_hand_item() const { return right_hand_item_; }
    Item* armor() const { return armor_; }

    void set_left_hand_item(Item* item);
    void set_right_hand_item(Item* item);
    void set_armor(Item* item);

    virtual std::string verb() const;
    virtual std::string verbs() const;
    virtual std::string verbed() const;

    void draw();

    void focus();

    void set_pos(Point pos);

    Point pos() const {
        return pos_;
    }

    unsigned long id() const {
        return id_;
    }

    const TYPE type() const {
        return type_;
    }

    const int symbol() const {
        return symbol_;
    }

    const std::string name() const {
        return name_;
    }

    int eyesight() const {
        return eyesight_;
    }

    int breathe_every() const {
        return breathe_every_;
    }

    int breathe_offset() const {
        return breathe_offset_;
    }

    int hp() const {
        return hp_;
    }

    int hp_max() const {
        return hp_max_;
    }

    int strength() const {
        return strength_;
    }

    int dexterity() const {
        return dexterity_;
    }

    int base_attack() const {
        return base_attack_;
    }

    unsigned long xp() const {
        return xp_;
    }

    int lvl() const { return lvl_; }
    unsigned long next_lvl_xp() const;

    int kill_worth() const {
        return kill_worth_;
    }

    bool is_good() { return good_; }

    bool can_open_doors() const {
        return can_open_doors_;
    }

    bool chasing_guy() const {
        return chasing_guy_;
    }

    void step_toward(Point dest);
    void step(bool avoid_walls = true);
    bool step(Direction::DIRECTION dir);

    void level_up();
    void attack(Monster* victim);
    void take_hit(Monster* attacker);

    Direction::DIRECTION get_relative_dir(Point dest);

    bool is_hostile() const {
        return hostile_;
    }

    void set_hostile(bool hostile) {
        hostile_ = hostile;
    }

    bool is_dead() const {
        return (hp_ <= 0);
    }

    Colour::COLOUR colour() const {
        return colour_;
    }

    Level* level() {
        return level_;
    }

    void set_level(Level* level) { level_ = level; }

    static bool are_allies(Monster* m1, Monster* m2) {
        if (m1->is_good() == m2->is_good()) return true;
        return false;
    }

    bool sees(Point pos, int eyesight_adjust = 0) const;

    void look_for_guy();

    void regenerate();

    std::vector<Item*>* inventory() { return inventory_; }

    void pickup_item(Item* item, bool quiet = false);
    void pickup_top();
    void drop_item(Item* drop_item);
    void drop_all();

    void throw_at(Point dest);

    void add_fresh_inventory(Item* item);

    const bool operator==(Monster& other) const {
        return (id_ == other.id());
    }

    const bool operator!=(Monster& other) const {
        return (id_ != other.id());
    }
};

}

#endif /* MONSTER_H_ */
