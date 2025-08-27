#ifndef KEYFRAMECOLLECTION_H
#define KEYFRAMECOLLECTION_H
#include <Eigen/Dense>
#include <iostream>
#include <cmath>
#include "Keyframe.h"

using Eigen::Vector3f;

enum class InterpolationType {
    Linear,
    Cubic
};


struct KeyframeCollection
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
    float r = 0.0f;
    float phi = 0.0f;
    KeyframeCurve curve = KeyframeCurve::Linear;

    KeyframeCollection() = default;
    KeyframeCollection(const KeyframeCollection&) = default;
    KeyframeCollection& operator=(const KeyframeCollection&) = default;

    void setCurve(std::string curveStr) {
        if (curveStr == "L") {
            curve = KeyframeCurve::Linear;
        } else if (curveStr == "I3") {
            curve = KeyframeCurve::EaseInCubic;
        } else if (curveStr == "O3") {
            curve = KeyframeCurve::EaseOutCubic;
        } else if (curveStr == "IO3") {
            curve = KeyframeCurve::EaseInOutCubic;
        } else if (curveStr == "SM") {
            curve = KeyframeCurve::StepMiddle;
        } else if (curveStr == "SE") {
            curve = KeyframeCurve::StepEnd;
        } else if (curveStr == "IE") {
            curve = KeyframeCurve::InExpo;
        } else if (curveStr == "OE") {
            curve = KeyframeCurve::OutExpo;
        } else if (curveStr == "IOE") {
            curve = KeyframeCurve::InOutExpo;
        } else if (curveStr == "IC") {
            curve = KeyframeCurve::InCirc;
        } else if (curveStr == "OC") {
            curve = KeyframeCurve::OutCirc;
        } else if (curveStr == "IOC") {
            curve = KeyframeCurve::InOutCirc;
        } else if (curveStr == "IO") {
            curve = KeyframeCurve::InOver;
        } else if (curveStr == "OO") {
            curve = KeyframeCurve::OutOver;
        } else if (curveStr == "IOO") {
            curve = KeyframeCurve::InOutOver;
        } else if (curveStr == "IJ") {
            curve = KeyframeCurve::InElastic;
        } else if (curveStr == "OJ") {
            curve = KeyframeCurve::OutElastic;
        } else if (curveStr == "IOJ") {
            curve = KeyframeCurve::InOutElastic;
        }else if (curveStr == "IS") {
            curve = KeyframeCurve::InSine;
        } else if (curveStr == "OS") {
            curve = KeyframeCurve::OutSine;
        } else if (curveStr == "IOS") {
            curve = KeyframeCurve::InOutSine;
        } else {
            std::cerr << "Unknown curve type: " << curveStr << std::endl;
        }
    }

    float applyCurve(float t) const {
        switch (curve) {
            case KeyframeCurve::Linear:
                return t;
            case KeyframeCurve::InSine:
                return 1 - std::cos((t * M_PI) / 2);
            case KeyframeCurve::OutSine:
                return std::sin((t * M_PI) / 2);
            case KeyframeCurve::InOutSine:
                return -0.5f * (std::cos(M_PI * t) - 1);
            case KeyframeCurve::EaseInCubic:
                return t * t * t;
            case KeyframeCurve::EaseOutCubic:
                return 1 - std::pow(1 - t, 3);
            case KeyframeCurve::EaseInOutCubic:
                if (t < 0.5f) {
                    return 4 * t * t * t;
                } else {
                    float f = ((2 * t) - 2);
                    return 0.5f * f * f * f + 1;
                }
            case KeyframeCurve::StepStart:
                return (t <= 0.0f) ? 0.0f : 1.0f;
            case KeyframeCurve::StepMiddle:
                return (t < 0.5f) ? 0.0f : 1.0f;
            case KeyframeCurve::StepEnd:
                return (t <= 1.0f) ? 0.0f : 1.0f;
            case KeyframeCurve::InExpo:
                return (t == 0.0f) ? 0.0f : std::pow(2, 10 * (t - 1));
            case KeyframeCurve::OutExpo:
                return (t == 1.0f) ? 1.0f : 1 - std::pow(2, -10 * t);
            case KeyframeCurve::InOutExpo:
                if (t == 0.0f) return 0.0f;
                if (t == 1.0f) return 1.0f;
                if (t < 0.5f) return std::pow(2, 20 * t - 10) / 2;
                return (2 - std::pow(2, -20 * t + 10)) / 2;
            case KeyframeCurve::InCirc:
                return 1 - std::sqrt(1 - t * t);
            case KeyframeCurve::OutCirc:
                return std::sqrt((2 - 1)*t);
            case KeyframeCurve::InOutCirc:
                if (t < 0.5f) {
                    return (1 - std::sqrt(1 - 4 * (t * t))) / 2;
                } else {
                    return (std::sqrt(-((2 * t) - 3) * ((2 * t) - 1)) + 1) / 2;
                }
            case KeyframeCurve::InOver:
                {
                    float s1 = 1.70158f;
                    return t * t * ((s1 + 1) * t - s1);
                }
            case KeyframeCurve::OutOver:
                {
                    float s2 = 1.70158f;
                    t = t - 1;
                    return t * t * ((s2 + 1) * t + s2) + 1;
                }
            case KeyframeCurve::InOutOver:
                {
                    float s3 = 1.70158f * 1.525f;
                    if (t < 0.5f) {
                        return (t * 2) * (t * 2) * ((s3 + 1) * (t * 2) - s3) / 2;
                    } else {
                        t = t * 2 - 2;
                        return (t * t * ((s3 + 1) * t + s3) + 2) / 2;
                    }
                }
            case KeyframeCurve::InElastic:
                {
                    float c4 = 10.75f;
                    float c5 = (2 * M_PI) / 3;
                    if (t == 0) return 0;
                    if (t == 1) return 1;
                    return -std::pow(2, 10 * (t - 1)) * std::sin((t*10-c4) * c5);
                }
            case KeyframeCurve::OutElastic:
                {
                    float c6 = 10.75f;
                    float c7 = (2 * M_PI) / 3;

                    if (t == 0) return 0;
                    if (t == 1) return 1;
                    return std::pow(2, -10 * t) * std::sin((t*10-c6) * c7) + 1;
                }
            case KeyframeCurve::InOutElastic:
                {
                    float c8 = 11.125f;
                    float c9 = (2 * M_PI) / 4.5;

                    if (t == 0) return 0;
                    if (t == 1) return 1;
                    if (t < 0.5f) {
                        return -(std::pow(2, 20 * t - 10) * std::sin((20 * t - c8) * c9)) / 2;
                    } else {
                        return (std::pow(2, -20 * t + 10) * std::sin((20 * t - c8) * c9)) / 2 + 1;
                    }
                }
            default:
                std::cerr << "Ease function not implemented yet" << std::endl;
                return t; // Fallback to linear if unknown
        }
    }
};


#endif