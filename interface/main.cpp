#include <windows.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cmath> 
#include <iostream>
#include "util.h"
#include "vetor.h"

#define PI 3.12159265f

using namespace std; 

// The next global variable controls the animation's state and speed.
float RotateAngle = 0.0f; // Angle in degrees of rotation around y-axis
float Azimuth = 0.0; // Rotated up or down by this amount
float AngleStepSize = 3.0f; // Step three degrees at a time
const float AngleStepMax = 10.0f;
const float AngleStepMin = 0.1f;
float t = 0.0f;
static bool paused = true;
 
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
double rotate_x = 0.7; 

// Desenha um cilindro entre dois pontos
void renderVector(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions,GLUquadricObj *quadric){
   float vx = x2-x1;
   float vy = y2-y1;
   float vz = z2-z1;

   //lida com o caso degenerado de z1 == z2 com uma aproximacao
   if(vz == 0){
       vz = .00000001;
   }    
   float v = sqrt(vx*vx + vy*vy + vz*vz);
   float ax = 57.2957795*acos(vz/v);
   if (vz < 0.0)
       ax = -ax;
   float rx = -vy*vz;
   float ry = vx*vz;
   glPushMatrix();

   //desenha o corpo do cilindro
   glTranslatef(x1, y1, z1);
   glRotatef(ax, rx, ry, 0.0);
   gluQuadricOrientation(quadric,GLU_OUTSIDE);
   gluCylinder(quadric, radius, radius, v-0.5, subdivisions, 1);

   //desenha uma tampa
   gluQuadricOrientation(quadric,GLU_INSIDE);
   gluDisk(quadric, 0.0, radius, subdivisions, 1);
   glTranslatef(0, 0, v-0.5);

   //desenha a outra tampa
   gluQuadricOrientation(quadric,GLU_OUTSIDE);
   //gluDisk(quadric, 0.0, radius, subdivisions, 1);
   gluCylinder(quadric, radius+0.1, 0, 0.5, subdivisions, 1);
   glPopMatrix();
}

//Resultados globais
Vetor resultado1, resultado2, parcial;
//Metodos de execucao de Gram Schimit
void gramSchimidt(Vetor v1, Vetor v2,Vetor v3){
    //Vetor resultado1, resultado2;
    float numerador, numerador2, denominador, denominador2, aux1, aux2, aux3;;
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
    parcial.vetor[0] = aux2-aux3;
    resultado2.vetor[0] = aux1-aux2-aux3;
    aux1 = v3.vetor[1];
    aux2 = (numerador/denominador)*resultado1.vetor[1];
    aux3 = (numerador2/denominador2)*v1.vetor[1];
    parcial.vetor[1] = aux2-aux3;
    resultado2.vetor[1] = aux1-aux2-aux3;
    aux1 = v3.vetor[2];
    aux2 = (numerador/denominador)*resultado1.vetor[2];
    aux3 = (numerador2/denominador2)*v1.vetor[2];
    parcial.vetor[2] = aux2-aux3;
    resultado2.vetor[2] = aux1-aux2-aux3;
    /* FIM DO W3*/
    cout<<"v2:";
    //Insere os dados do vetor 2 modificado no txt
    resultado1.showVetor(2);
    cout<<"v3:";
    //Insere os dados do vetor 3 modificado no txt
    resultado2.showVetor(3);
}

void inicializacaoGramSchimidt(Vetor v1,Vetor v2,Vetor v3){
    cout<<"v1:";
    //Insere os dados do vetor 1 no txt
    v1.showVetor(1);
    gramSchimidt(v1,v2,v3);
}

double anguloEntreVetores(double x1, double y1, double z1, double x2, double y2, double z2) {
    double a[3], b[3];
    double A, B;
    double total, twoDtotal, threeDtotal;
    double ans;

    a[0] = x1;
    a[1] = y1;
    a[2] = z1;

    b[0] = x2;
    b[1] = y2;
    b[2] = z2;

    twoDtotal = ((a[0] * b[0]) + (a[1] * b[1]));
    threeDtotal = ((a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]));

    A = pow(a[0], 2) + pow(a[1], 2) + pow(a[2], 2);
    A = sqrt(A);

    B = pow(b[0], 2) + pow(b[1], 2) + pow(b[2], 2);
    B = sqrt(B);

    total = (A * B);

    ans = threeDtotal / total;

    ans = acos(ans) * 180.0 / PI;

    return ans;
}

Vetor normalizaVetor(double x, double y, double z) {
    Vetor normalizado;
    double denominador;

    denominador = sqrt((x*x) + (y*y) + (z*z));
    normalizado.vetor[0] = (x/denominador) * 2;
    normalizado.vetor[1] = (y/denominador) * 2;
    normalizado.vetor[2] = (z/denominador) * 2;

    return normalizado;
}

void output(float x, float y, char *string) {
  int len, i;
  glRasterPos2f(x, y);
  len = (int) strlen(string);
  for (i = 0; i < len; i++) {
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
    // Dotted pattern for the lines
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
        glVertex3f(0,0,-5);
    glEnd();
    glDisable(GL_LINE_STIPPLE);     // Disable the line stipple
    glPopMatrix();      // Don't forget to pop the Matrix

    // Desenha os cones nos eixos
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
		glTranslatef(0, 0, 0.7);
		output(0, 0, "Z");
	glPopMatrix();

}

int step = 0;
float a = 0;
float b = 0;
float c = 0;
char *str1 = new char[30] ;
char *str2 = new char[30] ;
char *str3 = new char[30] ;
/*
* drawScene() handles the animation and the redrawing of the
* graphics window contents.
*/

double _xx1_norm, _yy1_norm, _zz1_norm;
double _xx2_norm, _yy2_norm, _zz2_norm;
double _xx3_norm, _yy3_norm, _zz3_norm;

int v1_normal, v2_normal, v3_normal;

int splash_flag = 1;

void drawScene(void) {
    //Desenha a Splash Scream
    if (splash_flag) {
        // Clear the rendering window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Add ambient light
        GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color(0.2, 0.2, 0.2)
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
        
        // Rotate the image
        glMatrixMode(GL_MODELVIEW); // Current matrix affects objects positions
        glLoadIdentity(); // Initialize to the identity
        // Zoom da camera
        glScalef(rotate_x, rotate_x, 1.0f); 
        //glScalef(1.0f, 1.0f, rotate_x); 
        glRotatef(rotate_by_key, -1.0f, 1.5f, -5.0f);
        
        glTranslatef(0.0, -0.3, -35.0); // Translate from origin (in front of viewer)
        glRotatef(RotateAngle, 0.0, 1.0, 0.0); // Rotate around y-axis
        glRotatef(Azimuth, 1.0, 0.0, 0.0); // Set Azimuth angle
        glDisable(GL_CULL_FACE);
        
        glPushMatrix();
            glColor3f(1.0f, 1.0f, 1.0f);
        	output(-2.7, 1.0, "Algoritmo de Gram-Schmidt");
            glColor3f(0.6f, 0.6f, 0.6f);
        	output(-6.5, -1.5, "Trabalho de Computacao Grafica");
        	output(-6.5, -2, "Universidade Estadual do Ceara");
        	output(-6.5, -2.5, "Professor: Thelmo de Araujo");
        	output(-6.5, -3, "Equipe: Rodrigo Magalhaes, Anderson Gomes e Kellton Leitao");
        	output(-6.5, -4.5, "Pressione a tecla \"P\" para iniciar.");
        glPopMatrix();
        
        splash_flag = 0;
        glutPostRedisplay();
        // Flush the pipeline, swap the buffers
        glFlush();
        glutSwapBuffers();
    }
    
    // Inicia o gram schmidt
    if(!paused){
        // Clear the rendering window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Add ambient light
        GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color(0.2, 0.2, 0.2)
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
        
        // Rotate the image
        glMatrixMode(GL_MODELVIEW); // Current matrix affects objects positions
        glLoadIdentity(); // Initialize to the identity
        
        // Zoom da camera
        glScalef(rotate_x, rotate_x, 1.0f); 
        //glScalef(1.0f, 1.0f, rotate_x); 
        glRotatef(rotate_by_key, -1.0f, 1.5f, -5.0f);
        
        glTranslatef(0.0, -0.3, -35.0); // Translate from origin (in front of viewer)
        glRotatef(RotateAngle, 0.0, 1.0, 0.0); // Rotate around y-axis
        glRotatef(Azimuth, 1.0, 0.0, 0.0); // Set Azimuth angle
        glDisable(GL_CULL_FACE);
        
        //Define os pontos de origem
        float x1 = 0;
        float y1 = 0;
        float z1 = 0;
        
        float radius = 0.03+(sin(t)/2+0.5)/5;
        
        //Chamada da funcao que desenha os eixos x,y,z
        if (step < 10) drawAxis();
        
        //Desenha o Vetor de (x1, y1, z1) a (x2, y2, z2)
        if (step < 1) {
            glPushMatrix();
                glColor3f(1.0, 0.4, 0.2); // Vermelho
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1, _yy1, _zz1, radius, 32, quadric);
            glPopMatrix();

            //Desenha o Vetor de (x1, y1, z1) a (x2, y2, z2)
            glPushMatrix();
               glColor3f(1.0, 1.0, 0.2); // Amarelo
               GLUquadricObj *quadric2=gluNewQuadric();
               gluQuadricNormals(quadric2, GLU_SMOOTH);
               renderVector(x1, y1, z1, _xx2, _yy2, _zz2, radius, 32, quadric2);
            glPopMatrix();
            
            //Desenha o Vetor de (x1, y1, z1) a (x2, y2, z2)
            glPushMatrix();
               glColor3f(0.0, 0.2, 0.7); // Azul
               GLUquadricObj *quadric3=gluNewQuadric();
               gluQuadricNormals(quadric3, GLU_SMOOTH);
               renderVector(x1, y1, z1, _xx3, _yy3, _zz3, radius, 32, quadric3);
            glPopMatrix();
        }        

        // Nova posicao do vetor 2
        float novo_xx2 = resultado1.vetor[0];
        float novo_yy2 = resultado1.vetor[1];
        float novo_zz2 = resultado1.vetor[2];
        
        // Posicao parcial do vetor 3
        float parcial_xx3 = parcial.vetor[0];
        float parcial_yy3 = parcial.vetor[1];
        float parcial_zz3 = parcial.vetor[2];
        
        // Nova posicao do vetor 3
        float novo_xx3 = resultado2.vetor[0];
        float novo_yy3 = resultado2.vetor[1];
        float novo_zz3 = resultado2.vetor[2];
        
        double angulo;
        
        //1? Passo Rotaciona a Camera
        if (step == 0) {
           RotateAngle = RotateAngle + 0.15f;
           if (RotateAngle > 40.0f) {
                _xx1_norm = _xx1;
                _yy1_norm = _yy1;
                _zz1_norm = _zz1;
        
                _xx2_norm = _xx2;
                _yy2_norm = _yy2;
                _zz2_norm = _zz2;
        
                _xx3_norm = _xx3;
                _yy3_norm = _yy3;
                _zz3_norm = _zz3;

               step++;
           }
        }

        //Normalizacao
        Vetor v1_normalizado, v2_normalizado, v3_normalizado;
        v1_normalizado = normalizaVetor(_xx1, _yy1, _zz1);
        v2_normalizado = normalizaVetor(_xx2, _yy2, _zz2);
        v3_normalizado = normalizaVetor(_xx3, _yy3, _zz3);


        //2? Passo deixa os vetores com opacidade baixa e normaliza o vetor v1, colocando em destaque
        if (step == 1) {
            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(1.0, 1.0, 0.2, 0.3f); // Amarelo
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadricFade2=gluNewQuadric();
                gluQuadricNormals(quadricFade2, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx2, _yy2, _zz2, radius, 32, quadricFade2);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();
            
            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(0.0, 0.2, 0.7, 0.3f); // Azul
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadricFade3=gluNewQuadric();
                gluQuadricNormals(quadricFade3, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx3, _yy3, _zz3, radius, 32, quadricFade3);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();

           
            glPushMatrix();
                glColor3f(1.0, 0.4, 0.2); // Vermelho
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
            glPopMatrix();
            
            if (_xx1_norm < v1_normalizado.vetor[0]) _xx1_norm = _xx1_norm/1.001;
            if (_xx1_norm > v1_normalizado.vetor[0]) _xx1_norm = _xx1_norm*0.999;
            if (_yy1_norm < v1_normalizado.vetor[1]) _yy1_norm = _yy1_norm/1.001;
            if (_yy1_norm > v1_normalizado.vetor[1]) _yy1_norm = _yy1_norm*0.999;
            if (_zz1_norm < v1_normalizado.vetor[2]) _zz1_norm = _zz1_norm/1.001;
            if (_zz1_norm > v1_normalizado.vetor[2]) _zz1_norm = _zz1_norm*0.999;

            if (fabs(v1_normalizado.vetor[0] - _xx1_norm) < 0.05 && fabs(v1_normalizado.vetor[1] - _yy1_norm) < 0.05 && fabs(v1_normalizado.vetor[2] - _zz1_norm) < 0.05) {
               step++;
            }

        }

        //3? Passo desenha os vetores destacando o vermelho
        if (step == 2) {
            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(1.0, 1.0, 0.2, 0.3f); // Amarelo
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadricFade2=gluNewQuadric();
                gluQuadricNormals(quadricFade2, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx2, _yy2, _zz2, radius, 32, quadricFade2);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();
            
            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(0.0, 0.2, 0.7, 0.3f); // Azul
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadricFade3=gluNewQuadric();
                gluQuadricNormals(quadricFade3, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx3, _yy3, _zz3, radius, 32, quadricFade3);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();

            glPushMatrix();
                glColor3f(1.0, 0.4, 0.2); // Vermelho
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
            glPopMatrix();
          
            step++;

        }

        //4? Passo desenha o plano e atualiza o vetor amarelo (destaque para os vetores vermelho e amarelo)
        if (step == 3) {
            glPushMatrix();
                glColor3f(1.0, 1.0, 0.2); // Amarelo
                GLUquadricObj *quadric2=gluNewQuadric();
                gluQuadricNormals(quadric2, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx2, _yy2, _zz2, radius, 32, quadric2);
            glPopMatrix();
            
            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(0.0, 0.2, 0.7, 0.3f); // Azul
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadricFade3=gluNewQuadric();
                gluQuadricNormals(quadricFade3, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx3, _yy3, _zz3, radius, 32, quadricFade3);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();

            glPushMatrix();
                glColor3f(1.0, 0.4, 0.2); // Vermelho
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
            glPopMatrix();

            //TENTATIVA DESENHAR PLANO 1
            FILE *read;
            read = fopen("outV2.txt", "r");
            float a, b, c, aa, bb, cc;
            
            if(read == NULL){
                printf("Houve um problema ao tentar abrir o arquivo!");
            } else {
                fscanf(read ,"%f %f %f", &a ,&b, &c);
            }
            fclose(read); 
            read = fopen("outV3.txt", "r");
            if(read == NULL){
                printf("Houve um problema ao tentar abrir o arquivo!");
            } else {
                fscanf(read ,"%f %f %f", &aa ,&bb, &cc);
            }
            fclose(read); 
           
            glPushMatrix();
               glColor4f(1.0, 1.0, 1.0, 0.4f); // Aqui tu coloca a cor (RGB) e o ultimo parametro ? a opacidade, quanto menor, mais transparente e quanto maior, mais opaco... vai de 0.0 a 1.0
               glBlendFunc(GL_SRC_ALPHA,GL_ONE); // aqui tu habilita
               glEnable(GL_BLEND);  // aqui tu habilita
               
                glBegin(GL_QUADS);      
                    glVertex3f((aa - a),(bb - b), (cc - c));             
                    glVertex3f((aa + a) * (-1), (bb + b)  * (-1), (cc + c)  * (-1));
                    glVertex3f((aa - a) * (-1),(bb - b) * (-1), (cc - c) * (-1));
                    glVertex3f(aa + a, bb + b, cc + c);                          
                glEnd();
 
                glDisable(GL_BLEND);
            glPopMatrix();    
            
            glPushMatrix();
               angulo = anguloEntreVetores(0, 1, 0, novo_xx2, novo_yy2, novo_zz2);
               glRotatef(angulo, 1, 0, 0);
               glColor4f(1.0, 0.4, 0.2, 0.4f);
               glBlendFunc(GL_SRC_ALPHA,GL_ONE);
               glEnable(GL_BLEND);
               glBegin(GL_QUADS);
                   //glVertex3f(-7, -7, 0);
                   //glVertex3f(-7, 7, 0);
                   //glVertex3f(7, 7, 0);
                   //glVertex3f(7, -7, 0);
               glEnd();
               glDisable(GL_BLEND);
            glPopMatrix();

           if (_xx2 < novo_xx2) _xx2 = _xx2 + 0.01;
           if (_xx2 > novo_xx2) _xx2 = _xx2 - 0.01;
           if (_yy2 < novo_yy2) _yy2 = _yy2 + 0.01;
           if (_yy2 > novo_yy2) _yy2 = _yy2 - 0.01;
           if (_zz2 < novo_zz2) _zz2 = _zz2 + 0.01;
           if (_zz2 > novo_zz2) _zz2 = _zz2 - 0.01;
           if (fabs(_xx2 - novo_xx2) < 0.05 && fabs(_yy2 - novo_yy2) < 0.05 && fabs(_zz2 - novo_zz2) < 0.05) {
              step++;
              _xx2_norm = novo_xx2;
              _yy2_norm = novo_yy2;
              _zz2_norm = novo_zz2;
              Sleep(1000);
           }
        }

        //5? Passo remove o plano e destaca apenas o vetor amarelo
        if (step == 4) {
            glPushMatrix();
                glColor3f(1.0, 1.0, 0.2); // Amarelo
                GLUquadricObj *quadric2=gluNewQuadric();
                gluQuadricNormals(quadric2, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx2_norm, _yy2_norm, _zz2_norm, radius, 32, quadric2);
            glPopMatrix();
            
            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(0.0, 0.2, 0.7, 0.3f); // Azul
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadricFade3=gluNewQuadric();
                gluQuadricNormals(quadricFade3, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx3, _yy3, _zz3, radius, 32, quadricFade3);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();

            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(1.0, 0.4, 0.2, 0.3f); // Vermelho
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();

            step++;
        }

        //6? Passo normaliza o vetor amarelo
        if (step == 5) {
            glPushMatrix();
                glColor3f(1.0, 1.0, 0.2); // Amarelo
                GLUquadricObj *quadric2=gluNewQuadric();
                gluQuadricNormals(quadric2, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx2_norm, _yy2_norm, _zz2_norm, radius, 32, quadric2);
            glPopMatrix();
            
            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(0.0, 0.2, 0.7, 0.3f); // Azul
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadricFade3=gluNewQuadric();
                gluQuadricNormals(quadricFade3, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx3, _yy3, _zz3, radius, 32, quadricFade3);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();

            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(1.0, 0.4, 0.2, 0.3f); // Vermelho
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();

            if (_xx2_norm < v2_normalizado.vetor[0]) _xx2_norm = _xx2_norm/1.001;
            if (_xx2_norm > v2_normalizado.vetor[0]) _xx2_norm = _xx2_norm*0.999;
            if (_yy2_norm < v2_normalizado.vetor[1]) _yy2_norm = _yy2_norm/1.001;
            if (_yy2_norm > v2_normalizado.vetor[1]) _yy2_norm = _yy2_norm*0.999;
            if (_zz2_norm < v2_normalizado.vetor[2]) _zz2_norm = _zz2_norm/1.001;
            if (_zz2_norm > v2_normalizado.vetor[2]) _zz2_norm = _zz2_norm*0.999;

            if (fabs(v2_normalizado.vetor[0] - _xx2_norm) < 0.05 && fabs(v2_normalizado.vetor[1] - _yy2_norm) < 0.05 && fabs(v2_normalizado.vetor[2] - _zz2_norm) < 0.05) {
               step++;
            }

        }

        //7? Passo destaca o vetor vermelho e amarelo e desenha um plano entre eles
        if (step == 6) {
            glPushMatrix();
                glColor3f(1.0, 1.0, 0.2); // Amarelo
                GLUquadricObj *quadric2=gluNewQuadric();
                gluQuadricNormals(quadric2, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx2_norm, _yy2_norm, _zz2_norm, radius, 32, quadric2);
            glPopMatrix();
            
            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(0.0, 0.2, 0.7, 0.3f); // Azul
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadricFade3=gluNewQuadric();
                gluQuadricNormals(quadricFade3, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx3, _yy3, _zz3, radius, 32, quadricFade3);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();

            glPushMatrix();
                glColor4f(1.0, 0.4, 0.2, 0.3f); // Vermelho
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
            glPopMatrix();

            //TENTATIVA DESENHAR PLANO 3
            FILE *read;
            read = fopen("outV2.txt", "r");
            float a, b, c;
            
            if(read == NULL){
                printf("Houve um problema ao tentar abrir o arquivo!");
            } else {
                fscanf(read ,"%f %f %f", &a ,&b, &c);
            }
            fclose(read); 
            glPushMatrix();
               glColor4f(1.0, 1.0, 1.0, 0.4f); // Aqui tu coloca a cor (RGB) e o ultimo parametro ? a opacidade, quanto menor, mais transparente e quanto maior, mais opaco... vai de 0.0 a 1.0
               glBlendFunc(GL_SRC_ALPHA,GL_ONE); // aqui tu habilita
               glEnable(GL_BLEND);  // aqui tu habilita
                glBegin(GL_QUADS);                              
                    glVertex3f((_xx1 - a),(_yy1 - b), (_zz1 - c));            
                    glVertex3f((_xx1 + a) * (-1), (_yy1 + b)  * (-1), (_zz1 + c)  * (-1));
                    glVertex3f((_xx1 - a) * (-1),(_yy1 - b) * (-1), (_zz1 - c) * (-1));                          
                    glVertex3f(_xx1 + a, _yy1 + b, _zz1 + c);                          
                glEnd();
                glDisable(GL_BLEND);
            glPopMatrix();

            step++;
        }

        //8? Passo destaca o vetor azul e coloca na nova posicao
        if (step == 7) {
            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(1.0, 1.0, 0.2, 0.3f); // Amarelo
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadric2=gluNewQuadric();
                gluQuadricNormals(quadric2, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx2_norm, _yy2_norm, _zz2_norm, radius, 32, quadric2);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();
            
            glPushMatrix();
                glColor3f(0.0, 0.2, 0.7); // Azul
                GLUquadricObj *quadricFade3=gluNewQuadric();
                gluQuadricNormals(quadricFade3, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx3, _yy3, _zz3, radius, 32, quadricFade3);
            glPopMatrix();

            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(1.0, 0.4, 0.2, 0.3f); // Vermelho
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();
            
            //TENTATIVA DESENHAR PLANO 3
            FILE *read;
            read = fopen("outV2.txt", "r");
            float a, b, c;
            
            if(read == NULL){
                printf("Houve um problema ao tentar abrir o arquivo!");
            } else {
                fscanf(read ,"%f %f %f", &a ,&b, &c);
            }
            fclose(read); 
            glPushMatrix();
               glColor4f(1.0, 1.0, 1.0, 0.4f); // Aqui tu coloca a cor (RGB) e o ultimo parametro ? a opacidade, quanto menor, mais transparente e quanto maior, mais opaco... vai de 0.0 a 1.0
               glBlendFunc(GL_SRC_ALPHA,GL_ONE); // aqui tu habilita
               glEnable(GL_BLEND);  // aqui tu habilita
                glBegin(GL_QUADS);                              
                    glVertex3f((_xx1 - a),(_yy1 - b), (_zz1 - c));            
                    glVertex3f((_xx1 + a) * (-1), (_yy1 + b)  * (-1), (_zz1 + c)  * (-1));
                    glVertex3f((_xx1 - a) * (-1),(_yy1 - b) * (-1), (_zz1 - c) * (-1));                          
                    glVertex3f(_xx1 + a, _yy1 + b, _zz1 + c);                          
                glEnd();
                glDisable(GL_BLEND);
            glPopMatrix();
            
            glPushMatrix();
               glColor4f(1.0, 1.0, 1.0, 0.4f);
               glBlendFunc(GL_SRC_ALPHA,GL_ONE);
               glEnable(GL_BLEND);
               glBegin(GL_QUADS);
                   //glVertex3f(x1, y1, z1);
                   //glVertex3f(_xx1_norm, _yy1_norm, _zz1_norm);
                   //glVertex3f(_xx1_norm + _xx2_norm, _yy1_norm + _yy2_norm, _zz1_norm + _zz2_norm);
                   //glVertex3f(_xx2_norm, _yy2_norm, _zz2_norm);
               glEnd();
               glDisable(GL_BLEND);
            glPopMatrix();

            if (_xx3 < novo_xx3) _xx3 = _xx3 + 0.01;
            if (_xx3 > novo_xx3) _xx3 = _xx3 - 0.01;
            if (_yy3 < novo_yy3) _yy3 = _yy3 + 0.01;
            if (_yy3 > novo_yy3) _yy3 = _yy3 - 0.01;
            if (_zz3 < novo_zz3) _zz3 = _zz3 + 0.01;
            if (_zz3 > novo_zz3) _zz3 = _zz3 - 0.01;
            if (fabs(_xx3 - novo_xx3) < 0.05 && fabs(_yy3 - novo_yy3) < 0.05 && fabs(_zz3 - novo_zz3) < 0.05) {
                step++;
            }
        }

        //9? Rotaciona a camera, mantendo o destaque ao vetor azul

        if (step == 8) {
            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(1.0, 1.0, 0.2, 0.3f); // Amarelo
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadric2=gluNewQuadric();
                gluQuadricNormals(quadric2, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx2_norm, _yy2_norm, _zz2_norm, radius, 32, quadric2);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();
            
            glPushMatrix();
                glColor3f(0.0, 0.2, 0.7); // Azul
                GLUquadricObj *quadricFade3=gluNewQuadric();
                gluQuadricNormals(quadricFade3, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx3, _yy3, _zz3, radius, 32, quadricFade3);
            glPopMatrix();

            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(1.0, 0.4, 0.2, 0.3f); // Vermelho
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();
            
            //TENTATIVA DESENHAR PLANO 3
            FILE *read;
            read = fopen("outV2.txt", "r");
            float a, b, c;
            
            if(read == NULL){
                printf("Houve um problema ao tentar abrir o arquivo!");
            } else {
                fscanf(read ,"%f %f %f", &a ,&b, &c);
            }
            fclose(read); 
            glPushMatrix();
               glColor4f(1.0, 1.0, 1.0, 0.4f); // Aqui tu coloca a cor (RGB) e o ultimo parametro ? a opacidade, quanto menor, mais transparente e quanto maior, mais opaco... vai de 0.0 a 1.0
               glBlendFunc(GL_SRC_ALPHA,GL_ONE); // aqui tu habilita
               glEnable(GL_BLEND);  // aqui tu habilita
                glBegin(GL_QUADS);                              
                    glVertex3f(_xx1, _yy1, _zz1);            
                    glVertex3f( 0.0f, 0.0f, 0.0f);                       
                    glVertex3f(a, b, c);  
                    glVertex3f(_xx1 + a, _yy1 + b, _zz1 + c);                          
                glEnd();
                
                glBegin(GL_QUADS);                              
                    glVertex3f(_xx1 * (-1), _yy1 * (-1), _zz1 * (-1));            
                    glVertex3f( 0.0f, 0.0f, 0.0f);                       
                    glVertex3f(a * (-1), b * (-1), c * (-1));  
                    glVertex3f((_xx1 + a) * (-1), (_yy1 + b)  * (-1), (_zz1 + c)  * (-1));                          
                glEnd();
                
                glBegin(GL_QUADS);                              
                    glVertex3f(_xx1 * (-1), _yy1 * (-1), _zz1 * (-1));            
                    glVertex3f( 0.0f, 0.0f, 0.0f);                       
                    glVertex3f(a, b, c);  
                    glVertex3f((_xx1 - a) * (-1),(_yy1 - b) * (-1), (_zz1 - c) * (-1));                          
                glEnd();
                
                glBegin(GL_QUADS);                              
                    glVertex3f(_xx1, _yy1, _zz1);            
                    glVertex3f( 0.0f, 0.0f, 0.0f);                       
                    glVertex3f(a * (-1), b * (-1), c * (-1));  
                    glVertex3f((_xx1 - a),(_yy1 - b), (_zz1 - c));                          
                glEnd();
                glDisable(GL_BLEND);
            glPopMatrix();
            
            glPushMatrix();
               glColor4f(1.0, 1.0, 1.0, 0.4f);
               glBlendFunc(GL_SRC_ALPHA,GL_ONE);
               glEnable(GL_BLEND);
               glBegin(GL_QUADS);
                   //glVertex3f(x1, y1, z1);
                   //glVertex3f(_xx1_norm, _yy1_norm, _zz1_norm);
                   //glVertex3f(_xx1_norm + _xx2_norm, _yy1_norm + _yy2_norm, _zz1_norm + _zz2_norm);
                   //glVertex3f(_xx2_norm, _yy2_norm, _zz2_norm);
               glEnd();
               glDisable(GL_BLEND);
            glPopMatrix();

           RotateAngle = RotateAngle - 0.15f;
           if (RotateAngle < -60.0f) {
               Sleep(700);
               _xx3_norm = _xx3;
               _yy3_norm = _yy3;
               _zz3_norm = _zz3;
               step++;
           }
        }

        //10? Passo normaliza o vetor azul
        if (step == 9) {
            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(1.0, 0.4, 0.2, 0.3f); // Vermelho
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();

            glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glColor4f(1.0, 1.0, 0.2, 0.3f); // Amarelo
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                glEnable(GL_BLEND);
                GLUquadricObj *quadric2=gluNewQuadric();
                gluQuadricNormals(quadric2, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx2_norm, _yy2_norm, _zz2_norm, radius, 32, quadric2);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            glPopMatrix();
            
            glPushMatrix();
                glColor3f(0.0, 0.2, 0.7); // Azul
                GLUquadricObj *quadricFade3=gluNewQuadric();
                gluQuadricNormals(quadricFade3, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx3_norm, _yy3_norm, _zz3_norm, radius, 32, quadricFade3);
            glPopMatrix();

            if (_xx3_norm < v3_normalizado.vetor[0]) _xx3_norm = _xx3_norm/1.001;
            if (_xx3_norm > v3_normalizado.vetor[0]) _xx3_norm = _xx3_norm*0.999;
            if (_yy3_norm < v3_normalizado.vetor[1]) _yy3_norm = _yy3_norm/1.001;
            if (_yy3_norm > v3_normalizado.vetor[1]) _yy3_norm = _yy3_norm*0.999;
            if (_zz3_norm < v3_normalizado.vetor[2]) _zz3_norm = _zz3_norm/1.001;
            if (_zz3_norm > v3_normalizado.vetor[2]) _zz3_norm = _zz3_norm*0.999;

            if (fabs(v3_normalizado.vetor[0] - _xx3_norm) < 0.05 && fabs(v3_normalizado.vetor[1] - _yy3_norm) < 0.05 && fabs(v3_normalizado.vetor[2] - _zz3_norm) < 0.05) {
                sprintf(str1,"q1: (%.3f, %.3f, %.3f)", _xx1_norm/2, _yy1_norm/2, _zz1_norm/2);
                sprintf(str2,"q2: (%.3f, %.3f, %.3f)", _xx2_norm/2, _yy2_norm/2, _zz2_norm/2);
                sprintf(str3,"q3: (%.3f, %.3f, %.3f)", _xx3_norm/2, _yy3_norm/2, _zz3_norm/2);
                step++;
            }

        }

        //11? Passo desenha todos os vetores em destaque
        if (step == 10) {
            glPushMatrix();
                glColor3f(1.0, 0.4, 0.2); // Vermelho
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
                output(-8.0, 6.0, str1);
            glPopMatrix();

            glPushMatrix();
                glColor3f(1.0, 1.0, 0.2); // Amarelo
                GLUquadricObj *quadric2=gluNewQuadric();
                gluQuadricNormals(quadric2, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx2_norm, _yy2_norm, _zz2_norm, radius, 32, quadric2);
                output(-8.0, 5.0, str2);
            glPopMatrix();
            
            glPushMatrix();
                glColor3f(0.0, 0.2, 0.7); // Azul
                GLUquadricObj *quadricFade3=gluNewQuadric();
                gluQuadricNormals(quadricFade3, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx3_norm, _yy3_norm, _zz3_norm, radius, 32, quadricFade3);
                output(-8.0, 4.0, str3);
            glPopMatrix();
        }
       
        glutPostRedisplay();
    }
    // Flush the pipeline, swap the buffers
    glFlush();
    glutSwapBuffers();
    
}
 
// Initialize OpenGL's rendering modes
void initRendering() {
    glEnable(GL_DEPTH_TEST); // Depth testing must be turned on
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Just show wireframes at first
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    GLfloat LightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat LightDiffuse[] = { 0.5f, 1.0f, 1.0f, 1.0f };
    GLfloat LightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);
    glEnable(GL_LIGHT0);
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

//TODO Adicionar as valida??es aqui para n?o poluir o c?digo 
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

void executaGramSchimidt(){
    //Adicionar valida??es:
    //Entrada nula
    //Vetores com valores inv?lidos ou muito divergentes
    readParameters();

    Vetor vetor1, vetor2, vetor3;

    vetor1.inicializaVetor(_xx1, _yy1, _zz1);
    vetor2.inicializaVetor(_xx2, _yy2, _zz2);
    vetor3.inicializaVetor(_xx3, _yy3, _zz3);
    
    inicializacaoGramSchimidt(vetor1, vetor2, vetor3);
    
    
    //TODO Reduzir o tamanho dos vetores para 2 para ficar interessante na anima??o
     
}

 
// glutKeyboardFunc is called below to set this function to handle
// all "normal" key presses.
void myKeyboardFunc(unsigned char key, int x, int y) {
    switch (key) {
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
        case 'p':
             paused = !paused;
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
        case GLUT_KEY_F9:
             executaGramSchimidt();
        break;
    }
    
    glutPostRedisplay();
}

    Vetor vetor1, vetor2, vetor3;
// Main routine
// Set up OpenGL, define the callbacks and start the main loop
int main(int argc, char** argv) {
    
    readParameters();



    vetor1.inicializaVetor(_xx1, _yy1, _zz1);
    vetor2.inicializaVetor(_xx2, _yy2, _zz2);
    vetor3.inicializaVetor(_xx3, _yy3, _zz3);
    
    inicializacaoGramSchimidt(vetor1, vetor2, vetor3);


    //Imprimi os par?metros dos vetores
    /*
    FILE *saida;
    saida = fopen("out.txt", "w");
    if(saida != NULL){
        fprintf(saida, "%f %f %f %f %f %f %f %f %f", _xx1, _yy1, _zz1, _xx2, _yy2, _zz2, _xx3, _yy3, _zz3);
    }
    fclose(saida);
    */
    
    glutInit(&argc, argv);
    // We're going to animate it, so double buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // Window position (from top corner), and size (width% and hight)
    glutInitWindowPosition(200, 60);
    // Tamanho da Janela
    glutInitWindowSize(800, 600);
    // T?tulo da Janela
    glutCreateWindow("Gram Schmidt");
    
    // Criar menu acess?vel atrav?s do bot?o direito do mouse
    int menuRightButton = glutCreateMenu(menu);
    glutAddMenuEntry("Visualizar Informa??es", 1);
    glutAddMenuEntry("Iniciar Anima??o", 2);
    glutAddMenuEntry("Fechar Aplica??o", 3);
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
