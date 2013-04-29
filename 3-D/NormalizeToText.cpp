/* Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/* File for "Terrain" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */



#include <iostream>
#include <stdlib.h>
#include <fstream>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include "imageloader.h"
#include "vec3f.h"

using namespace std;


string  data1 = "dataOrig.txt";
string data2 = "dataLevel.txt";
string data3 = "dataShade.txt";
string datastr = data1;
char* fn;
float normalConst = 1;
int n = 1;
float leveled[200][200];
float temp[200][200];

//Represents a terrain, by storing a set of heights and normals at 2D locations
class Terrain {
	private:
		int w; //Width
		int l; //Length
		float** hs; //Heights
		Vec3f** normals;
		bool computedNormals; //Whether normals is up-to-date
	public:
		Terrain(int w2, int l2) {
			w = w2;
			l = l2;
			
			hs = new float*[l];
			for(int i = 0; i < l; i++) {
				hs[i] = new float[w];
			}
			
			normals = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals[i] = new Vec3f[w];
			}
			
			computedNormals = false;
		}
		
		~Terrain() {
			for(int i = 0; i < l; i++) {
				delete[] hs[i];
			}
			delete[] hs;
			
			for(int i = 0; i < l; i++) {
				delete[] normals[i];
			}
			delete[] normals;
		}
		
		int width() {
			return w;
		}
		
		int length() {
			return l;
		}
		
		//Sets the height at (x, z) to y
		void setHeight(int x, int z, float y) {
			hs[z][x] = y;
			computedNormals = false;
		}
		
		//Returns the height at (x, z)
		float getHeight(int x, int z) {
			return hs[z][x];
		}
		
		//Computes the normals, if they haven't been computed yet
		void computeNormals() {
			if (computedNormals) {
				return;
			}
			
			//Compute the rough version of the normals
			Vec3f** normals2 = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals2[i] = new Vec3f[w];
			}
			
			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum(0.0f, 0.0f, 0.0f);
					
					Vec3f out;
					if (z > 0) {
						out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
					}
					Vec3f in;
					if (z < l - 1) {
						in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
					}
					Vec3f left;
					if (x > 0) {
						left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
					}
					Vec3f right;
					if (x < w - 1) {
						right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
					}
					
					if (x > 0 && z > 0) {
						sum += out.cross(left).normalize();
					}
					if (x > 0 && z < l - 1) {
						sum += left.cross(in).normalize();
					}
					if (x < w - 1 && z < l - 1) {
						sum += in.cross(right).normalize();
					}
					if (x < w - 1 && z > 0) {
						sum += right.cross(out).normalize();
					}
					
					normals2[z][x] = sum;
				}
			}
			
			//Smooth out the normals
			
			const float FALLOUT_RATIO = 0.5f;
			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum = normals2[z][x];
					
					if (x > 0) {
						sum += normals2[z][x - 1] * FALLOUT_RATIO;
					}
					if (x < w - 1) {
						sum += normals2[z][x + 1] * FALLOUT_RATIO;
					}
					if (z > 0) {
						sum += normals2[z - 1][x] * FALLOUT_RATIO;
					}
					if (z < l - 1) {
						sum += normals2[z + 1][x] * FALLOUT_RATIO;
					}
					
					if (sum.magnitude() == 0) {
						sum = Vec3f(0.0f, 1.0f, 0.0f);
					}
					normals[z][x] = sum;
				}
			}
			
			
			for(int i = 0; i < l; i++) 
			{
			  delete[] normals2[i];
			}
			delete[] normals2;
			
			computedNormals = true;
		}
		
		//Returns the normal at (x, z)
		Vec3f getNormal(int x, int z) {
			if (!computedNormals) {
				computeNormals();
			}
			return normals[z][x];
		}
};

float maxHeight = -9999999.0f, minHeight = 99999999.0f;
int dataCount = 0;
//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
Terrain* loadTerrain(const char* filename, float height) 
{	
	ifstream data (filename);
  	float f;
  	float h;
  	data >> f;
  	data >> f;
  	//TODO: make resolution variable
    float temp[200][200]; 
    int i;
    int j;
    maxHeight = -9999999.0f;
	minHeight = 99999999.0f;

    for(j = 0; j < 200; j++)
    {
      for(i = 0; i < 200; i++)
      {
        data >> f;
        temp[i][j] = f;
        if( f > maxHeight)
        	maxHeight = f;
       	if( f < minHeight)
        	minHeight = f;
      }
    }
    if(n == 1)
    {
    int blockRes = 2;
    float diffrence = 0;
    for(j = 0; j < 200; j+=blockRes)
    {
      for(i = 0; i < 200; i+=blockRes)
      {


      	float localMinHeight = 99999999.0f;
      	for(int blockX = 0; blockX < blockRes; blockX++)
      	{
      		for (int blockY = 0; blockY < blockRes; blockY++)
      		{
      			f = temp[i+blockX][j+blockY];
      			if( f < localMinHeight)
      				localMinHeight = f;
      		}
      	}

      	diffrence = localMinHeight - minHeight;

      	for(int blockX = 0; blockX < blockRes; blockX++)
      	{
      		for (int blockY = 0; blockY < blockRes; blockY++)
      		{
      			if ( temp[i+blockX][j+blockY] - diffrence > 0 )
      				temp[i+blockX][j+blockY] = temp[i+blockX][j+blockY] - diffrence;
      			else
      				temp[i+blockX][j+blockY] = 0;
      		}		
      	}

        

        
      }
    }
	}

	double myMax = 0;
	for(int i = 0; i < 200; i++)
	{
		for(int j = 0; j < 200; j++)
		{
			if(temp[i][j] > myMax)
				myMax = temp[i][j];
		}
	}
	
	for(int i = 0; i < 200; i++)
	{
		for(int j = 0; j < 200; j++)
		{
			cout << temp[i][j]/myMax << " ";
		}
		cout << "\n";
	}
	exit(0);
    
	Terrain* t = new Terrain(200, 200);
	//Read in and interpolate data
	j = 0;

 
  for(int y = 0; y < 200; y++)
  {
    i=0;

    for(int x = 0; x < 200; x++)
    {
    	h = temp[x][y];
		t->setHeight(x, y, h);
			
	}

   
  }
  
  
  
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
	return t;
}

float _angle = 60.0f;
float _angleLat = 60.0f;
float _angleLon = 0.0f;
float _walk = -10.0f;
float _stride = 0.0f;
float _elevate = 0.0f;
Terrain* _terrain;

void cleanup() {
	delete _terrain;
}

int p = 0;

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		
		case 27: //Escape key
		
			exit(0);
				
			break;
			
		case 97: // A key
		
			_angleLat += 2.0f;
			if (_angleLat > 360) {
			_angleLat -= 360;
			}
	
			glutPostRedisplay();
			break;
			
		case 100: // D key

			_angleLat -= 2.0f;
			if (_angleLat < 0) {
			_angleLat -= 360;
			}
	
			glutPostRedisplay();
			break;
			
		case 119: // W key
		
			_angleLon += 2.0f;
			if (_angleLon > 360) {
			_angleLon -= 360;
			}
	
			glutPostRedisplay();
			break;
			
		case 115: // S key

			_angleLon -= 2.0f;
			if (_angleLon < 0) {
			_angleLon -= 360;
			}
	
			glutPostRedisplay();
			break;
			
		case 105: // I key
		
			_walk += 0.5f;
			
	
			glutPostRedisplay();
			break;
			
		case 107: // K key

			_walk -= 0.5f;
			
	
			glutPostRedisplay();
			break;
			
		case 106: // J key
		
			_stride += 0.5f;
			
	
			glutPostRedisplay();
			break;
			
		case 108: // L key

			_stride -= 0.5f;
			
	
			glutPostRedisplay();
			break;
			
		case 113: // Q key
		
			_elevate += 0.5f;
			
	
			glutPostRedisplay();
			break;
			
		case 101: // E key

			_elevate -= 0.5f;
			
	
			glutPostRedisplay();
			break;

		case 110: // n key
			if(n == 0)
				n = 1;
			else if(n == 1)
				n = 0;
			break;
			
	    
			
	}
	
	//printf("%d\n",key);
}


void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}



void drawScene() 
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(.2f,.2f,.23f,0.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(_stride, _elevate, _walk);
	glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	
	glRotatef(-_angleLat, 0.0f, 1.0f, 0.0f);
	glRotatef(-_angleLon, -_angleLon, 1.0f, 0.0f);
	
	GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	float scale = 5.0f / max(_terrain->width() - 1, _terrain->length() - 1);
	glScalef(scale, scale, scale);
	glTranslatef(-(float)(_terrain->width() - 1) / 2,
				 0.0f,
				 -(float)(_terrain->length() - 1) / 2);
	//glClearColor(0.0f, 0.0f, .1f, 1.0f);
	glColor3f(0.3f, 0.7f, 0.5f);
	maxHeight = -9999999.0f; 
	minHeight = 99999999.0f;
	float value = 0;
	for(int z = 0; z < _terrain->length() - 5; z++) 
	{
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = _terrain->width() - 4; x > 1; x--)
    	{
    		
       		value = _terrain->getHeight(x, z)/maxHeight;
    		//glColor3f(value*0.9f, value, 1.0f);
        	
      		Vec3f normal = _terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z), z);
			normal = _terrain->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z + 1), z + 1);
			
			if(_terrain->getHeight(x,z) > maxHeight)
				maxHeight = _terrain->getHeight(x,z);
			if(_terrain->getHeight(x,z) < minHeight)
				minHeight = _terrain->getHeight(x,z);
		
    	}

   	 	glEnd();
	}

	
	//cout << maxHeight << "\n" << minHeight << "\n";
	glutSwapBuffers();
}

void update(int Value)
{
	/*
	_angle += 1.0f;
	if (_angle > 360) {
		_angle -= 360;
	}
	*/
	glutTimerFunc(25, update, 0);
	
		_terrain = loadTerrain(fn,20);
	
	glutPostRedisplay();
	
	
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	fn = argv[1];
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1280, 800);
	
	glutCreateWindow("Surface Visualization");
	initRendering();
	
	_terrain = loadTerrain(fn,20);
	
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutTimerFunc(25, update, 0);
	
	glutMainLoop();
	return 0;
}









