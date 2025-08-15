#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Camera.h"
#include "ImageGenerator.h"
#include "Keyframe.h"

class Animator
{
private:
    std::vector<Keyframe> keyframes;
    Camera camera;
    ImageGenerator imageGenerator;
    Object object;
    int fps;
public:
    Animator(Camera cam, ImageGenerator imgGen, Object object, int fps);
    Keyframe get_keyframe(float time, InterpolationType type) const;
    void animate(const std::string& filename) const; //Saves the entire animation as images (bmp)
    void save_keyframes(const std::string& filename) const; //Saves the keyframes in a txt file
    void load_keyframes(const std::string& filename);
    void add_keyframe(const Keyframe& keyframe);
};



#endif // ANIMATOR_H