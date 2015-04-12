#include <stdlib.h>
#include <stdio.h>

#include <GL/glut.h>

#include "input_utils.h"

bool wireframemode = false;
bool flatshading = false;

mat4 modelMat = mat4(1.0f);

void normalKeys(unsigned char key, int x, int y) {
    if (key == 27) {
        exit(0);
    } else if (key == 61) {
        // '+' == 43, zoom in
        modelMat = scale(modelMat, vec3(1.01, 1.01, 1.01));
    } else if (key == 45) {
        // '-' == 45, zoom out
        modelMat = scale(modelMat, vec3(0.99, 0.99, 0.99));
    } else if (key == 99) {
        wireframemode = !wireframemode;
    } else if (key == 115) {
        flatshading = !flatshading;
    }
}

void specialKeys(int key, int x, int y) {
    int mod = glutGetModifiers();

    if (key == GLUT_KEY_RIGHT) {
        if (mod == GLUT_ACTIVE_SHIFT) {

        } else {

        }
    } else if (key == GLUT_KEY_LEFT) {
        if (mod == GLUT_ACTIVE_SHIFT) {

        } else {

        }
    } else if (key == GLUT_KEY_UP) {
        if (mod == GLUT_ACTIVE_SHIFT) {

        } else {

        }
    } else if (key == GLUT_KEY_DOWN) {
        if (mod == GLUT_ACTIVE_SHIFT) {

        } else {

        }
    }
}

glm::mat4 getModelMat() {
    return modelMat;
}

bool isWireFrame() {
    return wireframemode;
}

bool isFlatShade() {
    return flatshading;
}
