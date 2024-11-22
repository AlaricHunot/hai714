
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
        float x=TabPointsOfCurve[i][0];
        float y=TabPointsOfCurve[i][1];
        float z=TabPointsOfCurve[i][2];
        glVertex3f(x,y,z);  
    }
    glEnd();
}

void HermiteCubicCurve(Vec3 P0,Vec3 P1, Vec3 V0, Vec3 V1, long nbU){
    float u;
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<nbU;i++){
        u=float(i)/(nbU-1);  
        float f1=2*pow(u,3)-3*pow(u,2)+1;
        float f2=-2*pow(u,3)+3*pow(u,2);
        float f3=pow(u,3)-2*pow(u,2)+u;
        float f4=pow(u,3)-3*pow(u,2);
        float x=f1*P0[0]+f2*P1[0]+f3*V0[0]+f4*V1[0];
        float y=f1*P0[1]+f2*P1[1]+f3*V0[1]+f4*V1[1];
        float z=f1*P0[2]+f2*P1[2]+f3*V0[2]+f4*V1[2];
        glVertex3f(x,y,z);
        glColor3f(0.f,1.f,0.f);
    }
    glEnd();
    

}

int factoriel(int n)
{
  return (n == 1 || n == 0) ? 1 : factoriel(n - 1) * n;
}
float B(unsigned int n,unsigned int i,float u){
    return float(factoriel(n))/(factoriel(i)*factoriel(n-i))*pow(u,i)*pow(1-u,n-i);
}


void BezierCurveByBernstein(const std::vector <Vec3> TabControlPoint, long nbControlPoint, long nbU){
    glBegin(GL_LINE_STRIP);
    float b,x,y,z;
    for(int i=0;i<nbU;i++){
        float u=float(i)/(nbU-1); 
        x=0;y=0;z=0;
        for(int j=0;j<nbControlPoint;j++){
            b=B(nbControlPoint-1,j,u);
            x+=b*TabControlPoint[j][0];
            y+=b*TabControlPoint[j][1];
            z+=b*TabControlPoint[j][2];
        }
        glVertex3f(x,y,z);
        glColor3f(1.f,0.f,0.f);
    }
    glEnd();
}
Vec3 P(std::vector<Vec3> control, unsigned int i, unsigned int k, float u){
    if(k==0)return control[i];
    return (1-u)*P(control,i,k-1,u)+u*P(control,i+1,k-1,u);
}

void BezierCurveByCasteljau(std::vector<Vec3> TabControlPoint, long nbControlPoint, long nbU){
    glBegin(GL_LINE_STRIP);
    Vec3 res;
    for(int i=0;i<nbU;i++){
        float u= float(i)/(nbU-1);
        res=P(TabControlPoint,0,nbControlPoint-1,u);

        glVertex3f(res[0],res[1],res[2]); 
        glColor3f(1.f,0.f,1.f);
    }
    glEnd();
}



Vec3 evaluateBezierCurve(const std::vector<Vec3>& c, float t) {
    int n = c.size() - 1; 
    Vec3 point(0, 0, 0);

    for (int i = 0; i <= n; i++) {
        float coef = B(n, i, t); 
       point[0] += c[i][0] * coef; // x
        point[1] += c[i][1] * coef; // y
        point[2] += c[i][2] * coef; // z

    }
    return point;
}


 


void drawCylinderSurface(const std::vector<Vec3>& u, Vec3 v, int nbU, int nbV) {
    glBegin(GL_QUADS);

    for (int i = 0; i < nbU - 1; i++) {
        for (int j = 0; j < nbV - 1; j++) {
            float u1 = float(i) / float(nbU - 1);
            float u2 = float(i + 1) / float(nbU - 1);
            float v1 = float(j) / float(nbV - 1);
            float v2 = float(j + 1) / float(nbV - 1);

            Vec3 b1 = evaluateBezierCurve(u, u1);
            Vec3 b2 = evaluateBezierCurve(u, u2);


            Vec3 c1 = b1+v1*v;
            Vec3 c2 = b2+v1*v;
            Vec3 c3 = b1+v2*v;
            Vec3 c4 = b2+v2*v;

            glColor3f(0.0, 0.5, 1);

            glVertex3f(c1[0], c1[1], c1[2]);
            glVertex3f(c2[0], c2[1], c2[2]);
            glVertex3f(c4[0], c4[1], c4[2]);
            glVertex3f(c3[0], c3[1], c3[2]);
        }
    }

    glEnd();
}


void drawRuledSurface(std::vector<Vec3> b1, std::vector<Vec3> b2, int nbU, int nbV) {
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i < nbU - 1; i++) {
        for (int j = 0; j < nbV; j++) {
            float u1 = float(i) / float(nbU - 1);
            float u2 = float(i + 1) / float(nbU - 1);
            float v = float(j) / float(nbV - 1);

            // Évaluez les points sur les deux courbes de Bézier
            Vec3 bPoint1 = evaluateBezierCurve(b1, u1);
            Vec3 bPoint2 = evaluateBezierCurve(b1, u2);

    
            Vec3 r1, r2;
            for (int k = 0; k < 3; k++) {
                r1[k] = bPoint1[k] + v * (b2[0][k] - b1[0][k]);
                r2[k] = bPoint2[k] + v * (b2[0][k] - b1[0][k]);
            }

          

            glColor3f(1.0, 0.0, 1.0);

            glVertex3f(r1[0], r1[1], r1[2]);
            glVertex3f(r2[0], r2[1], r2[2]);
        }
    }

    glEnd();
}
void drawBezierSurface(const std::vector<std::vector<Vec3>>& u, int ncU, int ncV, int nbU, int nbV) {
    glBegin(GL_QUADS);

    for (int i = 0; i < nbU - 1; i++) {
        for (int j = 0; j < nbV - 1; j++) {
            float u1 = float(i) / float(nbU - 1);
            float u2 = float(i + 1) / float(nbU - 1);
            float v1 = float(j) / float(nbV - 1);
            float v2 = float(j + 1) / float(nbV - 1);

            Vec3 p1, p2, p3, p4;
            p1[0] = p2[0] = p3[0] = p4[0] = 0.0;
            p1[1] = p2[1] = p3[1] = p4[1] = 0.0;
            p1[2] = p2[2] = p3[2] = p4[2] = 0.0;

            for (int k = 0; k < ncU; k++) {
                for (int l = 0; l < ncV; l++) {
                    float coef1 = B(ncU - 1, k, u1) * B(ncV - 1, l, v1);
                    float coef2 = B(ncU - 1, k, u2) * B(ncV - 1, l, v1);
                    float coef3 = B(ncU - 1, k, u2) * B(ncV - 1, l, v2);
                    float coef4 = B(ncU - 1, k, u1) * B(ncV - 1, l, v2);

                    p1[0] += u[k][l][0] * coef1;
                    p1[1] += u[k][l][1] * coef1;
                    p1[2] += u[k][l][2] * coef1;

                    p2[0] += u[k][l][0] * coef2;
                    p2[1] += u[k][l][1] * coef2;
                    p2[2] += u[k][l][2] * coef2;

                    p3[0] += u[k][l][0] * coef3;
                    p3[1] += u[k][l][1] * coef3;
                    p3[2] += u[k][l][2] * coef3;

                    p4[0] += u[k][l][0] * coef4;
                    p4[1] += u[k][l][1] * coef4;
                    p4[2] += u[k][l][2] * coef4;
                }
            }

            // Dessinez le quadrangle
            glColor3f(1.0, 0.5, 0.3);
            glVertex3f(p1[0], p1[1], p1[2]);
            glVertex3f(p2[0], p2[1], p2[2]);
            glVertex3f(p3[0], p3[1], p3[2]);
            glVertex3f(p4[0], p4[1], p4[2]);
        }
    }

    glEnd();
}




void display () {

    glLoadIdentity ();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.apply ();
    draw ();
    int numPointsU = 100;
    int numPointsV = 20; 
    std::vector<Vec3>  bezierControlPoints = {
    {0.f,0.f,0.f},
    {1.f,3.f,0.f},
    {2.f,6.f,0.f},
    {5.f,1.f,0.f}

    };

    std::vector<Vec3> bezierCurve1 = {
    {0.f,0.f,2.f},
    {1.f,3.f,3.f},
    {2.f,6.f,4.f},
    {5.f,1.f,0.f}
    };

 

    std::vector<Vec3> bezierCurve2= {
    {2.f,2.f,0.f},
    {1.f,0.f,6.f},
    {2.f,4.f,1.f},
    {3.f,3.f,7.f}
    };

    // Vecteur directeur du cylindre

    Vec3 cylinderDirection = {0.0, 0.0, 1.0};
    drawCylinderSurface(bezierCurve1,cylinderDirection,numPointsU,numPointsV);
    drawRuledSurface(bezierCurve1,bezierCurve2,numPointsU,numPointsV);
    std::vector<std::vector<Vec3>> controlPoints = {
    {Vec3(-1.0, -0.5, 0.), Vec3(-0.5, -0.5, 1.0), Vec3(0.5, -0.5, 1.0), Vec3(1.0, -0.5, 0.0)},
    {Vec3(-1.0, -0.5, 0.), Vec3(-0.5, -0.5, 1.0), Vec3(0.5, -0.5, 1.0), Vec3(1.0, -0.5, 0.0)},
    {Vec3(-1.0, 0.5, 0.0), Vec3(-0.5, 0.5, 1.0), Vec3(0.5, 0.5, 1.0), Vec3(1.0, 0.5, 0.0)},
    {Vec3(-1.0, 1.0, 0.0), Vec3(-0.5, 1.0, 1.0), Vec3(0.5, 1.0, 1.0), Vec3(1.0, 1.0, 0.0)}
    };
    drawBezierSurface(controlPoints,4,4,20,20);
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
    glutMainLoop ();
    return EXIT_SUCCESS;
}
