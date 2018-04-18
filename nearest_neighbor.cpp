#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<cfloat>
#include<cmath>
#include "netflix_data_structure.h"
using namespace std;

int nearest_neighbor_rating(int,int,NetflixDataStructure*);
double k_nearest_neighbor_rating(int,int,NetflixDataStructure*);
double get_difference_between_users(int,int,NetflixDataStructure*);

int main(int argc, char **argv)
{
  //const k=5;
  string train_filename, test_filename;
  if(argc==3)
    {
      train_filename = argv[1];
      test_filename = argv[2];
    }
  else
    {
      cout << "please give the training data and test data files\n";
      return 1;
    }
  NetflixDataStructure *structure = new NetflixDataStructure(train_filename);
  int user,movie,days,rating,rating_guess;
  ifstream test_file(test_filename);
  string line;
  int i=0;
  double rmse=0;
  while(getline(test_file,line))
    {
      istringstream(line) >> user >> movie >> days >> rating;
      //cout << user << " " << movie << " " << rating << " ";
      rating_guess = nearest_neighbor_rating(user,movie,structure);
      //cout << rating_guess << endl;
      i++;
      cout << i << endl;
      rmse+=(rating-rating_guess)*(rating-rating_guess);
      //cout << i++ << endl;
    }
  rmse/=i;
  rmse = sqrt(rmse);
  cout << "rmse: " << rmse << endl;
  delete structure;
  test_file.close();
}

int nearest_neighbor_rating(int user, int movie, NetflixDataStructure *structure)
{
  int reviewer, movie_temp, days, rating, Return;
  double dist, min=DBL_MAX;
  structure->goto_head_movie(movie);
  structure->peek(&reviewer,&movie_temp,&days,&rating);
  int i,j,k,l;
  while(movie_temp==movie)
    {
      //cout << reviewer << endl;
      dist = get_difference_between_users(user, reviewer,structure);
      //cout << "Distance: " << dist << "\n";
      //structure->peek(&i,&j,&k,&l);
      
      if(dist < min)
	{
	  //cout << "setting min\n";
	  min = dist;
	  Return = rating;
	}
      //cout << "resetting to the last reviewer\n";
      structure->get_user_rating_of_movie(reviewer,movie_temp);
      //structure->peek(&i,&j,&k,&l);
      //cout << i << " " << j << " " << k << " " << l
      //   << endl;

      //cout << "going to the next reviewer\n";
      structure->get_next_mu(&reviewer,&movie_temp,&days,&rating);
      //cout << reviewer << " " << movie_temp << " " << days << " " << rating
      //   << endl << endl;
    } 
  return Return;
}

double get_difference_between_users(int user1, int user2, NetflixDataStructure *structure)
{
  //cout << user1 << " " << user2 << endl;
  int n_movies_reviewed=0;
  int user1_temp, movie1, days1, rating1;
  int user2_temp, movie2, days2, rating2;
  bool current_pos_is_user1;
  int ratings_difference=0;
  const int unrated=-1; //de facto rating of an unrated movie
  //negative because if somebody hasn't watched the movie, they didn't care
  //about it. could be 3 if unrated is meant to mean indifference.
  structure->goto_head_user(user1);
  structure->peek(&user1_temp,&movie1,&days1,&rating1); 
  structure->store_current_pos();
  structure->goto_head_user(user2);
  structure->peek(&user2_temp,&movie2,&days2,&rating2);
  current_pos_is_user1=false;
  //cout << user1_temp << " " << movie1 << " " << days1 << " " << rating1
  //   << " " << user2_temp << " " << movie2 << " " << days2 << " "
  //   << rating2 << endl;

  while((user1_temp==user1) && (user2_temp==user2))
    {
      if(movie1 < movie2)
	{
	  ratings_difference+=abs(rating1-unrated);
	  
	  n_movies_reviewed++;
	  if(!current_pos_is_user1)
	    {
	      structure->swap_current_and_stored_positions();
	      current_pos_is_user1 = !current_pos_is_user1;
	    }
	  structure->get_next_um(&user1_temp, &movie1, &days1, &rating1);
	}
      else if (movie2 < movie1)
	{
	  ratings_difference+=abs(rating2-unrated);
	  n_movies_reviewed++;
	  if(current_pos_is_user1)
	    {
	      structure->swap_current_and_stored_positions();
	      current_pos_is_user1 = !current_pos_is_user1;
	    }
	  structure->get_next_um(&user2_temp, &movie2, &days2, &rating2);
	}
      else
	{
	  ratings_difference+=abs(rating2-rating1);
	  n_movies_reviewed++;
	  if(current_pos_is_user1)
	    {
	      structure->get_next_um(&user1_temp, &movie1, &days1, &rating1);
	      structure->swap_current_and_stored_positions();
	      structure->get_next_um(&user2_temp, &movie2, &days2, &rating2);
	      current_pos_is_user1 = false;
	    }
	  else
	    {
	      structure->get_next_um(&user2_temp, &movie2, &days2, &rating2);
	      structure->swap_current_and_stored_positions();
	      structure->get_next_um(&user1_temp, &movie1, &days1, &rating1);
	      current_pos_is_user1 = true;
	    }
	}
      //cout << user1_temp << " " << movie1 << " " << days1 << " " << rating1
      //	   << " " << user2_temp << " " << movie2 << " " << days2 << " "
      //<< rating2 << endl;
    }
  
   if(user1_temp==user1 && user2_temp!=user2)
    {
      if(!current_pos_is_user1)
	structure->swap_current_and_stored_positions();
      do
	{
	  //cout << "Hi\n";	  
	  ratings_difference+=abs(rating1-unrated);
	  structure->get_next_um(&user1_temp, &movie1, &days1, &rating1);
	  n_movies_reviewed++;
	} while(user1_temp==user1);
    }
   else if (user2_temp == user2 && user1_temp != user1)
    {
      if(current_pos_is_user1)
	structure->swap_current_and_stored_positions();
      do
	{
	  ratings_difference+=abs(rating2-unrated);
	  structure->get_next_um(&user2_temp, &movie2, &days2, &rating2);
	  n_movies_reviewed++;
	} while(user1_temp==user1);
    }
   structure->peek(&user1_temp,&movie1,&days1,&rating1); 
   //cout << ((double)ratings_difference) / n_movies_reviewed << endl;
   return ((double)ratings_difference) / n_movies_reviewed;
}
