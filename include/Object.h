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
    static Object getCircle();
    static Object getThetra();
    static Object getIco();
    static Object getDodeca();
    static Object getOcta();
    static Object getObjectByName(const std::string& name);

    Vector3f position;
    Matrix3f rotation_matrix;
    Matrix3f scale_matrix;
    float polar_r;
    float polar_phi;
    void setPosition(const Vector3f& position, const float& r, const float& phi);
    void setRotation(const Vector3f& axis, float angle);
    void setScale(const Vector3f& scale);
    std::vector<Vector3f> getCurrentPoints(float time, float history_length, float history_spacing);
    Vector3f getPositionAtTime(float time) const;
private:
    Vector3f rotation_axis;
    float rotation_angle;
};

#endif // OBJECT_H