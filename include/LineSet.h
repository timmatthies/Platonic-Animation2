#ifndef LINESET_H
#define LINESET_H

#include <vector>
#include "Line.h"

class LineSet
{
public:
    LineSet();
    LineSet(const std::vector<Line>& lines);
    ~LineSet();

    void addLine(const Line& line);
    float get_t(const Vector2f& point) const;
    float squaredDistance(const Vector2f& point) const;

private:
    std::vector<Line> lines;
};

#endif // LINESET_H