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
#define ESPERA 1200

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

// Desenha um vetor entre dois pontos
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

   //desenha a tampa
   gluQuadricOrientation(quadric,GLU_INSIDE);
   gluDisk(quadric, 0.0, radius, subdivisions, 1);
   glTranslatef(0, 0, v-0.5);

   //desenha um cone
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

bool gauss(Vetor v1,Vetor v2,Vetor v3){
    int i, j, k, n;
    int o, p;
    double fator, sum;
    double a[3][3]= {
        {v1.vetor[0], v1.vetor[1], v1.vetor[2]},
        {v2.vetor[0], v2.vetor[1], v2.vetor[2]},
        {v3.vetor[0], v3.vetor[1], v3.vetor[2]}
     };
    double b[3]= {0.0, 0.0, 0.0};
    double x[3];
    n = 3;
    for(k=0; k<=n-2; k++){
        for(i=k+1; i<=n-1; i++){
            fator = a[i][k]/a[k][k];
            for(j=k+1;j<=n-1;j++){
                a[i][j] = a[i][j] - (fator * a[k][j]);
            }
            b[i] = b[i] - (fator * b[k]);
            a[i][k] = 0.0f;
        }
     }
     x[n-1]=b[n-1]/a[n-1][n-1];
     for(i=n-2; i>=0; i--){
        sum = 0.0f;
        for(j=i+1; j<=n-1; j++){
            sum = sum + a[i][j] * x[j];
        }
        x[i] = (b[i] - sum)/a[i][i];
     }
    if(x[0]==0 && x[1]==0 && x[2]==0){
		return true;
    }
    else{
        return false;
    }
}

void inicializacaoGramSchimidt(Vetor v1,Vetor v2,Vetor v3){
	if(gauss(v1,v2,v3)==true){
        cout<<"v1:";
        //Insere os dados do vetor 1 no txt
        v1.showVetor(1);
        gramSchimidt(v1,v2,v3);
	}
    else{
		exit(0);
	}
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

double norma(double x, double y, double z) {
    double norma;

    norma = sqrt((x*x) + (y*y) + (z*z));

    return norma;
}

Vetor normalizaVetor(double x, double y, double z) {
    Vetor normalizado;
    double denominador;
    if(sqrt((x*x) + (y*y) + (z*z)) == 1){
         normalizado.vetor[0] = x;
         normalizado.vetor[1] = y;
         normalizado.vetor[2] = z;
         return normalizado;
    }
    denominador = norma(x, y, z);
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

// Desenha os eixos coordenados (x, y, z)
void drawAxis(void) {
    glPushMatrix();
        // Desenha a linha do lado positivo x,y,z
        glBegin(GL_LINES);
            glColor3f (0.0, 1.0, 0.0); // Verde para o eixo x
            glVertex3f(0,0,0);
            glVertex3f(5,0,0);
            glColor3f(1.0,0.0,0.0); // Vermelho para o eixo y
            glVertex3f(0,0,0);
            glVertex3f(0,5,0);
            glColor3f(0.0,0.0,1.0); // Azul para o eixo z
            glVertex3f(0,0,0);
            glVertex3f(0,0,5);
        glEnd();
    
        // Linha pontilhada para o lado negativo x,y,z
        glEnable(GL_LINE_STIPPLE);  // Habilita o line stipple para usar um padrao pontilhado
        glLineStipple(1, 0x0101);   // Define o padrao de pontilhado da linha
        glBegin(GL_LINES);
            glColor3f (0.0, 1.0, 0.0);  // Verde para o eixo x
            glVertex3f(-5,0,0);
            glVertex3f(0,0,0);
            glColor3f(1.0,0.0,0.0);     // Vermelho para o eixo y
            glVertex3f(0,0,0);
            glVertex3f(0,-5,0);
            glColor3f(0.0,0.0,1.0);     // Azul para o eixo z
            glVertex3f(0,0,0);
            glVertex3f(0,0,-5);
        glEnd();
        glDisable(GL_LINE_STIPPLE);     // Desabilita o line stipple
    glPopMatrix();

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

// Variavel para os passos da animacao
int step = 0;

float a = 0;
float b = 0;
float c = 0;

// Textos que aparecem na tela
char *str1 = new char[30];
char *str2 = new char[30];
char *str3 = new char[30];
char *V1 = new char[30];
char *V2 = new char[30];
char *V3 = new char[30];
/*
* drawScene() cuida da animacao, desenho e redesenho do conteudo grafico
*/

double _xx1_norm, _yy1_norm, _zz1_norm;
double _xx2_norm, _yy2_norm, _zz2_norm;
double _xx3_norm, _yy3_norm, _zz3_norm;

// Linha pontilhada origem/destino
double pont_o_xx1, pont_o_yy1, pont_o_zz1;
double pont_d_xx1, pont_d_yy1, pont_d_zz1;
int sleep_flag = 0;

// Flag para a Splash Screen
int splash_flag = 1;

void drawScene(void) {
    //Desenha a Splash Screen
    if (splash_flag) {
        // Limpa a janela de renderizacao
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Adiciona luz ambiente
        GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color(0.2, 0.2, 0.2)
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

        // Rotaciona a imagem
        glMatrixMode(GL_MODELVIEW); // Matriz atual afeta a posicao dos objetos
        glLoadIdentity(); // Inicializa para a identidade
        // Zoom da camera
        glScalef(rotate_x, rotate_x, 1.0f);
        //glScalef(1.0f, 1.0f, rotate_x);
        glRotatef(rotate_by_key, -1.0f, 1.5f, -5.0f);

        glTranslatef(0.0, -0.3, -35.0); // Translada a partir da origem (em frente ao espectador)
        glRotatef(RotateAngle, 0.0, 1.0, 0.0); // Rotaciona ao redor do eixo y
        glRotatef(Azimuth, 1.0, 0.0, 0.0); // Define o angulo azimuth (distancia angular medida sobre o horizonte)
        glDisable(GL_CULL_FACE);

        glPushMatrix();
            glColor3f(1.0, 0.4, 0.2); // Vermelho
            sprintf(V1,"V1: (%.1f, %.1f, %.1f)", _xx1, _yy1, _zz1);
            output(-6.5, 5.0, V1);
            glColor3f(1.0, 1.0, 0.2); // Amarelo
            sprintf(V2,"V2: (%.1f, %.1f, %.1f)", _xx2, _yy2, _zz2);
            output(-1.5, 5.0, V2);
            glColor3f(0.0, 0.2, 0.7); // Azul
            sprintf(V3,"V3: (%.1f, %.1f, %.1f)", _xx3, _yy3, _zz3);
            output(3.5, 5.0, V3);
            glColor3f(1.0f, 1.0f, 1.0f);
        	output(-2.7, 1.0, "Algoritmo de Gram-Schmidt");
            glColor3f(0.6f, 0.6f, 0.6f);
        	output(-6.5, -1.5, "Trabalho de Computacao Grafica");
        	output(-6.5, -2, "Universidade Estadual do Ceara");
        	output(-6.5, -2.5, "Professor: Thelmo de Araujo");
        	output(-6.5, -3, "Equipe: Rodrigo Magalhaes, Anderson Gomes e Kellton Leitao");
        	output(-6.5, -4.5, "Pressione a tecla \"p\" para iniciar.");
        glPopMatrix();

        splash_flag = 0;
        glutPostRedisplay();
        // Limpa o pipeline, troca os buffers
        glFlush();
        glutSwapBuffers();
    }

    // Inicia o gram schmidt
    if(!paused){
        // Limpa a janela de renderizacao
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Adiciona luz ambiente
        GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; // Cor(0.2, 0.2, 0.2)
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

        // Rotaciona a imagem
        glMatrixMode(GL_MODELVIEW); // Matriz atual afeta a posicao dos objetos
        glLoadIdentity(); // Inicializa para a identidade

        // Zoom da camera
        glScalef(rotate_x, rotate_x, 1.0f);
        //glScalef(1.0f, 1.0f, rotate_x);
        glRotatef(rotate_by_key, -1.0f, 1.5f, -5.0f);

        glTranslatef(0.0, -0.3, -35.0); // Translada a partir da origem (em frente ao espectador)
        glRotatef(RotateAngle, 0.0, 1.0, 0.0); // Rotaciona ao redor do eixo y
        glRotatef(Azimuth, 1.0, 0.0, 0.0); // Define o angulo azimuth (distancia angular medida sobre o horizonte)
        glDisable(GL_CULL_FACE);

        //Define os pontos de origem
        float x1 = 0;
        float y1 = 0;
        float z1 = 0;

        float radius = 0.03+(sin(t)/2+0.5)/5;

        //Chamada da funcao que desenha os eixos x,y,z
        if (step < 11) drawAxis();

        if (step < 1) {
            //Desenha o Vetor de (x1, y1, z1) a (x2, y2, z2)
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
        // float parcial_xx3 = parcial.vetor[0];
        // float parcial_yy3 = parcial.vetor[1];
        // float parcial_zz3 = parcial.vetor[2];

        // Nova posicao do vetor 3
        float novo_xx3 = resultado2.vetor[0];
        float novo_yy3 = resultado2.vetor[1];
        float novo_zz3 = resultado2.vetor[2];

        // 1o Passo Rotaciona a Camera
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

               // Pontilhado
               pont_o_xx1 = _xx1;
               pont_o_yy1 = _yy1;
               pont_o_zz1 = _zz1;

               step++;
           }
        }

        if (step == 1) {
            glPushMatrix();
                glColor3f(1.0, 0.4, 0.2); // Vermelho
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
            glPopMatrix();

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

            glLineWidth(1.5); 
            glColor3f(1.0, 1.0, 1.0);
            glLineStipple(2, 0xAAAA);
            glEnable(GL_LINE_STIPPLE);
            glBegin(GL_LINES);
                glVertex3f(pont_o_xx1, pont_o_yy1, pont_o_zz1);
                glVertex3f(novo_xx2, b, novo_zz2);
            glEnd();
            glDisable(GL_LINE_STIPPLE);

            if (b < novo_yy2) b = b + 0.005;
            if (b > novo_yy2) b = b - 0.005;

            if (fabs(b - novo_yy2) < 0.005) {
                // DESENHA O PLANO 1
                glPushMatrix();
                   glColor4f(1.0, 0.4, 0.2, 0.4f);
                   glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                   glEnable(GL_BLEND);
    
                    glBegin(GL_QUADS);
                        glVertex3f((novo_xx3 - novo_xx2) * 2,(novo_yy3 - novo_yy2) * 2, (novo_zz3 - novo_zz2) * 2);
                        glVertex3f((novo_xx3 + novo_xx2) * (-1) * 2, (novo_yy3 + novo_yy2)  * (-1) * 2, (novo_zz3 + novo_zz2)  * (-1) * 2);
                        glVertex3f((novo_xx3 - novo_xx2) * (-1) * 2,(novo_yy3 - novo_yy2) * (-1) * 2, (novo_zz3 - novo_zz2) * (-1) * 2);
                        glVertex3f(novo_xx3 + novo_xx2 * 2, novo_yy3 + novo_yy2 * 2, novo_zz3 + novo_zz2 * 2);
                    glEnd();
    
                    glDisable(GL_BLEND);
                glPopMatrix();
                step++;
            }
        }

        // Normalizacao
        Vetor v1_normalizado, v2_normalizado, v3_normalizado;
        v1_normalizado = normalizaVetor(_xx1, _yy1, _zz1);
        v2_normalizado = normalizaVetor(novo_xx2, novo_yy2, novo_zz2);
        v3_normalizado = normalizaVetor(novo_xx3, novo_yy3, novo_zz3);


        //3o Passo deixa os vetores com opacidade baixa e normaliza o vetor v1, colocando em destaque
        if (step == 2) {
            // Para um melhor efeito na animacao
            if (sleep_flag == 1) {
                Sleep(ESPERA);
                sleep_flag++;
            }
            if (sleep_flag == 0) {
                glLineWidth(1.5); 
                glColor3f(1.0, 1.0, 1.0);
                glLineStipple(2, 0xAAAA);
                glEnable(GL_LINE_STIPPLE);
                glBegin(GL_LINES);
                    glVertex3f(pont_o_xx1, pont_o_yy1, pont_o_zz1);
                    glVertex3f(novo_xx2, b, novo_zz2);
                glEnd();
                glDisable(GL_LINE_STIPPLE);
                sleep_flag++;
            }

            glPushMatrix();
                glColor3f(1.0, 0.4, 0.2); // Vermelho
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
            glPopMatrix();

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

            // DESENHA O PLANO 1
            glPushMatrix();
               glColor4f(1.0, 0.4, 0.2, 0.4f);
               glBlendFunc(GL_SRC_ALPHA,GL_ONE);
               glEnable(GL_BLEND);

                glBegin(GL_QUADS);
                    glVertex3f((novo_xx3 - novo_xx2) * 2,(novo_yy3 - novo_yy2) * 2, (novo_zz3 - novo_zz2) * 2);
                    glVertex3f((novo_xx3 + novo_xx2) * (-1) * 2, (novo_yy3 + novo_yy2)  * (-1) * 2, (novo_zz3 + novo_zz2)  * (-1) * 2);
                    glVertex3f((novo_xx3 - novo_xx2) * (-1) * 2,(novo_yy3 - novo_yy2) * (-1) * 2, (novo_zz3 - novo_zz2) * (-1) * 2);
                    glVertex3f(novo_xx3 + novo_xx2 * 2, novo_yy3 + novo_yy2 * 2, novo_zz3 + novo_zz2 * 2);
                glEnd();

                glDisable(GL_BLEND);
            glPopMatrix();

            if (norma(_xx1, _yy1, _zz1) < norma(v1_normalizado.vetor[0], v1_normalizado.vetor[1], v1_normalizado.vetor[2])) {
                if (_xx1_norm < v1_normalizado.vetor[0]) _xx1_norm = _xx1_norm*1.001;
                if (_xx1_norm > v1_normalizado.vetor[0]) _xx1_norm = _xx1_norm/0.999;
                if (_yy1_norm < v1_normalizado.vetor[1]) _yy1_norm = _yy1_norm*1.001;
                if (_yy1_norm > v1_normalizado.vetor[1]) _yy1_norm = _yy1_norm/0.999;
                if (_zz1_norm < v1_normalizado.vetor[2]) _zz1_norm = _zz1_norm*1.001;
                if (_zz1_norm > v1_normalizado.vetor[2]) _zz1_norm = _zz1_norm/0.999;
            }
            else {
                if (_xx1_norm < v1_normalizado.vetor[0]) _xx1_norm = _xx1_norm/1.001;
                if (_xx1_norm > v1_normalizado.vetor[0]) _xx1_norm = _xx1_norm*0.999;
                if (_yy1_norm < v1_normalizado.vetor[1]) _yy1_norm = _yy1_norm/1.001;
                if (_yy1_norm > v1_normalizado.vetor[1]) _yy1_norm = _yy1_norm*0.999;
                if (_zz1_norm < v1_normalizado.vetor[2]) _zz1_norm = _zz1_norm/1.001;
                if (_zz1_norm > v1_normalizado.vetor[2]) _zz1_norm = _zz1_norm*0.999;
            }

            if (fabs(v1_normalizado.vetor[0] - _xx1_norm) < 0.25 && fabs(v1_normalizado.vetor[1] - _yy1_norm) < 0.25 && fabs(v1_normalizado.vetor[2] - _zz1_norm) < 0.25) {
               step++;
            }

        }

        //4o Passo desenha os vetores e o primeiro plano destacando o vetor vermelho
        if (step == 3) {
            sleep_flag = 0;

            glPushMatrix();
                glColor3f(1.0, 0.4, 0.2); // Vermelho
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
            glPopMatrix();

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

            // DESENHA O PLANO 1
            glPushMatrix();
               glColor4f(1.0, 0.4, 0.2, 0.4f);
               glBlendFunc(GL_SRC_ALPHA,GL_ONE);
               glEnable(GL_BLEND);

                glBegin(GL_QUADS);
                    glVertex3f((novo_xx3 - novo_xx2) * 2,(novo_yy3 - novo_yy2) * 2, (novo_zz3 - novo_zz2) * 2);
                    glVertex3f((novo_xx3 + novo_xx2) * (-1) * 2, (novo_yy3 + novo_yy2)  * (-1) * 2, (novo_zz3 + novo_zz2)  * (-1) * 2);
                    glVertex3f((novo_xx3 - novo_xx2) * (-1) * 2,(novo_yy3 - novo_yy2) * (-1) * 2, (novo_zz3 - novo_zz2) * (-1) * 2);
                    glVertex3f(novo_xx3 + novo_xx2 * 2, novo_yy3 + novo_yy2 * 2, novo_zz3 + novo_zz2 * 2);
                glEnd();

                glDisable(GL_BLEND);
            glPopMatrix();

            step++;
        }

        //5o Passo desenha o plano e atualiza o vetor amarelo (destaque para os vetores vermelho e amarelo)
        if (step == 4) {
            if (sleep_flag == 0) {
                Sleep(ESPERA);
                sleep_flag++;
            }
            glPushMatrix();
                glColor3f(1.0, 0.4, 0.2); // Vermelho
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
            glPopMatrix();

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

            // DESENHA O PLANO 1
            glPushMatrix();
               glColor4f(1.0, 0.4, 0.2, 0.4f);
               glBlendFunc(GL_SRC_ALPHA,GL_ONE);
               glEnable(GL_BLEND);

                glBegin(GL_QUADS);
                    glVertex3f((novo_xx3 - novo_xx2) * 2,(novo_yy3 - novo_yy2) * 2, (novo_zz3 - novo_zz2) * 2);
                    glVertex3f((novo_xx3 + novo_xx2) * (-1) * 2, (novo_yy3 + novo_yy2)  * (-1) * 2, (novo_zz3 + novo_zz2)  * (-1) * 2);
                    glVertex3f((novo_xx3 - novo_xx2) * (-1) * 2,(novo_yy3 - novo_yy2) * (-1) * 2, (novo_zz3 - novo_zz2) * (-1) * 2);
                    glVertex3f(novo_xx3 + novo_xx2 * 2, novo_yy3 + novo_yy2 * 2, novo_zz3 + novo_zz2 * 2);
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
           }
        }

        // 6o Passo remove o plano e destaca apenas o vetor amarelo
        if (step == 5) {
            sleep_flag = 0;

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

            step++;
        }

        // 7o Passo normaliza o vetor amarelo
        if (step == 6) {
            if (sleep_flag == 0) {
                Sleep(ESPERA);
                sleep_flag++;
            }

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

            if (norma(novo_xx2, novo_yy2, novo_zz2) < norma(v2_normalizado.vetor[0], v2_normalizado.vetor[1], v2_normalizado.vetor[2])) {
                if (_xx2_norm < v2_normalizado.vetor[0]) _xx2_norm = _xx2_norm*1.001;
                if (_xx2_norm > v2_normalizado.vetor[0]) _xx2_norm = _xx2_norm/0.999;
                if (_yy2_norm < v2_normalizado.vetor[1]) _yy2_norm = _yy2_norm*1.001;
                if (_yy2_norm > v2_normalizado.vetor[1]) _yy2_norm = _yy2_norm/0.999;
                if (_zz2_norm < v2_normalizado.vetor[2]) _zz2_norm = _zz2_norm*1.001;
                if (_zz2_norm > v2_normalizado.vetor[2]) _zz2_norm = _zz2_norm/0.999;
            }
            else {
                if (_xx2_norm < v2_normalizado.vetor[0]) _xx2_norm = _xx2_norm/1.001;
                if (_xx2_norm > v2_normalizado.vetor[0]) _xx2_norm = _xx2_norm*0.999;
                if (_yy2_norm < v2_normalizado.vetor[1]) _yy2_norm = _yy2_norm/1.001;
                if (_yy2_norm > v2_normalizado.vetor[1]) _yy2_norm = _yy2_norm*0.999;
                if (_zz2_norm < v2_normalizado.vetor[2]) _zz2_norm = _zz2_norm/1.001;
                if (_zz2_norm > v2_normalizado.vetor[2]) _zz2_norm = _zz2_norm*0.999;
            }

            if (fabs(v2_normalizado.vetor[0] - _xx2_norm) < 0.25 && fabs(v2_normalizado.vetor[1] - _yy2_norm) < 0.25 && fabs(v2_normalizado.vetor[2] - _zz2_norm) < 0.25) {
               step++;
            }

        }

        // 8o Passo destaca o vetor vermelho e amarelo e desenha um plano entre eles
        if (step == 7) {
            sleep_flag = 0;

            glPushMatrix();
                glColor4f(1.0, 0.4, 0.2, 0.3f); // Vermelho
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
            glPopMatrix();

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

            // DESENHA O PLANO 2
            glPushMatrix();
               glColor4f(1.0, 1.0, 1.0, 0.4f);
               glBlendFunc(GL_SRC_ALPHA,GL_ONE);
               glEnable(GL_BLEND);
                glBegin(GL_QUADS);
                    glVertex3f((_xx1_norm - _xx2_norm),(_yy1_norm - _yy2_norm), (_zz1_norm - _zz2_norm));
                    glVertex3f((_xx1_norm + _xx2_norm) * (-1), (_yy1_norm + _yy2_norm)  * (-1), (_zz1_norm + _zz2_norm)  * (-1));
                    glVertex3f((_xx1_norm - _xx2_norm) * (-1),(_yy1_norm - _yy2_norm) * (-1), (_zz1_norm - _zz2_norm) * (-1));
                    glVertex3f(_xx1_norm + _xx2_norm, _yy1_norm + _yy2_norm, _zz1_norm + _zz2_norm);
                glEnd();
                glDisable(GL_BLEND);
            glPopMatrix();

            step++;
        }

        // 9o Passo destaca o vetor azul e coloca na nova posicao
        if (step == 8) {
            if (sleep_flag == 0) {
                Sleep(ESPERA);
                sleep_flag++;
            }

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
                renderVector(x1, y1, z1, _xx3, _yy3, _zz3, radius, 32, quadricFade3);
            glPopMatrix();

            // DESENHA O PLANO 2
            glPushMatrix();
               glColor4f(1.0, 1.0, 1.0, 0.4f);
               glBlendFunc(GL_SRC_ALPHA,GL_ONE);
               glEnable(GL_BLEND);
                glBegin(GL_QUADS);
                    glVertex3f((_xx1_norm - _xx2_norm),(_yy1_norm - _yy2_norm), (_zz1_norm - _zz2_norm));
                    glVertex3f((_xx1_norm + _xx2_norm) * (-1), (_yy1_norm + _yy2_norm)  * (-1), (_zz1_norm + _zz2_norm)  * (-1));
                    glVertex3f((_xx1_norm - _xx2_norm) * (-1),(_yy1_norm - _yy2_norm) * (-1), (_zz1_norm - _zz2_norm) * (-1));
                    glVertex3f(_xx1_norm + _xx2_norm, _yy1_norm + _yy2_norm, _zz1_norm + _zz2_norm);
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

        // 10o Passo rotaciona a camera, mantendo o destaque ao vetor azul

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
                renderVector(x1, y1, z1, _xx3, _yy3, _zz3, radius, 32, quadricFade3);
            glPopMatrix();

            // DESENHA O PLANO 2
            glPushMatrix();
               glColor4f(1.0, 1.0, 1.0, 0.4f);
               glBlendFunc(GL_SRC_ALPHA,GL_ONE);
               glEnable(GL_BLEND);
                glBegin(GL_QUADS);
                    glVertex3f((_xx1_norm - _xx2_norm),(_yy1_norm - _yy2_norm), (_zz1_norm - _zz2_norm));
                    glVertex3f((_xx1_norm + _xx2_norm) * (-1), (_yy1_norm + _yy2_norm)  * (-1), (_zz1_norm + _zz2_norm)  * (-1));
                    glVertex3f((_xx1_norm - _xx2_norm) * (-1),(_yy1_norm - _yy2_norm) * (-1), (_zz1_norm - _zz2_norm) * (-1));
                    glVertex3f(_xx1_norm + _xx2_norm, _yy1_norm + _yy2_norm, _zz1_norm + _zz2_norm);
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

        // 11o Passo normaliza o vetor azul
        if (step == 10) {
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

            if (norma(novo_xx3, novo_yy3, novo_zz3) < norma(v3_normalizado.vetor[0], v3_normalizado.vetor[1], v3_normalizado.vetor[2])) {
                if (_xx3_norm < v3_normalizado.vetor[0]) _xx3_norm = _xx3_norm*1.001;
                if (_xx3_norm > v3_normalizado.vetor[0]) _xx3_norm = _xx3_norm/0.999;
                if (_yy3_norm < v3_normalizado.vetor[1]) _yy3_norm = _yy3_norm*1.001;
                if (_yy3_norm > v3_normalizado.vetor[1]) _yy3_norm = _yy3_norm/0.999;
                if (_zz3_norm < v3_normalizado.vetor[2]) _zz3_norm = _zz3_norm*1.001;
                if (_zz3_norm > v3_normalizado.vetor[2]) _zz3_norm = _zz3_norm/0.999;
            }
            else {
                if (_xx3_norm < v3_normalizado.vetor[0]) _xx3_norm = _xx3_norm/1.001;
                if (_xx3_norm > v3_normalizado.vetor[0]) _xx3_norm = _xx3_norm*0.999;
                if (_yy3_norm < v3_normalizado.vetor[1]) _yy3_norm = _yy3_norm/1.001;
                if (_yy3_norm > v3_normalizado.vetor[1]) _yy3_norm = _yy3_norm*0.999;
                if (_zz3_norm < v3_normalizado.vetor[2]) _zz3_norm = _zz3_norm/1.001;
                if (_zz3_norm > v3_normalizado.vetor[2]) _zz3_norm = _zz3_norm*0.999;
            }

            if (fabs(v3_normalizado.vetor[0] - _xx3_norm) < 0.25 && fabs(v3_normalizado.vetor[1] - _yy3_norm) < 0.25 && fabs(v3_normalizado.vetor[2] - _zz3_norm) < 0.25) {
                sprintf(str1,"q1: (%.3f, %.3f, %.3f)", v1_normalizado.vetor[0]/2, v1_normalizado.vetor[1]/2, v1_normalizado.vetor[2]/2);
                sprintf(str2,"q2: (%.3f, %.3f, %.3f)", v2_normalizado.vetor[0]/2, v2_normalizado.vetor[1]/2, v2_normalizado.vetor[2]/2);
                sprintf(str3,"q3: (%.3f, %.3f, %.3f)", v3_normalizado.vetor[0]/2, v3_normalizado.vetor[1]/2, v3_normalizado.vetor[2]/2);
                step++;
            }

        }

        // 12o Passo desenha todos os vetores em destaque
        if (step == 11) {
            glPushMatrix();
                glColor3f(1.0, 0.4, 0.2); // Vermelho
                GLUquadricObj *quadric=gluNewQuadric();
                gluQuadricNormals(quadric, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx1_norm, _yy1_norm, _zz1_norm, radius, 32, quadric);
                output(-5.5, 6.0, str1);
            glPopMatrix();

            glPushMatrix();
                glColor3f(1.0, 1.0, 0.2); // Amarelo
                GLUquadricObj *quadric2=gluNewQuadric();
                gluQuadricNormals(quadric2, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx2_norm, _yy2_norm, _zz2_norm, radius, 32, quadric2);
                output(-5.5, 5.2, str2);
            glPopMatrix();

            glPushMatrix();
                glColor3f(0.0, 0.2, 0.7); // Azul
                GLUquadricObj *quadricFade3=gluNewQuadric();
                gluQuadricNormals(quadricFade3, GLU_SMOOTH);
                renderVector(x1, y1, z1, _xx3_norm, _yy3_norm, _zz3_norm, radius, 32, quadricFade3);
                output(-5.5, 4.4, str3);
            glPopMatrix();
        }

        glutPostRedisplay();
    }
    // Limpa o pipeline, troca o buffer
    glFlush();
    glutSwapBuffers();

}

void drawSceneError(void) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; // Cor(0.2, 0.2, 0.2)
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    // Rotaciona a imagem
    glMatrixMode(GL_MODELVIEW); // Matriz atual afeta a posicao dos objetos
    glLoadIdentity(); // Inicializa para a identidade

    // Zoom da camera
    glScalef(rotate_x, rotate_x, 1.0f);
    //glScalef(1.0f, 1.0f, rotate_x);
    glRotatef(rotate_by_key, -1.0f, 1.5f, -5.0f);

    glTranslatef(0.0, -0.3, -35.0); // Translada a partir da origem (em frente ao espectador)
    glRotatef(RotateAngle, 0.0, 1.0, 0.0); // Rotaciona ao redor do eixo y
    glRotatef(Azimuth, 1.0, 0.0, 0.0); // Define o angulo azimuth (distancia angular medida sobre o horizonte)
    glDisable(GL_CULL_FACE);

    glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
    	output(-2.7, 1.0, "Algoritmo de Gram-Schmidt");
        glColor3f(0.6f, 0.6f, 0.6f);
    	output(-6.5, -1.5, "Os vetores escolhidos para serem analizados nao sao LI...");
    	output(-6.5, -2, "Favor altera-los por vetores LI e executar novamente o programa...");
    glPopMatrix();

    glutPostRedisplay();

    // Limpa o pipeline, troca o buffer
    glFlush();
    glutSwapBuffers();

}

// Inicializa os modos de renderizacao do OpenGL
void initRendering() {
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
// Chamada quando a janela é redimensionada
// w, h - largura e altura da janela em pixels.
void resizeWindow(int w, int h) {
    double aspectRatio;
    // Define a porcao da janela usada para renderizacao do OpenGL.
    glViewport(0, 0, w, h); // View port usa toda a janela
    // Define a matriz de projecao: projecao perspectiva
    // Determina os valores min e max para x e y que devem aparecer na janela.
    // A complicacao é que a relacao de aspecto da janela pode nao corresponder a
    // relacao de aspecto da scena que queremos ver.
    w = (w == 0) ? 1 : w;
    h = (h == 0) ? 1 : h;
    aspectRatio = (double)w / (double)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(15.0, aspectRatio, 25.0, 45.0);
}

//TODO Adicionar as validacoes aqui para nao poluir o codigo
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
    // Adicionar validacoes:
    // Entrada nula
    // Vetores com valores invalidos ou muito divergentes
    readParameters();

    Vetor vetor1, vetor2, vetor3;

    vetor1.inicializaVetor(_xx1, _yy1, _zz1);
    vetor2.inicializaVetor(_xx2, _yy2, _zz2);
    vetor3.inicializaVetor(_xx3, _yy3, _zz3);

    inicializacaoGramSchimidt(vetor1, vetor2, vetor3);

}


// Cuida das chamadas via teclado
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

// Setas do teclado para rotacionar a camera
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
    Vetor vetor1, vetor2, vetor3;

// Main
// Executa o Gram-Schmidt e em seguida a animacao
int main(int argc, char** argv) {

    readParameters();

    vetor1.inicializaVetor(_xx1, _yy1, _zz1);
    vetor2.inicializaVetor(_xx2, _yy2, _zz2);
    vetor3.inicializaVetor(_xx3, _yy3, _zz3);

    if(gauss(vetor1,vetor2,vetor3) != true){
        glutInit(&argc, argv);
        // Buffer duplo
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        // Posicao da janela
        glutInitWindowPosition(200, 45);
        // Tamanho da Janela
        glutInitWindowSize(900, 660);
        // Titulo da Janela
        glutCreateWindow("Gram Schmidt");
    
        // Inicializa o OpenGL
        initRendering();
        // Define as funcoes para chamadas do teclado
        glutKeyboardFunc(myKeyboardFunc); // Cuida de simbolos ascii "normais"
        glutSpecialFunc(mySpecialKeyFunc); // Cuida de teclas "especial"
        // Define o callback para redimensionamento da janela
        glutReshapeFunc(resizeWindow);
        
        glutDisplayFunc(drawSceneError);
        glutMainLoop();
    
        return(0);
    }

    inicializacaoGramSchimidt(vetor1, vetor2, vetor3);

    glutInit(&argc, argv);
    // Buffer duplo
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // Posicao da janela
    glutInitWindowPosition(200, 45);
    // Tamanho da Janela
    glutInitWindowSize(900, 660);
    // Titulo da Janela
    glutCreateWindow("Gram Schmidt");

    // Criar menu acessivel atraves do botao direito do mouse
    //int menuRightButton = glutCreateMenu(menu);
    //glutAddMenuEntry("Visualizar Informacoes", 1);
    //glutAddMenuEntry("Iniciar Animacao", 2);
    //glutAddMenuEntry("Fechar Aplicacao", 3);
    //glutAttachMenu(GLUT_RIGHT_BUTTON);

    // Inicializa o OpenGL
    initRendering();
    // Define as funcoes para chamadas do teclado
    glutKeyboardFunc(myKeyboardFunc); // Cuida de simbolos ascii "normais"
    glutSpecialFunc(mySpecialKeyFunc); // Cuida de teclas "especial"
    // Define o callback para redimensionamento da janela
    glutReshapeFunc(resizeWindow);
    // Chamar para processamento em background
    // glutIdleFunc( myIdleFunction );

    glutDisplayFunc(drawScene);

    // Inicia o loop principal. glutMainLoop nunca retorna.
    glutMainLoop();

    return(0); // Essa linha nunca é chamada.
}



