#include "Animator.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

Animator::Animator(Camera cam, ImageGenerator imgGen, Object object, int fps)
    : camera(cam), imageGenerator(imgGen), object(object), fps(fps) {
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
            interpolated.t = prevFrame.t + t * (nextFrame.t - prevFrame.t);
            interpolated.length = prevFrame.length + t * (nextFrame.length - prevFrame.length);
            interpolated.decay_length = prevFrame.decay_length + t * (nextFrame.decay_length - prevFrame.decay_length);
            interpolated.glow_length = prevFrame.glow_length + t * (nextFrame.glow_length - prevFrame.glow_length);
            interpolated.object_position = prevFrame.object_position + t * (nextFrame.object_position - prevFrame.object_position);
            interpolated.object_rotation_axis = prevFrame.object_rotation_axis + t * (nextFrame.object_rotation_axis - prevFrame.object_rotation_axis);
            interpolated.object_scale = prevFrame.object_scale + t * (nextFrame.object_scale - prevFrame.object_scale);
            break;

        case InterpolationType::Cubic:
            // For cubic interpolation, we'd need more complex math
            // For now, fall back to linear interpolation
            // TODO: Implement proper cubic spline interpolation
            interpolated.t = prevFrame.t + t * (nextFrame.t - prevFrame.t);
            interpolated.length = prevFrame.length + t * (nextFrame.length - prevFrame.length);
            interpolated.decay_length = prevFrame.decay_length + t * (nextFrame.decay_length - prevFrame.decay_length);
            interpolated.glow_length = prevFrame.glow_length + t * (nextFrame.glow_length - prevFrame.glow_length);
            interpolated.object_position = prevFrame.object_position + t * (nextFrame.object_position - prevFrame.object_position);
            interpolated.object_rotation_axis = prevFrame.object_rotation_axis + t * (nextFrame.object_rotation_axis - prevFrame.object_rotation_axis);
            interpolated.object_scale = prevFrame.object_scale + t * (nextFrame.object_scale - prevFrame.object_scale);
            break;
    }

    return interpolated;
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
        animatedObject.setPosition(currentKeyframe.object_position);
        animatedObject.setRotation(currentKeyframe.object_rotation_axis.normalized(), 
                          currentKeyframe.object_rotation_axis.norm());
        
        // Generate lines and render
        LineSet lineSet = camera.convert_to_lines(animatedObject, currentKeyframe.t, currentKeyframe.length);
        
        // Create a copy of imageGenerator to modify
        ImageGenerator imgGen = imageGenerator;
        imgGen.clear();
        imgGen.drawLines(lineSet, Vector3f(1.0f, 0.0f, 0.0f), currentKeyframe.decay_length, currentKeyframe.glow_length); // Red color
        
        // Create filename with frame number
        std::stringstream ss;
        ss << filename << "_" << std::setfill('0') << std::setw(5) << frame << ".bmp";
        imgGen.saveImage(ss.str());
    }
    
    std::cout << "Animation saved successfully!" << std::endl;
    system("ffmpeg.exe -framerate 10 -i imgs/animation_output_%05d.bmp -c:v libx264 -preset slow -crf 18 -pix_fmt yuv420p imgs/animation_output.mp4 -y");
}

void Animator::save_keyframes(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    // Write header
    file << "# Keyframe data file\n";
    file << "# Format: time t length decay_length glow_length pos_x pos_y pos_z rot_x rot_y rot_z scale_x scale_y scale_z\n";
    file << keyframes.size() << "\n";

    // Write keyframes
    for (const auto& kf : keyframes) {
        file << std::fixed << std::setprecision(6);
        file << kf.time << " "
             << kf.t << " "
             << kf.length << " "
             << kf.decay_length << " "
             << kf.glow_length << " "
             << kf.object_position.x() << " "
             << kf.object_position.y() << " "
             << kf.object_position.z() << " "
             << kf.object_rotation_axis.x() << " "
             << kf.object_rotation_axis.y() << " "
             << kf.object_rotation_axis.z() << " "
             << kf.object_scale.x() << " "
             << kf.object_scale.y() << " "
             << kf.object_scale.z() << "\n";
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

    // Read number of keyframes
    int numKeyframes;
    if (!(std::stringstream(line) >> numKeyframes)) {
        std::cerr << "Error reading number of keyframes" << std::endl;
        return;
    }

    keyframes.reserve(numKeyframes);

    // Read keyframes
    for (int i = 0; i < numKeyframes; ++i) {
        if (!std::getline(file, line)) {
            std::cerr << "Unexpected end of file" << std::endl;
            break;
        }

        std::stringstream ss(line);
        Keyframe kf;
        
        if (!(ss >> kf.time >> kf.t >> kf.length >> kf.decay_length >> kf.glow_length
              >> kf.object_position.x() >> kf.object_position.y() >> kf.object_position.z()
              >> kf.object_rotation_axis.x() >> kf.object_rotation_axis.y() >> kf.object_rotation_axis.z()
              >> kf.object_scale.x() >> kf.object_scale.y() >> kf.object_scale.z())) {
            std::cerr << "Error parsing keyframe " << i << std::endl;
            continue;
        }

        keyframes.push_back(kf);
    }

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
