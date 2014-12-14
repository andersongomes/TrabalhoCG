#include <GL/glut.h>
#include <stdlib.h>
#include <math.h> 
#include <stdio.h>
#include <Windows.h>
#include <iostream>

using namespace std; 
 
// The next global variable controls the animation's state and speed.
float RotateAngle = 0.0f; // Angle in degrees of rotation around y-axis
float Azimuth = 20.0; // Rotated up or down by this amount
float AngleStepSize = 3.0f; // Step three degrees at a time
const float AngleStepMax = 10.0f;
const float AngleStepMin = 0.1f;
int WireFrameOn = 1; // == 1 for wire frame mode
GLUquadricObj* myReusableQuadric = 0;
 
void drawGluSlantCylinder(double height, double radiusBase, double radiusTop, int slices, int stacks) {
    if (!myReusableQuadric) {
        myReusableQuadric = gluNewQuadric();
        // Should (but don't) check if pointer is still null --- to catch memory allocation errors.
        gluQuadricNormals(myReusableQuadric, GL_TRUE);
    }
    // Draw the cylinder.
    gluCylinder(myReusableQuadric, radiusBase, radiusTop, height, slices, stacks);
}
 
void drawGluCylinder(double height, double radius, int slices, int stacks) {
    drawGluSlantCylinder(height, radius, radius, slices, stacks);
}
 
void drawGluSlantCylinderWithCaps(double height, double radiusBase, double radiusTop, int slices, int stacks) {
    // First draw the cylinder
    drawGluSlantCylinder(height, radiusBase, radiusTop, slices, stacks);
    // Draw the top disk cap
    glPushMatrix();
    glTranslated(0.0, 0.0, height);
    gluDisk(myReusableQuadric, 0.0, radiusTop, slices, stacks);
    glPopMatrix();
    // Draw the bottom disk cap
    glPushMatrix();
    glRotated(180.0, 1.0, 0.0, 0.0);
    gluDisk(myReusableQuadric, 0.0, radiusBase, slices, stacks);
    glPopMatrix();
}
 
void drawGluCylinderWithCaps(double height, double radius, int slices, int stacks) {
    drawGluSlantCylinderWithCaps(height, radius, radius, slices, stacks);
}
 
// glutKeyboardFunc is called below to set this function to handle
// all "normal" key presses.
void myKeyboardFunc(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
        WireFrameOn = 1 - WireFrameOn;
        if (WireFrameOn) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Just show wireframes
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Show solid polygons
        }
        glutPostRedisplay();
        break;
        case 'R':
        AngleStepSize *= 1.5;
        if (AngleStepSize>AngleStepMax) {
            AngleStepSize = AngleStepMax;
        }
        break;
        case 'r':
        AngleStepSize /= 1.5;
        if (AngleStepSize<AngleStepMin) {
            AngleStepSize = AngleStepMin;
        }
        break;
        case 27: // Escape key
        exit(1);
    }
}
 
// glutSpecialFunc is called below to set this function to handle
// all "special" key presses. See glut.h for the names of
// special keys.
void mySpecialKeyFunc(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
        Azimuth += AngleStepSize;
        if (Azimuth>180.0f) {
            Azimuth -= 360.0f;
        }
        break;
        case GLUT_KEY_DOWN:
        Azimuth -= AngleStepSize;
        if (Azimuth < -360.0f) {
            Azimuth += -360.0f;
        }
        break;
        case GLUT_KEY_LEFT:
        RotateAngle += AngleStepSize;
        if (RotateAngle > 180.0f) {
            RotateAngle -= 360.0f;
        }
        break;
        case GLUT_KEY_RIGHT:
        RotateAngle -= AngleStepSize;
        if (RotateAngle < -180.0f) {
            RotateAngle += 360.0f;
        }
        break;
    }
    glutPostRedisplay();
}
 
// Desenha a splash screen 
void drawSplashScreen(void) {
    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f (1.0, 1.0, 1.0);
    // Draw white polygon (rectangle) with
    // corners at (0.25, 0.25, 0.0)
    // and (0.75, 0.75, 0.0)
    glBegin(GL_POLYGON);
    glVertex3f (0.25, 0.25, 0.0);
    glVertex3f (0.75, 0.25, 0.0);
    glVertex3f (0.75, 0.75, 0.0);
    glVertex3f (0.25, 0.75, 0.0);
    glEnd();
    // Don't wait!
    // Start processing buffered routines
    glFlush ();    
    glutSwapBuffers();     
} 

 
/*
* drawScene() handles the animation and the redrawing of the
* graphics window contents.
*/
void drawScene(void) {
    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Rotate the image
    glMatrixMode(GL_MODELVIEW); // Current matrix affects objects positions
    glLoadIdentity(); // Initialize to the identity
    glTranslatef(-0.5, 0.0, -35.0); // Translate from origin (in front of viewer)
    glRotatef(RotateAngle, 0.0, 1.0, 0.0); // Rotate around y-axis
    glRotatef(Azimuth, 1.0, 0.0, 0.0); // Set Azimuth angle
    glDisable(GL_CULL_FACE);
    glPushMatrix();
    glTranslatef(1.5, 0.0, 0.0);
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glColor3f(1.0, 0.4, 0.2); // Reddish color
    // Parameters: height, radius, slices, stacks
    drawGluCylinder(3.0, 0.1, 10, 10);
    glPopMatrix();
    glEnable(GL_CULL_FACE);
    glPushMatrix();
    glTranslatef(1.5, 3.0, 0.0);
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glColor3f(0.2, 1.0, 0.2); // Greenish color
    // Parameters: height, base radius, top radius, slices, stacks
    drawGluSlantCylinderWithCaps(0.6, 0.2, 0.0, 10, 10);
    glPopMatrix();
 
 
    // Segundo Vetor
 
    glTranslatef(1.5, 0.0, 0.0);
    glRotatef(-45.0, 1.0, 0.0, 0.0);
    glColor3f(1.5, 1.2, 0.2); // Amarelo
    // Parameters: height, radius, slices, stacks
    drawGluCylinder(3.0, 0.1, 10, 10);
    glPopMatrix();
    glEnable(GL_CULL_FACE);
    glPushMatrix();
    glTranslatef(0.0, 0.0, 3.0);
    glRotatef(0.0, 1.0, 0.0, 0.0);
    glColor3f(0.2, 1.0, 0.2); // Greenish color
    // Parameters: height, base radius, top radius, slices, stacks
    drawGluSlantCylinderWithCaps(0.6, 0.2, 0.0, 10, 10);
    glPopMatrix();
 
 
    // Terceiro Vetor
 
    glTranslatef(0.0, 0.0, 0.0);
    glRotatef(-75.0, 0.0, 10.0, 0.0);
    glColor3f(0.0, 0.2, 0.7); // Amarelo
    // Parameters: height, radius, slices, stacks
    drawGluCylinder(3.0, 0.1, 10, 10);
    glPopMatrix();
    glEnable(GL_CULL_FACE);
    glPushMatrix();
    glTranslatef(0.0, 0.0, 3.0);
    glRotatef(0.0, 1.0, 0.0, 0.0);
    glColor3f(0.2, 1.0, 0.2); // Greenish color
    // Parameters: height, base radius, top radius, slices, stacks
    drawGluSlantCylinderWithCaps(0.6, 0.2, 0.0, 10, 10);
    glPopMatrix();  
 
    // Flush the pipeline, swap the buffers
    glFlush();
    glutSwapBuffers();
}
 
// Initialize OpenGL's rendering modes
void initRendering() {
    glEnable(GL_DEPTH_TEST); // Depth testing must be turned on
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Just show wireframes at first
}
  
void menu(int item) {
    switch (item) {
        case 1:
             cout << "1" ;
             glClear(GL_COLOR_BUFFER_BIT);
             break;
        case 2:
             cout << "2" ;
             glutCreateWindow("Schmidt");
             break;
        case 3:
             cout << "3" ;
             exit(0);
             break;
        default :
             break;
    } 
    glutPostRedisplay();
    return;
}
// Called when the window is resized
// w, h - width and height of the window in pixels.
void resizeWindow(int w, int h) {
    double aspectRatio;
    // Define the portion of the window used for OpenGL rendering.
    glViewport(0, 0, w, h); // View port uses whole window
    // Set up the projection view matrix: perspective projection
    // Determine the min and max values for x and y that should appear in the window.
    // The complication is that the aspect ratio of the window may not match the
    // aspect ratio of the scene we want to view.
    w = (w == 0) ? 1 : w;
    h = (h == 0) ? 1 : h;
    aspectRatio = (double)w / (double)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(15.0, aspectRatio, 25.0, 45.0);
}
 
// Main routine
// Set up OpenGL, define the callbacks and start the main loop
int main(int argc, char** argv) {
    
    
    glutInit(&argc, argv);
    // We're going to animate it, so double buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // Window position (from top corner), and size (width% and hight)
    glutInitWindowPosition(200, 60);
    // Tamanho da Janela
    glutInitWindowSize(800, 600);
    // Título da Janela
    glutCreateWindow("Gram Schmidt");
    
    // Criar menu acessível através do botão direito do mouse
    int menuRightButton = glutCreateMenu(menu);
    glutAddMenuEntry("Visualizar Informações", 1);
    glutAddMenuEntry("Iniciar Animação", 2);
    glutAddMenuEntry("Fechar Aplicação", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    // Initialize OpenGL as we like it..
    initRendering();
    // Set up callback functions for key presses
    glutKeyboardFunc(myKeyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(mySpecialKeyFunc); // Handles "special" keyboard keys
    // Set up the callback function for resizing windows
    glutReshapeFunc(resizeWindow);
    // Call this for background processing
    // glutIdleFunc( myIdleFunction );
    // call this whenever window needs redrawing
    //glutDisplayFunc(drawSplashScreen);
    glutDisplayFunc(drawScene);
    
    
    fprintf(stdout, "Arrow keys control viewpoint.n");
    fprintf(stdout, "Press \"w\" to toggle wireframe mode.n");
    fprintf(stdout, "Press \"R\" or \"r\" to increase or decrease rate of movement (respectively).n");
    // Start the main loop. glutMainLoop never returns.
    glutMainLoop();
    return(0); // This line is never reached.
}
