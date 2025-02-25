#include "Food.h"
#include "Utils.h"
#include <vector>

namespace Roguelike {

////////////////////////////////////////////////////

Food::Food(Point pos, int number) : Item(pos, number) {
    category_ = FOOD;
    food_type_ = REGULAR;
    symbol_ = '~';
    colour_ = Colour::GREY_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "<food item>";
    nouns_ = "<food items>";
    taste_ = "<Generic food.>";
    filling_ = 0;
}

Food::~Food() {}

////////////////////////////////////////////////////

Corpse::Corpse(Point pos, int number) : Food(pos, number) {
    food_type_ = CORPSE;
}

Corpse::~Corpse() {}

////////////////////////////////////////////////////

Entrails::Entrails(Point pos, int number, std::string noun) : Corpse(pos, number) {
    type_ = FOOD_ENTRAILS;
    symbol_ = '~';
    colour_ = Colour::LIGHTRED_ON_BLACK;
    a_or_an_ = "a";
    if (noun != "") {
        noun_ = noun;
    } else {
        std::vector<std::string> possible_nouns;
        possible_nouns.push_back("giblet");
        possible_nouns.push_back("gizzard");
        possible_nouns.push_back("spleen");
        possible_nouns.push_back("kidney");
        noun_ = possible_nouns.at(Utils::random_int(0, possible_nouns.size() - 1));
    }
    nouns_ = noun_ + "s";
    filling_ = 50;

    switch (Utils::random_int(0, 2)) {
        case 0:
            taste_ = "Puke!";
            break;
        case 1:
            taste_ = "Sick!";
            break;
        case 2:
            taste_ = "Disgusting!";
            break;
        case 3:
            taste_ = "This is how people get the gout, you know.";
            break;
    }
}

Entrails::~Entrails() {}

////////////////////////////////////////////////////

ZombieCorpse::ZombieCorpse(Point pos, int number) : Corpse(pos, number) {
    type_ = FOOD_ZOMBIE_CORPSE;
    symbol_ = '%';
    colour_ = Colour::GREEN_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "zombie corpse";
    nouns_ = "zombie corpses";
    filling_ = 80;

    switch (Utils::random_int(0, 3)) {
        case 0:
            taste_ = "Truly repulsive!";
            break;
        case 1:
            taste_ = "Putrid, rotting flesh!";
            break;
        case 2:
            taste_ = "So vile!";
            break;
        case 3:
            taste_ = "You choke it down.";
            break;
    }
}

ZombieCorpse::~ZombieCorpse() {}

////////////////////////////////////////////////////

MummyCorpse::MummyCorpse(Point pos, int number) : Corpse(pos, number) {
    type_ = FOOD_MUMMY_CORPSE;
    symbol_ = '%';
    colour_ = Colour::WHITE_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "mummy corpse";
    nouns_ = "mummy corpses";
    filling_ = 120;
    switch (Utils::random_int(0, 3)) {
        case 0:
            taste_ = "Yummy!";
            break;
        case 1:
            taste_ = "Jerky!";
            break;
        case 2:
            taste_ = "Spicy!";
            break;
        case 3:
            taste_ = "Preservatives!";
            break;
    }
}

MummyCorpse::~MummyCorpse() {}

////////////////////////////////////////////////////

VampireCorpse::VampireCorpse(Point pos, int number) : Corpse(pos, number) {
    type_ = FOOD_VAMPIRE_CORPSE;
    symbol_ = '%';
    colour_ = Colour::LIGHTRED_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "vampire corpse";
    nouns_ = "vampire corpses";
    filling_ = 250;
    switch (Utils::random_int(0, 3)) {
        case 0:
            taste_ = "This vampire corpse tastes great!";
            break;
        case 1:
            taste_ = "That's good!";
            break;
        case 2:
            taste_ = "You love this!";
            break;
        case 3:
            taste_ = "Can't get enough!";
            break;
    }
}

VampireCorpse::~VampireCorpse() {}

////////////////////////////////////////////////////

WolfManCorpse::WolfManCorpse(Point pos, int number) : Corpse(pos, number) {
    type_ = FOOD_WOLFMAN_CORPSE;
    symbol_ = '%';
    colour_ = Colour::BROWN_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "wolf man corpse";
    nouns_ = "wolf man corpses";
    filling_ = 160;
    switch (Utils::random_int(0, 3)) {
        case 0:
            taste_ = "Hoooo-wee!  That's good wolfman corpse.";
            break;
        case 1:
            taste_ = "Greasy!";
            break;
        case 2:
            taste_ = "Delicious.";
            break;
        case 3:
            taste_ = "Greasy!  And Delicious.";
            break;
        case 4:
            taste_ = "Pretty good.  Not as good as vampire corpse.";
            break;
    }
}

WolfManCorpse::~WolfManCorpse() {}

////////////////////////////////////////////////////

MachineElfCorpse::MachineElfCorpse(Point pos, int number) : Corpse(pos, number) {
    type_ = FOOD_MACHINE_ELF_CORPSE;
    symbol_ = '%';
    colour_ = Colour::CYAN_ON_BLACK;
    a_or_an_ = "a";
    noun_ = "machine elf corpse";
    nouns_ = "machine elf corpses";
    filling_ = 160;
    switch (Utils::random_int(0, 2)) {
        case 0:
            taste_ = "You never pictured yourself eating this.";
            break;
        case 1:
            taste_ = "You feel as full as you did when you ate that wolfman corpse.";
            break;
        case 2:
            taste_ = "Tastes good... very good!";
            break;
    }
}

MachineElfCorpse::~MachineElfCorpse() {}


}
