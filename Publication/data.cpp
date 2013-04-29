#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <sstream>


using namespace std;



int main(int argc, char** argv) 
{
  
  char* fn = argv[1];
  
  // Read in file and get rid of the header
  ifstream data (fn);
  string s, sub;
  float f;
  int i = 0;
  
  while( i < 25 )
  {
     getline(data, s);

     if(i == 13 || i == 12)
     {	 	
        cout << s << endl;
        istringstream iss(s);
        iss >> sub;
        iss >> sub;
        iss >> sub;
        
        if (i == 12)
        {
           iss >> sub;
           iss >> sub;
        }

        if(i==13)
        {
   
          iss >> sub;
        }	
     } 
     
     cout << sub.c_str() << endl;

     i++;
  }
  
  //i = 0;
  
  while( i < 25 )
  {
  	data >> f;
  	cout << f << endl;
  	i++;
  }

  return 0;
  
}
