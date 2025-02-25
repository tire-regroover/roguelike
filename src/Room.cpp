#include "Room.h"

namespace Roguelike {

Rect::Rect() {}
Rect::Rect(int y, int x, int h, int w) {
    pos_ = Point(y, x);
    height_ = h;
    width_ = w;
}
Rect::~Rect() {}
Point Rect::center() const { return Point(height_ / 2 + pos_.y, width_ / 2 + pos_.x); }

bool Rect::contains(Point pos) const {
    if (pos.y >= pos_.y && pos.y <= pos_.y + height_ - 1
            && pos.x >= pos_.x && pos.x <= pos_.x + width_ - 1)
        return true;
    return false;
}

////////////////////////////////////////////////////

unsigned long Room::top_id_ = 0;

Room::Room() : Rect() {
    id_ = top_id_++;
}
Room::Room(Rect rect) : Rect(rect) {
    id_ = top_id_++;
}
Room::~Room() {}

////////////////////////////////////////////////////

Hall::Hall(Rect rect) : Room(rect) {
    type_ = HALL;
}
Hall::~Hall() {}


////////////////////////////////////////////////////

Corridor::Corridor(Rect rect) : Room(rect) {
    type_ = Room::CORRIDOR;
}
Corridor::~Corridor() {}


}
