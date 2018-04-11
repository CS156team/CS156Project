#include<iostream>
#include<fstream>
using namespace std;

int main(int argc, char **argv)

{
  string directory="./";
  int i,id;
  if(argc==2)
    {
      directory+=argv[1];
      directory+="/";
    }
  ifstream movieData(directory+"all.dta");
  ifstream idData(directory+"all.idx");
  int len_MovieData=0, len_IdData=0;
  string buffer;
  while(getline(movieData,buffer))
    {
      len_MovieData++;
    }
  while(getline(idData,buffer))
    {
      len_IdData++;
    }
  if(len_MovieData!=len_IdData)
    {
      cout << "files have different sizes\n";
      movieData.close();
      idData.close();
      return 1;
    }
  movieData.close();
  idData.close();
  movieData.open(directory+"all.dta");
  idData.open(directory+"all.idx");
  ofstream base(directory+"split_base.txt");
  ofstream valid(directory+"split_valid.txt");
  ofstream hidden(directory+"split_hidden.txt");
  ofstream probe(directory+"split_probe.txt");
  ofstream qual(directory+"split_qual.txt");
  for(i=0;i<len_MovieData;i++)
    {
      getline(movieData,buffer);
      idData >> id;
      switch(id)
	{
	case 1:
	  base << buffer;
	  break;
	case 2:
	  valid << buffer;
	  break;
	case 3:
	  hidden << buffer;
	  break;
	case 4:
	  probe << buffer;
	  break;
	case 5:
	  qual << buffer;
	  break;
	default:
	  cout << "invalid ID: " << id << " at index " << i << endl;
	  movieData.close();
	  idData.close();
	  base.close();
	  valid.close();
	  hidden.close();
	  probe.close();
	  qual.close();
	  return 1;
	}
    }
  movieData.close();
  idData.close();
  base.close();
  valid.close();
  hidden.close();
  probe.close();
  qual.close();
  return 0;
}
