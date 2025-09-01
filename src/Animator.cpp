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

void Animator::set_debug_mode(bool mode) {
    debug_mode = mode;
    imageGenerator.set_debug_mode(mode);
}

void Animator::render_frame(float time) {
    // KeyframeCollection currentKeyframe = get_keyframe(time, InterpolationType::Linear);

    // Apply keyframe to object copy
    object.setPosition(keyframeSet.get_object_position(time), keyframeSet.get_r(time), keyframeSet.get_phi(time));
    object.setRotation(keyframeSet.get_object_rotation_axis(time), keyframeSet.get_object_rotation_angle(time));
    object.setScale(keyframeSet.get_object_scale(time) * Vector3f(1.0f, 1.0f, 1.0f));
    float x_err = dist(gen) * keyframeSet.get_cam_shift_err(time) * 0.05f;
    float y_err = dist(gen) * keyframeSet.get_cam_shift_err(time) * 0.05f;
    float shear_err = dist(gen) * keyframeSet.get_cam_shear_err(time) * 0.05f;
    float x_offset_err = dist(gen) * keyframeSet.get_obj_shift_err(time) * 0.05f;
    float y_offset_err = dist(gen) * keyframeSet.get_obj_shift_err(time) * 0.05f;


    camera.set_error(shear_err, x_err, y_err, x_offset_err, y_offset_err);
    camera.set_proj_matrix();

    // Generate lines and render
    LineSet lineSet = camera.convert_to_lines(object, keyframeSet.get_t(time), keyframeSet.get_length(time));

    imageGenerator.clear();
    imageGenerator.drawLines(lineSet, keyframeSet.get_decay_length(time), keyframeSet.get_glow_length(time));
    imageGenerator.drawPoint(lineSet.getStartPoint(), keyframeSet.get_point_glow_length(time));
}

// void Animator::animate(const std::string& filename) const {

//     // Calculate animation duration
//     float startTime = keyframeSet.get_start_time();
//     float endTime = keyframeSet.get_end_time();
//     float duration = endTime - startTime;
    
//     if (duration <= 0) {
//         std::cerr << "Invalid animation duration!" << std::endl;
//         return;
//     }

//     // Calculate number of frames
//     int totalFrames = static_cast<int>(duration * fps);
//     float timeStep = duration / totalFrames;

//     std::cout << "Generating " << totalFrames << " frames for animation..." << std::endl;

//     // Create a copy of the object to modify during animation
//     Object animatedObject = object;

//     for (int frame = 0; frame < totalFrames; ++frame) {
//         float currentTime = startTime + frame * timeStep;

//         // Get interpolated keyframe for current time
//         KeyframeCollection currentKeyframe = get_keyframe(currentTime, InterpolationType::Linear);
//         std::cout << "Processing frame " << frame + 1 << " of " << totalFrames << std::endl;
//         std::cout << "Current time: " << currentTime << ", Keyframe time: " << currentKeyframe.t << std::endl;

//         // Apply keyframe to object copy
//         animatedObject.setPosition(currentKeyframe.object_position, currentKeyframe.r, currentKeyframe.phi);
//         animatedObject.setRotation(currentKeyframe.object_rotation_axis.normalized(), 
//                           currentKeyframe.object_rotation_axis.norm());
//         animatedObject.setScale(currentKeyframe.object_scale*Vector3f(1.0f, 1.0f, 1.0f));
        
//         // Generate lines and render
//         LineSet lineSet = camera.convert_to_lines(animatedObject, currentKeyframe.t, currentKeyframe.length);
        
//         // Create a copy of imageGenerator to modify
//         ImageGenerator imgGen = imageGenerator;
//         imgGen.clear();
//         imgGen.drawLines(lineSet, currentKeyframe.decay_length, currentKeyframe.glow_length);
//         imgGen.drawPoint(lineSet.getStartPoint(), currentKeyframe.point_glow_length);

//         // Create filename with frame number
//         std::stringstream ss;
//         ss << filename << "_" << std::setfill('0') << std::setw(5) << frame << ".bmp";
//         imgGen.saveImage(ss.str(), color);
//     }
    
//     std::cout << "Animation saved successfully!" << std::endl;
//     // system("ffmpeg.exe -framerate 10 -i imgs/animation_output_%05d.bmp -c:v libx264 -preset slow -crf 18 -pix_fmt yuv420p imgs/animation_output.mp4 -y");
// }

void Animator::load_keyframes(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file for reading: " << filename << std::endl;
        return;
    }


    std::vector<KeyframeCollection> keyframes;
    
    std::string line;
    // Check if the first line is "# KeyframeSet"
    if (std::getline(file, line) && line == "# KeyframeSet") {
        keyframeSet = KeyframeSet(filename);
        std::cout << "Loaded " << keyframeSet.object_position_x.size() << " keyframes from " << filename << std::endl;
        std::cout << "posx start_t: " << keyframeSet.object_position_x.front().start_time << " end_t: " << keyframeSet.object_position_x.front().end_time << " start_val: " << keyframeSet.object_position_x.front().start_val << " end_val: " << keyframeSet.object_position_x.front().end_val << std::endl;
        std::cout << "Pos at t=0.0: " << keyframeSet.get_object_position(0.0f) << std::endl;
        std::cout << "Pos at t=0.5: " << keyframeSet.get_object_position(0.5f) << std::endl;
        std::cout << "Pos at t=1.0: " << keyframeSet.get_object_position(1.0f) << std::endl;
        return;
    }

    // Skip comment lines
    while (std::getline(file, line) && line.front() == '#') {
        continue;
    }

    // Read keyframes if line is empty or it is the last line stop
    int i = 0;
    do{

        std::stringstream ss(line);
        KeyframeCollection kf;
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
        [](const KeyframeCollection& a, const KeyframeCollection& b) { return a.time < b.time; });

    file.close();
    std::cout << "Loaded " << keyframes.size() << " keyframes from " << filename << std::endl;
    keyframeSet = KeyframeSet(keyframes);
}

Vector3f Animator::get_color() const {
    // Add minimal noise to color
    return color + Vector3f::Random() * 0.01f;
}

std::string Animator::get_name() const {
    return name;
}

float Animator::get_start_time() const {
    return keyframeSet.get_start_time();
}

float Animator::get_end_time() const {
    return keyframeSet.get_end_time();
}

void Animator::clear() {
    imageGenerator.clear();
}