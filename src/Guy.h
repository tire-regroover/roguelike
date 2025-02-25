#ifndef GUY_H_
#define GUY_H_

#include "Monster.h"

namespace Roguelike {

class Guy : public Roguelike::Monster {
private:
    int satiation_;

    std::vector<Point> get_adjacent_doors();
    void open_or_close(bool open);
    void open_or_close(Point pos, bool open);
    void sheath_left();
    void sheath_right();
    void take_off_armor();

public:
    Guy(Level* level, std::string name = std::string());
    virtual ~Guy();
    virtual std::string verb() const;
    virtual std::string verbs() const;
    virtual std::string verbed() const;

    void rest();

    void open();
    void close();
    void do_fov();
    void walk(Direction::DIRECTION dir);
    int wealth() const;
    int satiation() const { return satiation_; }
    void pickup_all();
    void pickup_items();
    void drop_inventory_item();
    void equip_item();
    void look_at(Point pos, bool detailed = false);

    void hunger();

    bool is_satiated() const;
    bool is_hungry() const;
    bool is_very_hungry() const;
    bool is_starving() const;

    void eat_item();

    bool throw_or_shoot();

    void use_item();
    void use_key(Key* key);
    void lock_or_unlock(Point pos, Key* key);
};

}

#endif /* GUY_H_ */
