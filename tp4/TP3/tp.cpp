// -------------------------------------------
// gMini : a minimal OpenGL/GLUT application
// for 3D graphics.
// Copyright (C) 2006-2008 Tamy Boubekeur
// All rights reserved.
// -------------------------------------------

// -------------------------------------------
// Disclaimer: this code is dirty in the
// meaning that there is no attention paid to
// proper class attribute access, memory
// management or optimisation of any kind. It
// is designed for quick-and-dirty testing
// purpose.
// -------------------------------------------

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include <GL/glut.h>
#include <float.h>
#include "src/Vec3.h"
#include "src/Camera.h"
#include "src/FastNoiseLit.h"
#include <math.h>


struct Triangle {
    inline Triangle () {
        v[0] = v[1] = v[2] = 0;
    }
    inline Triangle (const Triangle & t) {
        v[0] = t.v[0];   v[1] = t.v[1];   v[2] = t.v[2];
    }
    inline Triangle (unsigned int v0, unsigned int v1, unsigned int v2) {
        v[0] = v0;   v[1] = v1;   v[2] = v2;
    }
    unsigned int & operator [] (unsigned int iv) { return v[iv]; }
    unsigned int operator [] (unsigned int iv) const { return v[iv]; }
    inline virtual ~Triangle () {}
    inline Triangle & operator = (const Triangle & t) {
        v[0] = t.v[0];   v[1] = t.v[1];   v[2] = t.v[2];
        return (*this);
    }
    // membres :
    unsigned int v[3];
};


struct Mesh {
    std::vector< Vec3 > vertices;
    std::vector< Vec3 > normals;
    std::vector< Triangle > triangles;
    std::vector< Vec3 > colors;
};

Mesh mesh;

//Meshes to generate
Mesh unit_sphere;
Mesh tesselation;

bool display_normals;
bool display_loaded_mesh;
bool display_unit_sphere;
bool display_tesselation;

// -------------------------------------------
// OpenGL/GLUT application code.
// -------------------------------------------

static GLint window;
static unsigned int SCREENWIDTH = 1600;
static unsigned int SCREENHEIGHT = 900;
static Camera camera;
static bool mouseRotatePressed = false;
static bool mouseMovePressed = false;
static bool mouseZoomPressed = false;
static int lastX=0, lastY=0, lastZoom=0;
static bool fullScreen = false;
static FastNoiseLite noise;






    








// ------------------------------------

void initLight () {
    GLfloat light_position1[4] = {22.0f, 16.0f, 50.0f, 0.0f};
    GLfloat direction1[3] = {-52.0f,-16.0f,-50.0f};
    GLfloat color1[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat ambient[4] = {0.3f, 0.3f, 0.3f, 0.5f};

    glLightfv (GL_LIGHT1, GL_POSITION, light_position1);
    glLightfv (GL_LIGHT1, GL_SPOT_DIRECTION, direction1);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, color1);
    glLightfv (GL_LIGHT1, GL_SPECULAR, color1);
    glLightModelfv (GL_LIGHT_MODEL_AMBIENT, ambient);
    glEnable (GL_LIGHT1);
    glEnable (GL_LIGHTING);
}

void init () {
    camera.resize (SCREENWIDTH, SCREENHEIGHT);
    glCullFace (GL_BACK);
    //glEnable (GL_CULL_FACE);
    glDepthFunc (GL_LESS);
    glEnable (GL_DEPTH_TEST);
    glClearColor (0.2f, 0.2f, 0.3f, 1.0f);
    glEnable(GL_COLOR_MATERIAL);

    display_normals = false;
    display_unit_sphere = false;
    display_loaded_mesh = true;
    display_tesselation = false;
}




// ------------------------------------
// rendering.
// ------------------------------------


void drawVector( Vec3 const & i_from, Vec3 const & i_to ) {

    glBegin(GL_LINES);
    glVertex3f( i_from[0] , i_from[1] , i_from[2] );
    glVertex3f( i_to[0] , i_to[1] , i_to[2] );
    glEnd();
}




void draw () {
    
}

void DrawCurve(const std::vector <Vec3> TabPointsOfCurve, int nbPoints){
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<nbPoints;i++){
        glVertex3f(TabPointsOfCurve[i][0],TabPointsOfCurve[i][1],TabPointsOfCurve[i][2]);  
    }
    glEnd();
}

float F1(float u){
    return 2*u*u*u-3*u*u+1;
}

float F2(float u){
    return -2*u*u*u+3*u*u;
}

float F3(float u){
    return u*u*u-2*u*u+u;
}

float F4(float u){
    return u*u*u-3*u*u;
}


void HermiteCubicCurve(Vec3 P0,Vec3 P1, Vec3 V0, Vec3 V1, long nbU){
    float u;
    glBegin(GL_LINE_STRIP);
    glVertex3f(0,0,0);
    for(int i=0;i<nbU;i++){
        u=float(i)/(nbU-1);    
        glVertex3f(F1(u)*P0[0] + F2(u)*P1[0] + F3(u)*V0[0] + F4(u)*V1[0], F1(u)*P0[1] + F2(u)*P1[1] + F3(u)*V0[1] + F4(u)*V1[1], F1(u)*P0[2] + F2(u)*P1[2] + F3(u)*V0[2] + F4(u)*V1[2]);
    }
    glEnd();
    

}

int factoriel(int n){
    return n<1 ? 1 : n*factoriel(n-1);
}




void BezierCurveByBernstein(const std::vector <Vec3> TabControlPoint, long nbControlPoint, long nbU){
    glBegin(GL_LINE_STRIP);
    float n = (float)nbControlPoint;
    float bin;
    float x;
    float y;
    float z;
    for(int i=0;i<nbU;i++){
        float u=float(i)/(nbU-1); 
        x=0;
        y=0;
        z=0;
        for(int j=0;j<nbControlPoint;j++){
            bin=float(factoriel(n-1))/(float(factoriel(j)*factoriel(n-1-j)))*(float)pow(u,j)*(float)pow((1-u),n-1-j);
            x+=bin*TabControlPoint[j][0];
            y+=bin*TabControlPoint[j][1];
            z+=bin*TabControlPoint[j][2];
        }
        glVertex3f(x,y,z);
    }
    glEnd();
}

Vec3 P(std::vector<Vec3> points, unsigned int i, unsigned int k, float u){
    if(k==0)return points[i];
    return (1-u)*P(points,i,k-1,u)+u*P(points,i+1,k-1,u);
}

void BezierCurveByCasteljau(std::vector<Vec3> TabControlPoint, long nbControlPoint, long nbU){
    glBegin(GL_LINE_STRIP);
    Vec3 vec;
    for(int i=0;i<nbU;i++){
        float u= float(i)/(nbU-1);
        vec=P(TabControlPoint,0,nbControlPoint-1,u);
        glVertex3f(vec[0],vec[1],vec[2]); 
    }
    glEnd();
}

void surface_cylindrique(std::vector<Vec3> TabControlPoint,Vec3 A, long nbControlPoint, long nbU){
    //Vec3 tab[11][nbU];
    glBegin(GL_LINE_STRIP);
    glVertex3f(A[0],A[1],A[2]);
    Vec3 vec;
    for(float j=0;j<1.1;j+=0.1){
        glBegin(GL_LINE_STRIP);
        for(int i=0;i<nbU;i++){
            float u= float(i)/(nbU-1);
            vec=P(TabControlPoint,0,nbControlPoint-1,u);
            glVertex3f(vec[0],vec[1],vec[2]+j); 
            //tab[(int)j*10][i]=vec;
        }
        glEnd();

    }
    glEnd();
}


Vec3 interpolationlinéaire(const Vec3& p1, const Vec3& p2, double t) {
    double x = (1.0 - t) * p1[0] + t * p2[0];
    double y = (1.0 - t) * p1[1] + t * p2[1];
    double z = (1.0 - t) * p1[2] + t * p2[2];
    return Vec3(x, y, z);
}


void courbe_regle(std::vector<Vec3> TabControlPoint1,long nbControlPoint1,std::vector<Vec3> TabControlPoint2,long nbControlPoint2, long nbU,long nbV){ // 
    Vec3 TabVecFinal[nbU][nbV];
    std::vector<Vec3> tabVec1; // sauvegarde tous les points de la courbe 1
    glBegin(GL_LINE_STRIP);
    Vec3 vec1;
    for(int i=0;i<nbU;i++){
        float u= float(i)/(nbU-1);
        vec1=P(TabControlPoint1,0,nbControlPoint1-1,u);
        tabVec1.push_back(vec1);
        glVertex3f(vec1[0],vec1[1],vec1[2]); 
    }
    glEnd();
    std::vector<Vec3> tabVec2; // sauvegarde tous les points de la courbe 2
    glBegin(GL_QUADS);
    Vec3 vec2;
    for(int i=0;i<nbU;i++){
        float v= float(i)/(nbU-1);
        vec2=P(TabControlPoint2,0,nbControlPoint2-1,v);
        tabVec2.push_back(vec2);
        glVertex3f(vec2[0],vec2[1],vec2[2]); 
    }
    glEnd();

    Vec3 A ;
    Vec3 B ;
  
    std::vector<Vec3> VecV;
    
    for(int i=0;i<nbU;i++){
        glBegin(GL_LINE_STRIP); 
            
            A=Vec3(tabVec1[i][0],tabVec1[i][1],tabVec1[i][2]);
            B=Vec3(tabVec2[i][0],tabVec2[i][1],tabVec2[i][2]);
            glVertex3f(A[0],A[1],A[2]);
            glVertex3f(B[0],B[1],B[2]);
            for(int j=0;j<nbV;j++){
                
            }
             
        glEnd();
    }

    /*std::vector<Vec3> surfacePoints;

    for (int i = 0; i < nbU; ++i) {
        for (int j = 0; j <= nbV; ++j) {
            float t = float(j) / nbV;
            Vec3 interpolatedPoint =  interpolationlinéaire(tabVec1[i], tabVec2[i], t);
            surfacePoints.push_back(interpolatedPoint);
        }
    }
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<surfacePoints.size();i++){
        glVertex3f(surfacePoints[i][0],surfacePoints[i][1],surfacePoints[i][2]);
    }
    glEnd();
    */
    

  
}



void display () {
    glLoadIdentity ();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.apply ();
    draw ();
    std::vector< Vec3 > tablePoint;
    
    //tablePoint.push_back(Vec3(0.f,0.f,0.f)); // COURBE CUBIQUE DE HERMITE
    //tablePoint.push_back(Vec3(1.f,1.f,1.f));
    //DrawCurve(tablePoint,2);
    //HermiteCubicCurve(Vec3(0.f,0.f,0.f),Vec3(2.f,0.f,0.f),Vec3(1.f,1.f,0.f),Vec3(1.f,-1.f,0.f),20);

    //la droite
    Vec3 A = Vec3(0.f,0.f,1.f);
    long nbControlPoint = 4;

    std::vector< Vec3 > TabControlPoint; //COURBE DE BEZIER PAR BERNSTEIN
    TabControlPoint.push_back(Vec3(0.f,0.f,0.f));
    TabControlPoint.push_back(Vec3(1.f,0.f,0.f));
    TabControlPoint.push_back(Vec3(0.f,1.f,0.f));
    TabControlPoint.push_back(Vec3(1.f,1.f,0.f));
    //BezierCurveByBernstein(TabControlPoint, nbControlPoint, 20);
    //surface_cylindrique(TabControlPoint, A, nbControlPoint, 20); // DECOMMENTER POUR AFFICHER LA SURFACE CYLINDRIQUE

    //std::vector< Vec3 > TabControlPoint2; // COURBE DE BEZIER PAR CASTELJAU
    //TabControlPoint2.push_back(Vec3(0.f,0.f,0.1f));
    //TabControlPoint2.push_back(Vec3(1.f,0.f,0.1f));
    //TabControlPoint2.push_back(Vec3(0.f,1.f,0.1f));
    //TabControlPoint2.push_back(Vec3(1.f,1.f,0.1f));
    //BezierCurveByCasteljau(TabControlPoint2, nbControlPoint, 20);

    std::vector< Vec3 > TabControlPoint3; // COURBE REGLE
    long nbControlPoint3 = 5;
    TabControlPoint3.push_back(Vec3(1.f,0.f,0.f));
    TabControlPoint3.push_back(Vec3(2.f,1.f,1.f));
    TabControlPoint3.push_back(Vec3(1.f,2.f,2.f));
    TabControlPoint3.push_back(Vec3(2.f,2.f,2.f));
    TabControlPoint3.push_back(Vec3(3.f,2.f,3.f));

    courbe_regle(TabControlPoint,nbControlPoint,TabControlPoint3,nbControlPoint3,20,10);

    //Vec3 tab[11][20];
    
    glutSwapBuffers ();
}

void idle () {
    glutPostRedisplay ();
}

void key (unsigned char keyPressed, int x, int y) {
    switch (keyPressed) {
    case 'f':
        if (fullScreen == true) {
            glutReshapeWindow (SCREENWIDTH, SCREENHEIGHT);
            fullScreen = false;
        } else {
            glutFullScreen ();
            fullScreen = true;
        }
        break;


    case 'w':
        GLint polygonMode[2];
        glGetIntegerv(GL_POLYGON_MODE, polygonMode);
        if(polygonMode[0] != GL_FILL)
            glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
        else
            glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
        break;


    case 'n': //Press n key to display normals
        display_normals = !display_normals;
        break;

    case '1': //Toggle loaded mesh display
        display_loaded_mesh = !display_loaded_mesh;
        break;

    case '2': //Toggle unit sphere mesh display
        display_unit_sphere = !display_unit_sphere;
        break;

    case '3': //Toggle tesselation
        display_tesselation = !display_tesselation;
        break;


    default:
        break;
    }
    idle ();
}

void mouse (int button, int state, int x, int y) {
    if (state == GLUT_UP) {
        mouseMovePressed = false;
        mouseRotatePressed = false;
        mouseZoomPressed = false;
    } else {
        if (button == GLUT_LEFT_BUTTON) {
            camera.beginRotate (x, y);
            mouseMovePressed = false;
            mouseRotatePressed = true;
            mouseZoomPressed = false;
        } else if (button == GLUT_RIGHT_BUTTON) {
            lastX = x;
            lastY = y;
            mouseMovePressed = true;
            mouseRotatePressed = false;
            mouseZoomPressed = false;
        } else if (button == GLUT_MIDDLE_BUTTON) {
            if (mouseZoomPressed == false) {
                lastZoom = y;
                mouseMovePressed = false;
                mouseRotatePressed = false;
                mouseZoomPressed = true;
            }
        }
    }
    idle ();
}

void motion (int x, int y) {
    if (mouseRotatePressed == true) {
        camera.rotate (x, y);
    }
    else if (mouseMovePressed == true) {
        camera.move ((x-lastX)/static_cast<float>(SCREENWIDTH), (lastY-y)/static_cast<float>(SCREENHEIGHT), 0.0);
        lastX = x;
        lastY = y;
    }
    else if (mouseZoomPressed == true) {
        camera.zoom (float (y-lastZoom)/SCREENHEIGHT);
        lastZoom = y;
    }
}


void reshape(int w, int h) {
    camera.resize (w, h);
}



int main (int argc, char ** argv) {
    if (argc > 2) {
        exit (EXIT_FAILURE);
    }
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize (SCREENWIDTH, SCREENHEIGHT);
    window = glutCreateWindow ("TP HAI714I");

    
    
    


    


    

    init ();
    glutIdleFunc (idle);
    glutDisplayFunc (display);
    glutKeyboardFunc (key);
    glutReshapeFunc (reshape);
    glutMotionFunc (motion);
    glutMouseFunc (mouse);
    key ('?', 0, 0);

    noise.SetFractalOctaves(10);
    noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
    noise.SetFractalGain(0.5);
    noise.SetFractalLacunarity(1.8f);


    


    //Uncomment to see other meshes
    //openOFF("data/elephant_n.off", mesh.vertices, mesh.normals, mesh.triangles);

    glutMainLoop ();
    return EXIT_SUCCESS;
}

