#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Camera.h"
#include <random>
#include "ImageGenerator.h"
#include "KeyframeCollection.h"
#include "KeyframeSet.h"

class Animator
{
private:
    std::string name;
    Vector3f color;
    Camera camera;
    ImageGenerator imageGenerator;
    Object object;
    int fps;
    
    KeyframeSet keyframeSet;
    std::mt19937 gen;
    std::normal_distribution<float> dist;
    bool debug_mode = false;
public:
    Animator();
    Animator(std::string name, Vector3f color,Camera cam, ImageGenerator imgGen, Object object, int fps);
    // void animate(const std::string& filename) const; //Saves the entire animation as images (bmp)
    void render_frame(float time);
    void load_keyframes(const std::string& filename);
    Vector3f get_color() const;
    std::string get_name() const;
    float get_start_time() const;
    float get_end_time() const;
    void clear();
    float* get_alpha() {
        return imageGenerator.get_alpha();
    }
    void set_debug_mode(bool mode);

};



#endif // ANIMATOR_H