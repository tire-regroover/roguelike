#include "Utils.h"
#include <algorithm>
#include <sstream>

namespace Roguelike {

namespace Utils {
    int random_int(int low, int high) {
        return rand() % (high + 1 - low) + low;
    }

    bool percent_chance(int percent) {
        if (percent == 0) return false;
        if (percent >= 100) return true;
        return random_int(1, 100) <= percent;
    }

    std::string int_to_string(const int i) {
        std::ostringstream o;
        o << i;
        return o.str();
    }

    std::string capitalize(std::string text) {
        text[0] = toupper(text[0]);
        return text;
    }

    Colour::COLOUR colour(char c) {
        Colour::COLOUR colour = Colour::NO_COLOUR;
        switch (c) {
            case 'r':
                colour = Colour::RED_ON_BLACK;
                break;
            case 'g':
                colour = Colour::GREEN_ON_BLACK;
                break;
            case 'n':
                colour = Colour::BROWN_ON_BLACK;
                break;
            case 'b':
                colour = Colour::BLUE_ON_BLACK;
                break;
            case 'm':
                colour = Colour::MAGENTA_ON_BLACK;
                break;
            case 'c':
                colour = Colour::CYAN_ON_BLACK;
                break;
            case 'E':
                colour = Colour::GREY_ON_BLACK;
                break;
            case 'x':
                colour = Colour::BLACK_ON_BLACK;
                break;
            case 'R':
                colour = Colour::LIGHTRED_ON_BLACK;
                break;
            case 'G':
                colour = Colour::LIGHTGREEN_ON_BLACK;
                break;
            case 'y':
                colour = Colour::YELLOW_ON_BLACK;
                break;
            case 'B':
                colour = Colour::LIGHTBLUE_ON_BLACK;
                break;
            case 'M':
                colour = Colour::LIGHTMAGENTA_ON_BLACK;
                break;
            case 'C':
                colour = Colour::LIGHTCYAN_ON_BLACK;
                break;
            case 'w':
                colour = Colour::WHITE_ON_BLACK;
                break;
            case 'e':
                colour = Colour::DARKGREY_ON_BLACK;
                break;
        }
        return colour;
    }
}

Dice::Dice(int num, int sides, int bonus) {
    num_ = num;
    sides_ = sides;
    bonus_ = bonus;
}

Dice::~Dice() {}

std::string Dice::name() const {
    std::ostringstream o;
    o << num_ << 'd' << sides_;
    if (bonus_ > 0)
        o << '+' << bonus_;
    return o.str();
}

int Dice::roll() const {
    int result = 0;
    for (int i = 0; i < num_; i++) {
        result += Utils::random_int(1, sides_);
    }
    return result + bonus_;
}

int Dice::worth() const {
    return num_ * sides_ + bonus_ > 0 ? bonus_ + 1 : 0;
}

}
