 //TODO: - Read the file without having to take the header our, or even better make 
//        use of the header IN: loadTerrain

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
int iso[400][400];
Terrain  *_terrain;

//Loads a terrain from a file
Terrain* loadTerrain(const char* filename, float height) 
{
	// Read in file and get rid of the header
	ifstream data (filename);
  	float f;
  	float h;
  	data >> f;
  	data >> f;

    //TODO: make resolution variable (not important)
    float temp[200][200];
    
    // Incrimental vars used throught 
    int i;
    int j;

    // Used to record the Max and min heights at various plaes

    maxHeight = -9999999.0f;
	minHeight = 99999999.0f;

	// Find the max and minHeight
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
    printf("%f\n", maxHeight);

    // Code to normalize the height map 
    // Only executed if n = 1 which is set
    // by a button press
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
    
    // At this point we load the height map into the terrain
    // After it has been edited as needed
	Terrain* t = new Terrain(200, 200);
	
	
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

	// Fill the Histogram array up
	
	int index = 0;	
	histMax = -999999.0f;
	for(int z = 0; z < t->length() - 5; z++) 
	{
		
		for(int x = t->width() - 4; x > 1; x--)
    	{
        	if( t->getHeight(x, z) < thresh)
			{
        		t -> setHeight(x, z, 0);
			}
		
			index = (int) 1000 * (( (t->getHeight(x,z) ) / (maxHeight) ));
			if(index > 1000)
				index = 1000;
			if(index < 0)
				index = 0;

			if(index != 0)
				hist[index] += 1;	

			if(hist[index] > histMax)
				histMax = hist[index];
    	}
	
	}
	return t;
}


// Counts the number of particles above the given
// threshold
void count()
{
	int currentParticle = 2;
	

	// Make Ghost cells surrounding
	for(int j = 0; j < _terrain->width()+2; j++)
	{
		iso[_terrain->length()+2][j] = 0;
	}
	for(int j = 0; j < _terrain->width()+2; j++)
	{
		iso[0][j] = 0;
	}
	for(int j = 0; j < _terrain->width()+2; j++)
	{
		iso[j][_terrain->length()+2] = 0;
	}
	for(int j = 0; j < _terrain->width()+2; j++)
	{
		iso[j][0] = 0;
	}


	// Create Binary iso array
	for(int i = 1; i < _terrain->length(); i++)
	{
		for(int j = 1; j < _terrain->width(); j++)
		{
			if(_terrain->getHeight(i, j) > 0.01)
			{
				iso[i][j] = 1;
			}
			else
			{
				iso[i][j] = 0;
			}
		}
		
	}


	

	// Search throught the Binary ISO array to count closed surfaces
	for(int i = 10; i < _terrain->length()-10; i++)
	{
		for(int j = 10; j < _terrain->width()-10; j++)
		{
			if(iso[i][j] == 1)
			{
				if(iso[i+1][j] > 1 && iso[i+1][j] < currentParticle)
					iso[i][j] = iso[i+1][j];
				//else if(iso[i+1][j+1] > 1 && iso[i+1][j+1] < currentParticle)
				//	iso[i][j] = iso[i+1][j+1];
				else if(iso[i][j+1] > 1 && iso[i][j+1] < currentParticle)
					iso[i][j] = iso[i][j+1];
				else if(iso[i-1][j] > 1 && iso[i-1][j] < currentParticle)
					iso[i][j] = iso[i-1][j];
				//else if(iso[i-1][j+1] > 1 && iso[i-1][j+1] < currentParticle)
				//	iso[i][j] = iso[i-1][j+1];
				else if(iso[i][j-1] > 1 && iso[i][j-1] < currentParticle)
					iso[i][j] = iso[i][j-1];
				//else if(iso[i+1][j-1] > 1 && iso[i+1][j-1] < currentParticle)
				//	iso[i][j] = iso[i+1][j-1];
				//else if(iso[i-1][j-1] > 1 && iso[i-1][j-1] < currentParticle)
				//	iso[i][j] = iso[i-1][j-1];
				else
				{
					iso[i][j] = currentParticle;
					currentParticle++;
				}
				
				
			}	
			
			/*
			// Print out the iso
			for(int k = 1; k < _terrain->length()-180; k++)
			{
				for(int q = 1; q < _terrain->width()-180; q++)
				{
					
					printf("%d", iso[k][q]);
				}
				printf("\n");
			}			
			sleep(1);
			printf("\n");
			*/
			
		}
	}

	printf("%d\n", currentParticle-1);
}

void cleanup() {
	delete _terrain;
}

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {	
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
		case 107: // k key
			count();
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
	glClearColor(.20f,.2f,.21f,0.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(_stride, _elevate, _walk);
	glTranslatef(0.0f, 0.0f, 4.0f);
	glTranslatef(0.7f, -2.45f, 0.5f);
	glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
	
	glRotatef(-_angleLat, 0.0f, 1.0f, 0.0f);
	glRotatef(-_angleLon, -_angleLon, 1.0f, 0.0f);
	glRotatef(60.0f, 0.0f, 1.0f, 0.0f);
	
	GLfloat ambientColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
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
    				if(value >= 0 && value < 0.25f)
						glColor3f(0.0f, value*4, 1.0f);
					else if(value >= 0.25f && value < 0.50f)
						glColor3f(0.0f, 1.0f, 1-((value-0.25f)*4));
					else if(value >= 0.50f && value < 0.75f)
						glColor3f((value-0.50f)*4, 1.0f, 0.0f);
					else if(value >= 0.75f)
						glColor3f(1.0f, 1-((value-0.75f)*4), 0.0f);
				}
				else
					glColor3f(0.2f, 0.4f, 1.0f);


      			Vec3f normal = _terrain->getNormal(x, z);
				glNormal3f(normal[0], normal[2], normal[1]);
				glVertex3f(x, z, 0);
				normal = _terrain->getNormal(x, z + 1);
				glNormal3f(normal[0], normal[2], normal[1]);
				glVertex3f(x, z+1, 0);
    	}
   	 	glEnd(); 	 	
	}

	if(color == 1)
	{
		float x = 22;
		float y = -30;
		float dy = 20;
		float dx = 130;

			
	
		glColor3f(0.0f, 0.0f, 1.0f);

		glBegin(GL_QUADS);
		glVertex3f(x, y, 0.0f);
		glVertex3f(x, y+dy, 0.0f);
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(x+dx/4, y+dy, 0.0f);
		glVertex3f(x+dx/4, y, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glVertex3f(x+dx/4, y, 0.0f);
		glVertex3f(x+dx/4, y+dy, 0.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(x+dx/3, y+dy, 0.0f);
		glVertex3f(x+dx/3, y, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glVertex3f(x+dx/3, y, 0.0f);
		glVertex3f(x+dx/3, y+dy, 0.0f);
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(x+dx/2, y+dy, 0.0f);
		glVertex3f(x+dx/2, y, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glVertex3f(x+dx/2, y, 0.0f);
		glVertex3f(x+dx/2, y+dy, 0.0f);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(x+dx, y+dy, 0.0f);
		glVertex3f(x+dx, y, 0.0f);
		glEnd();
	}
}

void renderBitmapString(float x, float y, float z, char *string) 
{  
  char *c;
  glRasterPos3f(x, y,z);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
  }
}

void drawHist(int w, int h) 
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.0f, 0.0f, 0.0f);

	glTranslatef(-2.6f, -2.1f, 0.0f);
	float dx =  3.5f / 1000 ;
	
	float x = -0.1f;
	float y = 0.27f;
	int i = 0;
	
	// Draw the axis of the graph
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(-0.105f, y, -6.0f);
	glVertex3f(3.6f, y, -6.0f);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-0.1f, y,-6.0f);
	glVertex3f(-0.1f, y+2.1f, -6.0f);
	glEnd();
	

	// Draw the lines making up the histogram
	glColor3f(0.3f, 0.9f, 0.4f);	
	
	for(; x < 3.5f; x += dx)
	{
		
		if((hist[i] / histMax) > 0.0f && i < 999)
		{
			glBegin(GL_QUADS);
			glVertex3f(x, y, -6.0f);
			glVertex3f(x, y+ (2*(hist[i]/histMax))+0.269f, -6.0f);
			glVertex3f(x+dx, y+ (2*(hist[i]/histMax))+0.269f, -6.0f);
			glVertex3f(x+dx, y, -6.0f);
			glEnd();
		}
		
		//hy += dx;
		i++;
	}	
	
}

char* fn;
void update(int value)
{
	glutTimerFunc(60, update, 0);
	if(p == 0)
	{
		thresh += 0.2f;
	}
	_terrain = loadTerrain(fn, 20);
	glutPostRedisplay();	

}

void winDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int w = 600;
	int h = 800;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
	drawHist(w, h);

	glViewport(600, 0, w+190, h);
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
	int w = 1400;
	int h = 750;
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









