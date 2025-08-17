#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "Animator.h"
#include "Keyframe.h"
#include <string>


class Scene {
public:
    Scene(std::string filename);
    void animate();

private:
    std::vector<Animator> animators;
    float get_animation_start_time() const;
    float get_animation_end_time() const;
    void save_image(const int& frame_number, const std::vector<Vector3f>& screen) const;
    int fps;
    int width;
    int height;
    Vector3f backgroundColor;
    std::string img_path;
};

#endif // SCENE_H