#include "vec3f.h"
#include <iostream>


// The Terrain class written by the really awesome Videotutorials guy
class Terrain {
	private:
		int w; //Width
		int l; //Length
		float** hs; //Heights
		Vec3f** normals;
		bool computedNormals; //Whether normals is up-to-date
	public:
		Terrain(int w2, int l2);
		
		~Terrain();
		
		int width();
		
		int length();
		
		//Sets the height at (x, z) to y
		void setHeight(int x, int z, float y);
		
		//Returns the height at (x, z)
		float getHeight(int x, int z);
		
		//Computes the normals, if they haven't been computed yet
		void computeNormals();
		
		//Returns the normal at (x, z)
		Vec3f getNormal(int x, int z);
};
