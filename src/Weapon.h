#ifndef WEAPON_H_
#define WEAPON_H_

#include "Item.h"
#include "Utils.h"

namespace Roguelike {

class Weapon : public Item {
protected:
    Dice dice_;
    int weapon_category_;
    bool throwable_;
public:
    Weapon(Point pos, int number);
    virtual ~Weapon();

    enum WEAPON_CATEGORY {
        MELEE,
        RANGED
    };

    int weapon_category() const { return weapon_category_; }
    bool is_throwable() const { return throwable_; }

    const Dice& dice() const { return dice_; }
    virtual std::string verb() const;
    virtual std::string verbs() const;
};


class ThrowableWeapon : public Weapon {
protected:
public:
    ThrowableWeapon(Point pos, int number);
    virtual ~ThrowableWeapon();
    virtual std::string verb() const;
    virtual std::string verbs() const;
};


class Rock : public ThrowableWeapon {
protected:
public:
    Rock(Point pos, int number);
    virtual ~Rock();
};


class FunBall : public ThrowableWeapon {
protected:
public:
    FunBall(Point pos, int number, Colour::COLOUR colour_ = Colour::NO_COLOUR);
    virtual ~FunBall();
};


class Dagger : public Weapon {
protected:
public:
    Dagger(Point pos, int number);
    virtual ~Dagger();
    virtual std::string verb() const;
};

class DaggerPlusOne : public Dagger {
protected:
public:
    DaggerPlusOne(Point pos, int number);
    virtual ~DaggerPlusOne();
};


class Sword : public Weapon {
protected:
public:
    Sword(Point pos, int number);
    virtual ~Sword();
    virtual std::string verb() const;
};

class ShortSword : public Sword {
protected:
public:
    ShortSword(Point pos, int number);
    virtual ~ShortSword();
};

class ShortSwordPlusOne : public ShortSword {
protected:
public:
    ShortSwordPlusOne(Point pos, int number);
    virtual ~ShortSwordPlusOne();
};

class LongSword : public Sword {
protected:
public:
    LongSword(Point pos, int number);
    virtual ~LongSword();
};

class LongSwordPlusOne : public LongSword {
protected:
public:
    LongSwordPlusOne(Point pos, int number);
    virtual ~LongSwordPlusOne();
};

class BastardSword : public Sword {
protected:
public:
    BastardSword(Point pos, int number);
    virtual ~BastardSword();
    virtual std::string verb() const;
};


class RangedWeapon : public Weapon {
protected:
public:
    RangedWeapon(Point pos, int number);
    virtual ~RangedWeapon();
};

class Bow : public RangedWeapon {
protected:
public:
    Bow(Point pos, int number);
    virtual ~Bow();
};

class ShortBow : public Bow {
protected:
public:
    ShortBow(Point pos, int number);
    virtual ~ShortBow();
};

}

#endif /* WEAPON_H_ */
