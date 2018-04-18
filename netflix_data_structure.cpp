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
stored in 15 bits, leaving 14 bits for the day, which needs 12.  So, by using 
bitshifts, I can store all three numbers in a single integer with a two bits bit
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
userID needs 19 bits, there are a maximum of 1675 ratings per user, so that 
needs just 11 bits to store. 11+19 = 30 <32
 */
void NetflixAddressNode::set_Address(int userID,int pos)
{
  Address = userID;
  Address <<= 11;
  Address += pos;
}

void NetflixAddressNode::get_Address(int *userID, int *pos) const
{
  *pos = Address%2048;
  *userID = Address >> 11;
}

/*
the information is stored a an array of arrays, each array indexed by the user 
id of some user, and is sorted according to movie id. This allows the rating 
of any movie by any user to be found in a time independent of the amount of 
users, and proportional to the log of the number of ratings the user made, by 
implementing a binary search. Also, one can determine the next rating in 
constant time, allowing the entire dataset to be analyzed in time linear in 
the number of datapoints. 

In order to get all the reviews of a specific movie, I store the indices
of the reviews of each movie in an analogous list. This functionality can be
toggled on and off at will in the future, maybe.
*/

NetflixDataStructure::NetflixDataStructure(string filename)
{
  SourceFile=filename;
  int i,j,k;
  last_pos=0;
  
  n_movies=17770;
  n_users=458293; //done by inspection of the data file
  UserReviews = new NetflixDataNode*[n_users];
  n_UserReviews = new int[n_users];
  MovieReviews = new NetflixAddressNode*[n_movies];
  n_MovieReviews = new int[n_movies];
  int *n_UserReviews_temp = new int[n_users];
  int *n_MovieReviews_temp = new int[n_movies];
  for(i=0;i<n_users;i++)
    n_UserReviews[i]=0;
  for(i=0;i<n_users;i++)
    n_UserReviews_temp[i]=0;
  for(i=0;i<n_movies;i++)
    n_MovieReviews[i]=0;
  for(i=0;i<n_movies;i++)
    n_MovieReviews_temp[i]=0;
  
  ifstream file(filename);
  string line;
  int user, rating;
  short movie, days;
  int temp_user, temp_pos;
  //get size of everything to be allocated;
  while(getline(file,line))
    {
      istringstream(line) >> user >> movie;
      n_UserReviews[user-1]++;
      n_MovieReviews[movie-1]++;
    }
  int max=0;
  
  //sometimes you just want to know things
  for(i=0;i<n_users;i++)
    if(n_UserReviews[i]>max)
      max=n_UserReviews[i];
  cout << "max ratings by user: " << max << endl;
  max=0;
  for(i=0;i<n_movies;i++)
    if(n_MovieReviews[i]>max)
      max=n_MovieReviews[i];
  cout << "max ratings of a movie: " << max << endl;

  file.close();
  file.open(filename);
  for(i=0;i<n_users;i++)
    UserReviews[i]=new NetflixDataNode[n_UserReviews[i]];
  for(i=0;i<n_movies;i++)
    MovieReviews[i] = new NetflixAddressNode[n_MovieReviews[i]];
  max=0;
  while(getline(file,line))
    {
      istringstream(line) >> user >> movie >> days >> rating;
      if(days>max)
	max=days;
      j=n_UserReviews_temp[user-1];
      k=n_MovieReviews_temp[movie-1];
      UserReviews[user-1][j].set_NetflixDataNode(movie,days,rating);
      MovieReviews[movie-1][k].set_Address(user-1,j);
      if((movie!=UserReviews[user-1][j].get_MovieID()) ||
	 (days!=UserReviews[user-1][j].get_Days()) || (rating!=UserReviews[user-1][j].get_rating()))
	cout << "data is not stored properly.\ngiven: " << user << " " << movie << " " << days << " " << rating << endl
	     << "stored: " << user << UserReviews[user-1][j].get_MovieID() << " "
	     << UserReviews[user-1][j].get_Days() << " " << UserReviews[user-1][j].get_rating() << endl;
      MovieReviews[movie-1][n_MovieReviews_temp[movie-1]].get_Address(&temp_user, &temp_pos);
      if((temp_user!=user-1) || (temp_pos!=j))
	cout << "corrupt address.\n given: " << user-1 << " " << j << endl
	     << "stored: " << temp_user << " " << temp_pos << endl;
      n_UserReviews_temp[user-1]++;
      n_MovieReviews_temp[movie-1]++;
      
    }
  cout << "max days: " << max << endl;
  last_User=-1;
  last_Movie=-1;
  delete[] n_UserReviews_temp;
  delete[] n_MovieReviews_temp;
  cout << MovieReviews << endl;
  cout << MovieReviews[5659] << endl;
  file.close();
}

NetflixDataStructure::~NetflixDataStructure()
{
  int i;
  for(i=0;i<n_users;i++)
    delete[] UserReviews[i];
  delete[] UserReviews;
  for(i=0;i<n_movies;i++)
    delete[] MovieReviews[i];
  delete[] MovieReviews;
  delete[] n_UserReviews;
  delete[] n_MovieReviews;
}

int NetflixDataStructure::get_user_rating_of_movie(int user, int movie)
{
  //  cout << MovieReviews[0] << endl;
  //cout << user << " " << movie << " " << n_UserReviews[user-1] << endl;
  //int i;
  //for(i=0; i<n_UserReviews[user-1]; i++)
  //cout << UserReviews[user-1][i].get_MovieID() << " ";
  //cout << endl;
  int next_pos = last_pos+1;
  int next_id;

  if(user == last_User+1)
    {
      if(movie==last_Movie+1)
	return last_rating;
      if(next_pos==n_UserReviews[user-1])
	next_pos--;
      next_id = UserReviews[last_User][next_pos].get_MovieID();
      if(movie > last_Movie+1 && movie < next_id)
	 return -1;
      if(next_id==movie)
	{
	  last_pos=next_pos;
	  last_Movie=movie-1;
	  last_days = UserReviews[user-1][last_pos].get_Days();
	  last_rating = UserReviews[user-1][last_pos].get_rating();
	  return last_rating;
	}
    }
  
  int pos = binary_search_user(user-1, movie, 0, n_UserReviews[user-1]-1);
  if(pos==-1)
    return -1;
  last_User=user-1;
  last_Movie=movie-1;
  last_pos=pos;
  last_days = UserReviews[user-1][pos].get_Days();
  last_rating = UserReviews[user-1][pos].get_rating();
  //cout << MovieReviews[0] << endl;
  return last_rating;
}

int NetflixDataStructure::binary_search_user(int user, int movie, int min, int max)
{
  int min_movie, max_movie;
  min_movie = UserReviews[user][min].get_MovieID();
  max_movie = UserReviews[user][max].get_MovieID();
  //cout << movie << " " << min << " " << min_movie << " " << " " << max << " " << max_movie << endl;
    
  if((min_movie > movie) || (max_movie < movie))
    return -1;
  int mid = (min+max)/2;
  int mid_movie = UserReviews[user][mid].get_MovieID();
if(movie==mid_movie)
    return mid;
  else if(movie < mid_movie)
    return binary_search_user(user,movie,min,mid);
  else
    return binary_search_user(user,movie,mid+1,max);
}

int NetflixDataStructure::binary_search_movie(int user, int movie, int min, int max)
{
  //cout << "in binary search movie\n";
  int min_user, max_user;
  int min_pos, max_pos;
  MovieReviews[movie][min].get_Address(&min_user, &min_pos);
  MovieReviews[movie][max].get_Address(&max_user, &max_pos);

  if((min_user > user) || (max_user < user))
    return -1;
  int mid = (min+max)/2;
  int mid_user, mid_pos;
  MovieReviews[movie][mid].get_Address(&mid_user, &mid_pos);

  //cout << user << " " <<  min << " " << min_user << " " << mid << " " << mid_user << " " <<  max << " " << max_user << endl;
  //cout << user << " " << mid_user << endl;
  if(user==mid_user)
    {
      //cout << "returning mid\n";
      return mid;
    }
  else if(user < mid_user)
    return binary_search_movie(user,movie,min,mid);
  else
    return binary_search_movie(user,movie,mid+1,max);
}


bool NetflixDataStructure::get_next_um(int *user, int *movie, int *days, int *rating)
{
  if(last_User==-1)
    {
      last_User=0;
      last_pos=-1;
      last_Movie = UserReviews[last_User][last_pos+1].get_MovieID();
    }
  else if(last_pos >= n_UserReviews[last_User] || last_Movie!=UserReviews[last_User][last_pos].get_MovieID()-1)
    {
      //cout << "binary search\n";
      last_pos = binary_search_user(last_User,last_Movie+1,0,n_UserReviews[last_User]);
    }
  last_pos++;
  if(last_pos >= n_UserReviews[last_User])
    {
      last_pos=0;
      last_User++;
      if(last_User!=n_users)
	while(n_UserReviews[last_User]==0)
	  last_User++;
    }
  //cout << last_User << endl;
  if(last_User==n_users)
    {
      //cout << "finishing\n";
      last_User=-1;
      *user=-1;
      *movie=-1;
      *days=-1;
      *rating=-1;
      return false;
    }
  last_Movie = UserReviews[last_User][last_pos].get_MovieID()-1;
  last_rating = UserReviews[last_User][last_pos].get_rating();
  last_days = UserReviews[last_User][last_pos].get_Days();
  *user = last_User+1;
  *movie = last_Movie+1;
  *days = last_days;
  *rating = last_rating;
  return true;
}

bool NetflixDataStructure::get_next_mu(int *user, int *movie, int *days, int *rating)
{
  //cout << "getting next mu\n";
  int temp_user, temp_pos;
  NetflixAddressNode* address;
  if(last_Movie==-1)
    {

      last_Movie=0;
      last_pos=-1;
      MovieReviews[last_Movie][last_pos+1].get_Address(&temp_user,&temp_pos);
      last_User = temp_user;
    }
  else 
    {
      //cout << "last_pos: " << last_pos << " total_reviews: " << n_MovieReviews[last_Movie] << endl;
      //cout << "last_User: " << last_User << " last_Movie: " << last_Movie << endl;
      if(last_pos < n_MovieReviews[last_Movie])
	{
	  //cout << "Hi!\n";
	  //cout << MovieReviews << endl;
	  //cout << MovieReviews[5659] << endl;
	  address = MovieReviews[last_Movie]+last_pos;
	  //cout << address << endl;
	  //cout << "Hi, again!\n";
	  MovieReviews[last_Movie][last_pos].get_Address(&temp_user,&temp_pos);
	  //cout << "Hi, again!\n";
		  
	  if(last_User!=temp_user)
	    {
	      //cout << "binary search\n";
	      last_pos = binary_search_movie(last_User,last_Movie,0,n_MovieReviews[last_Movie]-1);
	      //cout << last_pos << endl;
	    }
	}
      else
	{
	  //cout << "binary search\n";
	  last_pos = binary_search_movie(last_User,last_Movie,0,n_MovieReviews[last_Movie]-1);
	}
    }
  last_pos++;

  if(last_pos >= n_MovieReviews[last_Movie])
    {
      last_pos=0;
      last_Movie++;
      if(last_Movie!=n_movies)
	while(n_MovieReviews[last_Movie]==0)
	  last_Movie++;
    }
  //cout << last_User << endl;
  if(last_Movie==n_movies)
    {
      //cout << "finishing\n";
      last_Movie=-1;
      *user=-1;
      *movie=-1;
      *days=-1;
      *rating=-1;
      return false;
    }
  MovieReviews[last_Movie][last_pos].get_Address(&temp_user,&temp_pos);
  last_rating = UserReviews[temp_user][temp_pos].get_rating();
  last_days = UserReviews[temp_user][temp_pos].get_Days();
  last_User = temp_user;
  *user = temp_user+1;
  *movie = last_Movie+1;
  *days = last_days;
  *rating = last_rating;
  return true;
}

void NetflixDataStructure::peek(int *user, int *movie, int *days, int *rating) const
{
  *user=last_User+1;
  *movie=last_Movie+1;
  *days=last_days;
  *rating=last_rating;
}

void NetflixDataStructure::store_current_pos()
{
  stored_User = last_User;
  stored_Movie = last_Movie;
  stored_pos = last_pos;
  stored_days = last_days;
  stored_rating = last_rating;
}

void NetflixDataStructure::goto_stored_pos()
{
  last_User = stored_User;
  last_Movie = stored_Movie;
  last_pos = stored_pos;
  last_days = stored_days;
  last_rating = stored_rating;
}

void NetflixDataStructure::swap_current_and_stored_positions()
{
  int temp;
  temp = stored_User;
  stored_User = last_User;
  last_User = temp;

  temp = stored_Movie;
  stored_Movie = last_Movie;
  last_Movie = temp;

  temp = stored_pos;
  stored_pos = last_pos;
  last_pos = temp;

  temp = stored_days;
  stored_days = last_days;
  last_days = temp;

  temp = stored_rating;
  stored_rating = last_rating;
  last_rating = temp;
}

void NetflixDataStructure::goto_head_user(int user)
{
  if(n_UserReviews[user]!=0)
    {
      last_User = user-1;
      last_Movie = UserReviews[user-1][0].get_MovieID()-1;
      last_pos = 0;
      last_days = UserReviews[user-1][0].get_Days();
      last_rating = UserReviews[user-1][0].get_rating();
    }
}

void NetflixDataStructure::goto_head_movie(int movie)
{
  int temp_user,temp_pos;
  if(n_MovieReviews[movie-1]!=0)
    {
      MovieReviews[movie-1][0].get_Address(&temp_user,&temp_pos);
      last_User = temp_user;
      last_Movie = movie-1;
      last_pos = 0;
      last_days = UserReviews[temp_user][temp_pos].get_Days();
      last_rating = UserReviews[temp_user][temp_pos].get_rating();
    }
}


