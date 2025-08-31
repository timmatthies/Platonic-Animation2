#include "Object.h"
#include <cmath>

using namespace Eigen;


Object::Object() {
    // Initialize the object with some default points. The points define a path of a triangle.
    setPosition(Vector3f(0.0f, 0.0f, 0.0f),0.0f,0.0f);
    setRotation(Vector3f(0.0f, 1.0f, 0.0f), 0.0f); // No rotation by default
    
}

Object Object::getCircle() {
    Object circle;
    // Define a path with points of a circle
    const int num_points = 100; // Number of points to approximate the circle
    for (int i = 0; i < num_points; ++i) {
        float angle = 2.0f * M_PI * i / num_points;
        circle.points.push_back(Vector3f(std::cos(angle), std::sin(angle), 0.0f));
    }
    return circle;
}

Object Object::getThetra() {
    Object thetra;
    float a = 4.0f/std::sqrt(6.0f); // Length of the edges
    float h_d = std::sqrt(3.0f) / 2.0f * a; // Height of the triangle base
    float h_p = std::sqrt(2.0f / 3.0f) * a; // Height of the pyramid
    thetra.points = {
        Vector3f(-0.5f*a, 0.0f, 0.0f), //A
        Vector3f(0.5f*a, 0.0f, 0.0f), //B
        Vector3f(0.0f, h_d, 0.0f),    //C
        Vector3f(-0.5f*a, 0.0f, 0.0f),//A
        Vector3f(0.0f, 1.0f/3.0f*h_d, h_p), // D
        Vector3f(0.0f, h_d, 0.0f), //C
        Vector3f(0.5f*a, 0.0f, 0.0f), //B
        Vector3f(0.0f, 1.0f/3.0f*h_d, h_p), //D
    };
    
    Vector3f center(0.0f, 1.0f/3.0f*h_d, h_p/4.0f);
    for (size_t i = 0; i < thetra.points.size(); i++)
    {
        thetra.points[i] -= center;
    }

    for (size_t i = 0; i < thetra.points.size(); i++)
    {
        thetra.points[i] = thetra.points[i].normalized();
    }

    return thetra;
}

Object Object::getObjectByName(const std::string& name) {
    std::string lower_case_name = name;
    std::transform(lower_case_name.begin(), lower_case_name.end(), lower_case_name.begin(), ::tolower);
    if (lower_case_name == "cube") {
        return getCube();
    } else if (lower_case_name == "square") {
        return getPolygon(4, true);
    }  else if (lower_case_name == "triangle") {
        return getPolygon(3, true);
    }  else if (lower_case_name == "pentagon") {
        return getPolygon(5, true);
    } else if (lower_case_name == "hexagon") {
        return getPolygon(6, true);
    }  else if (lower_case_name == "septagon") {
        return getPolygon(7, true);
    }  else if (lower_case_name == "circle") {
        return getCircle();
    } else if (lower_case_name == "thetra") {
        return getThetra();
    } else if (lower_case_name == "ico") {
        return getIco();
    } else if (lower_case_name == "dodeca") {
        return getDodeca();
    } else if (lower_case_name == "octa") {
        return getOcta();
    } else {
        throw std::invalid_argument("Unknown object name: " + name);
    }
}

Object Object::getCube() {
    Object cube;
    // Define a path with the 8 vertices of a cube
    float a = 1/3.0f;
    
    cube.points = {
        Vector3f(-a, a, -a  ),
        Vector3f( a, a, -a  ),
        Vector3f( a,-a, -a  ),
        Vector3f( a,-a,  a ),
        Vector3f( a, a,  a ),
        Vector3f(-a, a,  a  ),
        Vector3f(-a,-a,  a  ),
        Vector3f(-a,-a, -a  ),
        Vector3f(-a, a, -a  ), // Closing the cube path the first time
        Vector3f(-a, a,  a  ),
        Vector3f(-a,-a,  a  ),
        Vector3f( a,-a,  a   ),
        Vector3f( a, a,  a   ),
        Vector3f( a, a, -a  ),
        Vector3f( a,-a, -a  ),
        Vector3f(-a,-a, -a  ) // Closing the cube path the second time
    };

    for (size_t i = 0; i < cube.points.size(); i++)
    {
        cube.points[i] = cube.points[i].normalized();
    }

    return cube;
}

Object Object::getSquare() {
    Object square;
    // Define a path with the 4 vertices of a square
    square.points = {
        Vector3f(-1.0f, -1.0f, 0.0f),
        Vector3f(1.0f,  -1.0f, 0.0f),
        Vector3f(1.0f, 1.0f, 0.0f),
        Vector3f(-1.0f, 1.0f, 0.0f),
    };

    for (size_t i = 0; i < square.points.size(); i++)
    {
        square.points[i] = square.points[i].normalized();
    }

    return square;
}

Object Object::getPolygon(int n_sides, bool origin_lower) {
    Object polygon;
    float angle_step = 2 * M_PI / n_sides;
    float phi = M_PI / n_sides - M_PI / 2.0f;
    for (int i = 0; i < n_sides; ++i) {
        float angle = i * angle_step + phi;
        float x = std::cos(angle);
        float y = -std::sin(angle);
        polygon.points.push_back(Vector3f(x, y, 0.0f));
    }
    float side_length = (polygon.points[0] - polygon.points[1]).norm();
    for (size_t i = 0; i < polygon.points.size(); i++)
    {
        polygon.points[i] = polygon.points[i]/side_length;
    }
    if (origin_lower) {
        Vector3f m = (polygon.points[0] + polygon.points[n_sides-1]) / 2.0f;
        for (size_t i = 0; i < polygon.points.size(); i++)
        {
            polygon.points[i] -= m;
        }
    }
    
    return polygon;
}

Vector3f Object::getPositionAtTime(float time) const {
    int startIndex = static_cast<int>(time) % points.size();
    int endIndex = (startIndex + 1) % points.size();
    float t = time - static_cast<int>(time); // Fractional part of time
    return rotation_matrix*(points[startIndex] * (1 - t) + points[endIndex] * t) + position;
}

std::vector<Vector3f> Object::getCurrentPoints(float time, float history_length, float history_spacing) {
    // Returns the current position and a few of the previous positions in the past.
    int numPoints = static_cast<int>(std::ceil(history_length / history_spacing));
    std::vector<Vector3f> currentPoints;
    currentPoints.reserve(numPoints);

    for (int i = 0; i < numPoints; ++i) {
        float t = time - i * history_spacing;
        if (t < 0) break;
        currentPoints.push_back(getPositionAtTime(t));
    }

    return currentPoints;
}


void Object::setPosition(const Vector3f& position, const float& r, const float& phi) {
    this->polar_phi = phi;
    this->polar_r = r;
    this->position = Vector3f(position.x(), position.y(), position.z()) + r*Vector3f(std::cos(phi), std::sin(phi), 0.0f);
}

void Object::setRotation(const Vector3f& axis, float angle) {
    rotation_axis = axis;
    rotation_angle = angle;
    rotation_matrix = AngleAxisf(angle, axis.normalized()).toRotationMatrix();
}

void Object::setScale(const Vector3f& scale) {
    scale_matrix = Matrix3f::Identity();
    scale_matrix(0, 0) = scale.x();
    scale_matrix(1, 1) = scale.y();
    scale_matrix(2, 2) = scale.z();
}

Object Object::getIco() {
    Object obj;
    // Icosahedron vertices
    float phi = (1.0 + sqrt(5.0)) / 2.0;

    std::vector<Vector3f> p = {
        Vector3f(-1,  phi, 0), Vector3f(1,  phi, 0), Vector3f(-1, -phi, 0), Vector3f(1, -phi, 0),
        Vector3f(0, -1,  phi), Vector3f(0, 1,  phi), Vector3f(0, -1, -phi), Vector3f(0, 1, -phi),
        Vector3f( phi, 0, -1), Vector3f( phi, 0, 1), Vector3f(-phi, 0, -1), Vector3f(-phi, 0, 1)
    };
    obj.points.clear();
    for (size_t i = 0; i < p.size(); i++)
    {
        p[i] = p[i].normalized();
    }
    // ABHAKHGKCGHIGCDGIBJIDJEDCEFJBFAFLECLAKL
    std::string path_code = "ABHAKHGKCGHIGCDGIBJIDJEDCEFJBFAFLECLAKL";
    for (size_t i = 0; i < path_code.size(); i++)
    {
        obj.points.push_back(p[path_code[i] - 'A']);
    }
    return obj;
}

Object Object::getDodeca() {
    Object obj;
    // Dodecahedron vertices
    float phi = (1.0 + sqrt(5.0)) / 2.0;
    float a = 1.0f, c = 1.0f / phi, b = phi;
    std::vector<Vector3f> p = {
        Vector3f( a,  a,  a), Vector3f( a,  a, -a), Vector3f( a, -a,  a), Vector3f( a, -a, -a),
        Vector3f(-a,  a,  a), Vector3f(-a,  a, -a), Vector3f(-a, -a,  a), Vector3f(-a, -a, -a),
        Vector3f( 0,  b,  c), Vector3f( 0,  b, -c), Vector3f( 0, -b,  c), Vector3f( 0, -b, -c),
        Vector3f( b,  c, 0), Vector3f( b, -c, 0), Vector3f(-b,  c, 0), Vector3f(-b, -c, 0),
        Vector3f( c, 0,  b), Vector3f(-c, 0,  b), Vector3f( c, 0, -b), Vector3f(-c, 0, -b)
    };
    // AIJBMA QREI JFOE RGPO FTHP GKLHTSBMNDSDLKCQCNM
    std::string path_code = "AIJBMAQREIJFOERGPOFTHPGKLHTSBMNDSDLKCQCNM";
    obj.points.clear();
    for (size_t i = 0; i < path_code.size(); i++)
    {
        obj.points.push_back(p[path_code[i] - 'A']);
    }
    for (size_t i = 0; i < obj.points.size(); i++)
    {
        obj.points[i] = obj.points[i].normalized();
    }
    return obj;
}

Object Object::getOcta() {
    Object obj;
    float a = std::sqrt(2.0f); // Length of the edges
    // Octahedron vertices
    obj.points.clear();
    std::vector<Vector3f> p = {
        Vector3f( 0.5f*a, 0.0f, 0.5f*a), //A
        Vector3f(-0.5f*a, 0.0f, 0.5f*a), //B
        Vector3f(-0.5f*a,  0.0f, -0.5f*a), //C
        Vector3f(0.5f*a, 0.0f, -0.5f*a), //D
        Vector3f(0.0f, std::sqrt(0.5f)*a, 0.0f), //E
        Vector3f(0.0f, -std::sqrt(0.5f)*a, 0.0f), //F
    };
    //ABCDECFD
    obj.points = {
        p[0], p[1], p[2], p[3], // Base square
        p[4], p[2], p[5], p[3], // Side triangles
        p[0], p[5], p[1], p[4]  // Other triangles
    };

    for (size_t i = 0; i < obj.points.size(); i++)
    {
        obj.points[i] = obj.points[i].normalized();
    }
    

    return obj;
}
