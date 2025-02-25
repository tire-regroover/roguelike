#include "World.h"
#include "Utils.h"
#include "Food.h"
#include "Weapon.h"
#include "Armor.h"
#include "Monsters.h"

#include <iostream>
#include <algorithm>
#include <boost/regex.hpp>

namespace Roguelike {

World::World() {
    width_ = COLS - 2;
    height_ = LINES - 6;

    turns_ = 0;

    guy_ = NULL;

    create_dungeon();
    level_ = dungeon_.front();

    guy_ = new Guy(level_);
    level_->monsters()->push_back(guy_);
    current_level_ = 0;

    status_ = new std::vector<std::string>();
    status_history_ = new std::vector<std::string>();
}

World::~World() {
    for (std::vector<Level*>::iterator it = dungeon_.begin(); !dungeon_.empty();) {
        delete *it;
        dungeon_.erase(it);
    }
    delete guy_;
    delete status_;
    delete status_history_;
    endwin();
}

void World::go() {
    if (COLS < 80) throw LayoutError("COLS < 80!  Need a bigger terminal.");
    if (LINES < 25) throw LayoutError("LINES < 25!  Need a bigger terminal.");
    clear();

//    level_->draw_all();

    guy_->set_pos(level_->rooms()->front()->center());

//    guy_->add_fresh_inventory(new Key(Point(), 1, Key::BLUE));
//    guy_->add_fresh_inventory(new Rock(Point(), 10));
//    guy_->add_fresh_inventory(new Gold(Point(), 1));
//    guy_->add_fresh_inventory(new FunBall(Point(), 1));
//    guy_->add_fresh_inventory(new DaggerPlusOne(Point(), 1));
//    guy_->add_fresh_inventory(new ShortSword(Point(), 1));
//    guy_->add_fresh_inventory(new Dagger(Point(), 1));
//    guy_->add_fresh_inventory(new LongSword(Point(), 1));
//    guy_->add_fresh_inventory(new LongSwordPlusOne(Point(), 1));
//    guy_->add_fresh_inventory(new BastardSword(Point(), 1));
//    guy_->add_fresh_inventory(new SmallShield(Point(), 1));
//    guy_->add_fresh_inventory(new SmallWoodenShield(Point(), 1));
//    guy_->add_fresh_inventory(new MediumShield(Point(), 1));
//    guy_->add_fresh_inventory(new LeatherArmor(Point(), 1));
//    guy_->add_fresh_inventory(new StuddedLeatherArmor(Point(), 1));
//    guy_->add_fresh_inventory(new ChainMail(Point(), 1));
//    guy_->add_fresh_inventory(new ZombieCorpse(Point(), 1));
//    guy_->add_fresh_inventory(new MummyCorpse(Point(), 1));
//    guy_->add_fresh_inventory(new VampireCorpse(Point(), 1));
//    guy_->add_fresh_inventory(new WolfManCorpse(Point(), 1));
//    guy_->add_fresh_inventory(new Entrails(Point(), 1));

    draw_status();
    guy_->do_fov();

    bool quit = false;
    while (!quit) {
        bool turn = false;

        int ch = getch();
        //status(ch);
        switch (ch) {
            case 'k':
            case '8':
            case KEY_UP:
                turn = true;
                guy_->step(Direction::NORTH);
                break;
            case 'u':
            case '9':
            case KEY_PPAGE:
                turn = true;
                guy_->step(Direction::NORTHEAST);
                break;
            case 'l':
            case '6':
            case KEY_RIGHT:
                turn = true;
                guy_->step(Direction::EAST);
                break;
            case 'n':
            case '3':
            case KEY_NPAGE:
                turn = true;
                guy_->step(Direction::SOUTHEAST);
                break;
            case 'j':
            case '2':
            case KEY_DOWN:
                turn = true;
                guy_->step(Direction::SOUTH);
                break;
            case 'b':
            case '1':
            case KEY_END:
                turn = true;
                guy_->step(Direction::SOUTHWEST);
                break;
            case 'h':
            case '4':
            case KEY_LEFT:
                turn = true;
                guy_->step(Direction::WEST);
                break;
            case 'y':
            case '7':
            case KEY_HOME:
                turn = true;
                guy_->step(Direction::NORTHWEST);
                break;
            case '.':
            case ' ':
            case 10:
                turn = true;
                guy_->rest();
                break;
            case 'o':
                turn = true;
                guy_->open();
                break;
            case 'c':
                turn = true;
                guy_->close();
                break;
            case '<':
                turn = true;
                if (guy_->pos() == level_->stairs_up() && level_->tile_at(guy_->pos())->type() == Tile::STAIRS_UP) {
                    change_level(dungeon_.at(--current_level_));
                    status("You climb the stairs.");
                } else {
                    status("There's no way to go up here.");
                }
                break;
            case '>':
                turn = true;
                if (guy_->pos() == level_->stairs_down() && level_->tile_at(guy_->pos())->type() == Tile::STAIRS_DOWN) {
                    change_level(dungeon_.at(++current_level_));
                    status("You descend the stairs.");
                } else {
                    status("There's no way to go down here.");
                }
                break;
            case 'i':
                show_guy_inventory();
                break;
            case 9: //tab
                show_message_history();
                break;
            case ';': {
                status("Direction: ", true);
                Direction::DIRECTION dir = get_input_dir();
                if (dir != Direction::NONE)
                    guy_->walk(dir);
                break;
            }
            case ',':
                guy_->pickup_items();
                break;
            case 'd':
                turn = true;
                guy_->drop_inventory_item();
                break;
            case 'w':
                turn = true;
                guy_->equip_item();
                break;
            case 't':
                turn = guy_->throw_or_shoot();
                break;
            case 'e':
                turn = true;
                guy_->eat_item();
                break;
            case ':':
                guy_->look_at(guy_->pos(), true);
                break;
            case 'x' :
                examine();
                break;
            case 'U':
                turn = true;
                guy_->use_item();
                break;
            case 'Q': {
                level_->remember();
                clear();
                std::string q = "Are you sure you want to QUIT? ([y]/n)";
                mvaddstr(LINES / 2, (COLS - q.length()) / 2, q.c_str());
                quit = get_input_yes_or_no();
                if (!quit)
                    level_->draw_from_memory();
                break;
            }
            case '?':
                help();
                break;
            default: {
                std::ostringstream o;
                o << "\"" << (char)ch << "\"" << " unrecognized command.";
                status(o.str(), true);
                level_->world()->guy()->focus();
                break;
            }
        }

        if (turn) {
            do_turn();

            if (guy_->is_dead()) {
                quit = true;

                guy_->do_fov();
                level_->draw_tile_at(guy_->pos());
                guy_->stack_inventory_items();
                for (std::vector<Item*>::iterator it = guy_->inventory()->begin(); it != guy_->inventory()->end(); ++it) {
                    (*it)->set_pos(guy_->pos());
                    (*it)->draw();
                }
                guy_->focus();

                bool ok = false;
                while (!ok) {
                    switch (getch()) {
                        case 10:
                        case 27:
                        case ' ':
                            ok = true;
                            break;
                    }
                }
            } else {

                for (std::vector<Item*>::iterator it = guy_->inventory()->begin(); it != guy_->inventory()->end(); ++it) {
                    if ((*it)->type() == Item::GLOWING_ORB){
                        status("You picked up the glowing orb thing!  Congrats, you won the game!", true);
                        quit = true;
                        getch();
                        break;
                    }
                }

            }
        }
        refresh();
    }

    show_message_history();
    clear();
    std::string q = "See inventory? ([y]/n)";
    mvaddstr(LINES / 2, (COLS - q.length()) / 2, q.c_str());
    refresh();

    if (get_input_yes_or_no()) {
        if (guy_->inventory()->size() == 0) {
            clear();
            mvaddstr(0, 0, "You're not carrying anything!");
            refresh();
            getch();
        } else
            show_guy_inventory();
    }

    clear();
    q = "See level? ([y]/n)";
    mvaddstr(LINES / 2, (COLS - q.length()) / 2, q.c_str());
    refresh();
    if (get_input_yes_or_no()) {
        guy_->drop_all();
        level_->draw_all();
        guy_->focus();
        getch();
    }
}

void World::help() const {
    std::vector<std::string> lines;
    lines.push_back("                        COMMAND REFERENCE");
    lines.push_back("");
    lines.push_back("<------------------------- Environment ------------------------->");
    lines.push_back("  Move north: ............................ k or 8 or Up");
    lines.push_back("  Move east: ............................. l or 6 or Right");
    lines.push_back("  Move south: ............................ j or 2 or Down");
    lines.push_back("  Move west: ............................. h or 4 or Left");
    lines.push_back("  Move northeast: ........................ u or 9 or Page Up");
    lines.push_back("  Move southeast: ........................ n or 3 or Page Down");
    lines.push_back("  Move southwest: ........................ b or 1 or End");
    lines.push_back("  Move northwest: ........................ y or 7 or Home");
    lines.push_back("  Descend: ............................... >");
    lines.push_back("  Ascend: ................................ <");
    lines.push_back("  Open: .................................. o");
    lines.push_back("  Close: ................................. c");
    lines.push_back("  Examine: ............................... x");
    lines.push_back("  Examine ground: ........................ :");
    lines.push_back("  Walk: .................................. ;");
    lines.push_back("  Rest: .................................. . or Space or Enter");
    lines.push_back("  Throw at/target nearest: ............... t");
    lines.push_back("");
    lines.push_back("<-------------------------- Inventory -------------------------->");
    lines.push_back("  Show inventory: ........................ i");
    lines.push_back("  Pick up items: ......................... ,");
    lines.push_back("  Drop item: ............................. d");
    lines.push_back("  Wear/wield item: ....................... w");
    lines.push_back("  Eat food: .............................. e");
    lines.push_back("  Use tool: .............................. U");
    lines.push_back("");
    lines.push_back("<------------------------ Game Commands ------------------------>");
    lines.push_back("  Message history: ....................... Tab");
    lines.push_back("  Quit: .................................. Q");
    show_paginated(lines, "Help");
}

void World::create_dungeon() {
    for (int i = 0; i < 10; i++) {
        Level* level;
        switch (i) {
            case 0:
                level = new Level(this, Point(height_ / 2, width_ / 2), false);
                break;
            case 1:
                level = new Level(this, dungeon_.back()->stairs_down());
                break;
            case 2:
                level = new Level(this, dungeon_.back()->stairs_down());
                break;
            case 3:
                level = new Level(this, dungeon_.back()->stairs_down());
                break;
            case 4:
                level = new Level(this, dungeon_.back()->stairs_down());
                break;
            case 5:
            case 6:
            case 7:
            case 8:
                level = new Level(this, dungeon_.back()->stairs_down());
                break;
            case 9:
                level = new Level(this, dungeon_.back()->stairs_down(), true, false);
                break;
        }
        dungeon_.push_back(level);
        add_items(i);
        add_monsters(i);
    }
}

void World::add_monsters(int level) {
    std::vector<Monster*> monsters;
    switch (level) {
        case 0:
//            for (int i = 0; i < 50; i++) {
//                monsters.push_back(new Mummy(dungeon_.at(level)));
//            }
            for (int i = 0; i < Utils::random_int(15, 20); i++) {
                monsters.push_back(new Zombie(dungeon_.at(level)));
            }
            for (int i = 0; i < Utils::random_int(0, 2); i++) {
                monsters.push_back(new BabyMachineElf(dungeon_.at(level)));
            }
            break;
        case 1:
            for (int i = 0; i < Utils::random_int(15, 20); i++) {
                switch (Utils::random_int(0, 3)) {
                    case 0:
                        monsters.push_back(new Mummy(dungeon_.at(level)));
                        break;
                    default:
                        monsters.push_back(new Zombie(dungeon_.at(level)));
                        break;
                }
            }
            for (int i = 0; i < Utils::random_int(0, 3); i++) {
                monsters.push_back(new BabyMachineElf(dungeon_.at(level)));
            }
            break;
        case 2:
            for (int i = 0; i < Utils::random_int(15, 20); i++) {
                switch (Utils::random_int(0, 4)) {
                    case 0:
                        monsters.push_back(new Vampire(dungeon_.at(level)));
                        break;
                    default:
                        switch (Utils::random_int(0, 2)) {
                            case 0:
                                monsters.push_back(new Mummy(dungeon_.at(level)));
                                break;
                            default:
                                monsters.push_back(new Zombie(dungeon_.at(level)));
                                break;
                        }
                }
            }
            for (int i = 0; i < Utils::random_int(0, 4); i++) {
                monsters.push_back(new BabyMachineElf(dungeon_.at(level)));
            }
            break;
        case 3:
            for (int i = 0; i < Utils::random_int(15, 20); i++) {
                switch (Utils::random_int(0, 3)) {
                    case 0:
                        monsters.push_back(new Zombie(dungeon_.at(level)));
                        break;
                    case 1:
                    case 2:
                        monsters.push_back(new Mummy(dungeon_.at(level)));
                        break;
                    default:
                        monsters.push_back(new Vampire(dungeon_.at(level)));
                        break;
                }
            }
            for (int i = 0; i < Utils::random_int(0, 4); i++) {
                monsters.push_back(new BabyMachineElf(dungeon_.at(level)));
            }
            break;
        case 4:
            for (int i = 0; i < Utils::random_int(15, 20); i++) {
                switch (Utils::random_int(0, 3)) {
                    case 0:
                        monsters.push_back(new Zombie(dungeon_.at(level)));
                        break;
                    case 1:
                        monsters.push_back(new Mummy(dungeon_.at(level)));
                        break;
                    default:
                        monsters.push_back(new Vampire(dungeon_.at(level)));
                        break;
                }
            }
            for (int i = 0; i < Utils::random_int(0, 4); i++) {
                monsters.push_back(new BabyMachineElf(dungeon_.at(level)));
            }
            break;
        case 5:
            for (int i = 0; i < Utils::random_int(25, 30); i++) {
                monsters.push_back(new Vampire(dungeon_.at(level)));
            }
            for (int i = 0; i < Utils::random_int(0, 4); i++) {
                monsters.push_back(new BabyMachineElf(dungeon_.at(level)));
            }
            break;
        case 6:
            for (int i = 0; i < Utils::random_int(25, 30); i++) {
                switch (Utils::random_int(0, 3)) {
                    case 0:
                        monsters.push_back(new WolfMan(dungeon_.at(level)));
                        break;
                    default:
                        switch (Utils::random_int(0, 1)) {
                            case 0:
                                monsters.push_back(new Mummy(dungeon_.at(level)));
                                break;
                            default:
                                monsters.push_back(new Vampire(dungeon_.at(level)));
                                break;
                        }
                        break;
                }
            }
            for (int i = 0; i < Utils::random_int(5, 10); i++) {
                monsters.push_back(new BabyMachineElf(dungeon_.at(level)));
            }
            for (int i = 0; i < Utils::random_int(2, 5); i++) {
                monsters.push_back(new MachineElf(dungeon_.at(level)));
            }
            break;
        case 7:
            for (int i = 0; i < Utils::random_int(25, 30); i++) {
                switch (Utils::random_int(0, 1)) {
                    case 0:
                        monsters.push_back(new WolfMan(dungeon_.at(level)));
                        break;
                    default:
                        switch (Utils::random_int(0, 1)) {
                            case 0:
                                monsters.push_back(new Mummy(dungeon_.at(level)));
                                break;
                            default:
                                monsters.push_back(new Vampire(dungeon_.at(level)));
                                break;
                        }
                        break;
                }
            }
            for (int i = 0; i < Utils::random_int(5, 10); i++) {
                monsters.push_back(new BabyMachineElf(dungeon_.at(level)));
            }
            for (int i = 0; i < Utils::random_int(2, 5); i++) {
                monsters.push_back(new MachineElf(dungeon_.at(level)));
            }
            break;
        case 8:
            for (int i = 0; i < Utils::random_int(25, 30); i++) {
                switch (Utils::random_int(0, 3)) {
                    case 0:
                        switch (Utils::random_int(0, 1)) {
                            case 0:
                                monsters.push_back(new Mummy(dungeon_.at(level)));
                                break;
                            default:
                                monsters.push_back(new Vampire(dungeon_.at(level)));
                                break;
                        }
                        break;
                    default:
                        monsters.push_back(new WolfMan(dungeon_.at(level)));
                        break;
                }
            }
            for (int i = 0; i < 10; i++) {
                monsters.push_back(new MachineElf(dungeon_.at(level)));
            }
            break;
        case 9:
            for (int i = 0; i < 25; i++) {
                monsters.push_back(new Vampire(dungeon_.at(level)));
            }
            for (int i = 0; i < 25; i++) {
                monsters.push_back(new WolfMan(dungeon_.at(level)));
            }
            for (int i = 0; i < 25; i++) {
                monsters.push_back(new MachineElf(dungeon_.at(level)));
            }
            break;
    }
    dungeon_.at(level)->add_monsters(monsters);
    if (1) { //level == 0) {
        int room_one_monsters = 0;
        for (std::vector<Monster*>::iterator it = dungeon_.at(level)->monsters()->begin(); it != dungeon_.at(level)->monsters()->end();) {
            if (++room_one_monsters > 2 && *(dungeon_.at(level)->find_room(*it)) == *(dungeon_.at(level)->rooms()->front())) {
                dungeon_.at(level)->tile_at((*it)->pos())->set_passage_blocked(false);
                delete *it;
                dungeon_.at(level)->monsters()->erase(it);
            } else {
                ++it;
            }
        }
    }
}

void World::add_items(int level) {
    std::vector<Item*> items;
    switch (level) {
        case 0: {
            {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Key* item = new Key(pos, 1, Key::BLUE);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(10, 20); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Gold* item = new Gold(pos, Utils::random_int(1, 50));
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 5); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Rock* item = new Rock(pos, Utils::random_int(1, 5));
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(1, 3); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Dagger* item = new Dagger(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 3); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                SmallWoodenShield* item = new SmallWoodenShield(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(1, 2); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                LeatherArmor* item = new LeatherArmor(pos, 1);
                items.push_back(item);
            }
            break;
        }
        case 1: {
            {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Key* item = new Key(pos, 1, Key::GREEN);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(10, 20); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Gold* item = new Gold(pos, Utils::random_int(1, 50));
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 5); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Rock* item = new Rock(pos, Utils::random_int(1, 5));
                items.push_back(item);
            }
            if (Utils::percent_chance(20)) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                FunBall* item = new FunBall(pos, Utils::random_int(5, 15));
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 3); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Dagger* item = new Dagger(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(1, 3); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                SmallWoodenShield* item = new SmallWoodenShield(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 3); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                SmallShield* item = new SmallShield(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 2); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                LeatherArmor* item = new LeatherArmor(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 1); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                ShortSword* item = new ShortSword(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 1); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                DaggerPlusOne* item = new DaggerPlusOne(pos, 1);
                items.push_back(item);
            }
            break;
        }
        case 2: {
            {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Key* item = new Key(pos, 1, Key::RED);
                items.push_back(item);
            }
            {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                StuddedLeatherArmor* item = new StuddedLeatherArmor(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(10, 20); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Gold* item = new Gold(pos, Utils::random_int(1, 50));
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 5); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Rock* item = new Rock(pos, Utils::random_int(1, 5));
                items.push_back(item);
            }
            if (Utils::percent_chance(20)) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                FunBall* item = new FunBall(pos, Utils::random_int(5, 15));
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(1, 3); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Dagger* item = new Dagger(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 3); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                SmallWoodenShield* item = new SmallWoodenShield(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 3); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                SmallShield* item = new SmallShield(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 2); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                LeatherArmor* armor = new LeatherArmor(pos, 1);
                items.push_back(armor);
            }
            for (int i = 0; i < Utils::random_int(0, 2); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                ShortSword* item = new ShortSword(pos, 1);
                items.push_back(item);
            }
            if (Utils::percent_chance(20)) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                ShortSwordPlusOne* item = new ShortSwordPlusOne(pos, 1);
                items.push_back(item);
            }
            break;
        }
        case 3:
        case 4:
            if (Utils::percent_chance(20)) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                StuddedLeatherArmor* item = new StuddedLeatherArmor(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(10, 20); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Gold* item = new Gold(pos, Utils::random_int(1, 50));
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 5); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Rock* item = new Rock(pos, Utils::random_int(1, 5));
                items.push_back(item);
            }
            if (Utils::percent_chance(20)) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                FunBall* item = new FunBall(pos, Utils::random_int(5, 15));
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(1, 3); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Dagger* item = new Dagger(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 3); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                SmallWoodenShield* item = new SmallWoodenShield(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(1, 3); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                SmallShield* item = new SmallShield(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 2); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                LeatherArmor* armor = new LeatherArmor(pos, 1);
                items.push_back(armor);
            }
            for (int i = 0; i < Utils::random_int(0, 2); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                ShortSword* item = new ShortSword(pos, 1);
                items.push_back(item);
            }
            if (Utils::percent_chance(20)) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                ShortSwordPlusOne* item = new ShortSwordPlusOne(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 2); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                LongSword* item = new LongSword(pos, 1);
                items.push_back(item);
            }
            if (Utils::percent_chance(20)) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                LongSwordPlusOne* item = new LongSwordPlusOne(pos, 1);
                items.push_back(item);
            }
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            if (Utils::percent_chance(20)) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                ChainMail* item = new ChainMail(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 3); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                MediumShield* item = new MediumShield(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(10, 20); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Gold* item = new Gold(pos, Utils::random_int(1, 50));
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 5); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Rock* item = new Rock(pos, Utils::random_int(1, 5));
                items.push_back(item);
            }
            if (Utils::percent_chance(20)) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                FunBall* item = new FunBall(pos, Utils::random_int(5, 15));
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 2); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                ShortSword* item = new ShortSword(pos, 1);
                items.push_back(item);
            }
            if (Utils::percent_chance(20)) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                ShortSwordPlusOne* item = new ShortSwordPlusOne(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 2); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                LongSword* item = new LongSword(pos, 1);
                items.push_back(item);
            }
            if (Utils::percent_chance(20)) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                LongSwordPlusOne* item = new LongSwordPlusOne(pos, 1);
                items.push_back(item);
            }
            if (Utils::percent_chance(15)) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                BastardSword* item = new BastardSword(pos, 1);
                items.push_back(item);
            }
            break;
        case 9:
            if (Utils::percent_chance(20)) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                ChainMail* item = new ChainMail(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 3); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                MediumShield* item = new MediumShield(pos, 1);
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(10, 20); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Gold* item = new Gold(pos, Utils::random_int(1, 50));
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 5); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                Rock* item = new Rock(pos, Utils::random_int(1, 5));
                items.push_back(item);
            }
            if (Utils::percent_chance(20)) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                FunBall* item = new FunBall(pos, Utils::random_int(5, 15));
                items.push_back(item);
            }
            for (int i = 0; i < Utils::random_int(0, 2); i++) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                LongSword* item = new LongSword(pos, 1);
                items.push_back(item);
            }
            if (Utils::percent_chance(20)) {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                LongSwordPlusOne* item = new LongSwordPlusOne(pos, 1);
                items.push_back(item);
            }
            {
                Point pos = dungeon_.at(level)->get_random_room_pos();
                GlowingOrb* item = new GlowingOrb(pos, 1);
                items.push_back(item);
            }
            break;
    }
//TODO: ...
//    for (std::vector<Door*>::iterator it = level_->door_points().begin(); it != level_->door_points().end(); ++it) {
//        Door* door = static_cast<Door*>(level_->tile_at(*it));
//        if (door)
//    }


    dungeon_.at(level)->add_items(items);
}

void World::change_level(Level* new_level) {
    for (std::vector<Monster*>::iterator it = level_->monsters()->begin(); it != level_->monsters()->end(); ++it) {
        if (*(*it) == *guy_) {
            level_->monsters()->erase(it);
            break;
        }
    }
    for (std::vector<Item*>::iterator it = level_->items()->begin(); it != level_->items()->end();) {
        if ((*it)->held() && *(*it)->owner() == *guy_) {
            new_level->items()->push_back(*it);
            level_->items()->erase(it);
        } else {
            ++it;
        }
    }
    level_->remember();
    level_ = new_level;
    level_->monsters()->push_back(guy_);
    guy_->set_level(level_);
    level_->tile_at(guy_->pos())->set_passage_blocked(true);
    clear();
    level_->draw_from_memory();
}

void World::do_turn() {
    turns_++;

    bool guy_being_chased = false;

    size_t items_here = 0;
    size_t items_here_now = 0;
    std::vector<Item*> items = level_->find_items(guy_->pos());
    for (std::vector<Item*>::iterator it = items.begin(); it != items.end(); ++it)
        items_here += (*it)->quantity();

    bool keep_going = true;

    for (std::vector<Monster*>::iterator monster_it = level_->monsters()->begin(); keep_going && monster_it != level_->monsters()->end();) {
        if ((*monster_it)->is_dead()) {
            Tile* tile = level_->tile_at((*monster_it)->pos());

            if (*(*monster_it) != *guy_)
                (*monster_it)->drop_all();

            if ((*monster_it)->drops_corpse())
                (*monster_it)->drop_corpse();

            if (guy_->sees((*monster_it)->pos())) {
                level_->draw_tile_at((*monster_it)->pos());
                for (std::vector<Item*>::iterator item_it = level_->items()->begin(); item_it != level_->items()->end(); ++item_it) {
                    if ((*item_it)->pos() == (*monster_it)->pos() && guy_->sees((*monster_it)->pos()) && !(*item_it)->held()) {
                        (*item_it)->draw();
                    }
                }
            }

            tile->set_passage_blocked(false);

            if (*(*monster_it) != *guy_)
                delete *monster_it;
            level_->monsters()->erase(monster_it);
        }
        else {
            if (*(*monster_it) == *guy_) {
                int satiation_lvl_prev = 3;

                if (guy_->is_satiated()) {
                    satiation_lvl_prev = 4;
                } else if (guy_->is_hungry()) {
                    satiation_lvl_prev = 2;
                } else if (guy_->is_very_hungry()) {
                    satiation_lvl_prev = 1;
                } else if (guy_->is_starving()) {
                    satiation_lvl_prev = 0;
                }

                guy_->hunger();

                int satiation_lvl = 3;

                if (guy_->is_satiated()) {
                    satiation_lvl = 4;
                } else if (guy_->is_hungry()) {
                    satiation_lvl = 2;
                } else if (guy_->is_very_hungry()) {
                    satiation_lvl = 1;
                } else if (guy_->is_starving()) {
                    satiation_lvl = 0;
                }

                if (satiation_lvl != satiation_lvl_prev) {
                    if (guy_->is_satiated()) {
                        status("You're satiated.");
                    } else if (guy_->is_hungry()) {
                        status("You're hungry.");
                    } else if (guy_->is_very_hungry()) {
                        status("You're hungry!");
                    } else if (guy_->is_starving()) {
                        status("You're starving!");
                    }
                }
            }
            else {
                if ((*monster_it)->breathe_every() == 0 || (turns_ + (*monster_it)->breathe_offset()) % (*monster_it)->breathe_every() != 0) {
                    (*monster_it)->look_for_guy();
                    if ((*monster_it)->is_hostile() && (*monster_it)->chasing_guy()) {
                        guy_being_chased = true;

                        bool did_attack = false;
                        if ((*monster_it)->sees(guy_->pos()) && (*monster_it)->right_hand_item() != NULL && (*monster_it)->right_hand_item()->category() == Item::WEAPON) {
                            Weapon* weapon = static_cast<Weapon*>((*monster_it)->right_hand_item());
                            if (weapon->is_throwable()) {
                                (*monster_it)->throw_at(guy_->pos());
                                did_attack = true;
                            }
                        }

                        if (!did_attack)
                            (*monster_it)->step_toward(guy_->pos());
                    }
                    else {
                        (*monster_it)->step();
                        (*monster_it)->regenerate();
                    }
                } else if (!guy_->sees((*monster_it)->pos()) && level_->tile_at((*monster_it)->pos())->drawn()) {
                    level_->draw_tile_at((*monster_it)->pos());
                    for (std::vector<Item*>::iterator it = level_->items()->begin(); it != level_->items()->end(); ++it) {
                        if ((*it)->seen_pos() == (*monster_it)->pos())
                            (*it)->draw_at_seen_pos();
                    }
                }
            }

            if (guy_->is_dead())
                keep_going = false;

            ++monster_it;
        }
    }

    if (!guy_->is_dead()) {
        if (!guy_being_chased)
            guy_->regenerate();

        guy_->do_fov();

        items = level_->find_items(guy_->pos());
        for (std::vector<Item*>::iterator it = items.begin(); it != items.end(); ++it)
            items_here_now += (*it)->quantity();

        if (items_here_now > items_here)
            guy_->look_at(guy_->pos());
    }

    draw_status();
    guy_->focus();
}

std::vector<std::string> World::prepare_status() {
    std::vector<std::string> top_lines;
    boost::regex strip_tags_regex = boost::regex("(<`.\\:|`>)", boost::regex::extended);
    std::ostringstream o_line;
    bool end_sentence = false;

    for (size_t i = 0; i < status_->size(); i++) {
        std::string word;
        std::istringstream in(status_->at(i));

        while (!in.eof()) {
            in >> word;
            std::string stripped_word = boost::regex_replace(word, strip_tags_regex, "");
            std::string stripped_line = boost::regex_replace(o_line.str(), strip_tags_regex, "");
            if (stripped_line.length() + stripped_word.length() + 1 > (size_t)COLS) {
                top_lines.push_back(o_line.str());
                o_line.str("");
            } else if (o_line.str().length() != 0) {
                if (end_sentence)
                    word = " " + word;
                o_line << " ";
            }

            o_line << word;

            end_sentence = false;
            switch (word.at(word.length() - 1)) {
                case '.':
                case '!':
                case '?':
                    end_sentence = true;
                    break;
                case '>':
                    if (word.at(word.length() - 2) == '`') {
                        switch (word.at(word.length() - 3)) {
                            case '.':
                            case '!':
                            case '?':
                                end_sentence = true;
                                break;
                        }
                    }
                    break;
            }
        }
    }

    if (o_line.str().length() > 0)
        top_lines.push_back(o_line.str());

    return top_lines;
}

void World::draw_status() {
    std::vector<std::string> top_lines;
    std::ostringstream o_bottom;
    int dex_penalty, ac;

    for (int y = 0; y < 3; y++) {
        mvaddstr(y, 0, std::string(COLS, ' ').c_str());
    }
    for (int y = LINES - 3; y < LINES; y++) {
        mvaddstr(y, 0, std::string(COLS, ' ').c_str());
    }

    top_lines = prepare_status();

    if (top_lines.size() > 0) {
        int y = 0;
        size_t i = top_lines.size() > 3 ? top_lines.size() - 3 : 0;
        for (; i < top_lines.size(); i++) {
            draw_coloured_line(y, top_lines.at(i));
            y++;
        }
    }

    for (std::vector<std::string>::iterator it = status_->begin(); !status_->empty();) {
        std::ostringstream o;
        o << turns_ << ' ' << *it;
        status_history_->push_back(o.str());
        status_->erase(it);
    }

    if (guy_->is_satiated()) {
        draw_coloured_line(LINES - 2, "<`c:satiated`>");
    } else if (guy_->is_hungry()) {
        draw_coloured_line(LINES - 2, "hungry.");
    } else if (guy_->is_very_hungry()) {
        draw_coloured_line(LINES - 2, "<`y:hungry!`>");
    } else if (guy_->is_starving()) {
        draw_coloured_line(LINES - 2, "<`R:starving!`>");
    }

    dex_penalty = 0;
    if (guy_->right_hand_item() != NULL && guy_->right_hand_item()->category() == Item::WEAPON) {
        Weapon* weapon = static_cast<Weapon*>(guy_->right_hand_item());
        if (weapon->weapon_category() == Weapon::RANGED || weapon->is_throwable()) {
            if (guy_->armor() != NULL && guy_->armor()->category() == Item::ARMOR)
                dex_penalty += static_cast<Armor*>(guy_->armor())->dexterity_penalty();
            if (guy_->left_hand_item() != NULL && guy_->left_hand_item()->category() == Item::SHIELD)
                dex_penalty += static_cast<Shield*>(guy_->left_hand_item())->dexterity_penalty();
        }
    }

    ac = 10;
    if (guy_->armor() != NULL && guy_->armor()->category() == Item::ARMOR)
        ac += static_cast<Armor*>(guy_->armor())->ac();
    if (guy_->left_hand_item() != NULL && guy_->left_hand_item()->category() == Item::SHIELD)
        ac += static_cast<Shield*>(guy_->left_hand_item())->ac();

    o_bottom << "[hp:";
    if (guy_->hp() < 10)
        o_bottom << "<`r:" << guy_->hp() << "`>";
    else
        o_bottom << guy_->hp();
    o_bottom << '/' << guy_->hp_max() << "] ";

    o_bottom << "[xp:" << guy_->xp() << '/' << guy_->next_lvl_xp() << "] [lvl:" << guy_->lvl() << "] [s:" << guy_->strength() << "] ";

    o_bottom << "[d:" << guy_->dexterity();
    if (dex_penalty > 0)
        o_bottom << '-' << dex_penalty;

    o_bottom << "] ";
    o_bottom << "[ac:" << ac << "] ";
    o_bottom << "[u:" << current_level_ + 1 << "] ";
//    o << "[$:" << guy_->wealth() << "] ";
    o_bottom << "[t:" << turns_ << ']';

    draw_coloured_line(LINES - 1, o_bottom.str());

    refresh();
}

void World::status(std::string text, bool show_now) {
    if (show_now) {
        int a = -1;
        bool append = false;
        for (int y = 0; y < 3; y++) {
            if (mvinch(y, 0) != ' ') {
                a = y;
                append = true;
            }
        }
        if (a == 2 || a == -1) {
            a = 0;
            append = false;
            for (int y = 0; y < 3; y++)
                mvaddstr(y, 0, std::string(COLS, ' ').c_str());
        }
        mvaddstr(append ? ++a : 0, 0, text.c_str());
        refresh();
    } else
        status_->push_back(text);
}

void World::status(const Point pos) {
    status("y:" + Utils::int_to_string(pos.y) + ", x:" + Utils::int_to_string(pos.x), true);
}

void World::status(int i) {
    status(Utils::int_to_string(i), true);
}

void World::draw_coloured_line(int y, std::string line) const {
    Colour::COLOUR colour;

    move(y, 0);

    for (size_t i = 0; i < line.length(); i++) {
        char c = line.at(i);
        if (c == '<' && i < line.length() - 3 && line.at(i + 1) == '`') {
            i += 2;
            c = line.at(i);

            colour = Utils::colour(c);

            if (colour != Colour::NO_COLOUR) {
                attron(colour);
                i++;
            }
        } else if (c == '`' && line.at(i + 1) == '>') {
            attroff(colour);
            i++;
        } else
            addch(c);
    }

    standend();
}

void World::paginated_redraw(int top, std::vector<std::string> &lines, std::string bottom_line) const {
    clear();

    int y = 0;
    for (size_t i = top; y < LINES - 1 && i < lines.size(); i++) {
        draw_coloured_line(y, lines.at(i));
        y++;
    }

    draw_coloured_line(LINES - 1, bottom_line);

    bottom_line = boost::regex_replace(bottom_line, boost::regex("(<`.\\:|`>)", boost::regex::extended), "");
    move(LINES - 1, bottom_line.length());
    refresh();
}

void World::show_paginated(std::vector<std::string> &lines, std::string title) const {
    level_->remember();
    int top = 0;
    if (lines.size() > (size_t)(LINES - 1))
        top = lines.size() - LINES + 1;

    std::ostringstream bottom_line;
    bottom_line << title << " - [Home/End - Page Up/Page Down - Up/Down]";

    paginated_redraw(top, lines, bottom_line.str());

    bool done = false;
    while (!done) {
        switch (getch()) {
            case 'k':
            case '8':
            case KEY_UP:
                if ((int)lines.size() > LINES - 1) {
                    top--;
                    if (top < 0)
                        top = 0;
                    paginated_redraw(top, lines, bottom_line.str());
                }
                break;
            case 'j':
            case '2':
            case KEY_DOWN:
                if ((int)lines.size() > LINES - 1) {
                    top++;
                    if (top > (int)lines.size() - LINES + 1)
                        top = lines.size() - LINES + 1;

                    paginated_redraw(top, lines, bottom_line.str());
                }
                break;
            case KEY_PPAGE:
                if ((int)lines.size() > LINES - 1) {
                    top -= LINES - 1;
                    if (top < 0)
                        top = 0;

                    paginated_redraw(top, lines, bottom_line.str());
                }
                break;
            case KEY_NPAGE:
                if ((int)lines.size() > LINES - 1) {
                    top += LINES - 1;
                    if (top > (int)lines.size() - LINES + 1)
                        top = lines.size() - LINES + 1;

                    paginated_redraw(top, lines, bottom_line.str());
                }
                break;
            case KEY_HOME:
                if ((int)lines.size() > LINES - 1) {
                    top = 0;

                    paginated_redraw(top, lines, bottom_line.str());
                }
                break;
            case KEY_END:
                if ((int)lines.size() > LINES - 1) {
                    top = lines.size() - LINES + 1;

                    paginated_redraw(top, lines, bottom_line.str());
                }
                break;
            case 9:
            case 'Q':
            case 'q':
            case ' ':
            case 10:
            case 27:
                done = true;
                break;
        }
    }

    clear();
    level_->draw_from_memory();
}

std::string World::inventory_line(Item& item) {
    int s = 7 - Utils::int_to_string(item.quantity()).size();
    if (s < 1) s = 1;
    std::string spaces(s, ' ');

    std::ostringstream o;

    o << spaces;
    o << item.quantity();
    o << ' ' << (item.quantity() == 1 ? item.noun() : item.nouns());

    s = 31 - o.str().length();
    if (s < 1) s = 1;
    spaces = std::string(s, ' ');

    if (item.category() == Item::WEAPON)
        o << spaces << "[" << (static_cast<Weapon*>(&item))->dice().name() << ']';
    else if (item.category() == Item::SHIELD) {
        int ac = (static_cast<Shield*>(&item))->ac();
        o << spaces << "[ac";
        if (ac > 0) o <<  '+';
        o << ac << ']';
    }
    else if (item.category() == Item::ARMOR) {
        int ac = (static_cast<Armor*>(&item))->ac();
        o << spaces << "[ac";
        if (ac > 0) o <<  '+';
        o << ac << ']';
    }

    s = 41 - o.str().length();
    if (s < 1) s = 1;
    spaces = std::string(s, ' ');

    if (guy_->left_hand_item() != NULL && *(guy_->left_hand_item()) == item)
        o << spaces << "(equipped left hand)";
    else if (guy_->right_hand_item() != NULL && *(guy_->right_hand_item()) == item)
        o << spaces << "(equipped right hand)";
    else if (guy_->armor() != NULL && *(guy_->armor()) == item)
        o << spaces << "(being worn)";
    return o.str();
}

std::vector<std::string> World::format_items(std::vector<Item*>& items, bool do_key_map) {
    std::vector<std::string> lines;
    std::vector<Weapon> weapons;
    std::vector<Armor> armors;
    std::vector<Shield> shields;
    std::vector<Tool> tools;
    std::vector<Food> food;
    std::vector<PlotItem> plot_items;
    std::vector<Gold> money;

    for (std::vector<Item*>::iterator it = items.begin(); it != items.end(); ++it) {
        switch ((*it)->category()) {
            case (Item::WEAPON):
                weapons.push_back(*(static_cast<Weapon*>(*it)));
                break;
            case (Item::ARMOR):
                armors.push_back(*(static_cast<Armor*>(*it)));
                break;
            case (Item::SHIELD):
                shields.push_back(*(static_cast<Shield*>(*it)));
                break;
            case (Item::TOOL):
                tools.push_back(*(static_cast<Tool*>(*it)));
                break;
            case (Item::FOOD):
                food.push_back(*(static_cast<Food*>(*it)));
                break;
            case (Item::PLOT):
                plot_items.push_back(*(static_cast<PlotItem*>(*it)));
                break;
            case (Item::MONEY):
                money.push_back(*(static_cast<Gold*>(*it)));
                break;
            default:
                break;
        }
    }

    std::sort(weapons.begin(), weapons.end());
    std::sort(armors.begin(), armors.end());
    std::sort(shields.begin(), shields.end());
    std::sort(tools.begin(), tools.end());
    std::sort(food.begin(), food.end());
    std::sort(money.begin(), money.end());
    std::sort(plot_items.begin(), plot_items.end());

    if (do_key_map) inventory_keys_.clear();
    char c = 'a';
    if (weapons.size() > 0) {
        lines.push_back("<--------------------------- WEAPONS --------------------------->");
        for (std::vector<Weapon>::iterator iit = weapons.begin(); iit != weapons.end(); ++iit) {
            std::ostringstream o;
            if (do_key_map) {
                inventory_keys_[iit->id()] = c;
                o << ' ' << c;
            }
            o << inventory_line(*iit);
            if (++c == 'z' + 1) c = 'A';
            if (c == 'Z') throw std::runtime_error("no more alphabet letters!");
            lines.push_back(o.str());
        }
        lines.push_back("");
    }
    if (shields.size() > 0) {
        lines.push_back("<--------------------------- SHIELDS --------------------------->");
        for (std::vector<Shield>::iterator iit = shields.begin(); iit != shields.end(); ++iit) {
            std::ostringstream o;
            if (do_key_map) {
                inventory_keys_[iit->id()] = c;
                o << ' ' << c;
            }
            o << inventory_line(*iit);
            if (++c == 'z' + 1) c = 'A';
            if (c == 'Z') throw std::runtime_error("no more alphabet letters!");
            lines.push_back(o.str());
        }
        lines.push_back("");
    }
    if (armors.size() > 0) {
        lines.push_back("<---------------------------- ARMOR ---------------------------->");
        for (std::vector<Armor>::iterator iit = armors.begin(); iit != armors.end(); ++iit) {
            std::ostringstream o;
            if (do_key_map) {
                inventory_keys_[iit->id()] = c;
                o << ' ' << c;
            }
            o << inventory_line(*iit);
            if (++c == 'z' + 1) c = 'A';
            if (c == 'Z') throw std::runtime_error("no more alphabet letters!");
            lines.push_back(o.str());
        }
        lines.push_back("");
    }
    if (tools.size() > 0) {
        lines.push_back("<---------------------------- TOOLS ---------------------------->");
        for (std::vector<Tool>::iterator iit = tools.begin(); iit != tools.end(); ++iit) {
            std::ostringstream o;
            if (do_key_map) {
                inventory_keys_[iit->id()] = c;
                o << ' ' << c;
            }
            o << inventory_line(*iit);
            if (++c == 'z' + 1) c = 'A';
            if (c == 'Z') throw std::runtime_error("no more alphabet letters!");
            lines.push_back(o.str());
        }
        lines.push_back("");
    }
    if (food.size() > 0) {
        lines.push_back("<------------------------- CONSUMABLES ------------------------->");
        for (std::vector<Food>::iterator iit = food.begin(); iit != food.end(); ++iit) {
            std::ostringstream o;
            if (do_key_map) {
                inventory_keys_[iit->id()] = c;
                o << ' ' << c;
            }
            o << inventory_line(*iit);
            if (++c == 'z' + 1) c = 'A';
            if (c == 'Z') throw std::runtime_error("no more alphabet letters!");
            lines.push_back(o.str());
        }
        lines.push_back("");
    }
    if (plot_items.size() > 0) {
        lines.push_back("<------------------------- QUEST ITEMS ------------------------->");
        for (std::vector<PlotItem>::iterator iit = plot_items.begin(); iit != plot_items.end(); ++iit) {
            std::ostringstream o;
            if (do_key_map) {
                inventory_keys_[iit->id()] = c;
                o << ' ' << c;
            }
            o << inventory_line(*iit);
            if (++c == 'z' + 1) c = 'A';
            if (c == 'Z') throw std::runtime_error("no more alphabet letters!");
            lines.push_back(o.str());
        }
        lines.push_back("");
    }
    if (money.size() > 0) {
        lines.push_back("<--------------------------- COINAGE --------------------------->");
        for (std::vector<Gold>::iterator iit = money.begin(); iit != money.end(); ++iit) {
            std::ostringstream o;
            if (do_key_map) {
                inventory_keys_[iit->id()] = '$';
                o << " $";
            }
            o << inventory_line(*iit);
            if (++c == 'z' + 1) c = 'A';
            if (c == 'Z') throw std::runtime_error("no more alphabet letters!");
            lines.push_back(o.str());
        }
        lines.push_back("");
    }

    lines.pop_back();
    return lines;
}

void World::show_guy_inventory() {
    guy_->stack_inventory_items();
    if (guy_->inventory()->size() > 0) {
        std::vector<std::string> lines = format_items(*(guy_->inventory()));
        show_paginated(lines, "Inventory");
    } else {
        status("You're not carrying anything!", true);
        guy_->focus();
    }
}

void World::show_message_history() {
    level_->remember();
    show_paginated(*status_history_, "Message history");
}

Direction::DIRECTION World::opposite_dir(Direction::DIRECTION dir) const {
    switch (dir) {
        case Direction::NORTH:
            return Direction::SOUTH;
        case Direction::NORTHEAST:
            return Direction::SOUTHWEST;
        case Direction::EAST:
            return Direction::WEST;
        case Direction::SOUTHEAST:
            return Direction::NORTHWEST;
        case Direction::SOUTH:
            return Direction::NORTH;
        case Direction::SOUTHWEST:
            return Direction::NORTHEAST;
        case Direction::WEST:
            return Direction::EAST;
        case Direction::NORTHWEST:
            return Direction::SOUTHEAST;
        default:
            return Direction::NONE;
    }
}

Direction::DIRECTION World::clockwise_dir(Direction::DIRECTION dir) const {
    switch (dir) {
        case Direction::NORTH:
            return Direction::NORTHEAST;
        case Direction::NORTHEAST:
            return Direction::EAST;
        case Direction::EAST:
            return Direction::SOUTHEAST;
        case Direction::SOUTHEAST:
            return Direction::SOUTH;
        case Direction::SOUTH:
            return Direction::SOUTHWEST;
        case Direction::SOUTHWEST:
            return Direction::WEST;
        case Direction::WEST:
            return Direction::NORTHWEST;
        case Direction::NORTHWEST:
            return Direction::NORTH;
        default:
            return Direction::NONE;
    }

}

std::vector<Point> World::find_path(Monster* who, Point start, Point dest, bool diags, bool check_blocked) const {
    std::vector<Point> path;
    std::vector<PathItem> main_list;
    main_list.push_back(PathItem(dest.y, dest.x, 0));
    bool solved = false;
    for (size_t i = 0; !solved && i < main_list.size(); i++) {
        PathItem ml_item = main_list.at(i);

        std::vector<PathItem> adjs;
        adjs.push_back(PathItem(ml_item.pos.y - 1, ml_item.pos.x, ml_item.c + 1));
        adjs.push_back(PathItem(ml_item.pos.y, ml_item.pos.x + 1, ml_item.c + 1));
        adjs.push_back(PathItem(ml_item.pos.y + 1, ml_item.pos.x, ml_item.c + 1));
        adjs.push_back(PathItem(ml_item.pos.y, ml_item.pos.x - 1, ml_item.c + 1));

        if (diags) {
            adjs.push_back(PathItem(ml_item.pos.y - 1, ml_item.pos.x + 1, ml_item.c + 1));
            adjs.push_back(PathItem(ml_item.pos.y + 1, ml_item.pos.x + 1, ml_item.c + 1));
            adjs.push_back(PathItem(ml_item.pos.y + 1, ml_item.pos.x - 1, ml_item.c + 1));
            adjs.push_back(PathItem(ml_item.pos.y - 1, ml_item.pos.x - 1, ml_item.c + 1));
        }

        for (std::vector<PathItem>::iterator it = adjs.begin(); it != adjs.end();) {
            bool del = false;
            if (it->pos.y < 1 || it->pos.y > level_->height() - 2 || it->pos.x < 1 || it->pos.x > level_->width() - 2)
                del = true;
            else {
                bool invalid = it->pos != start && ( check_blocked ? level_->tile_at(it->pos)->passage_blocked() : level_->tile_at(it->pos)->type() == Tile::WALL );
                if (who->can_open_doors() && level_->tile_at(it->pos)->type() == Tile::DOOR) {
                    Door* door = static_cast<Door*>(level_->tile_at(it->pos));
                    if (!door->is_locked()) //TODO: || has_key ... ?
                        invalid = false;
                }
                if (invalid)
                    del = true;
                else {
                    for (std::vector<PathItem>::iterator mit = main_list.begin(); mit != main_list.end(); ++mit) {
                        if (mit->pos == it->pos && mit->c <= it->c) {
                            del = true;
                            break;
                        }
                    }
                }
            }
            if (del) adjs.erase(it);
            else ++it;
        }
        for (std::vector<PathItem>::iterator it = adjs.begin(); it != adjs.end(); ++it) {
            main_list.push_back(*it);
            if (it->pos == start) {
                solved = true;
                break;
            }
        }
    }

    if (solved) {
        Point current = start;
        PathItem entry;
        int max_lowest = level_->height() * level_->width();
        while (current != dest) {
            bool push = true;
            int lowest = max_lowest;
            for (std::vector<PathItem>::iterator mit = main_list.begin(); mit != main_list.end(); ++mit) {
                if ((diags && are_adjacent(mit->pos, current))
                        || (!diags && (!are_adjacent_diagonally(mit->pos, current) && are_adjacent(mit->pos, current)))) {
                    if (mit->c < lowest || (mit->c == lowest && Utils::percent_chance(50))) {
                        lowest = mit->c;
                        entry = *mit;
                    }
                }
            }
            current = entry.pos;
            if (push) path.push_back(entry.pos);
        }
    }
    return path;
}

std::vector<Point> World::bresenham_path(Point start, Point end) const {
    std::vector<Point> path;
    int dx = abs(end.x - start.x), sx = start.x < end.x ? 1 : -1;
    int dy = abs(end.y - start.y), sy = start.y < end.y ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;
    while (1) {
        path.push_back(start);
        if (start.x == end.x && start.y == end.y)
            break;
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            start.x += sx;
        }
        if (e2 < dy) {
            err += dx;
            start.y += sy;
        }
    }
    return path;
}

Point World::get_adjacent_pos(Point pos, Direction::DIRECTION dir) const {
    Point npos(-1, -1);
    switch (dir){
        case Direction::NORTH:
            npos = Point(pos.y - 1, pos.x);
            break;
        case Direction::NORTHEAST:
            npos = Point(pos.y - 1, pos.x + 1);
            break;
        case Direction::EAST:
            npos = Point(pos.y, pos.x + 1);
            break;
        case Direction::SOUTHEAST:
            npos = Point(pos.y + 1, pos.x + 1);
            break;
        case Direction::SOUTH:
            npos = Point(pos.y + 1, pos.x);
            break;
        case Direction::SOUTHWEST:
            npos = Point(pos.y + 1, pos.x - 1);
            break;
        case Direction::WEST:
            npos = Point(pos.y, pos.x - 1);
            break;
        case Direction::NORTHWEST:
            npos = Point(pos.y - 1, pos.x - 1);
            break;
        default:
            break;
    }
    return npos;
}

std::vector<Point> World::get_adjacent_points(Point pos) const {
    std::vector<Point> points;
    for (int y = pos.y - 1; y < pos.y + 2; y++) {
        for (int x = pos.x - 1; x < pos.x + 2; x++) {
            Point npos(y, x);
            if (npos != pos)
                points.push_back(npos);
        }
    }
    return points;
}

std::vector<Point> World::get_walk_check_points(Point pos, Direction::DIRECTION ignore_dir) const {
    std::vector<Point> points;
    if (ignore_dir != Direction::NORTH)
        points.push_back(Point(pos.y - 1, pos.x));
    if (ignore_dir != Direction::EAST)
        points.push_back(Point(pos.y, pos.x + 1));
    if (ignore_dir != Direction::SOUTH)
        points.push_back(Point(pos.y + 1, pos.x));
    if (ignore_dir != Direction::WEST)
        points.push_back(Point(pos.y, pos.x - 1));

    return points;
}

Direction::DIRECTION World::get_input_dir() {
    Direction::DIRECTION dir = Direction::NONE;
    bool valid = true;
    switch (getch()) {
        case 'k':
        case '8':
        case KEY_UP:
            dir = Direction::NORTH;
            break;
        case 'u':
        case '9':
        case KEY_PPAGE:
            dir = Direction::NORTHEAST;
            break;
        case 'l':
        case '6':
        case KEY_RIGHT:
            dir = Direction::EAST;
            break;
        case 'n':
        case '3':
        case KEY_NPAGE:
            dir = Direction::SOUTHEAST;
            break;
        case 'j':
        case '2':
        case KEY_DOWN:
            dir = Direction::SOUTH;
            break;
        case 'b':
        case '1':
        case KEY_END:
            dir = Direction::SOUTHWEST;
            break;
        case 'h':
        case '4':
        case KEY_LEFT:
            dir = Direction::WEST;
            break;
        case 'y':
        case '7':
        case KEY_HOME:
            dir = Direction::NORTHWEST;
            break;
        case '.':
        case ' ':
        case 10:
            break;
        default:
            valid = false;
            break;
    }
    if (valid)
        status("", true);
    else
        status("Invalid Direction", true);

    level_->world()->guy()->focus();

    return dir;
}

Item* World::get_input_item(std::vector<Item*>& items, std::string title) {
    std::vector<std::string> lines = level_->world()->format_items(items, true);
    level_->remember();
    int top = 0;
    if (lines.size() > (size_t)(LINES - 1))
        top = lines.size() - LINES + 1;

    clear();

    int y = 0;
    for (size_t i = top; i < lines.size(); i++) {
        mvaddstr(y, 0, lines.at(i).c_str());
        y++;
    }

    std::ostringstream bottom_line;
    bottom_line << title << " - select an item:";

    int curpos = bottom_line.str().length();
    int s = COLS - curpos;
    if (s < 1) s = 1;
    std::string spaces(s, ' ');
    bottom_line << spaces;
    mvaddstr(LINES - 1, 0, bottom_line.str().c_str());
    move(LINES - 1, curpos);

    refresh();

    bool done = false;
    while (!done) {
        int ch = getch();
        switch (ch) {
            case '8':
            case KEY_UP:
                if ((int)lines.size() > LINES - 1) {
                    top--;
                    if (top < 0)
                        top = 0;

                    clear();

                    int y = 0;
                    for (size_t i = top; i < lines.size(); i++) {
                        mvaddstr(y, 0, lines.at(i).c_str());
                        y++;
                    }
                    mvaddstr(LINES - 1, 0, bottom_line.str().c_str());
                    move(LINES - 1, curpos);
                    refresh();
                }
                break;
            case '2':
            case KEY_DOWN:
                if ((int)lines.size() > LINES - 1) {
                    top++;
                    if (top > (int)lines.size() - LINES + 1)
                        top = lines.size() - LINES + 1;

                    clear();

                    int y = 0;
                    for (size_t i = top; i < lines.size(); i++) {
                        mvaddstr(y, 0, lines.at(i).c_str());
                        y++;
                    }
                    mvaddstr(LINES - 1, 0, bottom_line.str().c_str());
                    move(LINES - 1, curpos);
                    refresh();
                }
                break;
            case KEY_PPAGE:
                if ((int)lines.size() > LINES - 1) {
                    top -= LINES - 1;
                    if (top < 0)
                        top = 0;

                    clear();

                    int y = 0;
                    for (size_t i = top; i < lines.size(); i++) {
                        mvaddstr(y, 0, lines.at(i).c_str());
                        y++;
                    }
                    mvaddstr(LINES - 1, 0, bottom_line.str().c_str());
                    move(LINES - 1, curpos);
                    refresh();
                }
                break;
            case KEY_NPAGE:
                if ((int)lines.size() > LINES - 1) {
                    top += LINES - 1;
                    if (top > (int)lines.size() - LINES + 1)
                        top = lines.size() - LINES + 1;

                    clear();

                    int y = 0;
                    for (size_t i = top; i < lines.size(); i++) {
                        mvaddstr(y, 0, lines.at(i).c_str());
                        y++;
                    }
                    mvaddstr(LINES - 1, 0, bottom_line.str().c_str());
                    move(LINES - 1, curpos);
                    refresh();
                }
                break;
            case KEY_HOME:
                if ((int)lines.size() > LINES - 1) {
                    top = 0;

                    clear();

                    int y = 0;
                    for (size_t i = top; i < lines.size(); i++) {
                        mvaddstr(y, 0, lines.at(i).c_str());
                        y++;
                    }
                    mvaddstr(LINES - 1, 0, bottom_line.str().c_str());
                    move(LINES - 1, curpos);
                    refresh();
                }
                break;
            case KEY_END:
                if ((int)lines.size() > LINES - 1) {
                    top = lines.size() - LINES + 1;

                    clear();

                    int y = 0;
                    for (size_t i = top; i < lines.size(); i++) {
                        mvaddstr(y, 0, lines.at(i).c_str());
                        y++;
                    }
                    mvaddstr(LINES - 1, 0, bottom_line.str().c_str());
                    move(LINES - 1, curpos);
                    refresh();
                }
                break;
            case 9:
            case ' ':
            case 10:
            case 27:
                done = true;
                break;
            default: {
                for (std::vector<Item*>::iterator it = items.begin(); it != items.end(); ++it) {
                    if (ch == inventory_keys_[(*it)->id()]) {
                        clear();
                        level_->draw_from_memory();
                        return *it;
                    }
                }
                break;
            }
        }
    }

    clear();
    level_->draw_from_memory();
    return NULL;
}

bool World::get_input_yes_or_no(bool default_yn) const {
    do {
        switch (getch()) {
            case 'Y':
            case 'y':
                return true;
            case ' ':
            case 10:
                return default_yn;
            case 'N':
            case 'n':
                return false;
        }
    } while (1);
    return false;
}

Hand::HAND World::get_input_hand() {
    status("Select a hand (l/r):", true);
    do {
        switch (getch()) {
            case 'L':
            case 'l':
                return Hand::LEFT;
            case 'R':
            case 'r':
                return Hand::RIGHT;
        }
    } while (1);
    return Hand::RIGHT;
}

YesNoAllNumberQuit::YES_NO_ALL_NUMBER_QUIT World::get_input_yes_no_number_all_or_quit(YesNoAllNumberQuit::YES_NO_ALL_NUMBER_QUIT default_ynq) const {
    do {
        switch (getch()) {
            case 'Y':
            case 'y':
                return YesNoAllNumberQuit::YES;
            case ' ':
            case 10:
                return default_ynq;
            case 'N':
            case 'n':
                return YesNoAllNumberQuit::NO;
            case 'A':
            case 'a':
                return YesNoAllNumberQuit::ALL;
            case '#':
                return YesNoAllNumberQuit::NUMBER;
            case 'Q':
            case 'q':
            case 27:
                return YesNoAllNumberQuit::QUIT;
        }
    } while (1);
    return YesNoAllNumberQuit::NO;
}

int World::get_input_quantity(size_t min, size_t max) {
    std::ostringstream o;
    o << "Enter quantity (" << min << "-[" << max << "]): " ;
    int n = - 1;
    echo();
    do {
        status(o.str(), true);
        scanw("%d", &n);
    } while (!(n < 0 || (n >= (int)min && n <= (int)max)));
    noecho();
    guy_->focus();
    return n < 0 ? max : n;
}

Point World::get_nearest_target() const {
     Point pos;
     Monster* monster = NULL;
     int dist = COLS;

     for (std::vector<Monster*>::iterator it = level_->monsters()->begin(); it != level_->monsters()->end(); ++it) {
         if (*(*it) != *guy_ && guy_->sees((*it)->pos())) {
             std::vector<Point> path = bresenham_path(guy_->pos(), (*it)->pos());
             if ((int)path.size() < dist) {
                 dist = path.size();
                 monster = *it;
             }
         }
     }

     if (monster != NULL)
         pos = monster->pos();

     return pos;
}

void World::examine() {
    for (int y = 0; y < 3; y++) {
        mvaddstr(y, 0, std::string(COLS, ' ').c_str());
    }
    status("[Examine]", true);
    guy_->focus();

    Point pos = guy_->pos();
    bool cont = true;
    while (cont) {
        Direction::DIRECTION dir = get_input_dir();
        if (dir == Direction::NONE)
            cont = false;
        else {
            Point adj = get_adjacent_pos(pos, dir);
            if (level_->is_valid_point(adj)
                    && adj.y >= guy_->pos().y - guy_->eyesight() && adj.y <= guy_->pos().y + guy_->eyesight()
                    && adj.x >= guy_->pos().x - guy_->eyesight() && adj.x <= guy_->pos().x + guy_->eyesight())
                pos = adj;
            move(pos.y + 3, pos.x + 1);
        }
    }

    if (guy_->sees(pos)) {
        guy_->look_at(pos, true);
    } else {
        status("You can't see that from here.", true);
        guy_->focus();
    }
}

bool World::play_again() const {
    clear();
    std::string q = "Play again? (y/[n])";
    mvaddstr(LINES / 2, (COLS - q.length()) / 2, q.c_str());
    return get_input_yes_or_no(false);
}

}
