#ifndef ARMOR_H_
#define ARMOR_H_

#include "Item.h"

namespace Roguelike {

class Shield : public Item {
protected:
    int ac_;
    int dexterity_penalty_;
public:
    Shield(Point pos, int number);
    virtual ~Shield();
    int ac() const { return ac_; }
    int dexterity_penalty() const { return dexterity_penalty_; }
};

class SmallWoodenShield : public Shield {
protected:
public:
    SmallWoodenShield(Point pos, int number);
    virtual ~SmallWoodenShield();
};

class SmallShield : public Shield {
protected:
public:
    SmallShield(Point pos, int number);
    virtual ~SmallShield();
};

class MediumShield : public Shield {
protected:
public:
    MediumShield(Point pos, int number);
    virtual ~MediumShield();
};


class Armor : public Item {
protected:
    int ac_;
    int dexterity_penalty_;
public:
    Armor(Point pos, int number);
    virtual ~Armor();
    int ac() const { return ac_; }
    int dexterity_penalty() const { return dexterity_penalty_; }
};

class LeatherArmor : public Armor {
protected:
public:
    LeatherArmor(Point pos, int number);
    virtual ~LeatherArmor();
};

class StuddedLeatherArmor : public Armor {
protected:
public:
    StuddedLeatherArmor(Point pos, int number);
    virtual ~StuddedLeatherArmor();
};

class ChainMail : public Armor {
protected:
public:
    ChainMail(Point pos, int number);
    virtual ~ChainMail();
};

}

#endif /* ARMOR_H_ */
