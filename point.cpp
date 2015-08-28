#include "point.h"
#include <QDebug>

Point::Point():x(0), y(0), origin(false)
{
    color[0] = blank;
    color[1] = blank;
    next[0] = 0;
    next[1] = 0;
    previous[0] = 0;
    previous[1] = 0;
    covered = false;
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
    color[0] = blank;
    color[1] = blank;
    next[0] = 0;
    next[1] = 0;
    previous[0] = 0;
    previous[1] = 0;
    rectFlag = false;
    covered = false;
}

void Point::getCovered()
{
    Point* temp = this;
    while(temp)
    {
        temp->covered = true;
        temp = temp->next[0];
    }
}

void Point::getUncovered()
{
    Point* temp = this;
    while(temp)
    {
        if(temp->previous[1])
            break;
        temp->covered = false;
        temp = temp->next[0];
    }
}
