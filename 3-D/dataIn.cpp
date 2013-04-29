// writing on a text file
#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;

int main () 
{
  ifstream myfile ("Data.txt");
  float f;
  float maxHeight = -9999999.0f, minHeight = 99999999.0f;
  if (myfile.is_open())
  {
  	myfile >> f;
  	myfile >> f;
    int i = 0;
    while(myfile >> f)
    {
      if(f > maxHeight)
      	maxHeight = f;
      if(f < minHeight)
      	minHeight = f;

      
    }
    
    cout << minHeight << "\n";
    cout << maxHeight << "\n";	


    myfile.close();
  }
  else cout << "Unable to open file";
  return 0;
}
