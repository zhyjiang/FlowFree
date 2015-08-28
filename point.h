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
    void getCovered();
    void getUncovered();

private:
    int x, y;
    enum {blank, red, blue, purple, yellow, orange, green, black, brown};
    int color[2];
    bool origin;
    bool covered;
    bool rectFlag;
    Point *next[2], *previous[2];
    friend class MainWindow;
};

#endif // POINT_H
