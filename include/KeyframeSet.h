

#include "KeyframeCollection.h"

struct KeyframeSet{
    std::vector<Keyframe> t;
    std::vector<Keyframe> length;
    std::vector<Keyframe> decay_length;
    std::vector<Keyframe> glow_length;
    std::vector<Keyframe> point_glow_length;
    std::vector<Keyframe> object_position_x;
    std::vector<Keyframe> object_position_y;
    std::vector<Keyframe> object_position_z;
    std::vector<Keyframe> object_rotation_x;
    std::vector<Keyframe> object_rotation_y;
    std::vector<Keyframe> object_rotation_z;
    std::vector<Keyframe> object_rotation_angle;
    std::vector<Keyframe> object_scale;
    std::vector<Keyframe> cam_shift_err;
    std::vector<Keyframe> obj_shift_err;
    std::vector<Keyframe> cam_shear_err;
    std::vector<Keyframe> r;
    std::vector<Keyframe> phi;

    float get_t(float time) {
        return get_value(t, time, 0.0f);
    }

    float get_length(float time) {
        return get_value(length, time, 0.0f);
    }

    float get_decay_length(float time) {
        return get_value(decay_length, time, 0.0f);
    }

    float get_glow_length(float time) {
        return get_value(glow_length, time, 0.5f);
    }

    float get_point_glow_length(float time) {
        return get_value(point_glow_length, time, 1.0f);
    }

    Vector3f get_object_position(float time) {
        return Vector3f(get_value(object_position_x, time, 0.0f), get_value(object_position_y, time, 0.0f), get_value(object_position_z, time, 0.0f));
    }

    Vector3f get_object_rotation_axis(float time) {
        return Vector3f(get_value(object_rotation_x, time, 0.0f), get_value(object_rotation_y, time, 0.0f), get_value(object_rotation_z, time, 0.0f)).normalized();
    }

    float get_object_rotation_angle(float time) {
        return get_value(object_rotation_angle, time, 0.0f);
    }

    float get_object_scale(float time) {
        return get_value(object_scale, time, 0.0f);
    }

    float get_cam_shift_err(float time) {
        return get_value(cam_shift_err, time, 0.0f);
    }

    float get_obj_shift_err(float time) {
        return get_value(obj_shift_err, time, 0.0f);
    }

    float get_cam_shear_err(float time) {
        return get_value(cam_shear_err, time, 0.0f);
    }

    float get_r(float time) {
        return get_value(r, time, 0.0f);
    }

    float get_phi(float time) {
        return get_value(phi, time, 0.0f);
    }

    KeyframeSet(std::vector<KeyframeCollection> collections);
    KeyframeSet(const std::string& filename);
    KeyframeSet() = default;

    void check_for_overlaps();
    void sort_keyframes();

    float get_start_time() const;
    float get_end_time() const;
    float get_vector_end_time(const std::vector<Keyframe>& keyframes) const;
    float get_vector_start_time(const std::vector<Keyframe>& keyframes) const;

    float apply_curve(float t, KeyframeCurve curve) const;
    float get_value(std::vector<Keyframe>& keyframes, float time, float default_val);
    KeyframeCurve string_to_curve(const std::string& str);
    Keyframe parse_keyframe(const std::string& line);
    float convert_string_to_float(std::string str);
};

