#include "Camera.h"
#include <iostream>

Camera::Camera(int screenWidth, int screenHeight) : Camera(screenWidth, screenHeight, true) {}

Camera::Camera(int screenWidth, int screenHeight, bool orthonormal) {
    // Overloaded constructor for different initialization
    W = 4.0f;
    H = 4.0f;
    w = screenWidth; // Width of the camera viewport in pixels
    h = screenHeight; // Height of the camera viewport in pixels
    f = 2.0f; // Example different focal length
    shear_error = 0.0f;
    x_offset_error = 0.0f;
    y_offset_error = 0.0f;
    projectionMatrix = MatrixXf::Zero(2, 3);
    if(orthonormal) {
        // Set up orthonormal projection matrix
        projectionMatrix = MatrixXf({{1.0f, shear_error, x_offset_error},
                            {0.0f, 1.0f, y_offset_error}});
    }else {
        // Not implemented
        throw std::runtime_error("Non-orthonormal projection not implemented");
    }

}

std::vector<Vector2f> Camera::getScreenPos(std::vector<Object> &objects) {
    std::vector<Vector2f> screenPositions;
    for (auto &object : objects) {
        // Project each point of the object onto the screen
        for (int i = 0; i < object.points.size(); ++i) {
            Vector2f point = projectionMatrix * (object.rotation_matrix * object.points[i] + object.position);
            point.x() = (point.x() / W) * w + w / 2; // Normalize and scale to viewport
            point.y() = (point.y() / H) * h + h / 2; // Normalize and scale to viewport
            screenPositions.push_back(point);
        }
    }
    return screenPositions;
}

std::vector<Vector2f> Camera::getScreenPos(std::vector<Vector3f> &points) {
    std::vector<Vector2f> screenPositions;
    for (const auto &point : points) {
        Vector2f screenPoint = projectionMatrix * point;
        screenPoint.x() = (screenPoint.x() / W) * w + w / 2; // Normalize and scale to viewport
        screenPoint.y() = (screenPoint.y() / H) * h + h / 2; // Normalize and scale to viewport
        screenPositions.push_back(screenPoint);
    }
    return screenPositions;
}

LineSet Camera::convert_to_lines(const Object& object) {
    LineSet lineSet;
    // Convert the object's points into lines
    for (size_t i = 0; i < object.points.size() - 1; ++i) {
        Vector2f start = projectionMatrix * (object.rotation_matrix * object.points[i] + object.position);
        Vector2f end = projectionMatrix * (object.rotation_matrix * object.points[i + 1] + object.position);
        start.x() = (start.x() / W) * w + w / 2; // Normalize and scale to viewport
        start.y() = (start.y() / H) * h + h / 2; // Normalize and scale to viewport
        end.x() = (end.x() / W) * w + w / 2; // Normalize and scale to viewport
        end.y() = (end.y() / H) * h + h / 2; // Normalize and scale to viewport
        lineSet.addLine(Line(start, end));
    }
    return lineSet;
}