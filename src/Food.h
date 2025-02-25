#ifndef FOOD_H_
#define FOOD_H_

#include "Item.h"

namespace Roguelike {

class Food : public Item {
public:
    enum FOOD_TYPE {
        REGULAR,
        CORPSE
    };
protected:
    FOOD_TYPE food_type_;
    int filling_;
    std::string taste_;
public:
    Food(Point pos, int number);
    virtual ~Food();
    FOOD_TYPE food_type() const { return food_type_; }
    int filling() const { return filling_; }
    std::string taste() const { return taste_; }
};


class Corpse : public Food {
protected:
public:
    Corpse(Point pos, int number);
    virtual ~Corpse();
};

class Entrails : public Corpse {
protected:
public:
    Entrails(Point pos, int number, std::string noun = "");
    virtual ~Entrails();
};

class ZombieCorpse : public Corpse {
protected:
public:
    ZombieCorpse(Point pos, int number);
    virtual ~ZombieCorpse();
};

class MummyCorpse : public Corpse {
protected:
public:
    MummyCorpse(Point pos, int number);
    virtual ~MummyCorpse();
};

class VampireCorpse : public Corpse {
protected:
public:
    VampireCorpse(Point pos, int number);
    virtual ~VampireCorpse();
};

class WolfManCorpse : public Corpse {
protected:
public:
    WolfManCorpse(Point pos, int number);
    virtual ~WolfManCorpse();
};

class MachineElfCorpse : public Corpse {
protected:
public:
    MachineElfCorpse(Point pos, int number);
    virtual ~MachineElfCorpse();
};

}

#endif /* FOOD_H_ */
