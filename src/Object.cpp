#include "Object.h"
#include <cmath>

using namespace Eigen;


Object::Object() {
    // Initialize the object with some default points. The points define a path of a triangle.
    points.push_back(Vector3f(0.0f, 0.0f, 0.0f));
    points.push_back(Vector3f(1.0f, 0.0f, 0.0f));
    points.push_back(Vector3f(0.0f, 1.0f, 1.0f));
    setPosition(Vector3f(0.0f, 0.0f, 0.0f));
    setRotation(Vector3f(0.0f, 1.0f, 0.0f), 0.0f); // No rotation by default
    
}

Object Object::getCube() {
    Object cube;
    // Define a path with the 8 vertices of a cube
    cube.points = {
        Vector3f(-1.0f,     1.0f,  -1.0f  ),
        Vector3f(1.0f,      1.0f, -1.0f  ),
        Vector3f(1.0f,     -1.0f, -1.0f  ),
        Vector3f(1.0f,     -1.0f, 1.0f  ),
        Vector3f(1.0f,      1.0f, 1.0f  ),
        Vector3f(-1.0f,     1.0f,  1.0f  ),
        Vector3f(-1.0f,    -1.0f,  1.0f  ),
        Vector3f(-1.0f,    -1.0f,  -1.0f  ),
        Vector3f(-1.0f,     1.0f,  -1.0f  ), // Closing the cube path the first time
        Vector3f(-1.0f,     1.0f,   1.0f  ),
        Vector3f(-1.0f,     -1.0f,  1.0f  ),
        Vector3f( 1.0f,     -1.0f, 1.0f   ),
        Vector3f(1.0f,      1.0f, 1.0f   ),
        Vector3f(1.0f,      1.0f, -1.0f  ),
        Vector3f(1.0f,     -1.0f, -1.0f  ),
        Vector3f(-1.0f,    -1.0f,  -1.0f  ) // Closing the cube path the second time
    };

    return cube;
}

Object Object::getSquare() {
    Object square;
    // Define a path with the 4 vertices of a square
    square.points = {
        Vector3f(-1.0f, -1.0f, 0.0f),
        Vector3f(1.0f,  -1.0f, 0.0f),
        Vector3f(1.0f, 1.0f, 0.0f),
        Vector3f(-1.0f, 1.0f, 0.0f),
    };

    return square;
}

Vector3f Object::getPositionAtTime(float time) const {
    int startIndex = static_cast<int>(time) % points.size();
    int endIndex = (startIndex + 1) % points.size();
    float t = time - static_cast<int>(time); // Fractional part of time
    return rotation_matrix*(points[startIndex] * (1 - t) + points[endIndex] * t) + position;
}

std::vector<Vector3f> Object::getCurrentPoints(float time, float history_length, float history_spacing) {
    // Returns the current position and a few of the previous positions in the past.
    int numPoints = static_cast<int>(std::ceil(history_length / history_spacing));
    std::vector<Vector3f> currentPoints;
    currentPoints.reserve(numPoints);

    for (int i = 0; i < numPoints; ++i) {
        float t = time - i * history_spacing;
        if (t < 0) break;
        currentPoints.push_back(getPositionAtTime(t));
    }

    return currentPoints;
}


void Object::setPosition(const Vector3f& position) {
    this->position = Vector3f(position.x(), position.y(), position.z());
}

void Object::setRotation(const Vector3f& axis, float angle) {
    rotation_axis = axis;
    rotation_angle = angle;
    rotation_matrix = AngleAxisf(angle, axis.normalized()).toRotationMatrix();
}

