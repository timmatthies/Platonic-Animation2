#define SAVE_BMP_IMPLEMENT
#include "save_bmp.h"

#include "ImageGenerator.h"
#include <iostream>

ImageGenerator::ImageGenerator(int width, int height)
    : width(width), height(height), imageData(width * height, Vector3f(0, 0, 0)) {
    }

void ImageGenerator::drawPoints(const std::vector<Vector2f>& points, const std::vector<float>& intensity, const Vector3f& color) {
    // Create a gaussian around each point
    float sigma = 2.0f; // Standard deviation for the Gaussian
    int threshold_sigma = ceil(3.0f * sigma); // Threshold for considering points within 3 sigma
    for (size_t i = 0; i < points.size(); ++i) {
        // Calculate the closest pixel coordinates to the gaussian center
        // We only consider points that are 3 sigma away from the center
        float x = points[i].x();
        float y = points[i].y();
        int ix = (int)x;
        int iy = (int)y;
        for(int dx = ix-threshold_sigma; dx <= ix+threshold_sigma; ++dx) {
            if(dx < 0 || dx >= width) continue; // Skip out of bounds x
            for(int dy = iy-threshold_sigma; dy <= iy+threshold_sigma; ++dy) {
                if( dy < 0 || dy >= height) continue; // Skip out of bounds y
                // Calculate the Gaussian value at this pixel
                float gaussian_value = gauss((float)dx - x, (float)dy - y, sigma);
                // Scale the color by intensity and Gaussian value
                Vector3f pixelColor = color * (intensity[i] * gaussian_value);
                imageData[dy * width + dx] += pixelColor;
            }
        }
    }
}

void ImageGenerator::drawLines(const LineSet& lineSet, const Vector3f& color) {
    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            Vector2f point((float)x, (float)y);
            float t = lineSet.get_t(point);
            float squaredDistance = lineSet.squaredDistance(point);
            imageData[y * width + x] += color * exp(-squaredDistance/4);
        }
    }
}

float ImageGenerator::gauss(float x, float y, float sigma) {
    return std::exp(-(x * x + y * y) / (2 * sigma * sigma));
}

void ImageGenerator::normalize() {
    float maxIntensity = 0.0f;
    for (const auto& pixel : imageData) {
        maxIntensity = std::max(maxIntensity, pixel.squaredNorm());
    }
    maxIntensity = std::sqrt(maxIntensity);
    if (maxIntensity > 0.0f) {
        for (auto& pixel : imageData) {
            pixel /= maxIntensity;
        }
    }
}

void ImageGenerator::saveImage(const std::string& filename) {
    std::vector<uint8_t> bmpData(width * height * 3);
    normalize(); // Normalize the image data before saving
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Vector3f color = imageData[y * width + x];
            bmpData[(y * width + x) * 3 + 0] = static_cast<uint8_t>(std::min(255.0f, color.x() * 255.0f));
            bmpData[(y * width + x) * 3 + 1] = static_cast<uint8_t>(std::min(255.0f, color.y() * 255.0f));
            bmpData[(y * width + x) * 3 + 2] = static_cast<uint8_t>(std::min(255.0f, color.z() * 255.0f));
        }
    }
    // Save the image as a BMP file
    enum save_bmp_result result = save_bmp(filename.c_str(), width, height, bmpData.data());
    // Check the result of saving the BMP file
    if (result != SAVE_BMP_SUCCESS) {
        std::cerr << "Error saving image: " << result << std::endl;
        std::cerr << save_bmp_str_result(result) << std::endl;
        return;
    }else {
        std::cout << "Image saved successfully to " << filename << std::endl;
    }
}
