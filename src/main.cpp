#include <iostream>
#include "Camera.h"
#include "ImageGenerator.h"
#include "LineSet.h"
#include <iomanip>
#include <sstream>
#include <string>
#include "Animator.h"
#include "Scene.h"

std::string snprint_to_string(int data) {
  char buffer[6];
  snprintf(buffer, 6, "%05d", data);
  return buffer;
}

int main(int argc, char** argv)
{
    std::cout << "Platonic Animation Application" << std::endl;

    std::string scene_name; // Default scene

    if (argc > 1) {
        scene_name = argv[1];
    } else {
        std::cerr << "No scene name provided." << std::endl;
        return 1;
    }

    Scene scene(scene_name);

    scene.animate();




    // ImageGenerator imageGen(imageWidth, imageHeight);
    // // LineSet test_set;
    // // test_set.addLine(Line(Vector2f(0, 0), Vector2f(3, 0)));
    // // test_set.addLine(Line(Vector2f(3, 0), Vector2f(3, 3)));
    // // std::vector<Vector2i> mask = imageGen.getMask(test_set);

    // // std::cout << mask.size() << std::endl;
    // // for (size_t i = 0; i < mask.size(); i++)
    // // {
    // //   Vector2i pixel = mask[i];
    // //   std::cout << "Mask: (" << pixel.x() << ", " << pixel.y() << ")" << std::endl;
    // // }
    


    // // Initialize and use the Camera class here
    // Camera camera(imageWidth, imageHeight);
    // Object square = Object::getSquare();
    // int fps = 30;
    // Animator animator("Square", Vector3f(1.0f, 0.0f, 0.0f), camera, imageGen, square, fps);
    // std::cout << "Animator created with name: " << animator.get_name() << std::endl;
    // Keyframe frame1;
    // frame1.time = 0.0f;
    // frame1.t = 0.0f;
    // frame1.length = 0.3f;
    // Keyframe frame2 = frame1;
    // frame2.time = 1.0f;
    // frame2.t = 1.0f;
    // frame2.length = 0.3f;
    // Keyframe frame3 = frame2;
    // frame3.time = 4.0f;
    // frame3.t = 10.0f;
    // frame3.length = 1.0f;
    // frame3.decay_length *= 10.0f;

    // animator.add_keyframe(frame1);
    // animator.add_keyframe(frame2);
    // animator.add_keyframe(frame3);

    // // animator.save_keyframes("keyframes.txt");
    // animator.load_keyframes("keyframes.txt");

    // // system("rm -r imgs");
    // // system("mkdir imgs");
    // animator.animate("imgs/animation_output");
    // std::stringstream ss;
    // ss << "ffmpeg.exe -framerate " << fps << " -i imgs/animation_output_%05d.bmp -c:v libx264 -preset slow -crf 15 -pix_fmt yuv420p -movflags +faststart animation_output.mp4 -y";
    
    // // High-quality video encoding with ffmpeg
    // system(ss.str().c_str());


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
