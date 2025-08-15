#ifndef IMAGE_GENERATOR_H
#define IMAGE_GENERATOR_H

#include <Eigen/Dense>
#include <vector>
#include "LineSet.h"

using namespace Eigen;

class ImageGenerator {
public:
    ImageGenerator(int width, int height);
    void drawPoints(const std::vector<Vector2f>& points, const std::vector<float>& intensity, const Vector3f& color);
    void drawLines(const LineSet& lineSet, const Vector3f& color, const float& decay_length=0.25f, const float& glow_length=0.5f);
    void saveImage(const std::string& filename);
    std::vector<Vector2i> getMask(const LineSet& lineSet) const;
    void normalize();
    void clear() {
        std::fill(imageData.begin(), imageData.end(), Vector3f(0, 0, 0));
    }

private:
    int width;
    int height;
    std::vector<Vector3f> imageData;
    float gauss(float x, float y, float sigma);
    float conversion_factor = 1.0f;
};

#endif // IMAGE_GENERATOR_H