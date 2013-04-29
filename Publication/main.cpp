//TODO: - Read the file without having to take the header our, or even better make 
//        use of the header IN: loadTerrain

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#include <GL/glu.h>
#endif

#include "vec3f.h"
#include "Terrain.h"

using namespace std;

// GLOBAL VARS
int color = 1;
int n = 0;
int p = 1;
float _angle = 60.0f;
float _angleLat = 60.0f;
float _angleLon = 0.0f;
float _walk = -10.0f;
float _stride = 0.0f;
float _elevate = 0.0f;
float maxHeight = -9999999.0f, minHeight = 99999999.0f;
float thresh = 0.0f;
float hist[1000];
float histMax=0;
string res, size;
Terrain  *_terrain;

//Loads a terrain from a file
Terrain* loadTerrain(const char* filename, float height) 
{
  // Read in file and get rid of the header
  ifstream data (filename);
  float f;
  float h;
  int resolution;
  string s, sub;

  int i = 0;
  
  while( i < 25 )
  {
     getline(data, s);

     if(i == 13 || i == 12)
     {
        istringstream iss(s);
        iss >> sub;
        iss >> sub;
        iss >> sub;
        
        if(i==12) 
        {
          iss >> size;
          iss >> size;
        }
        if(i==13) 
        {
          iss >> res;
        }
     }
     i++;
  }

  // Sets the resolution according to the file
  resolution = atoi(res.c_str());
  float temp[resolution][resolution];
    
  // Incrimental vars used throught 
  int j;

  // Used to record the Max and min heights at various plaes

  maxHeight = -9999999.0f;
  minHeight = 99999999.0f;

  // Find the max and minHeight
  for(j = 0; j < resolution; j++)
  {
    for(i = 0; i < resolution; i++)
    {
      data >> f;
      temp[i][j] = f;
      if( f > maxHeight)
      maxHeight = f;
      if( f < minHeight)
      minHeight = f;
    }
  }

   
  // At this point we load the height map into the terrain
  // After it has been edited as needed
  Terrain* t = new Terrain(resolution, resolution);
  
  
  j = 0;
  for(int y = 0; y < resolution; y++)
  {
    i=0;

    for(int x = 0; x < resolution; x++)
    {
      h = temp[x][y];
      t->setHeight(x, y, h);  
    }
  }
  
  for( i = 0; i <= 1000; i++)
    hist[i] = 0;
  
  t->computeNormals();
  
  maxHeight = -9999999.0f;
  minHeight = 99999999.0f;

  for(int z = 0; z < t->length() - 5; z++) 
  {
    for(int x = t->width() - 4; x > 1; x--)
    {
      if( t->getHeight(x, z) > maxHeight)
        maxHeight = t->getHeight(x, z);
      if( t->getHeight(x, z) < minHeight && t->getHeight(x,z) > 0)
        minHeight = t->getHeight(x, z);
            
      f = f + t->getHeight(x, z);
    }
  }

   
   size += "um"; 
  return t;
}

void cleanup() 
{
  delete _terrain;
}

void handleKeypress(unsigned char key, int x, int y) 
{
  switch (key) 
  {  
    case 27: //Escape key
      cleanup();
      exit(0);
      break;  
    case 112: // p key
      if(p == 0)
        p = 1;
      else if(p == 1)
        p = 0;
      break;
    case 99: // c key
      if(color == 0)
        color = 1;
      else if(color == 1)
        color = 0;
      break;
    case 110: // n key
      if(n == 0)
        n = 1;
      else if(n == 1)
        n = 0;
      break;
  }
  //printf("%d\n", key);
}


void initRendering() 
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);
}

void handleResize(int w, int h) 
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

void drawSurface() 
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearColor(.3f,.3f,.32f,0.0f);
  
  GLfloat ambientColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
  
  GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
  GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
  
  float scale = 5.0f / max(_terrain->width() - 1, _terrain->length() - 1);
  glScalef(scale, scale, scale);
  glTranslatef(-(float)(_terrain->width() - 1) / 2, 0.0f, -(float)(_terrain->length() - 1) / 2);
  glShadeModel (GL_SMOOTH);       // Enables Smooth Color Shading
  glEnable (GL_LINE_SMOOTH);

  glTranslatef(0.0f,-90.0f,-170.0f);
    
  maxHeight = -9999999.0f;
  minHeight = 99999999.0f;
  float value = 0;
  for(int z = 0; z < _terrain->length() - 5; z++) 
  {  
    for(int x = _terrain->width() - 4; x > 1; x--)
    {
       value = _terrain->getHeight(x, z);
       if(value < minHeight)
         minHeight = value;
       if(value > maxHeight)
         maxHeight = value;
          
    }
  }

  glColor3f(0.2f, 0.4f, 1.0f);
  

  
  for(int z = 0; z < _terrain->length() - 5; z++) 
  {
    //Makes OpenGL draw a triangle at every three consecutive vertices
    glBegin(GL_TRIANGLE_STRIP);
    
    for(int x = _terrain->width() - 4; x > 1; x--)
    {
      if(color == 1)
      {
        value = _terrain->getHeight(x, z)/maxHeight;  
        glColor3f(value*0.9f, value, 1.0f);    
      }

      else
        glColor3f(0.2f, 0.4f, 1.0f);


      Vec3f normal = _terrain->getNormal(x, z);
      glNormal3f(normal[0], normal[2], normal[1]);
      glVertex3f(x, z, 0.0f);
      normal = _terrain->getNormal(x, z + 1);
      glNormal3f(normal[0], normal[2], normal[1]);
      glVertex3f(x, z+1, 0.0f);
    }
    glEnd();      
  }
  
  // Draw Lines surrounding the surface
  glColor3f(1.0f,1.0f,1.0f);
  glBegin(GL_LINES);
  glVertex3f(-2.0f, 0.0f, 0.0f);
  glVertex3f(196.0f, 0.0f, 0.0f);
  glVertex3f(1.5f, 195.0f, 0.0f);
  glVertex3f(1.5f, -4.0f, 0.0f);
  glVertex3f(-1.5f, 195.0f, 0.0f);
  glVertex3f(195.5f, 195.0f, 0.0f);
  glVertex3f(196.0f, 195.0f, 0.0f);
  glVertex3f(196.0f, -2.0f, 0.0f);
  glEnd();

  // Draw the Color Bar
  if(color == 1)
  {
    float x = 30;
    float y = -30;
    float dy = 20;
    float dx = 130;
    value = 0;
    glColor3f(0.0f, 0.0f, 1.0f);
    value = .25f;
    glBegin(GL_QUADS);
    glVertex3f(x, y, 0.0f);
    glVertex3f(x, y+dy, 0.0f);
    glColor3f(value*.9f,  value, 1.0f);
    glVertex3f(x+dx/4, y+dy, 0.0f);
    glVertex3f(x+dx/4, y, 0.0f);
    glEnd();

    value = .5f;

    glBegin(GL_QUADS);
    glVertex3f(x+dx/4, y, 0.0f);
    glVertex3f(x+dx/4, y+dy, 0.0f);
    glColor3f(value*.9f, value, 1.0f);
    glVertex3f(x+dx/3, y+dy, 0.0f);
    glVertex3f(x+dx/3, y, 0.0f);
    glEnd();
     
    value = .75f;

    glBegin(GL_QUADS);
    glVertex3f(x+dx/3, y, 0.0f);
    glVertex3f(x+dx/3, y+dy, 0.0f);
    glColor3f(value*.9f, value, 1.0f);
    glVertex3f(x+dx/2, y+dy, 0.0f);
    glVertex3f(x+dx/2, y, 0.0f);
    glEnd();
  
    value = 1.0f;

    glBegin(GL_QUADS);
    glVertex3f(x+dx/2, y, 0.0f);
    glVertex3f(x+dx/2, y+dy, 0.0f);
    glColor3f(value*.9f, value, 1.0f);
    glVertex3f(x+dx, y+dy, 0.0f);
    glVertex3f(x+dx, y, 0.0f);
    glEnd();
  }
  
  
  // Draw the text needed to the screen
 
  // Draw the text to denote the size of the graph 
 
  glRasterPos2f(194.0f, -6.1f);
  glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*)size.c_str());
  glRasterPos2f(-12.0f, 190.1f);
  glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*)size.c_str());
  glRasterPos2f(-9.0f, 1.0f);
  glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*)"0um");
  glRasterPos2f(4.0f, -5.1f);
  glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*)"0um");

  // Draw the labels for the color bar
  glRasterPos2f(17.0f, -17.1f);
  glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*)"-90 ");
  glRasterPos2f(165.0f, -17.1f);
  glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*)"90 ");

  // Draw circles for degrees
  float delta_theta = 0.01;
  float offsetX = 27.4f, offsetY = -12.3f;
  float r = 0.7;
  float angle;

  glBegin( GL_LINE_LOOP );  
  for( angle = 0; angle < 2*3.14;angle += delta_theta )
	glVertex3f( r*cos(angle)+offsetX, r*sin(angle)+offsetY, 0 );
  glEnd();

  offsetX = 172.4f, offsetY = -13.3f;
  r = 0.7;
  glBegin( GL_LINE_LOOP );  
  for(angle = 0; angle < 2*3.14;angle += delta_theta )
	glVertex3f( r*cos(angle)+offsetX, r*sin(angle)+offsetY, 0 );
  glEnd();

  // Draw lines that make the u a mew
  glBegin(GL_LINES);
  glVertex3f(-6.0f, 190.0f, 0.0f);
  glVertex3f(-6.0f, 189.0f, 0.0f);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(-5.7f, 1.0f, 0.0f);
  glVertex3f(-5.7f, 0.0f, 0.0f);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(7.1f, -5.0f, 0.0f);
  glVertex3f(7.1f, -6.0f, 0.0f);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(199.9f, -6.0f, 0.0f);
  glVertex3f(199.9f, -7.0f, 0.0f);
  glEnd();

  

}

char* fn;

void update(int value)
{
  glutTimerFunc(60, update, 0);
  if(p == 0)
  {
    thresh += 0.2f;
  }
  glutPostRedisplay();  

}

void winDisplay()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  int w = 800;
  int h = 800;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
  drawSurface();
  
  glutSwapBuffers();
  
}

int main(int argc, char** argv) {
  
  fn = argv[1];
  _terrain = loadTerrain(fn, 20);
  
  glutInit(&argc, argv);
  int w = 800;
  int h = 800;
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(w, h);
  
  glutCreateWindow("Surface Visualization");
  
  glutKeyboardFunc(handleKeypress);
  glutReshapeFunc(handleResize);
  glutTimerFunc(25, update, 0);
  glutDisplayFunc(winDisplay);
  initRendering();
  
  glutMainLoop();
  return 0;
  
}









