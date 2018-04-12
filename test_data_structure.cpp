#include<iostream>
#include<string>
#include "netflix_data_structure.h"
using namespace std;

//test_data_structure.o um/all.dta will verify that 100% of the data can be
//correctly stored in around 400 megabytes.
//test_data_structure.o um/split_base.txt can be stored in less than 200 MB

int main(int argc, char **argv)
{
  int i;
  NetflixDataNode node;
  
  for(i=0;i<6;i++)
    {
      node.set_NetflixDataNode(1,1,i);
      cout << node.get_MovieID() << " "
	   << node.get_Days() << " "  << node.get_rating() << endl;
    }
  
  for(i=0;i<6;i++)
    {
      node.set_NetflixDataNode(17770,7000,i);
      cout << node.get_MovieID() << " "
	   << node.get_Days() << " "  << node.get_rating() << endl;
    }
  cout << sizeof(NetflixDataNode) <<  endl;
  
  string filename;
  if(argc==2)
    {
      filename = argv[1];
    }
  else
    {
      cout << "please just give the data file\n";
      return 1;
    }
  NetflixDataStructure structure(filename);
  cout << "done\n";
  cin >> i;
  return 0;
  
}
