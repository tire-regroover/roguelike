#include "Armor.h"
#include "Utils.h"

namespace Roguelike {

Shield::Shield(Point pos, int number) : Item(pos, number) {
    category_ = SHIELD;
    symbol_ = '[';
    colour_ = Colour::GREY_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "<shield>";
    nouns_ = "<shields>";
    equipable_ = true;
    ac_ = 0;
    dexterity_penalty_ = 0;
}

Shield::~Shield() {}

////////////////////////////////////////////////////

SmallWoodenShield::SmallWoodenShield(Point pos, int number) : Shield(pos, number) {
    type_ = SHIELD_SMALL_WOODEN;
    colour_ = Colour::BROWN_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "small wooden shield";
    nouns_ = "small wooden shields";
    ac_ = 1;
    dexterity_penalty_ = 0;
}

SmallWoodenShield::~SmallWoodenShield() {}

////////////////////////////////////////////////////

SmallShield::SmallShield(Point pos, int number) : Shield(pos, number) {
    type_ = SHIELD_SMALL;
    a_or_an_ = "a";
    noun_ = "small shield";
    nouns_ = "small shields";
    ac_ = 2;
    dexterity_penalty_ = 1;
}

SmallShield::~SmallShield() {}

////////////////////////////////////////////////////

MediumShield::MediumShield(Point pos, int number) : Shield(pos, number) {
    type_ = SHIELD_MEDIUM;
    a_or_an_ = "a";
    noun_ = "medium shield";
    nouns_ = "medium shields";
    ac_ = 4;
    dexterity_penalty_ = 2;
}

MediumShield::~MediumShield() {}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

Armor::Armor(Point pos, int number) : Item(pos, number) {
    category_ = ARMOR;
    symbol_ = '[';
    colour_ = Colour::GREY_ON_BLACK;
    a_or_an_ = "an";
    noun_ = "<armor>";
    nouns_ = "<armor>";
    equipable_ = true;
    ac_ = 0;
    dexterity_penalty_ = 0;
}

Armor::~Armor() {}

////////////////////////////////////////////////////

LeatherArmor::LeatherArmor(Point pos, int number) : Armor(pos, number) {
    type_ = ARMOR_LEATHER;
    colour_ = Colour::BROWN_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "leather armor";
    nouns_ = "leather armors";
    ac_ = 5;
    dexterity_penalty_ = 1;
}

LeatherArmor::~LeatherArmor() {}

////////////////////////////////////////////////////

StuddedLeatherArmor::StuddedLeatherArmor(Point pos, int number) : Armor(pos, number) {
    type_ = ARMOR_STUDDED_LEATHER;
    colour_ = Colour::BROWN_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "studded leather armor";
    nouns_ = "studded leather armors";
    ac_ = 7;
    dexterity_penalty_ = 1;
}

StuddedLeatherArmor::~StuddedLeatherArmor() {}

////////////////////////////////////////////////////

ChainMail::ChainMail(Point pos, int number) : Armor(pos, number) {
    type_ = ARMOR_CHAIN_MAIL;
    a_or_an_ = "a";
    noun_ = "chain mail armor";
    nouns_ = "chain mail armors";
    ac_ = 9;
    dexterity_penalty_ = 2;
}

ChainMail::~ChainMail() {}

////////////////////////////////////////////////////

}
