/*
** Cloth Simulation
** Author: Arturo Nereu
** Author: Diego Gurrusquieta
** 8 a 12 y 13:17
*/

#include <windows.h>
#include <GL/glut.h>
#include <GL/glui.h>

#include <stdio.h>
#include "Wire.h"
#include "Light.h"
#include "Material.h"

#define MAX_RESOLUTION 80
#define MIN_RESOLUTION 20
#define RESOLUTION_DELTA 10

#define WIRE_SIZE 600

#define SPHERE_RADIUS 100
#define SPHERE_SLICES 20

#define CYLINDER_RADIUS 80
#define CYLINDER_HEIGHT 240
#define CYLINDER_SLICES 10

#define MIN_PARTICLES_PER_HAIR 1
#define MAX_PARTICLES_PER_HAIR 10

#define REFRESH_RATE 60

/********** User IDs for callbacks ********/
#define MESH_RESOLUTION_ID 100
#define TEXTURE_ENABLED_ID 200
#define SHOW_WIRE_FRAME_ID 201
#define FLAT_SHADE_ID 202
#define SHOW_PARTICLES_ID 203
#define ENABLE_LIGHT_ID 300
#define LIGHT_LEVEL_ID 301
#define LIGHT_RED_COMPONENT_ID 302
#define LIGHT_GREEN_COMPONENT_ID 303
#define LIGHT_BLUE_COMPONENT_ID 304
#define GRAVITY_CONSTANT_ID 400
#define GRAVITY_DIRECTION_ID 401
#define KS_ID 402
#define KD_ID 403
#define PARTICLE_RADIUS_ID 404
#define START_ID 500
#define PAUSE_ID 501
#define RESTART_ID 502
#define FIGURE_ID 600

Wire *myWire;

Light *dirLight;

Material *material;

//Info to draw the capsule
//GLUquadricObj *cylinder;
//Vector *p0;
//Vector *p1;

//Hair related variables
float hairEnabled = 0;
int particlesPerHair = MIN_PARTICLES_PER_HAIR;

//Flags to show/hide elements
int showParticles = 1;
int showTexture = 1;
int showWireframe = 0;
int flatShade = 0;
int lightEnabled = 1;
int figure = 0;
bool isPaused = true;

int resolution = MIN_RESOLUTION;

float rotationX=0.0;
float rotationY=0.0;
float prevX=0.0;
float prevY=0.0;
bool mouseDown=false;

//Gravity force
float gravityForce = 10.0f;
Vector *gravityDirection;
float ks = 20;
float kd = 0.2f;
float particleRadius = 5;

//Texture
GLuint texName=0;
unsigned char woodtexture[512][512][3];

//Camera variabless
float global_ambient[4] = {0.0, 0.0, 0.0, 0.0};
float cameraEye[3] = {0.0, 0.0, 1000.0};
float cameraLookAt[4] = {0.0, 0.0, 0.0, 1.0};
float cameraUp[4] = {0.0, 1.0, 0.0};

float viewRotation[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
float lightRotation[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};

//Object position
float objPos[3] = {0.0f, 0.0f, 0.0f};

float fovy = 45.0;
float dNear = 100;
float dFar = 2000;

float intensity = 1.0f;

//Variables used for GLUI
int main_window;
GLUI_Spinner *spinnerResolution, *lightIntensitySpinner, *gravitySpinner, *gravityXSpinner, *gravityYSpinner, *gravityZSpinner
            ,*springKsSpinner, *springKdSpinner, *particleRadiusSpinner;
GLUI_Checkbox *textureEnabledCheckBox, *showWireframeCheckBox, *showFlatShadeCheckBox, *showParticlesCheckBox, *enableLightCheckBox;
GLUI_Scrollbar *redBar, *greenBar, *blueBar;
GLUI_Rotation *lightRotationBall, *sceneRotation;
GLUI_Translation *translationXY, *translationZ, *objTranslationXY, *objTranslationZ;
GLUI_Button *startButton, *pauseButton, *resetButton;
GLUI_RadioGroup *collisionObjecGroup;

void initTexture(){

   /* First, read this simple ppm file in */
   int w, h, d;
   FILE *fp;
   int i, j, k;
   fp = fopen("mantel.ppm","rb");
   fscanf(fp,"%*s ");
   fscanf(fp,"%d %d %d ", &w, &h, &d) ;
   for (i = 0 ; i < w ; i++)
     for (j = 0 ; j < h ; j++)
       for (k = 0 ; k < 3 ; k++)
	     fscanf(fp,"%c",&(woodtexture[i][j][k])) ;
   fclose(fp) ;
    printf("textura leida\n");
   /* Now, set up all the stuff for texturing, per page 368 */
   glGenTextures(1, &texName) ;
   glBindTexture(GL_TEXTURE_2D, texName) ;
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ;
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ;
   glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 512, 512, 0, GL_RGB,
	   GL_UNSIGNED_BYTE, woodtexture);
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

    glMultMatrixf(lightRotation);
    glLightfv(GL_LIGHT0, GL_POSITION, dirLight->pos);

    glLoadIdentity();
    gluLookAt(cameraEye[0], cameraEye[1], cameraEye[2], cameraLookAt[0], cameraLookAt[1], cameraLookAt[2], cameraUp[0], cameraUp[1], cameraUp[2]);

    glMultMatrixf(viewRotation);

    glRotatef(rotationX,1,0,0);
    glRotatef(rotationY,0,1,0);

    myWire->renderTriangles(showWireframe, flatShade);

    if(lightEnabled)
    {
        glEnable(GL_LIGHTING);
    }
    else
    {
        glDisable(GL_LIGHTING);
    }

    if(showParticles)
    {
        myWire->renderParticles();
    }

    glLightfv(GL_LIGHT0, GL_DIFFUSE, dirLight->difusse);

    if(showTexture)
    {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_COLOR_MATERIAL);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_COLOR_MATERIAL);
    }

    if(flatShade)
    {
        glShadeModel(GL_FLAT);
    }
    else
    {
        glShadeModel(GL_SMOOTH);
    }

    //myWire->renderSprings();
    glPushMatrix();
    glTranslatef(objPos[0], objPos[1], objPos[2]);
    glutSolidSphere(SPHERE_RADIUS, SPHERE_SLICES, SPHERE_SLICES);
    glPopMatrix();
/*
    if(showWireframe)
    {
        gluQuadricDrawStyle(cylinder, GLU_LINE);
    }
    else
    {
        gluQuadricDrawStyle(cylinder, GLU_FILL);
    }

    glPushMatrix();
        glRotatef(90, 0, 1, 0);
        glTranslatef(0, 0, -CYLINDER_HEIGHT/2);
        gluSphere(cylinder, CYLINDER_RADIUS, CYLINDER_SLICES, CYLINDER_SLICES);
        gluCylinder(cylinder, CYLINDER_RADIUS, CYLINDER_RADIUS, CYLINDER_HEIGHT, CYLINDER_SLICES, CYLINDER_SLICES);
        glTranslatef(0, 0, CYLINDER_HEIGHT);
        gluSphere(cylinder, CYLINDER_RADIUS, CYLINDER_SLICES, CYLINDER_SLICES);
    glPopMatrix();
*/
    glutSwapBuffers();
}

void init(void)
{
    rotationX = 0;
    rotationY = 0;

    gravityDirection = new Vector(0, -1, 0);

    //Cylinder related
    //cylinder = gluNewQuadric();
    //p0 = new Vector(-120, 0, 0);
    //p1 = new Vector(120, 0, 0);

    //Lighting related
    dirLight = new Light();
    dirLight->setPosition(0, 300, 100);
    dirLight->setAmbient(0, 0, 0);
    dirLight->setDifusse(0.8f, 0.8f, 0.8f);
    dirLight->setSpecular(0.0f, 0.8f, 0.0f);

    material = new Material();
    material->setAmbient(0, 0, 0, 1);
    material->setDifusse(0.7f, 0.7f, 0.7f, 1.0f);
    material->setSpecular(0.01f, 0.01f, 0.01f, 1.0f);
	material->setShininess(32);
/*
	glMaterialfv(GL_FRONT, GL_AMBIENT,  material->ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,  material->difusse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material->specular);
    glMaterialf(GL_FRONT, GL_SHININESS, material->shininess);
*/

    glEnable(GL_NORMALIZE);

	glEnable(GL_DEPTH_TEST);
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    initTexture();
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    //glEnable(GL_COLOR_MATERIAL);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glLightfv(GL_LIGHT0, GL_POSITION, dirLight->pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,   dirLight->ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR,  dirLight->specular);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,   dirLight->difusse);

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

void control_cb(int control)
{
    if(control == MESH_RESOLUTION_ID)
    {
        initWire();
    }
    else if(control == LIGHT_LEVEL_ID)
    {
        float diff1 = dirLight->difusse[0] + intensity;
        float diff2 = dirLight->difusse[1] + intensity;
        float diff3 = dirLight->difusse[2] + intensity;

        dirLight->setDifusse(diff1, diff2, diff3);
    }
    else if(control == START_ID)
    {
        isPaused = false;
        spinnerResolution->disable();
        collisionObjecGroup->disable();
    }
    else if(control == PAUSE_ID)
    {
        isPaused = true;
        spinnerResolution->enable();
        collisionObjecGroup->enable();
    }
    else if(control == RESTART_ID)
    {
        initWire();
    }

    glutPostRedisplay();
}

void glui()
{
    printf("GLUI version: %3.2f\n", GLUI_Master.get_version());

    //Create the right side subwindow for the menu
    GLUI *glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_RIGHT);

    //Create the planel to change the resolution
    GLUI_Panel *resolutionPanel = new GLUI_Panel(glui, "Resolution");
    spinnerResolution = new GLUI_Spinner(resolutionPanel, "Mesh resolution", &resolution, MESH_RESOLUTION_ID, control_cb);
    spinnerResolution->set_int_limits(MIN_RESOLUTION, MAX_RESOLUTION);

    //Create the checkboxes for the rendering
    GLUI_Panel *renderingPanel = new GLUI_Panel(glui, "Rendering");
    textureEnabledCheckBox = new GLUI_Checkbox(renderingPanel, "Texture", &showTexture, TEXTURE_ENABLED_ID, control_cb);
    showWireframeCheckBox = new GLUI_Checkbox(renderingPanel, "Wireframe", &showWireframe, SHOW_WIRE_FRAME_ID, control_cb);
    showFlatShadeCheckBox = new GLUI_Checkbox(renderingPanel, "Flat Shading", &flatShade, FLAT_SHADE_ID, control_cb);
    showParticlesCheckBox = new GLUI_Checkbox(renderingPanel, "Show Particles", &showParticles, SHOW_PARTICLES_ID, control_cb);

    //Create the UI for the lighting
    GLUI_Panel *lightPanel = new GLUI_Panel(glui, "Lighting");
    enableLightCheckBox = new GLUI_Checkbox(lightPanel, "Enable Light", &lightEnabled, ENABLE_LIGHT_ID, control_cb);
    lightIntensitySpinner = new GLUI_Spinner(lightPanel, "Light Intensity", &intensity, LIGHT_LEVEL_ID, control_cb);
    lightIntensitySpinner->set_int_limits(0.0, 1.0);

    redBar = new GLUI_Scrollbar(lightPanel, "Red Component", GLUI_SCROLL_HORIZONTAL, &dirLight->difusse[0], LIGHT_RED_COMPONENT_ID, control_cb);
    redBar->set_float_limits(0.0, 1.0);
    greenBar = new GLUI_Scrollbar(lightPanel, "Green Component", GLUI_SCROLL_HORIZONTAL, &dirLight->difusse[1], LIGHT_GREEN_COMPONENT_ID, control_cb);
    greenBar->set_float_limits(0.0, 1.0);
    blueBar = new GLUI_Scrollbar(lightPanel, "Blue Component", GLUI_SCROLL_HORIZONTAL, &dirLight->difusse[2], LIGHT_BLUE_COMPONENT_ID, control_cb);
    blueBar->set_float_limits(0.0, 1.0);

    lightRotationBall = new GLUI_Rotation(lightPanel, "Light rotation", lightRotation);
    lightRotationBall->set_spin(1.0);

    GLUI_Panel *scenePanel = new GLUI_Panel(glui, "Scene");
    sceneRotation = new GLUI_Rotation(scenePanel, "Rotate Scene", viewRotation);
    translationXY = new GLUI_Translation(scenePanel, "XY", GLUI_TRANSLATION_XY, cameraEye);
    translationXY->set_speed(5);
    translationZ = new GLUI_Translation(scenePanel, "Z", GLUI_TRANSLATION_Z, cameraEye);
    translationZ->set_speed(5);

    glui->set_main_gfx_window(main_window);
    //Let´s make a panel at the bottom for the physics
    GLUI *gluiPhys = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_BOTTOM);
    gluiPhys->set_main_gfx_window(main_window);
    GLUI_Panel *physicsPanel = new GLUI_Panel(gluiPhys, "Physics");
    gravitySpinner = new GLUI_Spinner(physicsPanel, "Gravity force", &gravityForce, GRAVITY_CONSTANT_ID, control_cb);
    gravitySpinner->set_float_limits(0, 20.0f);
    gravityXSpinner = new GLUI_Spinner(physicsPanel, "X Direction", &gravityDirection->x, GRAVITY_DIRECTION_ID, control_cb);
    gravityXSpinner->set_float_limits(-1.0f, 1.0f);
    gravityYSpinner = new GLUI_Spinner(physicsPanel, "Y Direction", &gravityDirection->y, GRAVITY_DIRECTION_ID, control_cb);
    gravityYSpinner->set_float_limits(-1.0f, 1.0f);
    gravityZSpinner = new GLUI_Spinner(physicsPanel, "Z Direction", &gravityDirection->z, GRAVITY_DIRECTION_ID, control_cb);
    gravityZSpinner->set_float_limits(-1.0f, 1.0f);

    glui->add_column_to_panel(physicsPanel, true);
    springKsSpinner = new GLUI_Spinner(physicsPanel, "Spring KS", &ks, KS_ID, control_cb);
    springKsSpinner->set_float_limits(10, 20);
    springKdSpinner = new GLUI_Spinner(physicsPanel, "Spring KD", &kd, KD_ID, control_cb);
    springKdSpinner->set_float_limits(0.1f, 0.9f);
    particleRadiusSpinner = new GLUI_Spinner(physicsPanel, "Particle radius", &particleRadius, PARTICLE_RADIUS_ID, control_cb);
    particleRadiusSpinner->set_float_limits(0.0f, 10.0f);

    new GLUI_Column(gluiPhys, false);
    //New panel for simulation control
    GLUI_Panel *controlPanel = new GLUI_Panel(gluiPhys, "Simulation control");
    startButton = new GLUI_Button(controlPanel, "Start", START_ID, control_cb);
    pauseButton = new GLUI_Button(controlPanel, "Pause", PAUSE_ID, control_cb);
    resetButton = new GLUI_Button(controlPanel, "Reset", RESTART_ID, control_cb);

    GLUI_Panel *collisionObjectPanel = new GLUI_Panel(gluiPhys, "Collision Object");
    collisionObjecGroup = new GLUI_RadioGroup(collisionObjectPanel, &figure, FIGURE_ID, control_cb);
    new GLUI_RadioButton(collisionObjecGroup, "Sphere");
    new GLUI_RadioButton(collisionObjecGroup, "Capsule");

    new GLUI_Column(gluiPhys, false);
    GLUI_Panel *collisionMovementPanel = new GLUI_Panel(gluiPhys, "Object Movement");
    objTranslationXY = new GLUI_Translation(collisionMovementPanel, "XY", GLUI_TRANSLATION_XY, objPos);
    objTranslationXY->set_speed(5);
    objTranslationZ = new GLUI_Translation(collisionMovementPanel, "Z", GLUI_TRANSLATION_Z, objPos);
    objTranslationZ->set_speed(5);
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
    if(!isPaused)
    {
        myWire->update(SPHERE_RADIUS, gravityForce, *gravityDirection, ks, kd, particleRadius, objPos);
        glutPostRedisplay();
    }

    //Uncoment to use glui
    //glutTimerFunc(60, update, 1);
    GLUI_Master.set_glutTimerFunc(REFRESH_RATE, update, 10);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(800,600);

    main_window = glutCreateWindow("Cloth");

    init();

    //Comment if you dont want to use GLUI
    //glutDisplayFunc(display);
    //glutReshapeFunc(reshape);
    //glutMouseFunc(mouse);
    //glutKeyboardFunc(key);
    //glutTimerFunc(60, update, 10);
    //glutMotionFunc(mouseMotion);

    //Uncoment if you want to use GLUI
    GLUI_Master.set_glutDisplayFunc(display);
    GLUI_Master.set_glutReshapeFunc(reshape);
    GLUI_Master.set_glutKeyboardFunc(key);
    GLUI_Master.set_glutMouseFunc(mouse);
    GLUI_Master.set_glutIdleFunc(NULL);
    GLUI_Master.set_glutTimerFunc(REFRESH_RATE, update, 10);

    glutMotionFunc(mouseMotion);

    initWire();

    glui();
    glutMainLoop();

    return(EXIT_SUCCESS);
}
