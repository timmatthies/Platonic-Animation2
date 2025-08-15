#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <Eigen/Dense>

using Eigen::Vector3f;
using Eigen::Matrix3f;

struct Object {
public:
    Object();
    std::vector<Vector3f> points;
    static Object getCube();
    static Object getSquare();
    Vector3f position;
    Matrix3f rotation_matrix;
    void setPosition(const Vector3f& position);
    void setRotation(const Vector3f& axis, float angle);
    std::vector<Vector3f> getCurrentPoints(float time, float history_length, float history_spacing);
    Vector3f getPositionAtTime(float time) const;
private:
    Vector3f rotation_axis;
    float rotation_angle;
};

#endif // OBJECT_H