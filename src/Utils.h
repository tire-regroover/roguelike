#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <ncurses.h>
#include <unistd.h>

namespace Roguelike {

class Dice {
    int num_, sides_, bonus_;
public:
    Dice(int num, int sides, int bonus = 0);
    virtual ~Dice();
    int roll() const;
    std::string name() const;
    int worth() const;
};

namespace Colour {
    enum COLOUR {
        NO_COLOUR = -1,
        RED_ON_BLACK          = COLOR_PAIR(1),
        GREEN_ON_BLACK        = COLOR_PAIR(2),
        BROWN_ON_BLACK        = COLOR_PAIR(3),
        BLUE_ON_BLACK         = COLOR_PAIR(4),
        MAGENTA_ON_BLACK      = COLOR_PAIR(5),
        CYAN_ON_BLACK         = COLOR_PAIR(6),
        GREY_ON_BLACK         = COLOR_PAIR(7),
        BLACK_ON_BLACK        = COLOR_PAIR(8),
        LIGHTRED_ON_BLACK     = (COLOR_PAIR(1) | A_BOLD),
        LIGHTGREEN_ON_BLACK   = (COLOR_PAIR(2) | A_BOLD),
        YELLOW_ON_BLACK       = (COLOR_PAIR(3) | A_BOLD),
        LIGHTBLUE_ON_BLACK    = (COLOR_PAIR(4) | A_BOLD),
        LIGHTMAGENTA_ON_BLACK = (COLOR_PAIR(5) | A_BOLD),
        LIGHTCYAN_ON_BLACK    = (COLOR_PAIR(6) | A_BOLD),
        WHITE_ON_BLACK        = (COLOR_PAIR(7) | A_BOLD),
        DARKGREY_ON_BLACK     = (COLOR_PAIR(8) | A_BOLD)
    };
}

namespace Utils {
    int random_int(int low, int high);
    bool percent_chance(int percent);
    std::string int_to_string(const int i);
    std::string capitalize(std::string text);
    Colour::COLOUR colour(char c);

	inline void sleep(unsigned ms)
	{
		::usleep(ms * 1000);
	}
}

namespace Hand {
    enum HAND {
        LEFT,
        RIGHT
    };
}

namespace Direction {
    enum DIRECTION {
        NONE  = 0x0,
        NORTH = 0x1,
        EAST  = 0x2,
        SOUTH = 0x4,
        WEST  = 0x8,
        NORTHEAST = (NORTH | EAST),
        SOUTHEAST = (SOUTH | EAST),
        SOUTHWEST = (SOUTH | WEST),
        NORTHWEST = (NORTH | WEST)
    };
}

namespace YesNoAllNumberQuit {
    enum YES_NO_ALL_NUMBER_QUIT {
        YES,
        NO,
        ALL,
        NUMBER,
        QUIT
    };
}

}

#endif /* UTILS_H_ */
