#ifndef MESH_H
#define MESH_H

#include <vector>

#define PI 3.141592f
#define degToRad(x) x*(3.141592f/180.0f)
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
using namespace glm;

bool loadVertices(int argc, char** argv,
                  std::vector<glm::vec3> & out_vertices,
                  std::vector<glm::vec3> & out_normals);

class Point {
 public:
    Point(float x_in, float y_in, float z_in, float u_in, float v_in);
    float x; float y; float z;
    float u; float v;
};

class Triangle {
 public:
    Triangle(Point* p0, Point* p1, Point* p2);
    Point* a; Point* b; Point* c;
};


#endif
