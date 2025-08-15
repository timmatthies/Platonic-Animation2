#include "LineSet.h"
#include <iostream>

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
            closest_line_index = i;
            min_t = t;
        }
    }
    if(min_t < 0){
        return closest_line_index/(float)lines.size();
    }else if(min_t > 1){
        return (closest_line_index + 1)/(float)lines.size();
    }else{
        return (closest_line_index + min_t)/(float)lines.size();
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

std::vector<Vector2i> LineSet::getMask(float size) const {
    std::vector<Vector2i> mask;
    Vector2f v1, v2, v3, v4, v5, v6;
    v1 = lines[0].startPoint+lines[0].normal*size/2;
    v2 = lines[0].startPoint-lines[0].normal*size/2;
    for (size_t i = 0; i < lines.size(); i++) {
        if(i != lines.size() - 1) {
            v3 = (lines[i].endPoint+lines[i].normal*size/2+lines[i+1].startPoint+lines[i+1].normal*size/2)/2;
            v4 = (lines[i].endPoint-lines[i].normal*size/2+lines[i+1].startPoint-lines[i+1].normal*size/2)/2;
        } else {
            v3 = lines[i].endPoint+lines[i].normal*size/2;
            v4 = lines[i].endPoint-lines[i].normal*size/2;
        }
        // Now fill the rectangle inclosed by v1, v2, v3, and v4
        for (int x = (int)std::min({v1.x(), v2.x(), v3.x(), v4.x()}); x <= (int)std::max({v1.x(), v2.x(), v3.x(), v4.x()}); ++x) {
            for (int y = (int)std::min({v1.y(), v2.y(), v3.y(), v4.y()}); y <= (int)std::max({v1.y(), v2.y(), v3.y(), v4.y()}); ++y) {
                mask.push_back(Vector2i(x, y));
            }
        }
        v1 = v3;
        v2 = v4;
    }
    
    return mask;
}