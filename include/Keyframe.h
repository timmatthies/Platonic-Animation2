#ifndef KEYFRAME_H
#define KEYFRAME_H
#include <string>
#include <iostream>
#include <cmath>

enum class KeyframeCurve
{
    Linear,
    InSine,
    OutSine,
    InOutSine,
    EaseInCubic,
    EaseOutCubic,
    EaseInOutCubic,
    StepStart,
    StepMiddle,
    StepEnd,
    InExpo,
    OutExpo,
    InOutExpo,
    InCirc,
    OutCirc,
    InOutCirc,
    InOver,
    OutOver,
    InOutOver,
    InElastic,
    OutElastic,
    InOutElastic
};

struct Keyframe{
    float start_time = 0.0f;
    float end_time = 0.0f;
    float end_val = 0.0f;
    float start_val = NAN; //Use value from previous keyframe if NAN
    KeyframeCurve curve = KeyframeCurve::Linear;
    public:
        Keyframe(float t_start, float t_end, float val, KeyframeCurve easing_curve = KeyframeCurve::Linear){
            start_time = t_start;
            end_time = t_end;
            start_val = NAN;
            end_val = val;
            curve = easing_curve;
        }
        Keyframe(float t_start, float t_end, float start_val, float end_val, KeyframeCurve easing_curve = KeyframeCurve::Linear){
            start_time = t_start;
            end_time = t_end;
            this->start_val = start_val;
            this->end_val = end_val;
            curve = easing_curve;
        }

        //Overload > operator
        bool operator>(const Keyframe& other) const {
            return start_time > other.start_time;
        }

        bool operator<(const Keyframe& other) const {
            return start_time < other.start_time;
        }

        float overlaps(const Keyframe& other) const {
            return (start_time < other.end_time && end_time > other.start_time);
        }

};

#endif