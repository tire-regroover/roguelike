#include "World.h"
#include <algorithm>

void init_ncurses() {
    initscr();
    cbreak();
    //raw();
    keypad(stdscr, TRUE);
    noecho();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    init_pair(8, COLOR_BLACK, COLOR_BLACK);
    assume_default_colors(COLOR_WHITE, COLOR_BLACK);
}

int main(void) {
    srand(time(0));

    while (1) {
        init_ncurses();
        Roguelike::World w;
        w.go();

        if (!w.play_again())
            break;
    }

    endwin();
    return EXIT_SUCCESS;
}

