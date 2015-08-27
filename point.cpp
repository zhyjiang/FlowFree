#include "point.h"

Point::Point():x(0), y(0), origin(false)
{
    color = blank;
    next = 0;
    previous = 0;
    rectFlag = false;
}

Point::~Point()
{
    delete next;
}

void Point::setPosition(int x1, int y1)
{
    x = x1;
    y = y1;
}

void Point::clear()
{
    color = blank;
    next = 0;
    previous = 0;
    rectFlag = false;
}
