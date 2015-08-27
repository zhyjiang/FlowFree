#ifndef POINT_H
#define POINT_H

class Point;
class MainWindow;

class Point
{
public:
    Point();
    ~Point();
    void setPosition(int, int);
    void clear();

private:
    int x, y;
    enum {blank, red, blue, purple, yellow, orange, green, black, brown};
    int color;
    bool origin;
    bool rectFlag;
    Point *next, *previous;
    friend class MainWindow;
};

#endif // POINT_H
