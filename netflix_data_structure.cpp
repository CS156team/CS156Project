#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include "netflix_data_structure.h"
using namespace std;

/*
how many bytes are needed to store the movie id, days, and rating? I'm not 
storing the user ID's because they are too large, and I can always find them 
later when the structure is being built). the rating is between 1 and 5 and 
thus needs 3 bits to store. the movie id is between 1 and 17770, so can be 
stored in 15 bits, leaving 14 bits for the day, which I haven't bothered to 
put a range on because effort is hard, but 13 seems about right. So, by using 
bitshifts, I can store all three numbers in a single integer with a single bit
to spare.
*/

NetflixDataNode::NetflixDataNode() : NetflixDataNode(0,0,0){};
NetflixDataNode::NetflixDataNode(const short inMovieID, const short inDays, const int inRating)
{
  set_NetflixDataNode(inMovieID, inDays, inRating);
}


void NetflixDataNode::set_NetflixDataNode(const short inMovieID, const short inDays, const int inRating)
{
  Data = inMovieID;
  Data <<=14;
  Data+=inDays;
  Data <<=3;
  Data+=inRating;
}

int NetflixDataNode::get_MovieID() const
{
  unsigned int copy=Data;
  copy>>=17;
  return copy; 
}

int NetflixDataNode::get_Days() const
{
  unsigned int copy=Data;
  copy>>=3;
  return copy%16384;
}

int NetflixDataNode::get_rating() const
{
  return Data%8;
}

/*
the information is stored a an array of arrays, each array indexed by the user 
id of some user, and is sorted according to movie id. This allows the rating 
of any movie by any user to be found in a time independent of the amount of 
users, and proportional to the log of the number of ratings the user made, by 
implementing a binary search. Also, one can determine the next rating in 
constant time, allowing the entire dataset to be analyzed in time linear in 
the number of datapoints. 
*/

NetflixDataStructure::NetflixDataStructure(string filename)
{
  int i,j;
  n_users=458293; //done by inspection of the data file
  Lists = new NetflixDataNode*[n_users];
  n_ratings = new int[n_users];
  for(i=0;i<n_users;i++)
    n_ratings[i]=0;
  
  ifstream file(filename);
  string line;
  int user, rating, last_user;
  short movie, days;
  while(getline(file,line))
    {
      istringstream(line) >> user >> movie >> days >> rating;
      n_ratings[user-1]++;
    }
  file.close();
  file.open(filename);
  for(i=0;i<n_users;i++)
    Lists[i]=new NetflixDataNode[n_ratings[i]];
  j=0;
  last_user=1;
  while(getline(file,line))
    {
      istringstream(line) >> user >> movie >> days >> rating;
      if(last_user!=user)
	j=0;
      Lists[user-1][j].set_NetflixDataNode(movie,days,rating);
      if((movie!=Lists[user-1][j].get_MovieID()) ||
	 (days!=Lists[user-1][j].get_Days()) || (rating!=Lists[user-1][j].get_rating()))
	cout << "data is not stored properly.\ngiven: " << user << " " << movie << " " << days << " " << rating << endl
	     << "stored: " << user << Lists[user-1][j].get_MovieID() << " "
	     << Lists[user-1][j].get_Days() << " " << Lists[user-1][j].get_rating() << endl;
      j++;
    }
  last_User=0;
  last_Movie=0;
}

NetflixDataStructure::~NetflixDataStructure()
{
  int i;
  for(i=0;i<n_users;i++)
    delete[] Lists[i];
  delete[] Lists;
  delete[] n_ratings;
}

