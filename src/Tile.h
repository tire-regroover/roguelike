#ifndef TILE_H_
#define TILE_H_

#include "Utils.h"
#include "Item.h"

namespace Roguelike {

class Tile {
public:
    enum TYPE {
        WALL,
        FLOOR,
        DOOR,
        STAIRS_DOWN,
        STAIRS_UP
    };
protected:
    TYPE type_;
    char symbol_;
    bool passage_blocked_;
    bool sight_blocked_;
    bool drawn_;
    int colour_;

public:
    Tile() {
        drawn_ = false;
    }

    virtual ~Tile() {}
    TYPE type() const {
        return type_;
    }

    char symbol() const {
        return symbol_;
    }

    int colour() const {
        return colour_;
    }

    void set_colour(int colour) {
        colour_ = colour;
    }

    bool passage_blocked() const {
        return passage_blocked_;
    }

    bool drawn() const {
        return drawn_;
    }

    void set_drawn(bool drawn) { drawn_ = drawn; }

    void set_passage_blocked(bool blocked) {
        passage_blocked_ = blocked;
    }

    bool sight_blocked() const {
        return sight_blocked_;
    }
};

class Wall : public Tile {
public:
    Wall();
    virtual ~Wall();
};

class Floor : public Tile {
public:
    Floor();
    virtual ~Floor();
};

class Door : public Tile {
private:
    bool open_;
    bool locked_;
    Key::KEY_TYPE key_type_;
public:
    Door(bool opened = true, Key::KEY_TYPE key_type = Key::NONE);
    virtual ~Door();

    void open();
    void close();
    void lock();
    void unlock();

    bool is_open() const { return open_; }
    bool is_locked() const { return locked_; }
    Key::KEY_TYPE key_type() const { return key_type_; }
};

class StairsUp : public Tile {
private:
public:
    StairsUp();
    virtual ~StairsUp();
};

class StairsDown : public Tile {
private:
public:
    StairsDown();
    virtual ~StairsDown();
};

}

#endif /* TILE_H_ */
