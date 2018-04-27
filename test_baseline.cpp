#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<math.h>
#include "netflix_data_structure.h"
using namespace std;

//test_data_structure.o um/all.dta will verify that 100% of the data can be
//correctly stored in around 400 megabytes.
//test_data_structure.o um/split_base.txt can be stored in less than 200 MB

int main(int argc, char **argv)
{
  int i,j;
  int user, pos;
 
  string filename;
  string validation_file = "./split_valid.txt";
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
  int movie,days,rating;
  
  double u;
  double avg_rating_per_movie[17770];
  double num_rating_per_movie[17770];



  double avg_rating_per_user[458293];
  double num_rating_per_user[458293];

  int last_movie = 1;
  double total_movie_rating = 0;
  double total_user_rating = 0;

  double bi, bu;

  
  for (int j=0; j<458293; j++){
    avg_rating_per_user[j] = 0;
    num_rating_per_user[j] = 0;
  }

  for (int j=0; j<17770; j++){
    avg_rating_per_movie[j] = 0;
    num_rating_per_movie[j] = 0;
  }

  u = 0;
  i=0;
  cout << "********* calculate movie bias and avg rating of movies ************ " << endl;
  int k = 0;
  while(structure.get_next_mu(&user,&movie,&days,&rating))
    {
      u = rating + u;
      avg_rating_per_movie[movie-1] = rating + avg_rating_per_movie[movie-1];
      num_rating_per_movie[movie-1] = num_rating_per_movie[movie-1] + 1;
      k++;
    }
  u = u/k;
  cout << "u is " << u << endl;

  for (int j=0; j<17770; j++){
    avg_rating_per_movie[j] = (avg_rating_per_movie[j]-u*num_rating_per_movie[j])/(25+num_rating_per_movie[j]);
    //cout << "avg_rating_per_movie[j] " << avg_rating_per_movie[j] << endl;
  }

  cout << "********* calculate user bias ************ " << endl;


  // calcualte bias from user

  NetflixDataStructure structure2(filename);
  while(structure2.get_next_mu(&user,&movie,&days,&rating)){
    avg_rating_per_user[user-1] = rating - u - avg_rating_per_movie[movie-1] + avg_rating_per_user[user-1];
    num_rating_per_user[user-1] = num_rating_per_user[user-1] + 1;
  }
 
  for (int j=0; j<458293; j++){
    avg_rating_per_user[j] = avg_rating_per_user[j] / (10+ num_rating_per_user[j]);
  }

  cout << "******** calculate RSME ********" << endl;


  // calculate RSME
  double total_score = 0;
  double num_ratings = 0;
  double RMSE = 0;
  double predicted_rating = 0;

  NetflixDataStructure structure3(validation_file);
  while(structure3.get_next_mu(&user,&movie,&days,&rating)){
    predicted_rating = u + avg_rating_per_movie[movie-1] + avg_rating_per_user[user-1];
    if (predicted_rating > 5) {
      predicted_rating = 5;
    }
    else if (predicted_rating < 0) {
      predicted_rating = 0;
    }
    total_score = (predicted_rating-rating)*(predicted_rating-rating);
    num_ratings++;
  }

  RMSE = sqrt(total_score/num_ratings)*100;
  
  cout << "RMSE" << " " << RMSE << endl;


  cout << "******** predicting result ********" << endl;

  // predicted rating
  string directory="./";
  string line;
  ofstream result(directory+"full_test_result.dta");

  ifstream qual(directory+"qual.dta");

  int user_test;
  int movie_test;
  int days_test;
  while(getline(qual, line)){
    istringstream(line) >> user_test >> movie_test >> days_test;
    predicted_rating = u + avg_rating_per_movie[movie_test-1] + avg_rating_per_user[user_test-1];
    result << predicted_rating;
    result << endl;
  }

  
  qual.close();
  result.close();


  return 0;
  
}
