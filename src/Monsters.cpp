#include "Monsters.h"
#include "Food.h"
#include "Level.h"
#include "Weapon.h"
#include <sstream>

namespace Roguelike {

////////////////////////////////////////////////////

Zombie::Zombie(Level* level, std::string name) : Monster(level, name) {
    if (name == "")
        name_ = "a zombie";
    type_ = ZOMBIE;
    symbol_ = 'Z';
    colour_ = Colour::GREEN_ON_BLACK;

    strength_ = 6;
    dexterity_ = 5;
    hp_max_ = 10;
    hp_ = hp_max_;
    xp_ = 20;
    kill_worth_ = 17;
    base_attack_ = 0;
    attack_dice_ = Dice(1, 3);

    breathe_every_ = 4;

    can_open_doors_ = false;
    give_up_chase_turns_ = 10;
    hostile_ = true;

    corpse_ = new ZombieCorpse(Point(), 1);
    corpse_->set_owner(this);
    level_->items()->push_back(corpse_);
}



Zombie::~Zombie() {}

std::string Zombie::verb() const {
    std::vector<std::string> possible_verbs;
    possible_verbs.push_back("gnaw on");
    possible_verbs.push_back("saunter at");
    possible_verbs.push_back("stagger at");
    return possible_verbs.at(Utils::random_int(0, possible_verbs.size() - 1));
}

std::string Zombie::verbs() const {
    std::string v = verb();
    if (v == "gnaw on")
        return "gnaws on";
    if (v == "saunter at")
        return "saunters at";
    if (v == "stagger at")
        return "staggers at";
    std::ostringstream o;
    o << v << 's';
    return o.str();
}

std::string Zombie::verbed() const {
    std::string v = verb();
    if (v == "gnaw on")
        return "gnawed on";
    if (v == "saunter at")
        return "sauntered at";
    if (v == "stagger at")
        return "staggered at";
    std::ostringstream o;
    o << v << 'd';
    return o.str();
}

////////////////////////////////////////////////////

Mummy::Mummy(Level* level, std::string name) : Monster(level, name) {
    if (name == "")
        name_ = "a mummy";
    type_ = MUMMY;
    symbol_ = 'M';
    colour_ = Colour::WHITE_ON_BLACK;
    strength_ = 15;
    dexterity_ = 10;
    hp_max_ = 10;
    hp_ = hp_max_;
    xp_ = 30;
    kill_worth_ = 29;
    base_attack_ = 4;
    attack_dice_ = Dice(1, 4);

    breathe_every_ = 6;

    can_open_doors_ = false;
    give_up_chase_turns_ = 10;
    saw_guy_turns_ago_ = give_up_chase_turns_;

    hostile_ = true;

    corpse_ = new MummyCorpse(Point(), 1);
    corpse_->set_owner(this);
    level_->items()->push_back(corpse_);
}

Mummy::~Mummy() {}

std::string Mummy::verb() const {
    std::vector<std::string> possible_verbs;
    possible_verbs.push_back("grab");
    possible_verbs.push_back("use mummy powers on");
    possible_verbs.push_back("scuffle with");
    return possible_verbs.at(Utils::random_int(0, possible_verbs.size() - 1));
}

std::string Mummy::verbs() const {
    std::string v = verb();
    if (v == "use mummy powers on")
        return "uses mummy powers on";
    if (v == "scuffle with")
        return "scuffles with";
    std::ostringstream o;
    o << v << 's';
    return o.str();
}

std::string Mummy::verbed() const {
    std::string v = verb();
    if (v == "use mummy powers on")
        return "used mummy powers on";
    if (v == "scuffle with")
        return "scuffled with";
    std::ostringstream o;
    o << v << 'd';
    return o.str();
}

////////////////////////////////////////////////////

Vampire::Vampire(Level* level, std::string name) : Monster(level, name) {
    if (name == "")
        name_ = "a vampire";
    type_ = VAMPIRE;
    symbol_ = 'V';
    colour_ = Colour::LIGHTRED_ON_BLACK;
    strength_ = 15;
    dexterity_ = 20;
    hp_max_ = 15;
    hp_ = hp_max_;
    xp_ = 50;
    kill_worth_ = 42;
    base_attack_ = 6;
    attack_dice_ = Dice(1, 5);

    can_open_doors_ = true;
    give_up_chase_turns_ = 15;
    saw_guy_turns_ago_ = give_up_chase_turns_;

    hostile_ = true;
    eyesight_ = 20;

    drops_corpse_ = Utils::percent_chance(10);

    corpse_ = new VampireCorpse(Point(), 1);
    corpse_->set_owner(this);
    level_->items()->push_back(corpse_);
}

Vampire::~Vampire() {}

std::string Vampire::verb() const {
    std::vector<std::string> possible_verbs;
    possible_verbs.push_back("bite");
    possible_verbs.push_back("flap at");
    return possible_verbs.at(Utils::random_int(0, possible_verbs.size() - 1));
}

std::string Vampire::verbs() const {
    std::string v = verb();
    if (v == "flap at")
        return "flaps at";
    std::ostringstream o;
    o << v << 's';
    return o.str();
}

std::string Vampire::verbed() const {
    std::string v = verb();
    if (v == "flap at")
        return "flapped at";
    if (v == "bite")
        return "bit";
    std::ostringstream o;
    o << v << 'd';
    return o.str();
}

////////////////////////////////////////////////////

WolfMan::WolfMan(Level* level, std::string name) : Monster(level, name) {
    if (name == "")
        name_ = "a wolfman";
    type_ = WOLFMAN;
    symbol_ = 'W';
    colour_ = Colour::BROWN_ON_BLACK;
    strength_ = 30;
    dexterity_ = 25;
    hp_max_ = 30;
    hp_ = hp_max_;
    xp_ = 60;
    kill_worth_ = 57;
    base_attack_ = 10;
    attack_dice_ = Dice(1, 5);

    can_open_doors_ = false;
    give_up_chase_turns_ = 10;
    saw_guy_turns_ago_ = give_up_chase_turns_;

    hostile_ = true;

    corpse_ = new WolfManCorpse(Point(), 1);
    corpse_->set_owner(this);
    level_->items()->push_back(corpse_);
}

WolfMan::~WolfMan() {}

std::string WolfMan::verb() const {
    std::vector<std::string> possible_verbs;
    possible_verbs.push_back("lunge at");
    possible_verbs.push_back("maul");
    possible_verbs.push_back("claw at");
    return possible_verbs.at(Utils::random_int(0, possible_verbs.size() - 1));
}

std::string WolfMan::verbs() const {
    std::string v = verb();
    if (v == "lunge at")
        return "lunges at";
    if (v == "claw at")
        return "claws at";
    std::ostringstream o;
    o << v << 's';
    return o.str();
}

std::string WolfMan::verbed() const {
    std::string v = verb();
    if (v == "lunge at")
        return "lunged at";;
    std::ostringstream o;
    o << v << 'd';
    return o.str();
}

////////////////////////////////////////////////////

MachineElf::MachineElf(Level* level, std::string name) : Monster(level, name) {
    if (name == "")
        name_ = "a machine elf";
    type_ = MACHINE_ELF;
    symbol_ = 'E';
    colour_ = Colour::CYAN_ON_BLACK;

    strength_ = 30;
    dexterity_ = 25;
    hp_max_ = 30;
    hp_ = hp_max_;
    xp_ = 70;
    kill_worth_ = 67;
    base_attack_ = 10;
    attack_dice_ = Dice(1, 7);

    breathe_every_ = 11;

    can_open_doors_ = false;
    give_up_chase_turns_ = 10;
    saw_guy_turns_ago_ = give_up_chase_turns_;

    hostile_ = true;

    eyesight_ = 5;

    corpse_ = new MachineElfCorpse(Point(), 1);
    corpse_->set_owner(this);
    level_->items()->push_back(corpse_);

    thrower_ = true;
    add_fresh_inventory(new Rock(Point(), Utils::random_int(1, 4)));
}

MachineElf::~MachineElf() {}

std::string MachineElf::verb() const {
    std::vector<std::string> possible_verbs;
    possible_verbs.push_back("speak gibberish at");
    possible_verbs.push_back("chastise");
    possible_verbs.push_back("enlighten");
    return possible_verbs.at(Utils::random_int(0, possible_verbs.size() - 1));
}

std::string MachineElf::verbs() const {
    std::string v = verb();
    if (v == "speak gibberish at")
        return "speaks gibberish at";
    std::ostringstream o;
    o << v << 's';
    return o.str();
}

std::string MachineElf::verbed() const {
    std::string v = verb();
    if (v == "speak gibberish at")
        return "spoke gibberish at";
    if (v == "enlighten")
        return "enlightened";
    std::ostringstream o;
    o << v << 'd';
    return o.str();
}

////////////////////////////////////////////////////

BabyMachineElf::BabyMachineElf(Level* level, std::string name) : MachineElf(level, name) {
    if (name == "")
        name_ = "a baby machine elf";
    type_ = BABY_MACHINE_ELF;
    symbol_ = 'e';

    strength_ = 6;
    dexterity_ = 1;
    hp_max_ = 10;
    hp_ = hp_max_;
    xp_ = 25;
    kill_worth_ = 22;
    base_attack_ = 5;
    attack_dice_ = Dice(1, 3);

    drops_corpse_ = false;
}

BabyMachineElf::~BabyMachineElf() {}

////////////////////////////////////////////////////

}
