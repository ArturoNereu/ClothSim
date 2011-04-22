/*
** Cloth Simulation
** Author: Arturo Nereu
*/

#include <windows.h>
#include <GL/glut.h>

#include <stdio.h>
#include "Wire.h"
#include "Light.h"
#include "Material.h"

#define MAX_RESOLUTION 80
#define MIN_RESOLUTION 10
#define RESOLUTION_DELTA 10

#define WIRE_SIZE 600
Wire *myWire;

Light *dirLight;

Material *material;

//Flags to show/hide elements
bool showParticles = true;
bool showTexture = true;
bool showWireframe = false;
bool flatShade = true;

int resolution = MIN_RESOLUTION;

float rotationX=0.0;
float rotationY=0.0;
float prevX=0.0;
float prevY=0.0;
bool mouseDown=false;

//Texture
GLuint texName=0;
unsigned char woodtexture[512][512][3];

//Camera variables
float global_ambient[4] = {0.0, 0.0, 0.0, 0.0};
float cameraEye[4] = {0.0, 0.0, 1000.0, 1.0};
float cameraLookAt[4] = {0.0, 0.0, 0.0, 1.0};
float cameraUp[4] = {0.0, 1.0, 0.0};

float fovy = 45.0;
float dNear = 0;
float dFar = 500;

void initTexture(){

   /* First, read this simple ppm file in */
   int w, h, d;
   FILE *fp;
   int i, j, k;
   fp = fopen("madera.ppm","rb");
   fscanf(fp,"%*s ");
   fscanf(fp,"%d %d %d ", &w, &h, &d) ;
   for (i = 0 ; i < w ; i++)
     for (j = 0 ; j < h ; j++)
       for (k = 0 ; k < 3 ; k++)
	     fscanf(fp,"%c",&(woodtexture[i][j][k])) ;
   fclose(fp) ;

   /* Now, set up all the stuff for texturing, per page 368 */
   glGenTextures(1, &texName) ;
   glBindTexture(GL_TEXTURE_2D, texName) ;
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ;
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ;
   glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 512, 512, 0, GL_RGB,
	   GL_UNSIGNED_BYTE, woodtexture);
    printf("textura leida");
}

void reshape(int w, int h){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, (GLdouble)w/(GLdouble)h, dNear, dFar);
    glViewport(0, 0, w, h);
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(cameraEye[0], cameraEye[1], cameraEye[2], cameraLookAt[0], cameraLookAt[1], cameraLookAt[2], cameraUp[0], cameraUp[1], cameraUp[2]);

    glRotatef(rotationX,1,0,0);
    glRotatef(rotationY,0,1,0);

    glPushMatrix();
    glTranslatef(0, -250, 0);
    glutSolidSphere(100, 20, 20);
    glPopMatrix();

    if(showParticles)
    {
        myWire->renderParticles();
    }

    if(showTexture)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);

    if(flatShade)
    {
        glShadeModel(GL_FLAT);
    }
    else
    {
        glShadeModel(GL_SMOOTH);
    }

    myWire->renderTriangles(showWireframe);
    //myWire->renderSprings();
    glutSwapBuffers();
}

void init(void)
{
    rotationX = 0;
    rotationY = 0;

    //Lighting related
    dirLight = new Light();
    dirLight->setPosition(0, 500, 0);
    dirLight->setAmbient(0, 0, 0);
    dirLight->setDifusse(0.0f, 0.8f, 0.0f);
    dirLight->setSpecular(0.8f, 0.8f, 0.8f);

    material = new Material();
    material->setAmbient(0, 0, 0, 1);
    material->setDifusse(0.01f, 0.01f, 0.01f, 1.0f);
    material->setSpecular(0.5f, 0.5f, 0.5f, 1.0f);
	material->setShininess(32);

	glMaterialfv(GL_FRONT, GL_AMBIENT,  material->ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,  material->difusse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material->specular);
    glMaterialf(GL_FRONT, GL_SHININESS, material->shininess);

	glEnable(GL_DEPTH_TEST);
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    initTexture();
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glLightfv(GL_LIGHT0, GL_POSITION, dirLight->pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,   dirLight->ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR,  dirLight->difusse);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,   dirLight->specular);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
}

void mouse(int button, int state, int x, int y){
    if(button == GLUT_LEFT_BUTTON && state==GLUT_DOWN){
        mouseDown = true;
        prevX = x - rotationY;
        prevY = y - rotationX;
    }else{
        mouseDown = false;
    }
}

void mouseMotion(int x, int y){
    if(mouseDown){
        rotationX = y - prevY;
        rotationY = x - prevX;
        glutPostRedisplay();
    }
}

void initWire()
{
    myWire = new Wire(resolution, WIRE_SIZE);
}

void key(unsigned char key, int x, int y)
{
   if(key == 'x') cameraEye[0]-= 50;
   if(key == 'X') cameraEye[0]+= 50;
   if(key == 'y') cameraEye[1]-= 50;
   if(key == 'Y') cameraEye[1]+= 50;
   if(key == 'z') cameraEye[2]-= 50;
   if(key == 'Z') cameraEye[2]+= 50;
   if(key == 'p') showParticles = !showParticles;
   if(key == 'P') showParticles = !showParticles;
   if(key == 't' || key == 'T') showTexture = !showTexture;
   if(key == 'w' || key == 'W') showWireframe = !showWireframe;
   if(key == 's' || key == 'S') flatShade = !flatShade;
   if(key == '+')
   {
       if(resolution < MAX_RESOLUTION)
       {
            resolution += RESOLUTION_DELTA;
            initWire();
       }
   }
   if(key == '-')
   {
        if(resolution > MIN_RESOLUTION)
        {
            resolution -= RESOLUTION_DELTA;
            initWire();
        }
   }

   glutPostRedisplay();
}

void update(int i)
{
    myWire->update();

    glutPostRedisplay();
    glutTimerFunc(100, update, 1);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Cloth");
    glutInitWindowSize(800,600);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(key);
    glutTimerFunc(100, update, 100);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    initWire();

    init();

    glutMainLoop();

    return(EXIT_SUCCESS);
}
