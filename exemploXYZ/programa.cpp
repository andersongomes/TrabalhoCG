#include <GL/glut.h> // Once you include glut.h (you don't need gl.h or glu.h)   


GLfloat X = 0.0f; // Translate screen to x direction (left or right)
GLfloat Y = 0.0f; // Translate screen to y direction (up or down)
GLfloat Z = 0.0f; // Translate screen to z direction (zoom in or out)
GLfloat rotX = 0.0f; // Rotate screen on x axis 
GLfloat rotY = 0.0f; // Rotate screen on y axis
GLfloat rotZ = 0.0f; // Rotate screen on z axis
GLfloat rotLx = 0.0f; // Translate screen by using the glulookAt function (left or right)
GLfloat rotLy = 0.0f; // Translate screen by using the glulookAt function (up or down)
GLfloat rotLz = 0.0f; // Translate screen by using the glulookAt function (zoom in or out)

void glDisplayLines(void); // Did declare the function 
            // so I did not have to check for order of the functions

// Initialize the OpenGL window
void init(void)
{
    glClearColor (0.0, 0.0, 0.0, 0.0); // Clear the color 
    glShadeModel (GL_FLAT); // Set the shading model to GL_FLAT
    glEnable (GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // Set Line Antialiasing
}

// Draw the lines (x,y,z)
void display(void)
{ 
    glClear (GL_COLOR_BUFFER_BIT); // Clear the Color Buffer 
    glPushMatrix();     // It is important to push the Matrix before calling 
            // glRotatef and glTranslatef
    glRotatef(rotX,1.0,0.0,0.0); // Rotate on x
    glRotatef(rotY,0.0,1.0,0.0); // Rotate on y
    glRotatef(rotZ,0.0,0.0,1.0); // Rotate on z
    glTranslatef(X, Y, Z);  // Translates the screen left or right, 
            // up or down or zoom in zoom out
    // Draw the positive side of the lines x,y,z
    glBegin(GL_LINES);
    glColor3f (0.0, 1.0, 0.0); // Green for x axis
    glVertex3f(0,0,0);
    glVertex3f(10,0,0);
    glColor3f(1.0,0.0,0.0); // Red for y axis
    glVertex3f(0,0,0);
    glVertex3f(0,10,0);
    glColor3f(0.0,0.0,1.0); // Blue for z axis
    glVertex3f(0,0,0); 
    glVertex3f(0,0,10);
    glEnd();

    // Dotted lines for the negative sides of x,y,z
    glEnable(GL_LINE_STIPPLE);  // Enable line stipple to use a 
                // dotted pattern for the lines
    glLineStipple(1, 0x0101);   // Dotted stipple pattern for the lines
    glBegin(GL_LINES); 
    glColor3f (0.0, 1.0, 0.0);  // Green for x axis
    glVertex3f(-10,0,0);
    glVertex3f(0,0,0);
    glColor3f(1.0,0.0,0.0);     // Red for y axis
    glVertex3f(0,0,0);
    glVertex3f(0,-10,0);
    glColor3f(0.0,0.0,1.0);     // Blue for z axis
    glVertex3f(0,0,0);
    glVertex3f(0,0,-10);
    glEnd();
    glDisable(GL_LINE_STIPPLE);     // Disable the line stipple
    glPopMatrix();      // Don't forget to pop the Matrix
    glutSwapBuffers();
}

// This function is called whenever the window size is changed
void reshape (int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h); // Set the viewport
    glMatrixMode (GL_PROJECTION);   // Set the Matrix mode
    glLoadIdentity (); 
    gluPerspective(75, (GLfloat) w /(GLfloat) h , 0.10, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (rotLx, rotLy, 15.0 + rotLz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); 
}

// This function is used for the navigation keys
void keyboard (unsigned char key, int x, int y)
{
switch (key) {   // x,X,y,Y,z,Z uses the glRotatef() function
    case 'x': // Rotates screen on x axis 
    rotX -= 0.5f;
    break;
    case 'X': // Opposite way 
    rotX += 0.5f;
    break;
    case 'y': // Rotates screen on y axis
    rotY -= 0.5f;
    break;
    case 'Y': // Opposite way
    rotY += 0.5f; 
    break; 
    case 'z': // Rotates screen on z axis
    rotZ -= 0.5f;
    break;
    case 'Z': // Opposite way
    rotZ += 0.5f;
    break;
    // j,J,k,K,l,L uses the gluLookAt function for navigation
    case 'j':
    rotLx -= 0.2f; 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (rotLx, rotLy, 15.0 + rotLz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    break;
    case 'J':
    rotLx += 0.2f;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (rotLx, rotLy, 15.0 + rotLz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    break; 
    case 'k':
    rotLy -= 0.2f;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (rotLx, rotLy, 15.0 + rotLz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    break;
    case 'K':
    rotLy += 0.2f;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (rotLx, rotLy, 15.0 + rotLz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    break;
    case 'l':   // It has a special case when the rotLZ becomes 
        // less than -15 the screen is viewed from the opposite side
    // therefore this if statement below does not allow rotLz be less than -15
    if(rotLz + 14 >= 0)
    rotLz -= 0.2f;           
    glMatrixMode(GL_MODELVIEW);    
    glLoadIdentity();  
    gluLookAt (rotLx, rotLy, 15.0 + rotLz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    break;
    case 'L':
    rotLz += 0.2f;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (rotLx, rotLy, 15.0 + rotLz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    break;
    case 'b': // Rotates on x axis by -90 degree
    rotX -= 90.0f;
    break;
    case 'B': // Rotates on y axis by 90 degree
    rotX += 90.0f; 
    break;
    case 'n': // Rotates on y axis by -90 degree
    rotY -= 90.0f;
    break;
    case 'N': // Rotates on y axis by 90 degree
    rotY += 90.0f;
    break;
    case 'm': // Rotates on z axis by -90 degree
    rotZ -= 90.0f; 
    break;
    case 'M': // Rotates on z axis by 90 degree
    rotZ += 90.0f;
    break;
    case 'o': // Default, resets the translations vies from starting view
    case 'O': 
    X = Y = 0.0f;
    Z = 0.0f;
    rotX = 0.0f;
    rotY = 0.0f;
    rotZ = 0.0f;
    rotLx = 0.0f;
    rotLy = 0.0f;
    rotLz = 0.0f;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(rotLx, rotLy, 15.0f + rotLz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);  
    break;
}
    glutPostRedisplay(); // Redraw the scene
}

// called on special key pressed
void specialKey(int key, int x, int y) { 

// The keys below are using the gluLookAt() function for navigation
// Check which key is pressed

switch(key) {
    case GLUT_KEY_LEFT : // Rotate on x axis
    X -= 0.1f;
    break;
    case GLUT_KEY_RIGHT : // Rotate on x axis (opposite)
    X += 0.1f;
    break;
    case GLUT_KEY_UP : // Rotate on y axis 
    Y += 0.1f;
    break;
    case GLUT_KEY_DOWN : // Rotate on y axis (opposite)
    Y -= 0.1f;
    break; 
    case GLUT_KEY_PAGE_UP: // Rotate on z axis
    Z -= 0.1f;
    break;
    case GLUT_KEY_PAGE_DOWN:// Rotate on z axis (opposite)
    Z += 0.1f;
    break;
}
    glutPostRedisplay(); // Redraw the scene
}

// Main entry point of the program
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);   // Setup display mode to 
                        // double buffer and RGB color
    glutInitWindowSize (600,600); // Set the screen size
    glutCreateWindow("OpenGL 3D Navigation Program"); 
    init ();
    glutReshapeFunc(reshape); 
    glutDisplayFunc(display); 
    glutKeyboardFunc(keyboard); // set window's key callback 
    glutSpecialFunc(specialKey); // set window's to specialKey callback
    glutMainLoop();

    return 0;
}
