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

  //split the files, quit if an invalid id is found
  ofstream base(directory+"split_base.txt");
  ofstream valid(directory+"split_valid.txt");
  ofstream hidden(directory+"split_hidden.txt");
  ofstream probe(directory+"split_probe.txt");
  ofstream qual(directory+"split_qual.txt");
  while(getline(movieData,buffer) && idData >> id)
    {
      switch(id)
        {
        case 1:
          base << buffer;
          base << endl;
          break;
        case 2:
          valid << buffer;
          valid << endl;
          break;
        case 3:
          hidden << buffer;
          hidden << endl;
          break;
        case 4:
          probe << buffer;
          probe << endl;
          break;
        case 5:
          qual << buffer;
          qual << endl;
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
