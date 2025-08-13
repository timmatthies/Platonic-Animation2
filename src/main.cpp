#include <iostream>
#include "Camera.h"
#include "ImageGenerator.h"
#include "LineSet.h"
#include <iomanip>
#include <sstream>
#include <string>

std::string snprint_to_string(int data) {
  char buffer[6];
  snprintf(buffer, 6, "%05d", data);
  return buffer;
}

int main()
{
    std::cout << "Platonic Animation Application" << std::endl;

    int imageWidth = 100;
    int imageHeight = 100;
    ImageGenerator imageGen(imageWidth, imageHeight);
    // Initialize and use the Camera class here
    Camera camera(imageWidth, imageHeight);
    Object cube = Object::getCube();

    // cube.setRotation(Vector3f(0.0f, 1.0f, 0.0f), 1); // Rotate cube over time
    for(int i = 0; i < 30; ++i) {
      float time = i * 0.03f; // Example time step
      cube.setRotation(Vector3f(0.5f, 1.0f, 0.5f), time); // Rotate cube over time
      LineSet lineSet = camera.convert_to_lines(cube);
      imageGen.clear(); // Clear the image data before drawing
      imageGen.drawLines(lineSet, Vector3f(1.0f, 0.0f, 0.0f)); // Draw lines in red
      imageGen.saveImage("imgs/output_" + snprint_to_string(i) + ".bmp");
    }



    // std::vector<float> intensity(20, 1.0f); // Example intensity values
    // Vector3f color(1.0f, 0.0f, 0.0f); // Example color (red)



    return 0;
}
