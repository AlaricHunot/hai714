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
#include <random>
#include <algorithm>
#include <GL/glut.h>
#include <float.h>
#include "src/Vec3.h"
#include "src/Camera.h"
#include "src/FastNoiseLit.h"

int sx=20;
int sy=20;
float offsetX = 0.0f;
float offsetY = 0.0f;

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

Vec3 crossProduct(Vec3& a, Vec3& b) {
    return Vec3(
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]
    );
}
Vec3 normalize(Vec3 v){
    float n=v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
    Vec3 v2;
    for(int i=0; i<3;i++){
        v2[i]=v[i]/n;
    }
    return v2;
}
int idx(int i,int j,int nX,int nY){ // permet de connaitre l'indice dans un tableau
    if(i==-1) i = nX-1;
    if(i==nX) i=0;
    if(j==-1) j = nY-1;
    if(j==nY) j=0;
    return i*nY+j;
}
//To complete for Exercice 1
void setUnitSphere( Mesh & o_mesh, int nX=20, int nY=20 )
{
    o_mesh.vertices.clear();
    o_mesh.colors.clear();
    o_mesh.vertices.clear();
    o_mesh.normals.clear();
    for(int i=0;i< nX; i++){
        for(int j=0; j<nY; j++){
            float theta = float(i)* (2.f * M_PI) / float (nX);
            float phi = (-M_PI/2.f) + float(j) * (M_PI / float(nY -1));
            float x = cos(theta) * cos(phi);
            float y = sin(theta) * cos(phi);
            float z = sin(phi);
            o_mesh.vertices.push_back(Vec3(x,y,z));
            o_mesh.normals.push_back(normalize(Vec3(x,y,z)));
            o_mesh.colors.push_back(Vec3(x,y,z));
        
        }
    }
    float n1,n2,n3,n4;
    for(int i=0;i< nX; i++){
        for(int j=0; j<nY-1; j++){
            n1=idx(i,j,nX,nY);
            n2=idx(i+1,j,nX,nY);
            n3=idx(i,j+1,nX,nY);
            n4=idx(i+1,j+1,nX,nY);
            o_mesh.triangles.push_back(Triangle(n1,n2,n3));
            o_mesh.triangles.push_back(Triangle(n2,n4,n3));
        }
    }

 } 
// To complete for Exercice 2
void setTesselatedSquare(Mesh & o_mesh, int nX=200, int nY=200)
{
        o_mesh.vertices.clear();
    for(int i=0;i< nX; i++){
        for(int j=0; j<nY; j++){
            float x = (float)i/float(nX-1) + offsetX + offsetY;
            float y = (float)j/float(nY-1) + offsetX + offsetY;
            float r1 = -1.0f + (float)rand() / ((float)RAND_MAX/(1.0f+1.0f));
            float r2 = -1.0f + (float)rand() / ((float)RAND_MAX/(1.0f+1.0f));
            float z = noise.GetNoise((float)std::min(r1,r2),(float)std::max(r1,r2));
            o_mesh.vertices.push_back(Vec3(x,y,z));
            Vec3 normal=normalize(Vec3(x,y,z));
            o_mesh.normals.push_back(normalize(Vec3(x,y,z)));
            Vec3 color;
            if (normal[0] == 0.0f && normal[1] == 0.0f && normal[2] != 0.0f) {
                // Si la normale est verticale, colorez en vert
                color = Vec3(0.0f, 1.0f, 0.0f);
                o_mesh.colors.push_back(color);
            } else {
                // Sinon, colorez en brun
                color = Vec3(0.5f, 0.2f, 0.0f);
                o_mesh.colors.push_back(color);
            }
        }
    }    
      
        float n1,n2,n3,n4;
    for(int i=0;i< nX-1; i++){
        for(int j=0; j<nY-1; j++){
            n1=idx(i,j,nX,nY);
            n2=idx(i+1,j,nX,nY);
            n3=idx(i,j+1,nX,nY);
            n4=idx(i+1,j+1,nX,nY);
            o_mesh.triangles.push_back(Triangle(n1,n2,n3));
            o_mesh.triangles.push_back(Triangle(n2,n4,n3));
        }
    }
    } 
    


bool saveOFF( const std::string & filename ,
              std::vector< Vec3 > & i_vertices ,
              std::vector< Vec3 > & i_normals ,
              std::vector< Triangle > & i_triangles,
              bool save_normals = true ) {
    std::ofstream myfile;
    myfile.open(filename.c_str());
    if (!myfile.is_open()) {
        std::cout << filename << " cannot be opened" << std::endl;
        return false;
    }

    myfile << "OFF" << std::endl ;

    unsigned int n_vertices = i_vertices.size() , n_triangles = i_triangles.size();
    myfile << n_vertices << " " << n_triangles << " 0" << std::endl;

    for( unsigned int v = 0 ; v < n_vertices ; ++v ) {
        myfile << i_vertices[v][0] << " " << i_vertices[v][1] << " " << i_vertices[v][2] << " ";
        if (save_normals) myfile << i_normals[v][0] << " " << i_normals[v][1] << " " << i_normals[v][2] << std::endl;
        else myfile << std::endl;
    }
    for( unsigned int f = 0 ; f < n_triangles ; ++f ) {
        myfile << 3 << " " << i_triangles[f][0] << " " << i_triangles[f][1] << " " << i_triangles[f][2];
        myfile << std::endl;
    }
    myfile.close();
    return true;
}

void openOFF( std::string const & filename,
              std::vector<Vec3> & o_vertices,
              std::vector<Vec3> & o_normals,
              std::vector< Triangle > & o_triangles,
              bool load_normals = true )
{
    std::ifstream myfile;
    myfile.open(filename.c_str());
    if (!myfile.is_open())
    {
        std::cout << filename << " cannot be opened" << std::endl;
        return;
    }

    std::string magic_s;

    myfile >> magic_s;

    if( magic_s != "OFF" )
    {
        std::cout << magic_s << " != OFF :   We handle ONLY *.off files." << std::endl;
        myfile.close();
        exit(1);
    }

    int n_vertices , n_faces , dummy_int;
    myfile >> n_vertices >> n_faces >> dummy_int;

    o_vertices.clear();
    o_normals.clear();

    for( int v = 0 ; v < n_vertices ; ++v )
    {
        float x , y , z ;

        myfile >> x >> y >> z ;
        o_vertices.push_back( Vec3( x , y , z ) );

        if( load_normals ) {
            myfile >> x >> y >> z;
            o_normals.push_back( Vec3( x , y , z ) );
        }
    }

    o_triangles.clear();
    for( int f = 0 ; f < n_faces ; ++f )
    {
        int n_vertices_on_face;
        myfile >> n_vertices_on_face;

        if( n_vertices_on_face == 3 )
        {
            unsigned int _v1 , _v2 , _v3;
            myfile >> _v1 >> _v2 >> _v3;

            o_triangles.push_back(Triangle( _v1, _v2, _v3 ));
        }
        else if( n_vertices_on_face == 4 )
        {
            unsigned int _v1 , _v2 , _v3 , _v4;
            myfile >> _v1 >> _v2 >> _v3 >> _v4;

            o_triangles.push_back(Triangle(_v1, _v2, _v3 ));
            o_triangles.push_back(Triangle(_v1, _v3, _v4));
        }
        else
        {
            std::cout << "We handle ONLY *.off files with 3 or 4 vertices per face" << std::endl;
            myfile.close();
            exit(1);
        }
    }

}

//o_mesh.colors.push_back(Vec3(1,1,0))

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
    initLight ();
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

void drawTriangleMesh( Mesh const & i_mesh ) {
    glBegin(GL_TRIANGLES);
    bool okNormals = (i_mesh.normals.size() == i_mesh.vertices.size());
    bool okColors = (i_mesh.colors.size() == i_mesh.vertices.size());

    Vec3 p0, p1, p2;
    Vec3 n0, n1, n2;
    Vec3 c0 = Vec3(1, 1, 1), c1 = Vec3(1, 1, 1), c2 = Vec3(1, 1, 1);
    for(unsigned int tIt = 0 ; tIt < i_mesh.triangles.size(); ++tIt) {
        p0 = i_mesh.vertices[i_mesh.triangles[tIt][0]];
        p1 = i_mesh.vertices[i_mesh.triangles[tIt][1]];
        p2 = i_mesh.vertices[i_mesh.triangles[tIt][2]];

        if (okNormals) {
            n0 = i_mesh.normals[i_mesh.triangles[tIt][0]];
            n1 = i_mesh.normals[i_mesh.triangles[tIt][1]];
            n2 = i_mesh.normals[i_mesh.triangles[tIt][2]];
        }
        if (okColors) {
            c0 = i_mesh.colors[i_mesh.triangles[tIt][0]];
            c1 = i_mesh.colors[i_mesh.triangles[tIt][1]];
            c2 = i_mesh.colors[i_mesh.triangles[tIt][2]];
        }

        glNormal3f( n0[0] , n0[1] , n0[2] );
        glColor3f( c0[0], c0[1], c0[2]);
        glVertex3f( p0[0] , p0[1] , p0[2] );
        
        glNormal3f( n1[0] , n1[1] , n1[2] );
        glColor3f( c1[0], c1[1], c1[2]);
        glVertex3f( p1[0] , p1[1] , p1[2] );
        
        glNormal3f( n2[0] , n2[1] , n2[2] );
        glColor3f( c2[0], c2[1], c2[2]);
        glVertex3f( p2[0] , p2[1] , p2[2] );
    }
    glEnd();

    if(display_normals){
        glLineWidth(1.);
        glColor3f(1.,0.,0.);
        for(unsigned int pIt = 0 ; pIt < i_mesh.normals.size() ; ++pIt) {
            Vec3 to = i_mesh.vertices[pIt] + 0.02*i_mesh.normals[pIt];
            drawVector(i_mesh.vertices[pIt], to);
        }

    }

}


void draw () {

    if( display_unit_sphere ){
        glColor3f(0.8,1,0.8);
        drawTriangleMesh(unit_sphere);
    }

    if( display_tesselation ){
        glColor3f(0.8,1,0.8);
        drawTriangleMesh(tesselation);
    }

    if( display_loaded_mesh ){
        glColor3f(0.8,0.8,1);
        drawTriangleMesh(mesh);
    }


}


void display () {
    glLoadIdentity ();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.apply ();
    draw ();
    glFlush ();
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
     case '+': 
        setUnitSphere(unit_sphere, sx++, sy++);
        break;

    case '-':
        setUnitSphere(unit_sphere, sx--, sy--);
        break;
    case 'a':
        offsetX -= 0.1f; // Réduire le décalage X
        break;

    case 'd':
        offsetX += 0.1f; // Augmenter le décalage X
        break;

    case 'z':
        offsetY += 0.1f; // Augmenter le décalage Y
        break;

    case 's':
        offsetY -= 0.1f; // Réduire le décalage Y
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

    //Unit sphere mesh loaded with precomputed normals
    openOFF("data/unit_sphere_n.off", mesh.vertices, mesh.normals, mesh.triangles);

    setUnitSphere(unit_sphere);

    setTesselatedSquare(tesselation);

    //Uncomment to see other meshes
    //openOFF("data/elephant_n.off", mesh.vertices, mesh.normals, mesh.triangles);

    glutMainLoop ();
    return EXIT_SUCCESS;
}

