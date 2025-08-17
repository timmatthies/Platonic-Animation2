#include "Scene.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include "save_bmp.h"

Scene::Scene(std::string path) {
    // See if there is a file called "path/scene.txt"
    std::ifstream file(path + "/scene.txt");

    // Does it exist?
    if (file.is_open()) {
        std::cout << "Loading scene from " << path << "/scene.txt" << std::endl;
        std::string line;
        // First lines give the screen height, the screen width, the fps, and the background color
        float bc_r, bc_g, bc_b;

        // Skip first line
        std::getline(file, line);
        if (file >> width >> height >> fps >> bc_r >> bc_g >> bc_b) {
            // Create the camera, image generator, and object with the loaded parameters
            backgroundColor = Vector3f(bc_r, bc_g, bc_b);
        }
        //Load the five animators (for each platonic solid)
        //Read the color and the file for the keyframes from "path/scene.txt"
        img_path = path + "/imgs";
        for (int i = 0; i < 5; ++i) {
            std::getline(file, line);
            float c_r, c_g, c_b;
            std::string keyframeFile;
            std::string objectName;
            if (file >> c_r >> c_g >> c_b >> objectName >> keyframeFile) {
                Vector3f color(c_r, c_g, c_b);
                std::string name = keyframeFile.substr(0, keyframeFile.find_last_of('.'));
                Camera cam = Camera(width, height);
                ImageGenerator imgGen = ImageGenerator(width, height);
                Object object = Object::getObjectByName(objectName);
                Animator ani = Animator(name, color, cam, imgGen, object, fps);
                ani.load_keyframes(path + "/keyframes/" + keyframeFile);
                animators.push_back(ani);
            }
        }

        file.close();
    }else {
        std::cout << "No scene file found at " << path << "/scene.txt" << std::endl;
        std::cout << "Creating default scene..." << std::endl;
        int height = 100;
        int width = 100;
        int fps = 10;
        Vector3f backgroundColor(0.1529f, 0.1373f, 0.1804f); // Default background color
        // If the file does not exist, create a default Animator and save it to filename
        Camera cam = Camera(width, height);
        ImageGenerator imgGen = ImageGenerator(width, height);
        Animator ani = Animator("Cube", Vector3f(0.22f, 1.0f, 0.98f), cam, imgGen, Object::getCube(), fps);
        Keyframe k;
        k.time = 0.0f;
        k.object_position = Vector3f(0.0f, 0.0f, 0.0f);
        k.object_scale = 0.0f;
        ani.add_keyframe(k);
        k.time = 1.0f;
        k.object_scale = 0.0f;
        ani.add_keyframe(k);
        animators.emplace_back(ani);
        cam = Camera(width, height);
        imgGen = ImageGenerator(width, height);
        ani = Animator("Thetra", Vector3f(1.0f, 0.669f, 0.22f), cam, imgGen, Object::getCube(), fps);
        k = Keyframe();
        k.time = 0.0f;
        k.object_position = Vector3f(0.0f, 0.0f, 0.0f);
        k.object_scale = 0.0f;
        ani.add_keyframe(k);
        k.time = 1.0f;
        k.object_position = Vector3f(1.0f, 1.0f, 0.0f);
        k.object_scale = 0.0f;
        ani.add_keyframe(k);
        animators.emplace_back(ani);
        cam = Camera(width, height);
        imgGen = ImageGenerator(width, height);
        ani = Animator("Octa", Vector3f(0.396f, 1.0f, 0.22f), cam, imgGen, Object::getCube(), fps);
        k = Keyframe();
        k.time = 0.0f;
        k.object_position = Vector3f(0.0f, 0.0f, 0.0f);
        k.object_scale = 0.0f;
        ani.add_keyframe(k);
        k.time = 1.0f;
        k.object_position = Vector3f(-1.0f, 1.0f, 0.0f);
        k.object_scale = 0.0f;
        ani.add_keyframe(k);
        animators.emplace_back(ani);
        cam = Camera(width, height);
        imgGen = ImageGenerator(width, height);
        ani = Animator("Ico", Vector3f(0.356f, 0.22f, 1.0f), cam, imgGen, Object::getCube(), fps);
        k = Keyframe();
        k.time = 0.0f;
        k.object_position = Vector3f(0.0f, 0.0f, 0.0f);
        k.object_scale = 0.0f;
        ani.add_keyframe(k);
        k.time = 1.0f;
        k.object_position = Vector3f(-1.0f, -1.0f, 0.0f);
        k.object_scale = 0.0f;
        ani.add_keyframe(k);
        animators.emplace_back(ani);
        cam = Camera(width, height);
        imgGen = ImageGenerator(width, height);
        ani = Animator("Dodeca", Vector3f(1.0f, 0.22f, 0.708f), cam, imgGen, Object::getCube(), fps);
        k = Keyframe();
        k.time = 0.0f;
        k.object_position = Vector3f(0.0f, 0.0f, 0.0f);
        k.object_scale = 0.0f;
        ani.add_keyframe(k);
        k.time = 1.0f;
        k.object_position = Vector3f(1.0f, -1.0f, 0.0f);
        k.object_scale = 0.0f;
        ani.add_keyframe(k);
        animators.emplace_back(ani);

        std::filesystem::create_directory(path);
        std::filesystem::create_directory(path + "/keyframes");
        std::filesystem::create_directory(path + "/imgs");
        img_path = path + "/imgs";
        // Save the default scene to the specified filename
        std::ofstream outFile(path + "/scene.txt");
        if (outFile.is_open()) {
            outFile << "Width Height FPS BackgroundColor\n";
            // Write the camera and image generator parameters
            outFile << width << " " << height << " " << fps << " " << backgroundColor.x() << " " << backgroundColor.y() << " " << backgroundColor.z() << "\n";
            for (const auto& animator : animators) {
                outFile << animator.get_color().x() << " " << animator.get_color().y() << " " << animator.get_color().z() << " " << animator.get_name() << " " << animator.get_name() << ".txt" << "\n";
                animator.save_keyframes(path + "/keyframes/" + animator.get_name() + ".txt");
            }
            outFile.close();
        }
    }
}

void Scene::animate() {
    float start_time = get_animation_start_time();
    float end_time = get_animation_end_time();
    float duration = end_time - start_time;

    int num_frames = static_cast<int>(duration * fps);

    if (num_frames <= 0) {
        std::cerr << "Invalid number of frames!" << std::endl;
        return;
    }

    if (duration <= 0) {
        std::cerr << "Invalid animation duration!" << std::endl;
        return;
    }

    std::vector<Vector3f> screen(width * height, Vector3f(0.0f, 0.0f, 0.0f));
    std::vector<float> alpha(width * height, 0.0f);

    for (int i = 0; i < num_frames; i++)
    {
        float time = start_time + i * (1.0f / fps);
        std::cout << "Processing frame " << i + 1 << " of " << num_frames << ", Current time: " << time << std::endl;
        for (size_t j = 0; j < animators.size(); j++){
            animators[j].clear();
            animators[j].render_frame(time);
        }

        // Add the current frame to the screen
        for (size_t k = 0; k < animators.size(); k++) {
            float* a = animators[k].get_alpha();
            #pragma omp parallel for
            for (int l = 0; l < width * height; l++){
                alpha[l] += a[l];
                screen[l] += animators[k].get_color() * a[l];
            }
        }
        #pragma omp parallel for
        for (int m = 0; m < width * height; m++)
        {
            alpha[m] = std::min(1.0f, alpha[m]);
            screen[m] = alpha[m] * screen[m] / screen[m].norm() + (1 - alpha[m]) * backgroundColor;
            if (alpha[m] <= 0.0f) {
                screen[m] = backgroundColor;
            }
        }
        
        // Save the current frame as an image
        save_image(i, screen);
        std::fill(screen.begin(), screen.end(), Vector3f(0.0f, 0.0f, 0.0f)); // Reset screen for next frame
        std::fill(alpha.begin(), alpha.end(), 0.0f); // Reset alpha for next frame
    }
    std::cout << "Animation completed and saved to " << img_path << std::endl;

    std::stringstream ss;
    ss << "ffmpeg.exe -framerate " << fps << " -i " << img_path << "/frame_%05d.bmp -c:v libx264 -preset slow -crf 15 -pix_fmt yuv420p -movflags +faststart " << img_path << "/animation_output.mp4 -y";

    // High-quality video encoding with ffmpeg
    system(ss.str().c_str());
}

void Scene::save_image(const int& frame_number, const std::vector<Vector3f>& screen) const {
    std::vector<uint8_t> bmpData(width * height * 3);
    // normalize(); // Normalize the image data before saving

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Vector3f c = screen[y * width + x];
            bmpData[(y * width + x) * 3 + 0] = static_cast<uint8_t>(std::min(255.0f, c.x() * 255.0f));
            bmpData[(y * width + x) * 3 + 1] = static_cast<uint8_t>(std::min(255.0f, c.y() * 255.0f));
            bmpData[(y * width + x) * 3 + 2] = static_cast<uint8_t>(std::min(255.0f, c.z() * 255.0f));
        }
    }
    std::stringstream ss;
    ss << img_path << "/frame_" << std::setfill('0') << std::setw(5) << frame_number << ".bmp";
    enum save_bmp_result result = save_bmp(ss.str().c_str(), width, height, bmpData.data());
    // Check the result of saving the BMP file
    if (result != SAVE_BMP_SUCCESS) {
        std::cerr << "!!!Error saving image: " << result << std::endl;
        std::cerr << save_bmp_str_result(result) << std::endl;
        return;
    }
}

float Scene::get_animation_start_time() const {
    if (animators.empty()) {
        std::cerr << "No animators available to determine start time." << std::endl;
        return 0.0f;
    }

    //Check that all animators have the same start time
    float startTime = animators.front().get_start_time();
    for (const auto& animator : animators) {
        if (animator.get_start_time() != startTime) {
            std::cerr << "Inconsistent start times found." << std::endl;
            return 0.0f;
        }
    }
    return startTime;
}

float Scene::get_animation_end_time() const {
    if (animators.empty()) {
        std::cerr << "No animators available to determine end time." << std::endl;
        return 0.0f;
    }

    // Check that all animators have the same end time
    float endTime = animators.front().get_end_time();
    for (const auto& animator : animators) {
        if (animator.get_end_time() != endTime) {
            std::cerr << "Inconsistent end times found." << std::endl;
            return 0.0f;
        }
    }
    return endTime;
}
