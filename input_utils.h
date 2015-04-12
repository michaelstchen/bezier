#ifndef INPUT_UTILS_H
#define INPUT_UNTILS_H

#define degToRad(x) x*(3.141592f/180.0f)
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

void normalKeys(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);

mat4 getModelMat();

bool isWireFrame();
bool isFlatShade();

#endif