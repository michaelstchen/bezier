#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include "shader.h"

//****************************************************
// Global variables
//****************************************************
int windowWidth = 500;
int windowHeight = 500;

GLuint programID;
GLuint VertexArrayID;
GLuint vertexbuffer;


//****************************************************
// Callback Functions
//****************************************************
void renderScene() {

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(programID);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 3); //3 indices starting at 0->1 triangle

    glDisableVertexAttribArray(0);

    // swap buffers
    glutSwapBuffers();
}


void windowResize(int w, int h) {
    // check for division by zero
    if (h == 0) h = 1;
    float ratio = (float) w / (float) h;

    // reset our current projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // reset the viewport dimensions
    glViewport(0,0,w,h);
    // set perspective (fov angle, aspect ratio, zNear, zFar)
    gluPerspective(45, ratio, 1, 1000);

    // return to the model view (for camera and transforming objs)
    glMatrixMode(GL_MODELVIEW);
}


//****************************************************
// Program Start Point
//****************************************************
int main(int argc, char **argv) {

    // initiate GLUT and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE );
    glutInitContextFlags(GLUT_DEBUG);

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("CS184 Assignement 3: Bezier Surfaces");

    // register callbacks
    glutDisplayFunc(renderScene);
    glutReshapeFunc(windowResize);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();
 
    // print context information
    printf("Vendor: %s\n", glGetString (GL_VENDOR));
    printf("Renderer: %s\n", glGetString (GL_RENDERER));
    printf("Version: %s\n", glGetString (GL_VERSION));
    printf("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    static const GLfloat vertex_buffer_data[] = { 
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
    };

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

    // enter GLUT event processing loop
    glutMainLoop();

    return 1;

}
