#ifndef KEYFRAME_H
#define KEYFRAME_H
#include <Eigen/Dense>

using Eigen::Vector3f;

enum class InterpolationType {
    Linear,
    Cubic
};

struct Keyframe
{
    float time = 0.0f; // Time in seconds
    float t = 0.0f;
    float length = 1.0f;
    float decay_length = 4.0f;
    float glow_length = 0.5f;
    float point_glow_length = 1.0f; // Length of the glow effect for the point
    Vector3f object_position;
    Vector3f object_rotation_axis; // Euler angles in radians
    float object_scale;
    float camera_shift_error = 0.0f;
    float object_shift_error = 0.0f;
    float camera_shear_error = 0.0f;

    Keyframe() = default;
    Keyframe(const Keyframe&) = default;
    Keyframe& operator=(const Keyframe&) = default;
};


#endif