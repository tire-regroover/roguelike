#include "Tile.h"

namespace Roguelike {

Wall::Wall() : Tile() {
    type_ = WALL;
    symbol_ = '#';
    colour_ = Colour::DARKGREY_ON_BLACK;
    passage_blocked_ = true;
    sight_blocked_ = true;
}
Wall::~Wall() {}

///////////////////////////////////////

Floor::Floor() : Tile() {
    type_ = FLOOR;
    symbol_ = '.';
    colour_ = Colour::GREY_ON_BLACK;
    passage_blocked_ = false;
    sight_blocked_ = false;
}
Floor::~Floor() {}

///////////////////////////////////////

Door::Door(bool opened, Key::KEY_TYPE key_type) : Tile() {
    type_ = DOOR;
    colour_ = Colour::BROWN_ON_BLACK;

    if (key_type == Key::NONE)
        key_type = Key::random_type();

    key_type_ = key_type;
    locked_ = false;

    if (opened) open();
    else close();
}

Door::~Door() {}

void Door::open() {
    open_ = true;
    symbol_ = '/';
    passage_blocked_ = false;
    sight_blocked_ = false;
}

void Door::close() {
    open_ = false;
    symbol_ = '+';
    passage_blocked_ = true;
    sight_blocked_ = true;
}

void Door::lock() {
    locked_ = true;
}

void Door::unlock() {
    locked_ = false;
}

///////////////////////////////////////

StairsUp::StairsUp() : Tile() {
    type_ = STAIRS_UP;
    symbol_ = '<';
    colour_ = Colour::WHITE_ON_BLACK;
    passage_blocked_ = false;
    sight_blocked_ = false;
}

StairsUp::~StairsUp() {}

///////////////////////////////////////

StairsDown::StairsDown() : Tile() {
    type_ = STAIRS_DOWN;
    symbol_ = '>';
    colour_ = Colour::WHITE_ON_BLACK;
    passage_blocked_ = false;
    sight_blocked_ = false;
}

StairsDown::~StairsDown() {}

}
