#ifndef ITEM_H_
#define ITEM_H_

//#include "Monster.h"
#include "Room.h"
#include "Utils.h"
#include <string>

namespace Roguelike {

class Monster;

class Item {
public:
    enum TYPE {
        NONE,
        GOLD,
        KEY,
        GLOWING_ORB,
        WEAPON_DAGGER,
        WEAPON_DAGGER_PLUS_ONE,
        WEAPON_SHORT_SWORD,
        WEAPON_SHORT_SWORD_PLUS_ONE,
        WEAPON_LONG_SWORD,
        WEAPON_LONG_SWORD_PLUS_ONE,
        WEAPON_BASTARD_SWORD,
        WEAPON_ROCK,
        WEAPON_FUNBALL,
        WEAPON_SHORT_BOW,
        SHIELD_SMALL_WOODEN,
        SHIELD_SMALL,
        SHIELD_MEDIUM,
        ARMOR_LEATHER,
        ARMOR_STUDDED_LEATHER,
        ARMOR_CHAIN_MAIL,
        FOOD_ZOMBIE_CORPSE,
        FOOD_MUMMY_CORPSE,
        FOOD_VAMPIRE_CORPSE,
        FOOD_WOLFMAN_CORPSE,
        FOOD_MACHINE_ELF_CORPSE,
        FOOD_ENTRAILS
    };

    enum ITEM_CATEGORY {
        NO_CATEGORY,
        TOOL,
        PLOT,
        MONEY,
        WEAPON,
        SHIELD,
        ARMOR,
        FOOD
    };

protected:
    static unsigned long top_id_;
    unsigned long id_;

    int quantity_;

    TYPE type_;
    char symbol_;
    Colour::COLOUR colour_;

    ITEM_CATEGORY category_;

    bool stackable_;

    Point pos_;
    bool held_;
    Monster* owner_;

    bool equipable_;

    Point seen_pos_;

    std::string a_or_an_;
    std::string noun_;
    std::string nouns_;

public:
    Item(Point pos, int number);
    virtual ~Item();


    static Item* copy_item(Item* item, int quantity);
    void draw();
    void draw_at_seen_pos();

    TYPE type() const { return type_; }
    Colour::COLOUR colour() const { return colour_; }
    ITEM_CATEGORY category() const { return category_; }
    unsigned long id() const { return id_; }

    int quantity() const { return quantity_; }
    void set_quantity(int quantity) { quantity_ = quantity; }

    const std::string& a_or_an() const { return a_or_an_; }
    const std::string& noun() const { return noun_; }
    const std::string& nouns() const { return nouns_; }

    Point pos() const { return pos_; }
    void set_pos(Point pos) { pos_ = pos; }

    Point seen_pos() const { return seen_pos_; }
    void set_seen_pos(Point seen_pos) { seen_pos_ = seen_pos; }

    bool held() const { return held_; }
    bool is_equipable() const { return equipable_; }

    bool is_stackable() const { return stackable_; }
    void set_stackable(bool stackable) { stackable_ = stackable; }

    Monster* owner() const { return owner_; }
    void set_owner(Monster* owner);

    virtual const bool operator==(Item& other) const {
        return (id_ == other.id());
    }

    virtual const bool operator!=(Item& other) const {
        return (id_ != other.id());
    }

    virtual const bool operator<(Item other) const {
        return (noun_ < other.noun());
    }
};

class Gold : public Item {
private:
public:
    Gold(Point pos, int number);
    virtual ~Gold();
};

class Tool : public Item {
private:
public:
    Tool(Point pos, int number);
    virtual ~Tool();
};


class Key : public Tool {
public:
    enum KEY_TYPE {
        NONE,
        SKELETON,
        GREEN,
        RED,
        BLUE
    };

private:
    KEY_TYPE key_type_;

public:
    Key(Point pos, int number, KEY_TYPE key_type);
    virtual ~Key();

    KEY_TYPE key_type() const { return key_type_; }

    static KEY_TYPE random_type();
};


class PlotItem : public Item {
private:
public:
    PlotItem(Point pos, int number);
    virtual ~PlotItem();
};

class GlowingOrb : public PlotItem {
private:
public:
    GlowingOrb(Point pos, int number);
    virtual ~GlowingOrb();
};

}
#endif /* ITEM_H_ */
