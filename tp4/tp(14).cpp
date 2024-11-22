

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




Vec3 evaluateBezierCurve(const std::vector<Vec3>& controlPoints, float t) {
    int n = controlPoints.size() - 1; 
    Vec3 pointOnCurve(0, 0, 0);

    for (int i = 0; i <= n; i++) {
        float coef = B(n, i, t); 
       pointOnCurve[0] += controlPoints[i][0] * coef; // x
        pointOnCurve[1] += controlPoints[i][1] * coef; // y
        pointOnCurve[2] += controlPoints[i][2] * coef; // z

    }

    return pointOnCurve;
}


 


void drawCylinderSurface(const std::vector<Vec3>& bezierCurve, Vec3 cylinderDirection, int numPointsU, int numPointsV) {
    glBegin(GL_QUADS);

    for (int i = 0; i < numPointsU - 1; i++) {
        for (int j = 0; j < numPointsV - 1; j++) {
            float u1 = float(i) / float(numPointsU - 1);
            float u2 = float(i + 1) / float(numPointsU - 1);
            float v1 = float(j) / float(numPointsV - 1);
            float v2 = float(j + 1) / float(numPointsV - 1);

            Vec3 bezierPoint1 = evaluateBezierCurve(bezierCurve, u1);
            Vec3 bezierPoint2 = evaluateBezierCurve(bezierCurve, u2);


            Vec3 cylinderPoint1 = bezierPoint1 + v1 * cylinderDirection;
            Vec3 cylinderPoint2 = bezierPoint2 + v1 * cylinderDirection;
            Vec3 cylinderPoint3 = bezierPoint1 + v2 * cylinderDirection;
            Vec3 cylinderPoint4 = bezierPoint2 + v2 * cylinderDirection;

            glVertex3f(cylinderPoint1[0], cylinderPoint1[1], cylinderPoint1[2]);
            glVertex3f(cylinderPoint2[0], cylinderPoint2[1], cylinderPoint2[2]);
            glVertex3f(cylinderPoint4[0], cylinderPoint4[1], cylinderPoint4[2]);
            glVertex3f(cylinderPoint3[0], cylinderPoint3[1], cylinderPoint3[2]);
        }
    }

    glEnd();
}


void drawRuledSurface(std::vector<Vec3> bezierCurve1, std::vector<Vec3> bezierCurve2, int numPointsU, int numPointsV) {
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i < numPointsU - 1; i++) {
        for (int j = 0; j < numPointsV; j++) {
            float u1 = float(i) / float(numPointsU - 1);
            float u2 = float(i + 1) / float(numPointsU - 1);
            float v = float(j) / float(numPointsV - 1);

            // Évaluez les points sur les deux courbes de Bézier
            Vec3 bezierPoint1 = evaluateBezierCurve(bezierCurve1, u1);
            Vec3 bezierPoint2 = evaluateBezierCurve(bezierCurve1, u2);

    
            Vec3 ruledPoint1, ruledPoint2;
            for (int k = 0; k < 3; k++) {
                ruledPoint1[k] = bezierPoint1[k] + v * (bezierCurve2[0][k] - bezierCurve1[0][k]);
                ruledPoint2[k] = bezierPoint2[k] + v * (bezierCurve2[0][k] - bezierCurve1[0][k]);
            }

          

            glColor3f(1.0, 0.0, 1.0);

            // Dessinez 
            glVertex3f(ruledPoint1[0], ruledPoint1[1], ruledPoint1[2]);
            glVertex3f(ruledPoint2[0], ruledPoint2[1], ruledPoint2[2]);
        }
    }

    glEnd();
}
void drawBezierSurface(const std::vector<std::vector<Vec3>>& controlPoints, int numControlPointsU, int numControlPointsV, int numPointsU, int numPointsV) {
    glBegin(GL_QUADS);

    for (int i = 0; i < numPointsU - 1; i++) {
        for (int j = 0; j < numPointsV - 1; j++) {
            float u1 = float(i) / float(numPointsU - 1);
            float u2 = float(i + 1) / float(numPointsU - 1);
            float v1 = float(j) / float(numPointsV - 1);
            float v2 = float(j + 1) / float(numPointsV - 1);

            Vec3 point1, point2, point3, point4;
            point1[0] = point2[0] = point3[0] = point4[0] = 0.0;
            point1[1] = point2[1] = point3[1] = point4[1] = 0.0;
            point1[2] = point2[2] = point3[2] = point4[2] = 0.0;

            for (int k = 0; k < numControlPointsU; k++) {
                for (int l = 0; l < numControlPointsV; l++) {
                    float coefficient1 = B(numControlPointsU - 1, k, u1) * B(numControlPointsV - 1, l, v1);
                    float coefficient2 = B(numControlPointsU - 1, k, u2) * B(numControlPointsV - 1, l, v1);
                    float coefficient3 = B(numControlPointsU - 1, k, u2) * B(numControlPointsV - 1, l, v2);
                    float coefficient4 = B(numControlPointsU - 1, k, u1) * B(numControlPointsV - 1, l, v2);

                    point1[0] += controlPoints[k][l][0] * coefficient1;
                    point1[1] += controlPoints[k][l][1] * coefficient1;
                    point1[2] += controlPoints[k][l][2] * coefficient1;

                    point2[0] += controlPoints[k][l][0] * coefficient2;
                    point2[1] += controlPoints[k][l][1] * coefficient2;
                    point2[2] += controlPoints[k][l][2] * coefficient2;

                    point3[0] += controlPoints[k][l][0] * coefficient3;
                    point3[1] += controlPoints[k][l][1] * coefficient3;
                    point3[2] += controlPoints[k][l][2] * coefficient3;

                    point4[0] += controlPoints[k][l][0] * coefficient4;
                    point4[1] += controlPoints[k][l][1] * coefficient4;
                    point4[2] += controlPoints[k][l][2] * coefficient4;
                }
            }

            // Dessinez le quadrangle
            glColor3f(1.0, 0.5, 0.3);
            glVertex3f(point1[0], point1[1], point1[2]);
            glVertex3f(point2[0], point2[1], point2[2]);
            glVertex3f(point3[0], point3[1], point3[2]);
            glVertex3f(point4[0], point4[1], point4[2]);
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


    {4.f,4.f,1.f},


    {6.f,3.f,7.f}

};

// Vecteur directeur du cylindre

Vec3 cylinderDirection = {0.0, 0.0, 1.0};

 


    //drawCylinderSurface(bezierCurve1,cylinderDirection,numPointsU,numPointsV);


    //drawRuledSurface(bezierCurve1,bezierCurve2,numPointsU,numPointsV);

std::vector<std::vector<Vec3>> controlPoints = {
    {Vec3(3, 4, 0.0), Vec3(6, 2, 8), Vec3(2, 1, 4), Vec3(5, 2, 8)},
    {Vec3(-1.0, -0.5, 0.0), Vec3(-0.5, -0.5, 1.0), Vec3(0.5, -0.5, 1.0), Vec3(1.0, -0.5, 0.0)},
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

 

