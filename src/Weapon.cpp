#include "Weapon.h"
#include <sstream>
#include <vector>

namespace Roguelike {

Weapon::Weapon(Point pos, int number) : Item(pos, number), dice_(1, 1) {
    category_ = WEAPON;
    weapon_category_ = MELEE;
    symbol_ = ')';
    colour_ = Colour::GREY_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "<weapon>";
    nouns_ = "<weapons>";
    equipable_ = true;
    throwable_ = false;
    //TODO: min lvl
}

Weapon::~Weapon() {}

std::string Weapon::verb() const {
    return "<Weapon::verb>";
}

std::string Weapon::verbs() const {
    std::ostringstream o;
    o << verb() << 's';
    return o.str();
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

Dagger::Dagger(Point pos, int number) : Weapon(pos, number) {
    type_ = WEAPON_DAGGER;
    colour_ = Colour::GREY_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "dagger";
    nouns_ = "daggers";
    dice_ = Dice(1, 5);
}

Dagger::~Dagger() {}

std::string Dagger::verb() const {
    std::vector<std::string> possible_verbs;
    possible_verbs.push_back("stick");
    possible_verbs.push_back("stab");
    possible_verbs.push_back("slice");
    return possible_verbs.at(Utils::random_int(0, possible_verbs.size() - 1));
}

////////////////////////////////////////////////////

DaggerPlusOne::DaggerPlusOne(Point pos, int number) : Dagger(pos, number) {
    type_ = WEAPON_DAGGER_PLUS_ONE;
    dice_ = Dice(1, 5, 1);
    noun_ = "dagger+1";
    nouns_ = "daggers+1";
}

DaggerPlusOne::~DaggerPlusOne() {}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

Sword::Sword(Point pos, int number) : Weapon(pos, number) {
    noun_ = "<sword>";
    nouns_ = "<swords>";
}

Sword::~Sword() {}

std::string Sword::verb() const {
    std::vector<std::string> possible_verbs;
    possible_verbs.push_back("impale");
    possible_verbs.push_back("strike");
    possible_verbs.push_back("gut");
    possible_verbs.push_back("disembowel");
    return possible_verbs.at(Utils::random_int(0, possible_verbs.size() - 1));
}

////////////////////////////////////////////////////

ShortSword::ShortSword(Point pos, int number) : Sword(pos, number) {
    type_ = WEAPON_SHORT_SWORD;
    dice_ = Dice(1, 6);
    noun_ = "short sword";
    nouns_ = "short swords";
}

ShortSword::~ShortSword() {}

////////////////////////////////////////////////////

ShortSwordPlusOne::ShortSwordPlusOne(Point pos, int number) : ShortSword(pos, number) {
    type_ = WEAPON_SHORT_SWORD_PLUS_ONE;
    dice_ = Dice(1, 6, 1);
    noun_ = "short sword+1";
    nouns_ = "short swords+1";
}

ShortSwordPlusOne::~ShortSwordPlusOne() {}

////////////////////////////////////////////////////

LongSword::LongSword(Point pos, int number) : Sword(pos, number) {
    type_ = WEAPON_LONG_SWORD;
    dice_ = Dice(1, 8);
    noun_ = "long sword";
    nouns_ = "long swords";
}

LongSword::~LongSword() {}

////////////////////////////////////////////////////

LongSwordPlusOne::LongSwordPlusOne(Point pos, int number) : LongSword(pos, number) {
    type_ = WEAPON_LONG_SWORD_PLUS_ONE;
    dice_ = Dice(1, 8, 1);
    noun_ = "long sword+1";
    nouns_ = "long swords+1";
}

LongSwordPlusOne::~LongSwordPlusOne() {}

////////////////////////////////////////////////////

BastardSword::BastardSword(Point pos, int number) : Sword(pos, number) {
    type_ = WEAPON_BASTARD_SWORD;
    dice_ = Dice(2, 5);
    noun_ = "bastard sword";
    nouns_ = "bastard swords";
}

BastardSword::~BastardSword() {}

std::string BastardSword::verb() const {
    std::vector<std::string> possible_verbs;
    possible_verbs.push_back("bastardize");
    possible_verbs.push_back("impale");
    possible_verbs.push_back("strike");
    possible_verbs.push_back("gut");
    possible_verbs.push_back("disembowel");
    return possible_verbs.at(Utils::random_int(0, possible_verbs.size() - 1));
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

ThrowableWeapon::ThrowableWeapon(Point pos, int number) : Weapon(pos, number) {
    throwable_ = true;
    symbol_ = '*';
}

ThrowableWeapon::~ThrowableWeapon() {}

std::string ThrowableWeapon::verb() const {
    std::vector<std::string> possible_verbs;
    possible_verbs.push_back("hurl");
    possible_verbs.push_back("chuck");
    possible_verbs.push_back("heave");
    possible_verbs.push_back("pitch");
    possible_verbs.push_back("cast");
    possible_verbs.push_back("propel");
    return possible_verbs.at(Utils::random_int(0, possible_verbs.size() - 1));
}

std::string ThrowableWeapon::verbs() const {
    std::string verb = this->verb();
    std::ostringstream o;
    if (verb == "pitch")
        o << verb << "es";
    else
        o << verb << 's';
    return o.str();
}

////////////////////////////////////////////////////

Rock::Rock(Point pos, int number) : ThrowableWeapon(pos, number) {
    type_ = WEAPON_ROCK;
    colour_ = Colour::GREY_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "rock";
    nouns_ = "rocks";
    dice_ = Dice(1, 3);
}

Rock::~Rock() {}

////////////////////////////////////////////////////

FunBall::FunBall(Point pos, int number, Colour::COLOUR colour) : ThrowableWeapon(pos, number) {
    type_ = WEAPON_FUNBALL;
    if (colour == -1) {
        switch (Utils::random_int(0, 5)) {
            case 0:
                colour = Colour::LIGHTMAGENTA_ON_BLACK;
                break;
            case 1:
                colour = Colour::LIGHTCYAN_ON_BLACK;
                break;
            case 2:
                colour = Colour::LIGHTRED_ON_BLACK;
                break;
            case 3:
                colour = Colour::LIGHTBLUE_ON_BLACK;
                break;
            case 4:
                colour = Colour::LIGHTGREEN_ON_BLACK;
                break;
            case 5:
                colour = Colour::YELLOW_ON_BLACK;
                break;
        }
    }

    colour_ = colour;

    switch (colour_) {
        case Colour::LIGHTMAGENTA_ON_BLACK:
            a_or_an_ = "a";
            noun_ = "magenta funball";
            dice_ = Dice(1, 6);
            break;
        case Colour::LIGHTCYAN_ON_BLACK:
            a_or_an_ = "a";
            noun_ = "cyan funball";
            dice_ = Dice(1, 6);
            break;
        case Colour::LIGHTRED_ON_BLACK:
            a_or_an_ = "a";
            noun_ = "red funball";
            dice_ = Dice(1, 6, 1);
            break;
        case Colour::LIGHTBLUE_ON_BLACK:
            a_or_an_ = "a";
            noun_ = "blue funball";
            dice_ = Dice(1, 6);
            break;
        case Colour::LIGHTGREEN_ON_BLACK:
            a_or_an_ = "a";
            noun_ = "green funball";
            dice_ = Dice(1, 6);
            break;
        case Colour::YELLOW_ON_BLACK:
            a_or_an_ = "a";
            noun_ = "yellow funball";
            dice_ = Dice(1, 6);
            break;
        default:
            break;

    }
    nouns_ = noun_ + "s";
}

FunBall::~FunBall() {}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

RangedWeapon::RangedWeapon(Point pos, int number) : Weapon(pos, number) {
    weapon_category_ = RANGED;
    colour_ = Colour::BROWN_ON_BLACK;
    symbol_ = '{';
    a_or_an_ = "a";
    noun_ = "<RangedWeapon>";
    nouns_ = "<RangedWeapons>";
}

RangedWeapon::~RangedWeapon() {}

////////////////////////////////////////////////////

Bow::Bow(Point pos, int number) : RangedWeapon(pos, number) {
    a_or_an_ = "a";
    noun_ = "<bow>";
    nouns_ = "<bows>";
}

Bow::~Bow() {}

////////////////////////////////////////////////////

ShortBow::ShortBow(Point pos, int number) : Bow(pos, number) {
    type_ = WEAPON_SHORT_BOW;
    a_or_an_ = "a";
    noun_ = "short bow";
    nouns_ = "short bows";
    dice_ = Dice(1, 5);
}

ShortBow::~ShortBow() {}

}
