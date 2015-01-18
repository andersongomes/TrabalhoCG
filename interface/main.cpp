#include <windows.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h> 
#include <stdio.h>
#include "util.h"

#include "vetor.h"

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
 
//Variáveis Globais 

float heightCilindro = 3.0;
float heightCone = 3.0;
float heightCone2 = 3.0;
float rotateAngle2 = -90;
float posCone = 0.0;
float corretor = 0.01;

//Vetor 1
float _xx1 = 0; 
float _yy1 = 0;
float _zz1 = 0;

//Vetor 2
float _xx2 = 0; 
float _yy2 = 0;
float _zz2 = 0;

//Vetor 3
float _xx3 = 0; 
float _yy3 = 0;
float _zz3 = 0; 

// Zoom in e out
double rotate_by_key = 0; 
double rotate_x = 1.0; 

//Métodos de execução de Gram Schimit

void gramSchimidt(Vetor v1, Vetor v2,Vetor v3){
    Vetor resultado1,resultado2;
    float numerador, numerador2,denominador, denominador2,aux1,aux2,aux3;;
    /* DESCOBRINDO O W2*/
    numerador = (v1.vetor[0]*v2.vetor[0] + v1.vetor[1]*v2.vetor[1] + v1.vetor[2]*v2.vetor[2]);
    denominador = pow(v1.vetor[0],2) + pow(v1.vetor[1],2) + pow(v1.vetor[2],2);
    resultado1.vetor[0] = v2.vetor[0] - ((numerador/denominador)*v1.vetor[0]);
    resultado1.vetor[1] = v2.vetor[1] - ((numerador/denominador)*v1.vetor[1]);
    resultado1.vetor[2] = v2.vetor[2] - ((numerador/denominador)*v1.vetor[2]);
    /* FIM DO W2 */

    /* DESCOBRINDO O W3*/
    numerador = (resultado1.vetor[0]*v3.vetor[0] + resultado1.vetor[1]*v3.vetor[1] + resultado1.vetor[2]*v3.vetor[2]);
    numerador2 = (v1.vetor[0]*v3.vetor[0] + v1.vetor[1]*v3.vetor[1] + v1.vetor[2]*v3.vetor[2]);
    denominador = pow(resultado1.vetor[0],2) + pow(resultado1.vetor[1],2) + pow(resultado1.vetor[2],2);
    denominador2 = pow(v1.vetor[0],2) + pow(v1.vetor[1],2) + pow(v1.vetor[2],2);
    aux1 = v3.vetor[0];
    aux2 = (numerador/denominador)*resultado1.vetor[0];
    aux3 = (numerador2/denominador2)*v1.vetor[0];
    resultado2.vetor[0] = aux1-aux2-aux3;
    aux1 = v3.vetor[1];
    aux2 = (numerador/denominador)*resultado1.vetor[1];
    aux3 = (numerador2/denominador2)*v1.vetor[1];
    resultado2.vetor[1] = aux1-aux2-aux3;
    aux1 = v3.vetor[2];
    aux2 = (numerador/denominador)*resultado1.vetor[2];
    aux3 = (numerador2/denominador2)*v1.vetor[2];
    resultado2.vetor[2] = aux1-aux2-aux3;
    /* FIM DO W3*/
    cout<<"v2:";
    resultado1.showVetor(2);
    cout<<"v3:";
    resultado2.showVetor(3);
}

void inicializacaoGramSchimidt(Vetor v1,Vetor v2,Vetor v3){
    cout<<"v1:";
    v1.showVetor(1);
    gramSchimidt(v1,v2,v3);
}

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


void output(float x, float y, char *string)
{
  int len, i;
  glRasterPos2f(x, y);
  len = (int) strlen(string);
  for (i = 0; i < len; i++)
  {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
  }
}
 
// Draw the lines (x,y,z)
void drawAxis(void) { 
    glPushMatrix();     // It is important to push the Matrix before calling 

    // Draw the positive side of the lines x,y,z
    glBegin(GL_LINES);
        glColor3f (0.0, 1.0, 0.0); // Green for x axis
        glVertex3f(0,0,0);
        glVertex3f(5,0,0);
        glColor3f(1.0,0.0,0.0); // Red for y axis
        glVertex3f(0,0,0);
        glVertex3f(0,5,0);
        glColor3f(0.0,0.0,1.0); // Blue for z axis
        glVertex3f(0,0,0); 
        glVertex3f(0,0,5);
    glEnd();

    // Dotted lines for the negative sides of x,y,z
    glEnable(GL_LINE_STIPPLE);  // Enable line stipple to use a 
                // dotted pattern for the lines
    glLineStipple(1, 0x0101);   // Dotted stipple pattern for the lines
    glBegin(GL_LINES); 
        glColor3f (0.0, 1.0, 0.0);  // Green for x axis
        glVertex3f(-5,0,0);
        glVertex3f(0,0,0);
        glColor3f(1.0,0.0,0.0);     // Red for y axis
        glVertex3f(0,0,0);
        glVertex3f(0,-5,0);
        glColor3f(0.0,0.0,1.0);     // Blue for z axis
        glVertex3f(0,0,0);
        glVertex3f(0,0,-10);
    glEnd();
    glDisable(GL_LINE_STIPPLE);     // Disable the line stipple
    glPopMatrix();      // Don't forget to pop the Matrix

    //desenha os cones nos eixos
	glColor3f( 0.0f, 1.0f, 0.0f);
	glPushMatrix();
		glTranslatef(5, 0, 0);
		output(0.4, 0, "X");
		glRotatef(90, 0, 1, 0);
		glutSolidCone(0.10f, 0.3f, 20, 20);
	glPopMatrix();

	glColor3f( 1.0f, 0.0f, 0.0f);
	glPushMatrix();
		glTranslatef(0, 5, 0);
		output(0, 0.4, "Y");
		glRotatef(270, 1, 0, 0);
		glutSolidCone(0.10f, 0.3f, 20, 20);
	glPopMatrix();

	glColor3f( 0.0f, 0.0f, 1.0f);
	glPushMatrix();
		glTranslatef(0, 0, 5);
		glutSolidCone(0.10f, 0.3f, 20, 20);
		glTranslatef(0, 0, 1.25);
		output(0, 0, "Z");
	glPopMatrix();

}

/*
* drawScene() handles the animation and the redrawing of the
* graphics window contents.
*/
void drawScene(void) {
    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Add ambient light
    GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color(0.2, 0.2, 0.2)
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    // Add positioned light
    GLfloat lightColor0[] = {0.8f, 0.8f, 0.8f, 1.0f}; //Color (0.5, 0.5, 0.5)
    GLfloat lightPos0[] = {4.0f, 0.0f, 8.0f, 0.2f}; //Positioned at (4, 0, 8)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    
    // Rotate the image
    glMatrixMode(GL_MODELVIEW); // Current matrix affects objects positions
    glLoadIdentity(); // Initialize to the identity

    // Zoom da camera
    glScalef(rotate_x, rotate_x, 1.0f); 
    //glScalef(1.0f, 1.0f, rotate_x); 
    glRotatef(rotate_by_key, -1.0f, 1.5f, -5.0f);

    glTranslatef(-1.5, 0.0, -35.0); // Translate from origin (in front of viewer)
    glRotatef(RotateAngle, 0.0, 1.0, 0.0); // Rotate around y-axis
    glRotatef(Azimuth, 1.0, 0.0, 0.0); // Set Azimuth angle
    glDisable(GL_CULL_FACE);
    glPushMatrix();
    glTranslatef(1.5, 0.0, 0.0);
    glRotatef(rotateAngle2, 1.0, 0.0, 0.0);
    glColor3f(1.0, 0.4, 0.2); // Reddish color
    // Parameters: height, radius, slices, stacks
    drawGluCylinder(heightCilindro, 0.1, 10, 10);
    glPopMatrix();
    glEnable(GL_CULL_FACE);
    glPushMatrix();
    glTranslatef(1.5, heightCone2, posCone);
    glRotatef(rotateAngle2, 1.0, 0.0, 0.0);
    glColor3f(0.2, 1.0, 0.2); // Greenish color
    // Parameters: height, base radius, top radius, slices, stacks
    drawGluSlantCylinderWithCaps(0.6, 0.2, 0.0, 10, 10);
    glPopMatrix();
 
    // Segundo Vetor
    glTranslatef(1.5, 0.0, 0.0);
    glRotatef(-45.0, 1.0, 0.0, 0.0);
    glColor3f(1.5, 1.2, 0.2); // Amarelo
    // Parameters: height, radius, slices, stacks
    drawGluCylinder(heightCilindro, 0.1, 10, 10);
    glPopMatrix();
    glEnable(GL_CULL_FACE);
    glPushMatrix();
    glTranslatef(0.0, 0.0, heightCone);
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
    drawGluCylinder(heightCilindro, 0.1, 10, 10);
    glPopMatrix();
    glEnable(GL_CULL_FACE);
    glPushMatrix();
    glTranslatef(0.0, 0.0, heightCone);
    glRotatef(0.0, 1.0, 0.0, 0.0);
    glColor3f(0.2, 1.0, 0.2); // Greenish color
    // Parameters: height, base radius, top radius, slices, stacks
    drawGluSlantCylinderWithCaps(0.6, 0.2, 0.0, 10, 10);
    glPopMatrix();  
 
    //Chamada da função que desenha os eixos x,y,z
    drawAxis();

    // Flush the pipeline, swap the buffers
    glFlush();
    glutSwapBuffers();
    
}
 
void reduzVetor(void) {
     heightCilindro -= 1.0;
     heightCone -= 1.0; 
     heightCone2 -= 1.0;
}

void aumentaVetor(void) {
     heightCilindro += 1.0;
     heightCone += 1.0; 
     heightCone2 += 1.0;
}

void afastaVetor(void) {
    heightCone2 += -0.04 - corretor;  
    rotateAngle2 += 5;
    posCone += 0.25;
    corretor += 0.01;
}

void afastaVetor2(void) {
    heightCone2 += 0.04 + corretor;
    rotateAngle2 -= 5;
    posCone -= 0.25;
    corretor += 0.01;
}

// Initialize OpenGL's rendering modes
void initRendering() {
    glEnable(GL_DEPTH_TEST); // Depth testing must be turned on
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Just show wireframes at first
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_NORMALIZE);
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
 
void readParameters(){ 
    int i = 1;
    float x, y, z;
    FILE *entrada;
    
    //r = read , w = write
    entrada = fopen("in.txt", "r");
    
    if(entrada == NULL){
        printf("Houve um problema ao tentar abrir o arquivo!");
    } else {
        while(fscanf(entrada ,"%f %f %f", &x ,&y, &z) != EOF){
            if(i == 1){
                _xx1 = x;
                _yy1 = y;
                _zz1 = z;
            }
            if(i == 2){
                _xx2 = x;
                _yy2 = y;
                _zz2 = z;
            }
            if(i == 3){
                _xx3 = x;
                _yy3 = y;
                _zz3 = z;
            }
            i++;                 
        }
    }
    
    fclose(entrada); 
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
        case 'g':
             rotate_x += .05; 
             glutPostRedisplay();
             break;
        case 'b':
             rotate_x -= .05;
             glutPostRedisplay();
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
        //Diminui o Tamanho do vetor
        case GLUT_KEY_F1:
            reduzVetor(); 
            Sleep(500);
        break;
        //Aumenta o Tamanho do vetor
        case GLUT_KEY_F2:
            aumentaVetor(); 
            Sleep(500);
        break;
        //Rotaciona um vetor específico
        case GLUT_KEY_F3:
            afastaVetor(); 
            Sleep(500);
        break;
        case GLUT_KEY_F4:
            afastaVetor2(); 
            Sleep(500);
        break;
    }
    
    glutPostRedisplay();
}

// Main routine
// Set up OpenGL, define the callbacks and start the main loop
int main(int argc, char** argv) {
    
    readParameters();

    Vetor vetor1, vetor2, vetor3;

    vetor1.inicializaVetor(_xx1, _yy1, _zz1);
    vetor2.inicializaVetor(_xx2, _yy2, _zz2);
    vetor3.inicializaVetor(_xx3, _yy3, _zz3);
    
    inicializacaoGramSchimidt(vetor1, vetor2, vetor3);


    //Imprimi os parâmetros dos vetores
    FILE *saida;
    saida = fopen("out.txt", "w");
    if(saida != NULL){
        fprintf(saida, "%f %f %f %f %f %f %f %f %f", _xx1, _yy1, _zz1, _xx2, _yy2, _zz2, _xx3, _yy3, _zz3);
    }
    fclose(saida);

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

    glutDisplayFunc(drawScene);
    
    
    fprintf(stdout, "Use as setas no teclado para rotacioanr.n");
    fprintf(stdout, "Aperte \"w\" para ativar o modo solido.n");
    fprintf(stdout, "Aperte \"R\" ou \"r\" para aumentar ou diminuir a taxa de velocidade.n");
    // Start the main loop. glutMainLoop never returns.
    glutMainLoop();
    
    return(0); // This line is never reached.
}
