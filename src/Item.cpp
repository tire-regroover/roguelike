#include "Item.h"
#include "Food.h"
#include "Weapon.h"
#include "Armor.h"
#include <stdexcept>

namespace Roguelike {

unsigned long Item::top_id_ = 0;

Item::Item(Point pos, int number) {
    id_ = top_id_++;
    type_ = NONE;
    category_ = NO_CATEGORY;
    pos_ = pos;
    seen_pos_ = Point();
    held_ = false;
    quantity_ = number;
    owner_ = NULL;
    stackable_ = true;
    a_or_an_ = "an";
    noun_ = "<item>";
    nouns_ = "<items>";
    equipable_ = false;
}

Item::~Item() {}

//TODO: always update this
Item* Item::copy_item(Item* item, int quantity) {
    switch (item->type()) {
        case GOLD:
            return new Gold(Point(), quantity);

        case KEY:
            return new Key(Point(), quantity, static_cast<Key*>(item)->key_type());

        case GLOWING_ORB:
            return new GlowingOrb(Point(), quantity);

        case WEAPON_ROCK:
            return new Rock(Point(), quantity);
        case WEAPON_FUNBALL:
            return new FunBall(Point(), quantity, item->colour());

        case WEAPON_DAGGER:
            return new Dagger(Point(), quantity);
        case WEAPON_DAGGER_PLUS_ONE:
            return new DaggerPlusOne(Point(), quantity);
        case WEAPON_SHORT_SWORD:
            return new ShortSword(Point(), quantity);
        case WEAPON_SHORT_SWORD_PLUS_ONE:
            return new ShortSwordPlusOne(Point(), quantity);
        case WEAPON_LONG_SWORD:
            return new LongSword(Point(), quantity);
        case WEAPON_LONG_SWORD_PLUS_ONE:
            return new LongSwordPlusOne(Point(), quantity);
        case WEAPON_BASTARD_SWORD:
            return new BastardSword(Point(), quantity);

        case SHIELD_SMALL:
            return new SmallShield(Point(), quantity);
        case SHIELD_SMALL_WOODEN:
            return new SmallWoodenShield(Point(), quantity);
        case SHIELD_MEDIUM:
            return new MediumShield(Point(), quantity);

        case WEAPON_SHORT_BOW:
            return new ShortBow(Point(), quantity);

        case ARMOR_LEATHER:
            return new LeatherArmor(Point(), quantity);
        case ARMOR_STUDDED_LEATHER:
            return new StuddedLeatherArmor(Point(), quantity);
        case ARMOR_CHAIN_MAIL:
            return new ChainMail(Point(), quantity);


        case FOOD_ZOMBIE_CORPSE:
            return new ZombieCorpse(Point(), quantity);
        case FOOD_MUMMY_CORPSE:
            return new MummyCorpse(Point(), quantity);
        case FOOD_VAMPIRE_CORPSE:
            return new VampireCorpse(Point(), quantity);
        case FOOD_WOLFMAN_CORPSE:
            return new WolfManCorpse(Point(), quantity);
        case FOOD_MACHINE_ELF_CORPSE:
            return new WolfManCorpse(Point(), quantity);
        case FOOD_ENTRAILS:
            return new Entrails(Point(), quantity, item->noun());
        default:
            throw std::runtime_error("Item type bad");
    }
}

void Item::draw() {
    attron(colour_);
    mvaddch(pos_.y + 3, pos_.x + 1, symbol_);
    attroff(colour_);
}

void Item::draw_at_seen_pos() {
    attron(colour_);
    mvaddch(seen_pos_.y + 3, seen_pos_.x + 1, symbol_);
    attroff(colour_);
}

void Item::set_owner(Monster* owner) {
    owner_ = owner;
    if (owner == NULL) {
        held_ = false;
    }
    else {
        held_ = true;
    }
}

////////////////////////////////////////////////////

Gold::Gold(Point pos, int number) : Item(pos, number) {
    type_ = GOLD;
    category_ = MONEY;
    symbol_ = '$';
    colour_ = Colour::YELLOW_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "gold coin";
    nouns_ = "gold coins";
}

Gold::~Gold() {}

////////////////////////////////////////////////////

Tool::Tool(Point pos, int number) : Item(pos, number) {
    category_ = TOOL;
    symbol_ = '(';
    colour_ = Colour::GREY_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "<tool>";
    nouns_ = "<tools>";
}

Tool::~Tool() {}

////////////////////////////////////////////////////

Key::Key(Point pos, int number, KEY_TYPE key_type) : Tool(pos, number) {
    type_ = KEY;
    a_or_an_ = "a";
    key_type_ = key_type;

    switch (key_type) {
        case SKELETON:
            noun_ = "skeleton key";
            nouns_ = "skeleton keys";
            colour_ = Colour::WHITE_ON_BLACK;
            break;
        case GREEN:
            noun_ = "green key";
            nouns_ = "green keys";
            colour_ = Colour::LIGHTGREEN_ON_BLACK;
            break;
        case RED:
            noun_ = "red key";
            nouns_ = "red keys";
            colour_ = Colour::LIGHTRED_ON_BLACK;
            break;
        case BLUE:
            noun_ = "blue key";
            nouns_ = "blue keys";
            colour_ = Colour::LIGHTBLUE_ON_BLACK;
            break;
        default:
            break;
    }
}

Key::~Key() {}

Key::KEY_TYPE Key::random_type() {
    switch (Utils::random_int(0, 2)) {
        case 0:
            return GREEN;
        case 1:
            return RED;
        case 2:
            return BLUE;
    }
    return NONE;
}

////////////////////////////////////////////////////

PlotItem::PlotItem(Point pos, int number) : Item(pos, number) {
    category_ = PLOT;
    symbol_ = '0';
    colour_ = Colour::LIGHTCYAN_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "<plot item>";
    nouns_ = "<plot items>";
}

PlotItem::~PlotItem() {}

////////////////////////////////////////////////////

GlowingOrb::GlowingOrb(Point pos, int number) : PlotItem(pos, number) {
    type_ = GLOWING_ORB;
    category_ = PLOT;
    symbol_ = '0';
    colour_ = Colour::LIGHTCYAN_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "glowing orb thing";
    nouns_ = "glowing orb things";
}

GlowingOrb::~GlowingOrb() {}

}
