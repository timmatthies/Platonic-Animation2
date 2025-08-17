#ifndef LINESET_H
#define LINESET_H

#include <vector>
#include <Eigen/Dense>
#include "Line.h"

using Vector2i = Eigen::Vector2i;

class LineSet
{
public:
    LineSet();
    LineSet(const std::vector<Line>& lines);
    ~LineSet();

    void addLine(const Line& line);
    float get_t(const Vector2f& point) const;
    float squaredDistance(const Vector2f& point) const;
    std::vector<Vector2i> getMask(float size) const;
    Vector2f getStartPoint() const;

private:
    std::vector<Line> lines;
};

#endif // LINESET_H