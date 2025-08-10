#include "Line.h"

Line::Line(const Vector2f& start, const Vector2f& end)
    : startPoint(start), endPoint(end) {
    direction = endPoint - startPoint;
    normal = Vector2f(-direction.y(), direction.x()).normalized();
}

Line::~Line() {
    // Destructor can be empty since we are not managing any resources manually
}

float Line::get_t(const Vector2f& point) const {
    Vector2f diff = point - startPoint;
    return diff.dot(direction) / direction.squaredNorm();
}

Vector2f Line::getPoint(float t) const {
    return startPoint + t * direction;
}

float Line::squaredDistance(const Vector2f& point, const float& t) const {
    Vector2f closestPoint = getPoint(t);
    Vector2f diff = point - closestPoint;
    if (t< 0) {
        diff = point - startPoint;
    } else if (t > 1) {
        diff = point - endPoint;
    }
    return diff.squaredNorm();
}

float Line::distance(const Vector2f& point, const float& t) const {
    return std::sqrt(squaredDistance(point, t));
}