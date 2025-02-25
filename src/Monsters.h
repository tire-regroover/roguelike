#ifndef MONSTERS_H_
#define MONSTERS_H_

#include "Monster.h"

namespace Roguelike {

class Zombie : public Monster {
public:
    Zombie(Level* level, std::string name = std::string());
    virtual ~Zombie();
    virtual std::string verb() const;
    virtual std::string verbs() const;
    virtual std::string verbed() const;
};

class Mummy : public Monster {
public:
    Mummy(Level* level, std::string name = std::string());
    virtual ~Mummy();
    virtual std::string verb() const;
    virtual std::string verbs() const;
    virtual std::string verbed() const;
};

class Vampire : public Monster {
public:
    Vampire(Level* level, std::string name = std::string());
    virtual ~Vampire();
    virtual std::string verb() const;
    virtual std::string verbs() const;
    virtual std::string verbed() const;
};

class WolfMan : public Monster {
public:
    WolfMan(Level* level, std::string name = std::string());
    virtual ~WolfMan();
    virtual std::string verb() const;
    virtual std::string verbs() const;
    virtual std::string verbed() const;
};

class MachineElf : public Monster {
public:
    MachineElf(Level* level, std::string name = std::string());
    virtual ~MachineElf();
    virtual std::string verb() const;
    virtual std::string verbs() const;
    virtual std::string verbed() const;
};

class BabyMachineElf : public MachineElf {
public:
    BabyMachineElf(Level* level, std::string name = std::string());
    virtual ~BabyMachineElf();
};

}


#endif /* MONSTERS_H_ */
