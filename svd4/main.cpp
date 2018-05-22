// Iman Wahle
// April 14 2018

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include "svd4.hpp"

using namespace std;

vector<vector<int> > traindata;
vector<vector<int> > valdata;
vector<vector<int> > hiddendata;
vector<vector<int> > probedata;
vector<vector<int> > testdata;

int NUM_USERS = 458293;
int NUM_MOVIES = 17770;
int NUM_DATAPTS = 102416306;
 
void read_data(){
	ifstream ds("um/all.dta");
	ifstream is("um/all.idx");
	char s[20];
	char t[2];
	int count = 0;
	int count_testdta = 0;
    while (ds.getline(s, 30)) {
    	//print progress
    	if (count % 10000000 == 0){cout<<(float)count/NUM_DATAPTS<<endl;}
    	count++;

    	is.getline(t,2);
    	vector<int> v;
		char * pch;
		pch = strtok (s," ");
		while (pch != NULL) {
			v.push_back(atoi(pch));
		  	pch = strtok (NULL, " ");
		}

		if (t[0]-'0'==1){ traindata.push_back(v); }
		else if (t[0]-'0'==2){ valdata.push_back(v); }
		else if (t[0]-'0'==3){ hiddendata.push_back(v); }
		else if (t[0]-'0'==4){ probedata.push_back(v); }
		else if (t[0]-'0'==5){ testdata.push_back(v);}
		else {cout << "yikes"<< endl;return;}
    }
    cout << "Num train data: " << traindata.size() << endl;
    cout << "Num val data: " << valdata.size() << endl;
    cout << "Num test data: " << testdata.size()<< endl;
    ds.close();
}
int main(){
	// read in data into NUM_USERSxNUM_MOVIES rating matrix
	cout << "Reading in data" << endl;
	read_data();
	cout << "Finished reading" << endl;

	// run svd
	float reg = 0.0015;
	float eta = .008;
	int max_epochs = 20; // TODO: implement early stopping
	SVD4 svd(&traindata, traindata.size(), &valdata, valdata.size(), reg, eta); 
	cout << "Beginning training"<<endl;
	svd.train_main(max_epochs);
	cout << "Finished training" << endl;

	// predict new ratings
	cout << "Making predictions to submit" << endl;
	svd.gen_prediction(&testdata,testdata.size(), "testpred2.dta");
	cout << "Predictions finished" << endl;

	cout << "Making predictions on probe for blend" << endl;
	svd.gen_prediction(&probedata,probedata.size(), "probepred.dta");
	cout << "Predictions finished" << endl;
	return 0;
}