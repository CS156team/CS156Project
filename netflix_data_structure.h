#ifndef NETFLIX_DATA_STRUCTURE
#define NETFLIX_DATA_STRUCTURE
using namespace std;
#include<string>
/*
there are going to be a bunch of NetflixDataNodes, so I'm making them as small
as possible. unfortunately, given the size of the data, I can't make this
smaller than 17 bytes while storing the full information and next node.
However,by finding somewhere else to store the ratings, I can reduce this to
16 bytes, and by implementing an effective linked list without requiring any 
links, I can further reduce it to 8 bytes, and by simply chucking the user id 
away because it is taking up half the space, this is reduced to a mere 4 bytes.
Unfortunately, the C compiler sees two shorts and allocates 8 bytes to do it. 
So, I store all 4 integers in a single integer. How we find all four values 
will be explained later. this corresponds to a total size of 194 megabytes to 
store the full matrix
*/

class NetflixDataNode
{
 private:
  unsigned int Data;
 public:
  NetflixDataNode();
  NetflixDataNode(const short, const short, const int);
  void set_NetflixDataNode(const short, const short, const int);

  int get_MovieID() const;
  int get_Days() const;
  int get_rating() const;
};

class NetflixDataStructure
{
 private:
  NetflixDataNode **Lists;
  int n_users;
  int *n_ratings;
  int last_User;
  int last_Movie;

 public:
  NetflixDataStructure(string);
  ~NetflixDataStructure();
  int get_user_rating_of_movie(int,int);
  void get_next_using_of_movie(int*, int*, int*);
  int get_day_of_urm(int,int);
  
};

#endif
