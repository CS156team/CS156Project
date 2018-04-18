// Iman Wahle
// April 14 2018

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include "svd1.hpp"

using namespace std;

vector<vector<int> > lstdata;
vector<vector<int> > testdata;

int NUM_USERS = 458293;
int NUM_MOVIES = 17770;
int NUM_DATAPTS = 102416306;

void read_data(){
	ifstream ds("mu/all.dta");
	ifstream is("mu/all.idx");
	char s[20];
	char t[2];
	int count = 0;
	int count_testdta = 0;
    while (ds.getline(s, 30)) {
    	//print progress
    	if (count % 1000000 == 0){cout<<(float)count/NUM_DATAPTS<<endl;}
    	count++;

    	is.getline(t,2);
    	// save training set 1
    	if (t[0]-'0'==1){ //train set
	    	vector<int> v;
			char * pch;
			pch = strtok (s," ");
			while (pch != NULL) {
				v.push_back(atoi(pch));
			  	pch = strtok (NULL, " ,.-");
			}
			lstdata.push_back(v);
	    }
	    // save qual set 5
	    else if (t[0]-'0'==5){ // qual set
	    	count_testdta++;
	    	vector<int> v;
			char * pch;
			pch = strtok (s," ");
			while (pch != NULL) {
				v.push_back(atoi(pch));
			  	pch = strtok (NULL, " ,.-");
			}
			testdata.push_back(v);
	    }
    }
    cout << "Num test data: " << count_testdta << endl;
    ds.close();
}
int main(){
	// read in data into NUM_USERSxNUM_MOVIES rating matrix
	cout << "Reading in data" << endl;
	read_data();
	cout << "Finished reading" << endl;

	// run svd
	float reg = 0.01;
	float eta = .07;
	int max_epochs = 10;
	SVD1 svd(&lstdata, lstdata.size(), reg, eta); 
	cout << "Begining training"<<endl;
	svd.train(max_epochs);
	cout << "Finished training" << endl;

	// predict new ratings
	cout << "Making predictions" << endl;
	svd.predict(&testdata,testdata.size());
	cout << "Predictions finished" << endl;
}