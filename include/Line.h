#ifndef LINE_H
#define LINE_H

#include <Eigen/Dense>

using Eigen::Vector2f;

class Line
{
public:
    Line(const Vector2f& start, const Vector2f& end);
    ~Line();

    float get_t(const Vector2f& point) const;
    Vector2f getPoint(float t) const;
    float squaredDistance(const Vector2f& point, const float& t) const;
    float distance(const Vector2f& point, const float& t) const;
private:
    Vector2f startPoint;
    Vector2f endPoint;
    Vector2f direction;
    Vector2f normal;
};

#endif // LINE_H