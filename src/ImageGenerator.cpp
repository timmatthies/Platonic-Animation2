#define SAVE_BMP_IMPLEMENT
#include "save_bmp.h"

#include "ImageGenerator.h"
#include <iostream>

ImageGenerator::ImageGenerator()
    : width(100), height(100) {
        conversion_factor = (width + height) / 2.0f/100.0f;
        max_radius = 7*conversion_factor;
        alpha = new float[width * height * 3];
        std::fill(alpha, alpha + width * height * 3, 0.0f);
    }

ImageGenerator::ImageGenerator(int width, int height)
    : width(width), height(height) {
        conversion_factor = (width + height) / 2.0f/100.0f;
        max_radius = 7*conversion_factor;
        alpha = new float[width * height * 3];
        std::fill(alpha, alpha + width * height * 3, 0.0f);
    }

void ImageGenerator::drawPoints(const std::vector<Vector2f>& points, const std::vector<float>& intensity) {
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
                alpha[dy * width + dx] += (intensity[i] * gaussian_value);
            }
        }
    }
}

std::vector<Vector2i> ImageGenerator::getMask(const LineSet& lineSet) const {
    std::vector<Vector2i> mask = lineSet.getMask(max_line_distance);

    return mask;
}

void ImageGenerator::drawLines(const LineSet& lineSet, const float& decay_length, const float& glow_length) {
    std::vector<Vector2i> mask = getMask(lineSet);
    #pragma omp parallel for
    for (size_t i = 0; i < mask.size(); ++i) {
        Vector2f point((float)mask[i].x(), (float)mask[i].y());
        float t = lineSet.get_t(point);
        float squaredDistance = lineSet.squaredDistance(point);
        alpha[mask[i].y() * width + mask[i].x()] = exp(-sqrt(squaredDistance)/glow_length/(conversion_factor))*exp(-t/decay_length/conversion_factor*100.0f);
    }
}

void ImageGenerator::drawPoint(const Vector2f& point, const float& glow_length) {
    int ix = (int)point.x();
    int iy = (int)point.y();
    for (int dx = ix - max_radius; dx <= ix + max_radius; ++dx) {
        if (dx < 0 || dx >= width) continue; // Skip out of bounds x
        for (int dy = iy - max_radius; dy <= iy + max_radius; ++dy) {
            if (dy < 0 || dy >= height) continue; // Skip out of bounds y
            float squaredDistance = (dx - ix) * (dx - ix) + (dy - iy) * (dy - iy);
            float prev_mag = alpha[dy * width + dx];
            float new_mag = exp(-sqrt(squaredDistance)/glow_length/(conversion_factor));
            if (new_mag < 1/255.0f) continue; // Skip very small contributions
            if (prev_mag>new_mag) {
                // If the previous magnitude is greater, skip this pixel
                continue;
            } else if (prev_mag < new_mag) {
                // If the new magnitude is greater, update the pixel color
                alpha[dy * width + dx] = exp(-sqrt(squaredDistance)/glow_length/(conversion_factor));
            }
            // imageData[dy * width + dx] += color * exp(-sqrt(squaredDistance) / glow_length / (conversion_factor));
            // imageData[dy * width + dx] = imageData[dy * width + dx] / 2.0f;
        }
    }
}

float ImageGenerator::gauss(float x, float y, float sigma) {
    return std::exp(-(x * x + y * y) / (2 * sigma * sigma));
}

void ImageGenerator::saveImage(const std::string& filename, const Vector3f& color) {
    std::vector<uint8_t> bmpData(width * height * 3);
    // normalize(); // Normalize the image data before saving

    Vector3f avg_color = Vector3f(0.0f,0.0f,0.0f);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Vector3f c = color*alpha[y * width + x];
            avg_color += c;
            bmpData[(y * width + x) * 3 + 0] = static_cast<uint8_t>(std::min(255.0f, c.x() * 255.0f));
            bmpData[(y * width + x) * 3 + 1] = static_cast<uint8_t>(std::min(255.0f, c.y() * 255.0f));
            bmpData[(y * width + x) * 3 + 2] = static_cast<uint8_t>(std::min(255.0f, c.z() * 255.0f));
        }
    }
    avg_color = avg_color/(height*width);
    std::cout << "Avg Color: (" << avg_color.x() << "," << avg_color.y() << "," << avg_color.z() << ")" << std::endl;
    // Save the image as a BMP file
    enum save_bmp_result result = save_bmp(filename.c_str(), width, height, bmpData.data());
    // Check the result of saving the BMP file
    if (result != SAVE_BMP_SUCCESS) {
        std::cerr << "Error saving image: " << result << std::endl;
        std::cerr << save_bmp_str_result(result) << std::endl;
        return;
    }

}

void ImageGenerator::set_debug_mode(bool mode) {
    debug_mode = mode;
    if (debug_mode) {
        max_radius = 1;
        max_line_distance = 1.0f;
    } else {
        max_radius = 7*conversion_factor;
        max_line_distance = 7.0f*conversion_factor;
    }
}