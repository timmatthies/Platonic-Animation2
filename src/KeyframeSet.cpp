#include "KeyframeSet.h"
#include <fstream>
#include <iostream>
#include <sstream>

float KeyframeSet::apply_curve(float t, KeyframeCurve curve) const {
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

KeyframeSet::KeyframeSet(std::vector<KeyframeCollection> collections) {
    float start_time = std::numeric_limits<float>::max();
    float end_time = std::numeric_limits<float>::min();

    for (const auto& collection : collections) {
        start_time = std::min(start_time, collection.time);
        end_time = std::max(end_time, collection.time);
    }

    for (size_t i = 0; i < collections.size(); ++i) {
        const auto& collection = collections[i];
        float t_start = (i == 0) ? start_time : collections[i - 1].time;
        float t_end = collection.time;
        KeyframeCurve curve = (i == 0) ? collection.curve : collections[i - 1].curve;

        t.emplace_back(t_start, t_end, collection.t, curve);
        length.emplace_back(t_start, t_end, collection.length, curve);
        decay_length.emplace_back(t_start, t_end, collection.decay_length, curve);
        glow_length.emplace_back(t_start, t_end, collection.glow_length, curve);
        point_glow_length.emplace_back(t_start, t_end, collection.point_glow_length, curve);
        object_position_x.emplace_back(t_start, t_end, collection.object_position.x(), curve);
        object_position_y.emplace_back(t_start, t_end, collection.object_position.y(), curve);
        object_position_z.emplace_back(t_start, t_end, collection.object_position.z(), curve);
        object_rotation_x.emplace_back(t_start, t_end, collection.object_rotation_axis.x(), curve);
        object_rotation_y.emplace_back(t_start, t_end, collection.object_rotation_axis.y(), curve);
        object_rotation_z.emplace_back(t_start, t_end, collection.object_rotation_axis.z(), curve);
        object_rotation_angle.emplace_back(t_start, t_end, collection.object_rotation_axis.norm(), curve);
        object_scale.emplace_back(t_start, t_end, collection.object_scale, curve);
        cam_shift_err.emplace_back(t_start, t_end, collection.camera_shift_error, curve);
        obj_shift_err.emplace_back(t_start, t_end, collection.object_shift_error, curve);
        cam_shear_err.emplace_back(t_start, t_end, collection.camera_shear_error, curve);
        r.emplace_back(t_start, t_end, collection.r, curve);
        phi.emplace_back(t_start, t_end, collection.phi, curve);
    }
    
    //print x_position of object
    for (const auto& kf : object_position_x) {
        // std::cout << "Object: " << kf.object_name << std::endl;
        std::cout << "Object Position X - Start: " << kf.start_time << ", End: " << kf.end_time << ", Value: " << kf.start_val << ", End Value: " << kf.end_val << std::endl;
    }

    check_for_overlaps();
    sort_keyframes();
}

KeyframeSet::KeyframeSet(const std::string& filename) {
    // Load keyframes from a file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip lines starting with #
        if (line[0] == '#') continue;

        if (line.empty()) continue;

        
        if (line[0] == 'K'){
            std::istringstream ss(line);
            std::string token;
            ss >> token; // Read "Keyframe"
            ss >> token; // Read keyframe type
            
            if(token == "px"){
                while (std::getline(file, line)) {
                    std::cout << "Loading keyframe: " << line << std::endl;
                    if (line.empty()) break;
                    Keyframe keyframe = parse_keyframe(line);
                    object_position_x.push_back(keyframe);

                    std::cout << "Loaded keyframeuiui: " << keyframe.start_time << " " << keyframe.start_val << " " << keyframe.end_time << " " << keyframe.end_val << std::endl;
                }
            }
        }

    }

    file.close();

    check_for_overlaps();
    sort_keyframes();
}

Keyframe KeyframeSet::parse_keyframe(const std::string& line) {
    std::istringstream ss(line);
    std::vector<std::string> tokens;
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }

    if (tokens.size() < 4) {
        std::cerr << "Error: Invalid keyframe line: " << line << std::endl;
        return Keyframe(0, 0, 0);
    }

    if(tokens.size() > 5){
        std::cerr << "Warning: Extra tokens in keyframe line: " << line << std::endl;
    }
    float start_time = 0.0f;
    float end_time = 0.0f;
    float start_val = NAN; // Use previous keyframe's end_val
    float end_val = 0.0f;
    KeyframeCurve curve = KeyframeCurve::Linear; // Default to Linear

    if (tokens.size() == 5) {
        start_time = std::stof(tokens[0]);
        start_val = std::stof(tokens[1]);
        end_time = std::stof(tokens[2]);
        end_val = std::stof(tokens[3]);
        curve = string_to_curve(tokens[4]);
        Keyframe kf = Keyframe(start_time, end_time, start_val, end_val, curve);
        std::cout << "Loaded keyframiiiiiie: " << kf.start_time << " " << kf.start_val << " " << kf.end_time << " " << kf.end_val << std::endl;

    } else if (tokens.size() == 4) {
        if (std::isdigit(tokens[3][0]) || tokens[3][0] == '-') {
            start_time = std::stof(tokens[0]);
            start_val = std::stof(tokens[1]);
            end_time = std::stof(tokens[2]);
            end_val = std::stof(tokens[3]);
        } else {
            start_time = std::stof(tokens[0]);
            end_time = std::stof(tokens[1]);
            end_val = std::stof(tokens[2]);
            curve = string_to_curve(tokens[3]);
        }
    }

    return Keyframe(start_time, end_time, start_val, end_val, curve);
}

KeyframeCurve KeyframeSet::string_to_curve(const std::string& curve_str) {
    if (curve_str == "Linear" || curve_str == "L") return KeyframeCurve::Linear;
    if (curve_str == "InSine" || curve_str == "IS") return KeyframeCurve::InSine;
    if (curve_str == "OutSine" || curve_str == "OS") return KeyframeCurve::OutSine;
    if (curve_str == "InOutSine" || curve_str == "IOS") return KeyframeCurve::InOutSine;
    if (curve_str == "EaseInCubic" || curve_str == "I3") return KeyframeCurve::EaseInCubic;
    if (curve_str == "EaseOutCubic" || curve_str == "O3") return KeyframeCurve::EaseOutCubic;
    if (curve_str == "EaseInOutCubic" || curve_str == "IO3") return KeyframeCurve::EaseInOutCubic;
    if (curve_str == "StepStart" || curve_str == "SS") return KeyframeCurve::StepStart;
    if (curve_str == "StepMiddle" || curve_str == "SM") return KeyframeCurve::StepMiddle;
    if (curve_str == "StepEnd" || curve_str == "SE") return KeyframeCurve::StepEnd;
    if (curve_str == "InExpo" || curve_str == "IE") return KeyframeCurve::InExpo;
    if (curve_str == "OutExpo" || curve_str == "OE") return KeyframeCurve::OutExpo;
    if (curve_str == "InOutExpo" || curve_str == "IOE") return KeyframeCurve::InOutExpo;
    if (curve_str == "InCirc" || curve_str == "IC") return KeyframeCurve::InCirc;
    if (curve_str == "OutCirc" || curve_str == "OC") return KeyframeCurve::OutCirc;
    if (curve_str == "InOutCirc" || curve_str == "IOC") return KeyframeCurve::InOutCirc;
    if (curve_str == "InOver" || curve_str == "IO") return KeyframeCurve::InOver;
    if (curve_str == "OutOver" || curve_str == "OO") return KeyframeCurve::OutOver;
    if (curve_str == "InOutOver" || curve_str == "IOO") return KeyframeCurve::InOutOver;
    if (curve_str == "InElastic" || curve_str == "IE") return KeyframeCurve::InElastic;
    if (curve_str == "OutElastic" || curve_str == "OE") return KeyframeCurve::OutElastic;
    if (curve_str == "InOutElastic" || curve_str == "IOE") return KeyframeCurve::InOutElastic;

    std::cerr << "Warning: Unknown curve type '" << curve_str << "', defaulting to Linear." << std::endl;
    return KeyframeCurve::Linear; // Default to Linear
}

void KeyframeSet::check_for_overlaps() {
    // Check for overlapping keyframes and handle them
    for (size_t i = 0; i < t.size(); ++i) {
        for (size_t j = i + 1; j < t.size(); ++j) {
            if (t[i].overlaps(t[j])) {
                std::cerr << "Warning: Overlapping keyframes detected in 't' at indices " << i << " and " << j << std::endl;
                // Handle overlap
                if (t[i] < t[j]) {
                    t[j].start_time = t[i].end_time;
                } else {
                    t[i].start_time = t[j].end_time;
                }
            }
        }
    }
}

void KeyframeSet::sort_keyframes() {
    auto sort_fn = [](std::vector<Keyframe>& keyframes) {
        std::sort(keyframes.begin(), keyframes.end(),
                  [](const Keyframe& a, const Keyframe& b) { return a < b; });
    };

    sort_fn(t);
    sort_fn(length);
    sort_fn(decay_length);
    sort_fn(glow_length);
    sort_fn(point_glow_length);
    sort_fn(object_position_x);
    sort_fn(object_position_y);
    sort_fn(object_position_z);
    sort_fn(object_rotation_x);
    sort_fn(object_rotation_y);
    sort_fn(object_rotation_z);
    sort_fn(object_scale);
    sort_fn(cam_shift_err);
    sort_fn(obj_shift_err);
    sort_fn(cam_shear_err);
    sort_fn(r);
    sort_fn(phi);
}

float KeyframeSet::get_value(std::vector<Keyframe>& keyframes, float time, float default_val) {
    if (keyframes.empty()) return default_val;

    for (size_t i = 0; i < keyframes.size(); ++i) {
        const auto& kf = keyframes[i];
        if (kf.start_time <= time && kf.end_time >= time) {
            if (kf.start_time == kf.end_time) {
                return kf.end_val; // Avoid division by zero
            }
            // If we found a keyframe that contains the time, return its value
            float start_val = kf.start_val;
            if(std::isnan(start_val)) {
                if(i == 0) {
                    start_val = kf.end_val; // If it's the first keyframe, use its end_val
                } else {
                    // Find the previous keyframe's end_val
                    start_val = keyframes[i - 1].end_val;
                }
            }
            // std::cout << "Im right here!" << std::endl;
            // std::cout << start_val <<" " << kf.end_val << " " << (kf.end_val - start_val) << std::endl;
            return start_val + apply_curve((time - kf.start_time) / (kf.end_time - kf.start_time), kf.curve) * (kf.end_val - start_val);
        }
    }

    //Check if the time is between two keyframes.
    //If so linear interpolate
    for (size_t i = 0; i < keyframes.size() - 1; ++i) {
        if (keyframes[i].end_time >= time && keyframes[i + 1].start_time <= time) {
            float t = (time - keyframes[i].end_time) / (keyframes[i + 1].start_time - keyframes[i].end_time);
            float start_val = keyframes[i+1].start_val;
            if(std::isnan(start_val)){
                return keyframes[i].end_val;
            }
            return keyframes[i].end_val + t * (start_val - keyframes[i].end_val);
        }
    }

    //Check if the time is before the first keyframe
    if (time < keyframes.front().start_time) {
        if(std::isnan(keyframes.front().start_val)){
            return keyframes.front().end_val;
        }
    }

    //Check if the time is after the last keyframe
    if (time > keyframes.back().end_time) {
        return keyframes.back().end_val;
    }

    return default_val;
}

float KeyframeSet::get_start_time() const {
    // Get min start time over all vectors
    float start_time = std::numeric_limits<float>::max();
    start_time = std::min(start_time, get_vector_start_time(length));
    start_time = std::min(start_time, get_vector_start_time(decay_length));
    start_time = std::min(start_time, get_vector_start_time(glow_length));
    start_time = std::min(start_time, get_vector_start_time(point_glow_length));
    start_time = std::min(start_time, get_vector_start_time(object_position_x));
    start_time = std::min(start_time, get_vector_start_time(object_position_y));
    start_time = std::min(start_time, get_vector_start_time(object_position_z));
    start_time = std::min(start_time, get_vector_start_time(object_rotation_x));
    start_time = std::min(start_time, get_vector_start_time(object_rotation_y));
    start_time = std::min(start_time, get_vector_start_time(object_rotation_z));
    start_time = std::min(start_time, get_vector_start_time(object_scale));
    start_time = std::min(start_time, get_vector_start_time(cam_shift_err));
    start_time = std::min(start_time, get_vector_start_time(obj_shift_err));
    start_time = std::min(start_time, get_vector_start_time(cam_shear_err));
    start_time = std::min(start_time, get_vector_start_time(r));
    start_time = std::min(start_time, get_vector_start_time(phi));

    return start_time;
}

float KeyframeSet::get_end_time() const {
    // Get max end time over all vectors
    float end_time = 0.0f;
    end_time = std::max(end_time, get_vector_end_time(length));
    end_time = std::max(end_time, get_vector_end_time(decay_length));
    end_time = std::max(end_time, get_vector_end_time(glow_length));
    end_time = std::max(end_time, get_vector_end_time(point_glow_length));
    end_time = std::max(end_time, get_vector_end_time(object_position_x));
    end_time = std::max(end_time, get_vector_end_time(object_position_y));
    end_time = std::max(end_time, get_vector_end_time(object_position_z));
    end_time = std::max(end_time, get_vector_end_time(object_rotation_x));
    end_time = std::max(end_time, get_vector_end_time(object_rotation_y));
    end_time = std::max(end_time, get_vector_end_time(object_rotation_z));
    end_time = std::max(end_time, get_vector_end_time(object_scale));
    end_time = std::max(end_time, get_vector_end_time(cam_shift_err));
    end_time = std::max(end_time, get_vector_end_time(obj_shift_err));
    end_time = std::max(end_time, get_vector_end_time(cam_shear_err));
    end_time = std::max(end_time, get_vector_end_time(r));
    end_time = std::max(end_time, get_vector_end_time(phi));

    return end_time;
}

float KeyframeSet::get_vector_end_time(const std::vector<Keyframe>& keyframes) const {
    if (keyframes.empty()) return 0.0f;
    return keyframes.back().end_time;
}

float KeyframeSet::get_vector_start_time(const std::vector<Keyframe>& keyframes) const {
    if (keyframes.empty()) return 0.0f;
    return keyframes.front().start_time;
}
