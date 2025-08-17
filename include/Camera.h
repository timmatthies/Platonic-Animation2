#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include "Object.h"
#include <Eigen/Dense>
#include "LineSet.h"

using Eigen::Vector2f;
using Eigen::Vector3f;
using Eigen::MatrixXf;

class Camera {
public:
    Camera();
    Camera(int screenWidth, int screenHeight);
    Camera(int screenWidth, int screenHeight, bool isOrthographic);
    std::vector<Vector2f> getScreenPos(std::vector<Object> &objects);
    std::vector<Vector2f> getScreenPos(std::vector<Vector3f> &points);
    Vector2f get_screen_position(const Vector3f& world_position, const Object& object) const;
    LineSet convert_to_lines(const Object& object) const;
    LineSet convert_to_lines(const Object &object, float start_t, float length) const;
    Vector2f get_point(float start_t, const Object &object) const;
    MatrixXf projectionMatrix;
    void set_proj_matrix();
    void set_error(float shear, float x_offset, float y_offset, float x_err, float y_err);

private:
    float f;
    float shear_error;
    float x_scale_error;
    float y_scale_error;
    float x_offset_error;
    float y_offset_error;
    bool ortho; // True if the camera uses orthonormal projection
    float W; // Width of the camera view
    float H; // Height of the camera view
    int w; // Width of the camera's viewport in pixels
    int h; // Height of the camera's viewport in pixels
};

#endif // CAMERA_H