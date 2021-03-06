//***************************************************
// Author: Michael Stephen Chen
//
// Description:
//    our windows callback functions detailing how to respond
//    to user inputs for translation, rotation, and zoom, etc. for
//    the object displayed by our scene.
//
//****************************************************



#include <stdlib.h>
#include <stdio.h>

#include <GL/glut.h>

#include "keylistener.h"

bool wireframemode = false;
bool flatshading = false;

vec3 camPos = vec3(0, 0, -5);
vec3 origin = vec3(0, 0, 0);
vec3 up = vec3(0,1,0);
vec3 right = glm::normalize(glm::cross(origin - camPos, up)); 

mat4 viewMat = glm::lookAt(camPos, origin, up);

mat4 modelMat = mat4(1.0f);

void normalKeys(unsigned char key, int x, int y) {
    if (key == 27) {
        exit(0);
    } else if (key == 61) {
        // '+' == 43, zoom in
        modelMat = glm::scale(modelMat, vec3(1.01, 1.01, 1.01));
    } else if (key == 45) {
        // '-' == 45, zoom out
        modelMat = glm::scale(modelMat, vec3(0.99, 0.99, 0.99));
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
            viewMat = glm::translate(viewMat, 0.1f * right);
        } else {
            modelMat = glm::rotate(mat4(1.0f), degToRad(2.0f), up) * modelMat;
        }
    } else if (key == GLUT_KEY_LEFT) {
        if (mod == GLUT_ACTIVE_SHIFT) {
            viewMat = glm::translate(viewMat, -0.1f * right);
        } else {
            modelMat = glm::rotate(mat4(1.0f), degToRad(-2.0f), up) * modelMat;
        }
    } else if (key == GLUT_KEY_UP) {
        if (mod == GLUT_ACTIVE_SHIFT) {
            viewMat = glm::translate(viewMat, 0.1f * up);
        } else {
            modelMat = glm::rotate(mat4(1.0f), degToRad(-2.0f), right) * modelMat;
        }
    } else if (key == GLUT_KEY_DOWN) {
        if (mod == GLUT_ACTIVE_SHIFT) {
            viewMat = glm::translate(viewMat, -0.1f * up);
        } else {
            modelMat = glm::rotate(mat4(1.0f), degToRad(2.0f), right) * modelMat;
        }
    }
}

glm::mat4 getModelMat() {
    return modelMat;
}

glm::mat4 getViewMat() {
    return viewMat;
}

bool isWireFrame() {
    return wireframemode;
}

bool isFlatShade() {
    return flatshading;
}
