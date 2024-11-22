

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

 

void DrawCurve(const std::vector <Vec3> TabpsOfCurve, int nbps){


    glBegin(GL_LINE_STRIP);


    for(int i=0;i<nbps;i++){


        float x=TabpsOfCurve[i][0];


        float y=TabpsOfCurve[i][1];


        float z=TabpsOfCurve[i][2];


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

 

 

void BezierCurveByBernstein(const std::vector <Vec3> TabControlp, long nbControlp, long nbU){


    glBegin(GL_LINE_STRIP);


    float b,x,y,z;


    for(int i=0;i<nbU;i++){


        float u=float(i)/(nbU-1); 


        x=0;y=0;z=0;


        for(int j=0;j<nbControlp;j++){


            b=B(nbControlp-1,j,u);


            x+=b*TabControlp[j][0];


            y+=b*TabControlp[j][1];


            z+=b*TabControlp[j][2];


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

 

void BezierCurveByCasteljau(std::vector<Vec3> TabControlp, long nbControlp, long nbU){


    glBegin(GL_LINE_STRIP);


    Vec3 res;


    for(int i=0;i<nbU;i++){


        float u= float(i)/(nbU-1);


        res=P(TabControlp,0,nbControlp-1,u);

 


        glVertex3f(res[0],res[1],res[2]); 


        glColor3f(1.f,0.f,1.f);

}


    glEnd();

}




Vec3 evaluateBezierCurve(const std::vector<Vec3>& controlps, float t) {
    int n = controlps.size() - 1; 
    Vec3 pOnCurve(0, 0, 0);

    for (int i = 0; i <= n; i++) {
        float coef = B(n, i, t); 
       pOnCurve[0] += controlps[i][0] * coef; // x
        pOnCurve[1] += controlps[i][1] * coef; // y
        pOnCurve[2] += controlps[i][2] * coef; // z

    }

    return pOnCurve;
}


 


void drawCylinderSurface(const std::vector<Vec3>& bezierCurve, Vec3 cylinderDirection, int numpsU, int numpsV) {
    glBegin(GL_QUADS);

    for (int i = 0; i < numpsU - 1; i++) {
        for (int j = 0; j < numpsV - 1; j++) {
            float u1 = float(i) / float(numpsU - 1);
            float u2 = float(i + 1) / float(numpsU - 1);
            float v1 = float(j) / float(numpsV - 1);
            float v2 = float(j + 1) / float(numpsV - 1);

            Vec3 bezierp1 = evaluateBezierCurve(bezierCurve, u1);
            Vec3 bezierp2 = evaluateBezierCurve(bezierCurve, u2);


            Vec3 cylinderp1 = bezierp1 + v1 * cylinderDirection;
            Vec3 cylinderp2 = bezierp2 + v1 * cylinderDirection;
            Vec3 cylinderp3 = bezierp1 + v2 * cylinderDirection;
            Vec3 cylinderp4 = bezierp2 + v2 * cylinderDirection;

            glVertex3f(cylinderp1[0], cylinderp1[1], cylinderp1[2]);
            glVertex3f(cylinderp2[0], cylinderp2[1], cylinderp2[2]);
            glVertex3f(cylinderp4[0], cylinderp4[1], cylinderp4[2]);
            glVertex3f(cylinderp3[0], cylinderp3[1], cylinderp3[2]);
        }
    }

    glEnd();
}


void drawRuledSurface(std::vector<Vec3> bezierCurve1, std::vector<Vec3> bezierCurve2, int numpsU, int numpsV) {
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i < numpsU - 1; i++) {
        for (int j = 0; j < numpsV; j++) {
            float u1 = float(i) / float(numpsU - 1);
            float u2 = float(i + 1) / float(numpsU - 1);
            float v = float(j) / float(numpsV - 1);

            // Évaluez les ps sur les deux courbes de Bézier
            Vec3 bezierp1 = evaluateBezierCurve(bezierCurve1, u1);
            Vec3 bezierp2 = evaluateBezierCurve(bezierCurve1, u2);

    
            Vec3 ruledp1, ruledp2;
            for (int k = 0; k < 3; k++) {
                ruledp1[k] = bezierp1[k] + v * (bezierCurve2[0][k] - bezierCurve1[0][k]);
                ruledp2[k] = bezierp2[k] + v * (bezierCurve2[0][k] - bezierCurve1[0][k]);
            }

          

            glColor3f(1.0, 0.0, 1.0);

            // Dessinez 
            glVertex3f(ruledp1[0], ruledp1[1], ruledp1[2]);
            glVertex3f(ruledp2[0], ruledp2[1], ruledp2[2]);
        }
    }

    glEnd();
}
void drawBezierSurface(const std::vector<std::vector<Vec3>>& controlps, int numControlpsU, int numControlpsV, int numpsU, int numpsV) {
    glBegin(GL_QUADS);

    for (int i = 0; i < numpsU - 1; i++) {
        for (int j = 0; j < numpsV - 1; j++) {
            float u1 = float(i) / float(numpsU - 1);
            float u2 = float(i + 1) / float(numpsU - 1);
            float v1 = float(j) / float(numpsV - 1);
            float v2 = float(j + 1) / float(numpsV - 1);

            Vec3 p1, p2, p3, p4;
            p1[0] = p2[0] = p3[0] = p4[0] = 0.0;
            p1[1] = p2[1] = p3[1] = p4[1] = 0.0;
            p1[2] = p2[2] = p3[2] = p4[2] = 0.0;

            for (int k = 0; k < numControlpsU; k++) {
                for (int l = 0; l < numControlpsV; l++) {
                    float coefficient1 = B(numControlpsU - 1, k, u1) * B(numControlpsV - 1, l, v1);
                    float coefficient2 = B(numControlpsU - 1, k, u2) * B(numControlpsV - 1, l, v1);
                    float coefficient3 = B(numControlpsU - 1, k, u2) * B(numControlpsV - 1, l, v2);
                    float coefficient4 = B(numControlpsU - 1, k, u1) * B(numControlpsV - 1, l, v2);

                    p1[0] += controlps[k][l][0] * coefficient1;
                    p1[1] += controlps[k][l][1] * coefficient1;
                    p1[2] += controlps[k][l][2] * coefficient1;

                    p2[0] += controlps[k][l][0] * coefficient2;
                    p2[1] += controlps[k][l][1] * coefficient2;
                    p2[2] += controlps[k][l][2] * coefficient2;

                    p3[0] += controlps[k][l][0] * coefficient3;
                    p3[1] += controlps[k][l][1] * coefficient3;
                    p3[2] += controlps[k][l][2] * coefficient3;

                    p4[0] += controlps[k][l][0] * coefficient4;
                    p4[1] += controlps[k][l][1] * coefficient4;
                    p4[2] += controlps[k][l][2] * coefficient4;
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
void DisplayVoxel(Vec3 centre, double length) {
    double h = length / 2.0;

    Vec3 vertices[8] = {
        Vec3(centre[0] - h, centre[1] - h, centre[2] - h),
        Vec3(centre[0] + h, centre[1] - h, centre[2] - h),
        Vec3(centre[0] + h, centre[1] + h, centre[2] - h),
        Vec3(centre[0] - h, centre[1] + h, centre[2] - h),
        Vec3(centre[0] - h, centre[1] - h, centre[2] + h),
        Vec3(centre[0] + h, centre[1] - h, centre[2] + h),
        Vec3(centre[0] + h, centre[1] + h, centre[2] + h),
        Vec3(centre[0] - h, centre[1] + h, centre[2] + h)
    };

    int faces[6][4] = {
        {0, 1, 2, 3},
        {4, 5, 6, 7},
        {0, 3, 7, 4},
        {1, 2, 6, 5},
        {0, 1, 5, 4},
        {3, 2, 6, 7}
    };

    glBegin(GL_QUADS);
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            glVertex3f(vertices[faces[i][j]][0], vertices[faces[i][j]][1], vertices[faces[i][j]][2]);
        }
    }
    glEnd();
}
void DisplaySphereVolumic(Vec3 centre, double rayon, double resolution) {
    double minx = centre[0] - rayon;
    double maxx = centre[0] + rayon;
    double miny = centre[1] - rayon;
    double maxy = centre[1] + rayon;
    double minz = centre[2] - rayon;
    double maxz = centre[2] + rayon;

    for (double x = minx; x <= maxx; x += resolution) {
        for (double y = miny; y <= maxy; y += resolution) {
            for (double z = minz; z <= maxz; z += resolution) {
                Vec3 p(x, y, z);
                double d = sqrt(pow(p[0] - centre[0], 2) +
                                       pow(p[1] - centre[1], 2) +
                                       pow(p[2] - centre[2], 2));
                if (d <= rayon) {
                    DisplayVoxel(p, resolution);
                }
            }
        }
    }
}
void DisplayCylinderVolumic(Vec3 axisOrigin, Vec3 axisVector, double rayon, double resolution) {

    for (double x = -rayon; x <= rayon; x += resolution) {
        for (double y = -rayon; y <= rayon; y += resolution) {
            double distance = sqrt(x * x + y * y);
            
            if (distance <= rayon) {
                Vec3 start(axisOrigin[0] + x, axisOrigin[1] + y, axisOrigin[2]);
                DisplayVoxel(start, resolution);


                for (double z = axisOrigin[2]; z <= axisOrigin[2]+axisVector[2]; z += resolution) {
                    Vec3 in(axisOrigin[0] + x, axisOrigin[1] + y, z);
                    DisplayVoxel(in, resolution);
                }
            }
        }
    }
}
void Display_INTERSECTION_SphereCylinder(Vec3 centreSphere, double
rayonSphere, Vec3 axisOriginCylinder, Vec3 axisVectorCylinder, double
rayonCylinder, double resolution){
    double minx = centreSphere[0] - rayonSphere;
    double maxx = centreSphere[0] + rayonSphere;
    double miny = centreSphere[1] - rayonSphere;
    double maxy = centreSphere[1] + rayonSphere;
    double minz = centreSphere[2] - rayonSphere;
    double maxz = centreSphere[2] + rayonSphere;

    for (double x = minx; x <= maxx; x += resolution) {
        for (double y = miny; y <= maxy; y += resolution) {
            for (double z = minz; z <= maxz; z += resolution) {
                Vec3 p(x, y, z);
                double d = sqrt(pow(p[0] - centreSphere[0], 2) +
                                       pow(p[1] - centreSphere[1], 2) +
                                       pow(p[2] - centreSphere[2], 2));
                if (d <= rayonSphere && d <= rayonCylinder) {
                    DisplayVoxel(p, resolution);
                      Vec3 start(axisOriginCylinder[0] + x, axisOriginCylinder[1] + y, axisOriginCylinder[2]);
                DisplayVoxel(start, resolution);


                for (double z = axisOriginCylinder[2]; z <= axisOriginCylinder[2]+axisVectorCylinder[2]; z += resolution) {
                    Vec3 in(axisOriginCylinder[0] + x, axisOriginCylinder[1] + y, z);
                    DisplayVoxel(in, resolution);
                }
                }
            }
        }
    }

}
void display () {


    glLoadIdentity ();


    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    camera.apply ();


    draw ();
   // DisplayVoxel(Vec3(0,0,0),1);
   DisplaySphereVolumic(Vec3(2,2,2),0.4,0.01);
   DisplayCylinderVolumic(Vec3(0,0,-0.5),Vec3(0,0,1),0.4,0.05); // Attention, le fonction ne s'allonge que via l'axe z 
   //Display_INTERSECTION_SphereCylinder(Vec3(0,0,0),0.4,Vec3(0,0,-0.5),Vec3(0,0,1),0.4,0.05);
    glColor3f(1,0,0);



    

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

 

