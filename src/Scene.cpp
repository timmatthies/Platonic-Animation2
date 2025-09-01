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
        std::filesystem::create_directory(path + "/imgs");
        std::cout << "Loading scene from " << path << "/scene.txt" << std::endl;
        std::string line;
        // First lines give the screen height, the screen width, the fps, and the background color
        float bc_r, bc_g, bc_b;
        int debug_mode_int = 0;
        // Skip first line
        std::getline(file, line);
        if (file >> width >> height >> fps >> bc_r >> bc_g >> bc_b >> upscale_factor >> debug_mode_int) {
            // Create the camera, image generator, and object with the loaded parameters
            backgroundColor = Vector3f(bc_r, bc_g, bc_b);
            debug_mode = (debug_mode_int != 0);
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

        set_debug_mode(debug_mode);
        file.close();
    }else {
        std::cout << "No scene file found at " << path << "/scene.txt" << std::endl;
        std::cout << "Please create a scene.txt file with the following format:" << std::endl;
        std::cout << "width height fps background_color_r background_color_g background_color_b upscale_factor debug_mode" << std::endl;
        std::cout << "color_r color_g color_b object_name keyframe_file" << std::endl;
    }
}

void Scene::set_debug_mode(bool mode) {
    std::cout << "Setting debug mode to " << (mode ? "ON" : "OFF") << std::endl;
    debug_mode = mode;
    for (auto& animator : animators) {
        animator.set_debug_mode(mode);
    }
}

void Scene::animate() {
    float start_time = get_animation_start_time();
    float end_time = get_animation_end_time();
    float duration = end_time - start_time;
    std::cout << start_time << " " << end_time << " " << duration << std::endl;

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
    std::vector<Vector3f> upscaled_screen(width * height * upscale_factor * upscale_factor, Vector3f(0.0f, 0.0f, 0.0f));
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
        std::vector<float> total_alpha(width * height, 0.0f);
        std::vector<Vector3f> color_sum(width * height, Vector3f(0.0f, 0.0f, 0.0f));

        // First pass: collect all contributions and calculate total alpha
        for (size_t k = 0; k < animators.size(); k++) {
            std::vector<float> a = animators[k].get_alpha();
            #pragma omp parallel for
            for (int l = 0; l < width * height; l++) {
                total_alpha[l] += a[l];
                color_sum[l] += animators[k].get_color() * a[l];
            }
        }

        // Second pass: normalize and blend with background
        #pragma omp parallel for
        for (int m = 0; m < width * height; m++) {
            if (total_alpha[m] > 0.0f) {
                // Normalize the color by total alpha to avoid over-saturation
                Vector3f blended_color = color_sum[m] / total_alpha[m];
                // Cap alpha at 1.0
                float final_alpha = std::min(1.0f, total_alpha[m]);
                // Blend with background
                screen[m] = final_alpha * blended_color + (1.0f - final_alpha) * backgroundColor;
            } else {
                screen[m] = backgroundColor;
            }
        }

        save_image(i, screen);
        
        

        // Save the current frame as an image
        std::fill(screen.begin(), screen.end(), Vector3f(0.0f, 0.0f, 0.0f)); // Reset screen for next frame
        std::fill(alpha.begin(), alpha.end(), 0.0f); // Reset alpha for next frame
    }
    std::cout << "Animation completed and saved to " << img_path << std::endl;

    // Load audio 
    std::stringstream ss;
    ss << "ffmpeg -ss " << start_time << " -i PL.mp3 -t " << (end_time - start_time) << " -acodec copy " << img_path << "/audio.mp3 -y";
    system(ss.str().c_str());
    ss.str("");

    ss << "ffmpeg -framerate " << fps << " -i " << img_path << "/frame_%05d.bmp -i " << img_path << "/audio.mp3 -c:v libx264 -preset slow -crf 15 -pix_fmt yuv420p -movflags +faststart " << img_path << "/animation_output.mp4 -y ";

    // High-quality video encoding with ffmpeg
    system(ss.str().c_str());
}

void Scene::save_image(const int& frame_number, const std::vector<Vector3f>& screen) const {
    // Calculate the upscaled dimensions safely
    size_t upscaled_width = static_cast<size_t>(width) * upscale_factor;
    size_t upscaled_height = static_cast<size_t>(height) * upscale_factor;
    
    // Use size_t for safety with large allocations
    std::vector<uint8_t> bmpData(upscaled_width * upscaled_height * 3);
    
    if (upscale_factor == 1) {
        #pragma omp parallel for
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                Vector3f c = screen[y * width + x];
                size_t idx = (y * width + x) * 3;
                // Bounds check
                if (idx + 2 < bmpData.size()) {
                    bmpData[idx + 0] = static_cast<uint8_t>(std::min(255.0f, c.x() * 255.0f));
                    bmpData[idx + 1] = static_cast<uint8_t>(std::min(255.0f, c.y() * 255.0f));
                    bmpData[idx + 2] = static_cast<uint8_t>(std::min(255.0f, c.z() * 255.0f));
                }
            }
        }
    } else {
        #pragma omp parallel for
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                Vector3f c = screen[y * width + x];
                for (int dy = 0; dy < upscale_factor; ++dy) {
                    for (int dx = 0; dx < upscale_factor; ++dx) {
                        size_t upscaled_x = static_cast<size_t>(x) * upscale_factor + dx;
                        size_t upscaled_y = static_cast<size_t>(y) * upscale_factor + dy;
                        size_t idx = (upscaled_y * upscaled_width + upscaled_x) * 3;
                        
                        // Add bounds check
                        if (idx + 2 < bmpData.size()) {
                            bmpData[idx + 0] = static_cast<uint8_t>(std::min(255.0f, c.x() * 255.0f));
                            bmpData[idx + 1] = static_cast<uint8_t>(std::min(255.0f, c.y() * 255.0f));
                            bmpData[idx + 2] = static_cast<uint8_t>(std::min(255.0f, c.z() * 255.0f));
                        }
                    }
                }
            }
        }
    }
    
    std::stringstream ss;
    ss << img_path << "/frame_" << std::setfill('0') << std::setw(5) << frame_number << ".bmp";
    enum save_bmp_result result = save_bmp(ss.str().c_str(), upscaled_width, upscaled_height, bmpData.data());
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

    //Find minimum start time
    float startTime = animators.front().get_start_time();
    for (const auto& animator : animators) {
        startTime = std::min(startTime, animator.get_start_time());
    }

    return startTime;
}

float Scene::get_animation_end_time() const {
    if (animators.empty()) {
        std::cerr << "No animators available to determine end time." << std::endl;
        return 0.0f;
    }

    // Find maximum end time
    float endTime = animators.front().get_end_time();
    for (const auto& animator : animators) {
        endTime = std::max(endTime, animator.get_end_time());
    }

    return endTime;
}
