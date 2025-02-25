#ifndef ROOM_H_
#define ROOM_H_

namespace Roguelike {

struct Point {
    int x;
    int y;
    Point() { x = -1; y = -1; }
    Point(int py, int px) {
        x = px;
        y = py;
    }
    ~Point() {}
    const bool operator==(Point other) {
        return (y == other.y && x == other.x);
    }
    const bool operator!=(Point other) {
        return (y != other.y || x != other.x);
    }
};

class Rect {
protected:
    Point pos_;
    int width_;
    int height_;
public:
    Rect();
    Rect(int y, int x, int h, int w);
    virtual ~Rect();
    Point center() const;
    bool contains(Point pos) const;

    Point& pos() { return pos_; }
    int height() const { return height_; }
    int width() const { return width_; }
    void set_height(int height) { height_ = height; }
    void set_width(int width) { width_ = width; }

    bool contains(Point pos) {
        return (pos.y >= pos_.y && pos.y < pos_.y + height_
                && pos.x >= pos_.x && pos.x < pos_.x + width_);
    }

};

class Room : public Rect {
public:
    enum TYPE {
        HALL,
        CORRIDOR
    };

protected:
    TYPE type_;
    unsigned long id_;
    static unsigned long top_id_;

public:
    Room();
    Room(Rect rect);
    virtual ~Room();
    TYPE type() const { return type_; }

    unsigned long id() const {
        return id_;
    }

    const bool operator==(Room other) const {
        return (id_ == other.id());
    }

    const bool operator!=(Room other) const {
        return (id_ != other.id());
    }
};

class Hall : public Room {
public:
    Hall(Rect rect);
    virtual ~Hall();
};

class Corridor : public Room {
public:
    Corridor(Rect rect);
    virtual ~Corridor();
};

}

#endif /* ROOM_H_ */
