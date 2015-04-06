#include <GL/glut.h>
#include <GL/glu.h>

//****************************************************
// Global Variables
//****************************************************

// Initalize the viewport size
int width = 500;
int height = 500;


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  width = w;
  height = h;

  glViewport (0,0,width,height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, width, 0, height);

}


//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

    glClear(GL_COLOR_BUFFER_BIT);    // clear the color buffer
    glMatrixMode(GL_MODELVIEW);      // specifying camera transforms

    glLoadIdentity();	             // make sure transformation is "zero'd"


    // Start drawing
    glBegin(GL_POINTS);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            glColor3f((GLfloat) 0.2, (GLfloat) 0.0, (GLfloat) 0.2);
            glVertex2f(x + 0.5, y + 0.5);
        }
    }
    glEnd();

    glFlush();
    glutSwapBuffers();	             // swap buffers (double buffer)
}


//****************************************************
// our program start point
//****************************************************
int main(int argc, char *argv[]) {

    //This initializes glut
    glutInit(&argc, argv);

    //This tells glut to use a double-buffered window with red, green, and blue channels 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    //The size and position of the window
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0,0);
    glutCreateWindow(argv[0]);

    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);	

    glutMainLoop();				   

    return 0;

}
