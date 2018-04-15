#include<iostream>
#include<string>
#include "netflix_data_structure.h"
using namespace std;

//test_data_structure.o um/all.dta will verify that 100% of the data can be
//correctly stored in around 400 megabytes.
//test_data_structure.o um/split_base.txt can be stored in less than 200 MB

int main(int argc, char **argv)
{
  int i,j;
  NetflixDataNode node;
  NetflixAddressNode address;
  int user, pos;
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
  address.set_Address(0,0);
  address.get_Address(&user,&pos);
  cout << user << " " << pos << endl;
  address.set_Address(458293,1675);
  address.get_Address(&user,&pos);
  cout << user << " " << pos << endl;
  cout << sizeof(NetflixDataNode) << " " << sizeof(NetflixAddressNode)<< endl;
  
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
  /*
  cout << "should print 1 5 2 1 2 3 3 3 3 2 -1 -1 -1:\n             ";
  cout << structure.get_user_rating_of_movie(4,1571) << " "
       << structure.get_user_rating_of_movie(4,2881) << " "
       << structure.get_user_rating_of_movie(4,3141) << " "
       << structure.get_user_rating_of_movie(4,4118) << " "
       << structure.get_user_rating_of_movie(4,5446) << " "
       << structure.get_user_rating_of_movie(4,5960) << " "
       << structure.get_user_rating_of_movie(4,6025) << " "
       << structure.get_user_rating_of_movie(4,8230) << " "
       << structure.get_user_rating_of_movie(4,13778) << " "
       << structure.get_user_rating_of_movie(4,13917) << " "
       << structure.get_user_rating_of_movie(4,1570) << " "
       << structure.get_user_rating_of_movie(4,17510) << " "
       << structure.get_user_rating_of_movie(4,5959) << endl;
  */
  /*
  for(i=1;i<458293;i++)
    {
      //cout << i << endl;
      for(j=1;j<17770;j++)
	structure.get_user_rating_of_movie(i,j);
    }
  */
  int movie,days,rating;
  i=0;
  while(structure.get_next_mu(&user,&movie,&days,&rating))
    {
      cout << user << " " << movie << " " << days << " "<<  rating  << endl;
    }
  cout << "done\n";
							   
  //cin >> i;
  return 0;
  
}
