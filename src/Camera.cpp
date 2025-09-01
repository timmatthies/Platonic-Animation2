#include "Camera.h"
#include <iostream>

Camera::Camera() : Camera(100, 100, true) {}

Camera::Camera(int screenWidth, int screenHeight) : Camera(screenWidth, screenHeight, true) {}

Camera::Camera(int screenWidth, int screenHeight, bool orthonormal) {
    // Overloaded constructor for different initialization
    W = 4.0f;
    H = 4.0f;
    w = screenWidth; // Width of the camera viewport in pixels
    h = screenHeight; // Height of the camera viewport in pixels
    f = 2.0f; // Example different focal length
    shear_error = 0.0f;
    x_scale_error = 0.0f;
    y_scale_error = 0.0f;
    projectionMatrix = MatrixXf::Zero(2, 3);
    ortho = orthonormal;
    set_proj_matrix();

}

void Camera::set_proj_matrix(){
    if(ortho) {
        // Set up orthonormal projection matrix
        projectionMatrix = MatrixXf({{1.0f, shear_error, x_scale_error},
                            {0.0f, 1.0f, y_scale_error}});
    }else {
        // Not implemented
        throw std::runtime_error("Non-orthonormal projection not implemented");
    }
}

std::vector<Vector2f> Camera::getScreenPos(std::vector<Object> &objects) {
    std::vector<Vector2f> screenPositions;
    for (auto &object : objects) {
        // Project each point of the object onto the screen
        for (size_t i = 0; i < object.points.size(); ++i) {
            Vector2f point = projectionMatrix * (object.rotation_matrix * object.scale_matrix * object.points[i] + object.position + Vector3f(x_offset_error, y_offset_error, 0));
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

LineSet Camera::convert_to_lines(const Object& object) const {
    LineSet lineSet;
    // Convert the object's points into lines
    for (size_t i = 0; i < object.points.size(); ++i) {
        Vector2f start = projectionMatrix * (object.rotation_matrix * object.scale_matrix * object.points[i] + object.position + Vector3f(x_offset_error, y_offset_error, 0));
        Vector2f end = projectionMatrix * (object.rotation_matrix * object.scale_matrix * object.points[(i + 1) % object.points.size()] + object.position + Vector3f(x_offset_error, y_offset_error, 0));
        start.x() = (start.x() / W) * w + w / 2; // Normalize and scale to viewport
        start.y() = (start.y() / H) * h + h / 2; // Normalize and scale to viewport
        end.x() = (end.x() / W) * w + w / 2; // Normalize and scale to viewport
        end.y() = (end.y() / H) * h + h / 2; // Normalize and scale to viewport
        lineSet.addLine(Line(start, end));
    }
    return lineSet;
}

Vector2f Camera::get_screen_position(const Vector3f& world_position, const Object& object) const {
    Vector2f screen_position = projectionMatrix * (object.rotation_matrix * object.scale_matrix * world_position + object.position + Vector3f(x_offset_error, y_offset_error, 0));
    screen_position.x() = (screen_position.x() / W) * w + w / 2; // Normalize and scale to viewport
    screen_position.y() = (screen_position.y() / H) * h + h / 2; // Normalize and scale to viewport
    return screen_position;
}

LineSet Camera::convert_to_lines(const Object& object, float start_t, float length) const {
    // Make start_t mod 1
    
    LineSet lineSet;
    if(length >= 1.0f){
        length = 1.0f;
    }
    
    Vector2f p_s = get_point(start_t, object);

    int k = std::floor(start_t * object.points.size());
    float q = k * 1 / (float)object.points.size();

    // Paralizeable in the future
    while (q > start_t-length)
    {
        Vector2f p_e = get_point(q, object);
        lineSet.addLine(Line(p_s, p_e));
        p_s = p_e;
        k--;
        q = k * 1 / (float)object.points.size();
    }
    Vector2f p_e = get_point(start_t - length, object);
    lineSet.addLine(Line(p_s, p_e));
    return lineSet;
}

Vector2f Camera::get_point(float t, const Object &object) const
{
    float o = t - std::floor(t);
    int l = static_cast<int>(o * object.points.size());
    float m = 1 / (float)object.points.size();

    Vector2f v_s = get_screen_position(object.points[l], object);
    Vector2f v_e = get_screen_position(object.points[(l + 1) % object.points.size()], object);

    return v_s + (o - l * m) / m * (v_e - v_s);
}

void Camera::set_error(float shear, float x_err, float y_err, float x_offset, float y_offset)
{
    Vector2f rand_error = Vector2f::Random() * 0.001f;
    Vector3f rand_shear = Vector3f::Random() * 0.001f;
    shear_error = shear + rand_shear.x();
    x_scale_error = x_err + rand_shear.y();
    y_scale_error = y_err + rand_shear.z();
    x_offset_error = x_offset + rand_error.x();
    y_offset_error = y_offset + rand_error.y();

}