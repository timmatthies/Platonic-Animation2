#include "Animator.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

Animator::Animator(std::string name, Vector3f color, Camera cam, ImageGenerator imgGen, Object object, int fps)
    : name(name), color(color), camera(cam), imageGenerator(imgGen), object(object), fps(fps) {
    std::random_device rd;
    gen = std::mt19937(rd());
    dist = std::normal_distribution<float>(0.0f, 1.0f);
}

Animator::Animator() {
    std::random_device rd;
    gen = std::mt19937(rd());
    dist = std::normal_distribution<float>(0.0f, 1.0f);
}

Keyframe Animator::get_keyframe(float time, InterpolationType type) const {
    if (keyframes.empty()) {
        // Return a default keyframe if no keyframes exist
        Keyframe defaultFrame;
        defaultFrame.time = time;
        return defaultFrame;
    }

    // Find the keyframes to interpolate between
    auto it = std::lower_bound(keyframes.begin(), keyframes.end(), time,
        [](const Keyframe& kf, float t) { return kf.time < t; });

    if (it == keyframes.begin()) {
        // Time is before the first keyframe, return the first keyframe
        return keyframes.front();
    }
    
    if (it == keyframes.end()) {
        // Time is after the last keyframe, return the last keyframe
        return keyframes.back();
    }

    // Get the two keyframes to interpolate between
    const Keyframe& nextFrame = *it;
    const Keyframe& prevFrame = *(it - 1);

    // Calculate interpolation factor
    float t = (time - prevFrame.time) / (nextFrame.time - prevFrame.time);
    t = std::clamp(t, 0.0f, 1.0f);

    // Create interpolated keyframe
    Keyframe interpolated;
    interpolated.time = time;

    switch (type) {
        case InterpolationType::Linear:
            // Linear interpolation
            interpolated.t = prevFrame.t + prevFrame.applyCurve(t) * (nextFrame.t - prevFrame.t);
            interpolated.length = prevFrame.length + prevFrame.applyCurve(t) * (nextFrame.length - prevFrame.length);
            interpolated.decay_length = prevFrame.decay_length + prevFrame.applyCurve(t) * (nextFrame.decay_length - prevFrame.decay_length);
            interpolated.glow_length = prevFrame.glow_length + prevFrame.applyCurve(t) * (nextFrame.glow_length - prevFrame.glow_length);
            interpolated.object_position = prevFrame.object_position + prevFrame.applyCurve(t) * (nextFrame.object_position - prevFrame.object_position);
            interpolated.object_rotation_axis = prevFrame.object_rotation_axis + prevFrame.applyCurve(t) * (nextFrame.object_rotation_axis - prevFrame.object_rotation_axis);
            interpolated.object_scale = prevFrame.object_scale + prevFrame.applyCurve(t) * (nextFrame.object_scale - prevFrame.object_scale);
            interpolated.camera_shift_error = prevFrame.camera_shift_error + prevFrame.applyCurve(t) * (nextFrame.camera_shift_error - prevFrame.camera_shift_error);
            interpolated.camera_shear_error = prevFrame.camera_shear_error + prevFrame.applyCurve(t) * (nextFrame.camera_shear_error - prevFrame.camera_shear_error);
            interpolated.object_shift_error = prevFrame.object_shift_error + prevFrame.applyCurve(t) * (nextFrame.object_shift_error - prevFrame.object_shift_error);
            interpolated.point_glow_length = prevFrame.point_glow_length + prevFrame.applyCurve(t) * (nextFrame.point_glow_length - prevFrame.point_glow_length);
            break;

        case InterpolationType::Cubic:
            std::cerr << "Cubic Interpolation not implemented yet" << std::endl;
            break;
    }

    return interpolated;
}

void Animator::render_frame(float time) {
    Keyframe currentKeyframe = get_keyframe(time, InterpolationType::Linear);

    // Apply keyframe to object copy
    object.setPosition(currentKeyframe.object_position, currentKeyframe.r, currentKeyframe.phi);
    object.setRotation(currentKeyframe.object_rotation_axis.normalized(), 
                       currentKeyframe.object_rotation_axis.norm());
    object.setScale(currentKeyframe.object_scale*Vector3f(1.0f, 1.0f, 1.0f));
    float x_err = dist(gen) * currentKeyframe.camera_shift_error*0.05f;
    float y_err = dist(gen) * currentKeyframe.camera_shift_error*0.05f;
    float shear_err = dist(gen) * currentKeyframe.camera_shear_error*0.05f;
    float x_offset_err = dist(gen) * currentKeyframe.object_shift_error*0.05f;
    float y_offset_err = dist(gen) * currentKeyframe.object_shift_error*0.05f;

    camera.set_error(shear_err, x_err, y_err, x_offset_err, y_offset_err);
    camera.set_proj_matrix();

    // Generate lines and render
    LineSet lineSet = camera.convert_to_lines(object, currentKeyframe.t, currentKeyframe.length);

    imageGenerator.clear();
    imageGenerator.drawLines(lineSet, currentKeyframe.decay_length, currentKeyframe.glow_length);
    imageGenerator.drawPoint(lineSet.getStartPoint(), currentKeyframe.point_glow_length);
}

void Animator::animate(const std::string& filename) const {
    if (keyframes.empty()) {
        std::cerr << "No keyframes to animate!" << std::endl;
        return;
    }

    // Calculate animation duration
    float startTime = keyframes.front().time;
    float endTime = keyframes.back().time;
    float duration = endTime - startTime;
    
    if (duration <= 0) {
        std::cerr << "Invalid animation duration!" << std::endl;
        return;
    }

    // Calculate number of frames
    int totalFrames = static_cast<int>(duration * fps);
    float timeStep = duration / totalFrames;

    std::cout << "Generating " << totalFrames << " frames for animation..." << std::endl;

    // Create a copy of the object to modify during animation
    Object animatedObject = object;

    for (int frame = 0; frame < totalFrames; ++frame) {
        float currentTime = startTime + frame * timeStep;

        // Get interpolated keyframe for current time
        Keyframe currentKeyframe = get_keyframe(currentTime, InterpolationType::Linear);
        std::cout << "Processing frame " << frame + 1 << " of " << totalFrames << std::endl;
        std::cout << "Current time: " << currentTime << ", Keyframe time: " << currentKeyframe.t << std::endl;

        // Apply keyframe to object copy
        animatedObject.setPosition(currentKeyframe.object_position, currentKeyframe.r, currentKeyframe.phi);
        animatedObject.setRotation(currentKeyframe.object_rotation_axis.normalized(), 
                          currentKeyframe.object_rotation_axis.norm());
        animatedObject.setScale(currentKeyframe.object_scale*Vector3f(1.0f, 1.0f, 1.0f));
        
        // Generate lines and render
        LineSet lineSet = camera.convert_to_lines(animatedObject, currentKeyframe.t, currentKeyframe.length);
        
        // Create a copy of imageGenerator to modify
        ImageGenerator imgGen = imageGenerator;
        imgGen.clear();
        imgGen.drawLines(lineSet, currentKeyframe.decay_length, currentKeyframe.glow_length);
        imgGen.drawPoint(lineSet.getStartPoint(), currentKeyframe.point_glow_length);

        // Create filename with frame number
        std::stringstream ss;
        ss << filename << "_" << std::setfill('0') << std::setw(5) << frame << ".bmp";
        imgGen.saveImage(ss.str(), color);
    }
    
    std::cout << "Animation saved successfully!" << std::endl;
    // system("ffmpeg.exe -framerate 10 -i imgs/animation_output_%05d.bmp -c:v libx264 -preset slow -crf 18 -pix_fmt yuv420p imgs/animation_output.mp4 -y");
}

void Animator::save_keyframes(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    // Write header
    file << "# Keyframe data file\n";

    file << "# time   t          length    decay_le  glow_len  glow_poi  pos_x     pos_y     pos_z     rot_x     rot_y     rot_z     scale_x   shift_e   screw_e   obj_sh_e  polar_r   polar_phi  curve \n";
    // file << keyframes.size() << "\n";

    // Write keyframes
    for (const auto& kf : keyframes) {
        file << std::fixed << std::setprecision(6);
        file << kf.time << "  "
             << kf.t << "  "
             << kf.length << "  "
             << kf.decay_length << "  "
             << kf.glow_length << "  "
             << kf.point_glow_length << "  "
             << kf.object_position.x() << "  "
             << kf.object_position.y() << "  "
             << kf.object_position.z() << "  "
             << kf.object_rotation_axis.x() << "  "
             << kf.object_rotation_axis.y() << "  "
             << kf.object_rotation_axis.z() << "  "
             << kf.object_scale << "  "
             << kf.camera_shift_error << "  "
             << kf.camera_shear_error << "  "
             << kf.object_shift_error << "  "
             << kf.r << "  "
             << kf.phi << "  \n";

    }

    file.close();
    std::cout << "Keyframes saved to " << filename << std::endl;
}

void Animator::load_keyframes(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file for reading: " << filename << std::endl;
        return;
    }

    keyframes.clear();
    
    std::string line;
    // Skip comment lines
    while (std::getline(file, line) && line.front() == '#') {
        continue;
    }

    // Read keyframes if line is empty or it is the last line stop
    int i = 0;
    do{

        std::stringstream ss(line);
        Keyframe kf;
        std::string key_frame_curve;

        // Try to read all numeric fields first
        if (!(ss >> kf.time >> kf.t >> kf.length >> kf.decay_length >> kf.glow_length >> kf.point_glow_length
              >> kf.object_position.x() >> kf.object_position.y() >> kf.object_position.z()
              >> kf.object_rotation_axis.x() >> kf.object_rotation_axis.y() >> kf.object_rotation_axis.z()
              >> kf.object_scale >> kf.camera_shift_error >> kf.camera_shear_error >> kf.object_shift_error >> kf.r >> kf.phi)) {
            std::cerr << "Error parsing keyframe " << i << std::endl;
            continue;
        }

        // Try to read curve string if present
        if (ss >> key_frame_curve) {
            kf.setCurve(key_frame_curve);
        }

        keyframes.push_back(kf);
        i++;
    } while (std::getline(file, line) && !line.empty());

    // Sort keyframes by time
    std::sort(keyframes.begin(), keyframes.end(),
        [](const Keyframe& a, const Keyframe& b) { return a.time < b.time; });

    file.close();
    std::cout << "Loaded " << keyframes.size() << " keyframes from " << filename << std::endl;
}

void Animator::add_keyframe(const Keyframe& keyframe) {
    // Insert keyframe in the correct position to maintain time order
    auto it = std::lower_bound(keyframes.begin(), keyframes.end(), keyframe,
        [](const Keyframe& a, const Keyframe& b) { return a.time < b.time; });
    
    keyframes.insert(it, keyframe);
    std::cout << "Added keyframe at time " << keyframe.time << std::endl;
}

Vector3f Animator::get_color() const {
    return color;
}

std::string Animator::get_name() const {
    return name;
}

float Animator::get_start_time() const {
    if (keyframes.empty()) {
        std::cerr << "No keyframes available to determine start time." << std::endl;
        return 0.0f;
    }
    return keyframes.front().time;
}

float Animator::get_end_time() const {
    if (keyframes.empty()) {
        std::cerr << "No keyframes available to determine end time." << std::endl;
        return 0.0f;
    }
    return keyframes.back().time;
}

void Animator::clear() {
    imageGenerator.clear();
}