#ifndef IMAGE_GENERATOR_H
#define IMAGE_GENERATOR_H

#include <Eigen/Dense>
#include <vector>
#include "LineSet.h"

using namespace Eigen;

class ImageGenerator {
public:
    ImageGenerator();
    ImageGenerator(int width, int height);
    void drawPoints(const std::vector<Vector2f>& points, const std::vector<float>& intensity);
    void drawLines(const LineSet& lineSet, const float& decay_length=0.25f, const float& glow_length=0.5f);
    void drawPoint(const Vector2f& point, const float& glow_length);
    void saveImage(const std::string& filename, const Vector3f& color);
    std::vector<Vector2i> getMask(const LineSet& lineSet) const;
    void normalize();
    void clear() {
        std::fill(alpha, alpha + width * height, 0.0f);
    };
    float* get_alpha() {return alpha;}
    void set_debug_mode(bool mode);


private:
    int width;
    int height;
    float* alpha;
    float gauss(float x, float y, float sigma);
    float conversion_factor = 1.0f;
    int max_radius = 7;
    float max_line_distance = 7.0f;
    bool debug_mode = false;
};

#endif // IMAGE_GENERATOR_H