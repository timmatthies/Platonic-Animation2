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
    if (lines.empty()) return mask; // Prevent crash if no lines

    // First create the outline mask as before
    Vector2f v1, v2, v3, v4;
    v1 = lines[0].startPoint+lines[0].normal*size/2;
    v2 = lines[0].startPoint-lines[0].normal*size/2;
    
    // Track all the outline points for later filling
    std::vector<Vector2f> outlinePoints;
    outlinePoints.push_back(v1);
    outlinePoints.push_back(v2);
    
    // Generate mask as before
    for (size_t i = 0; i < lines.size(); i++) {
        if(i != lines.size() - 1) {
            v3 = (lines[i].endPoint+lines[i].normal*size/2+lines[i+1].startPoint+lines[i+1].normal*size/2)/2;
            v4 = (lines[i].endPoint-lines[i].normal*size/2+lines[i+1].startPoint-lines[i+1].normal*size/2)/2;
        } else {
            v3 = lines[i].endPoint+lines[i].normal*size/2;
            v4 = lines[i].endPoint-lines[i].normal*size/2;
        }
        
        // Add outline points
        outlinePoints.push_back(v3);
        outlinePoints.push_back(v4);
        
        // Fill the rectangle as before
        int min_x = std::floor(std::min({v1.x(), v2.x(), v3.x(), v4.x()}));
        int max_x = std::ceil(std::max({v1.x(), v2.x(), v3.x(), v4.x()}));
        int min_y = std::floor(std::min({v1.y(), v2.y(), v3.y(), v4.y()}));
        int max_y = std::ceil(std::max({v1.y(), v2.y(), v3.y(), v4.y()}));
        
        // Limit rectangle size to prevent excessive memory usage
        const int MAX_SIZE = 1000;
        if (max_x - min_x > MAX_SIZE) max_x = min_x + MAX_SIZE;
        if (max_y - min_y > MAX_SIZE) max_y = min_y + MAX_SIZE;
        
        for (int x = min_x; x <= max_x; ++x) {
            for (int y = min_y; y <= max_y; ++y) {
                mask.push_back(Vector2i(x, y));
            }
        }
        
        v1 = v3;
        v2 = v4;
    }
    
    return mask;
}

Vector2f LineSet::getStartPoint() const
{
    return lines.front().startPoint;
}
