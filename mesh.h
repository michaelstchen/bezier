#ifndef MESH_H
#define MESH_H

#include <vector>

#define degToRad(x) x*(3.141592f/180.0f)
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

bool loadVertices(char* filename,
                  std::vector<glm::vec3> & out_vertices,
                  std::vector<glm::vec3> & out_normals);

#endif
