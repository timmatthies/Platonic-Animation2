#include <iostream>
#include "Camera.h"
#include "ImageGenerator.h"
#include "LineSet.h"
#include <iomanip>
#include <sstream>
#include <string>
#include "Animator.h"

std::string snprint_to_string(int data) {
  char buffer[6];
  snprintf(buffer, 6, "%05d", data);
  return buffer;
}

int main()
{
    std::cout << "Platonic Animation Application" << std::endl;

    int imageWidth =  500;
    int imageHeight = 500;
    ImageGenerator imageGen(imageWidth, imageHeight);
    // LineSet test_set;
    // test_set.addLine(Line(Vector2f(0, 0), Vector2f(3, 0)));
    // test_set.addLine(Line(Vector2f(3, 0), Vector2f(3, 3)));
    // std::vector<Vector2i> mask = imageGen.getMask(test_set);

    // std::cout << mask.size() << std::endl;
    // for (size_t i = 0; i < mask.size(); i++)
    // {
    //   Vector2i pixel = mask[i];
    //   std::cout << "Mask: (" << pixel.x() << ", " << pixel.y() << ")" << std::endl;
    // }
    


    // Initialize and use the Camera class here
    Camera camera(imageWidth, imageHeight);
    Object square = Object::getSquare();
    Animator animator(camera, imageGen, square, 10); // 10 FPS
    Keyframe frame1;
    frame1.time = 0.0f;
    frame1.t = 0.0f;
    Keyframe frame2 = frame1;
    frame2.time = 1.0f;
    frame2.t = 1.0f;
    Keyframe frame3 = frame2;
    frame3.time = 2.0f;
    frame3.t = 3.0f;
    frame3.decay_length *= 2.0f;

    animator.add_keyframe(frame1);
    animator.add_keyframe(frame2);
    animator.add_keyframe(frame3);

    animator.save_keyframes("keyframes.txt");
    animator.load_keyframes("keyframes.txt");
    animator.animate("imgs/animation_output");
    
    // High-quality video encoding with ffmpeg
    system("ffmpeg.exe -framerate 10 -i imgs/animation_output_%05d.bmp -c:v libx264 -preset slow -crf 15 -pix_fmt yuv420p -movflags +faststart imgs/animation_output.mp4 -y");


    // Old test
    // // cube.setRotation(Vector3f(0.0f, 1.0f, 0.0f), 1); // Rotate cube over time
    // for(int i = 0; i < 100; ++i) {
    //   float time = i * 0.01f; // Example time step
    //   // cube.setRotation(Vector3f(0.5f, 1.0f, 0.5f), time); // Rotate cube over time

    //   LineSet lineSet = camera.convert_to_lines(square, time, 1.0f);
    //   imageGen.clear(); // Clear the image data before drawing
    //   imageGen.drawLines(lineSet, Vector3f(1.0f, 0.0f, 0.0f)); // Draw lines in red
    //   imageGen.saveImage("imgs/output_" + snprint_to_string(i) + ".bmp");
    //   std::cout << "Saved image: imgs/output_" << snprint_to_string(i) << ".bmp" << std::endl;
    // }



    return 0;
}
