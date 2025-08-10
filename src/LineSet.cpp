#include "LineSet.h"

LineSet::LineSet() {}

LineSet::LineSet(const std::vector<Line>& lines)
    : lines(lines) {}

LineSet::~LineSet() {}

void LineSet::addLine(const Line& line) {
    lines.push_back(line);
}

float LineSet::get_t(const Vector2f& point) const {
    float min_t = std::numeric_limits<float>::max();
    float min_dist = std::numeric_limits<float>::max();
    int closest_line_index = -1;
    for (size_t i = 0; i < lines.size(); ++i) {
        float t = lines[i].get_t(point);
        float dist = lines[i].squaredDistance(point, t);
        if (dist < min_dist) {
            min_dist = dist;
            min_t = t;
        }
    }
    if(min_t < 0){
        return closest_line_index;
    }else if(min_t > 1){
        return closest_line_index + 1;
    }else{
        return closest_line_index + min_t;
    }
}

float LineSet::squaredDistance(const Vector2f& point) const {
    float min_dist = std::numeric_limits<float>::max();
    for (const Line& line : lines) {
        float t = line.get_t(point);
        float dist = line.squaredDistance(point, t);
        if (dist < min_dist) {
            min_dist = dist;
        }
    }
    return min_dist;
}
