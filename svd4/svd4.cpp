#include "svd4.hpp"
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>     /* abs */
#include <string>
#include <algorithm>    // std::shuffle
#include <cmath>
#include <ctime>


///////////////////////////////////////////////////////
/* Things to do:
-shuffle
-regularization parameters
-randomize initialization
-cache sumIV
*/
//////////////////////////////////////////////////////
SVD4::SVD4(vector<vector<int> > * tdata, int tsize, 
			vector<vector<int> > * vdata, int vsize, 
			float regularization, float learning_rate) {
	X = tdata;
	Xsz = tsize;
	Xval = vdata;
	Xvsz = vsize;
	reg = regularization;
	eta = learning_rate;
	NUM_USERS = 458293;
	NUM_MOVIES = 17770;
	NUM_FEATS = 40;
	MEAN = 3.6095;
	MINCHANGE = 0.0005;
	U.resize(NUM_USERS, vector<float>(NUM_FEATS, 0.0));
	V.resize(NUM_MOVIES, vector<float>(NUM_FEATS, 0.0));
	bu.resize(NUM_USERS, 0.0);
	bi.resize(NUM_MOVIES, 0.0);
	IV.resize(NUM_MOVIES, vector<float>(NUM_FEATS, 0.0));
	sumIV.resize(NUM_USERS, vector<float>(NUM_FEATS, 0.0));
	N.resize(NUM_USERS,0);
	bulrate = 0.00267;
	bureg = 0.0255;
	bilrate = 0.00048;
	bireg = 0.0255;

	for (int a=0; a<Xsz;a++){
		o.push_back(a);
	}
	for (int i=0;i<NUM_USERS;i++){
		for (int j=0;j<NUM_FEATS;j++){
			U[i][j]= static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/0.01));
		}
	}

	for (int i=0;i<NUM_MOVIES;i++){
		for (int j=0;j<NUM_FEATS;j++){
			V[i][j]= static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/0.01));
		}
	}
}
SVD4::~SVD4() {
	// TODO
}

void SVD4::fill_N(){
	int i;
	for (int t=0; t< Xsz; t++){
		i = (*X)[t][0]-1;
		N[i] += 1;
	}
	for (int i=0; i<N.size(); i++){
		if (N[i] == 0){ cout << "cries" << endl;}
	}
}


void SVD4::train_main(int max_epochs) {

	cout << "Filling N... ";	
	fill_N();
	cout << "finished" << endl;
	float oldRMSE = 1000;
	for (int ep = 0; ep < max_epochs; ep++){
		cout << "Epoch: " << ep << endl;
		if (ep % 2 == 0){
			cout << "Shuffling order" <<endl;
			std::random_shuffle(o.begin(), o.end()); 
		}
		train_epoch();
		eta=eta*0.9;
		cout << "Validating... "<<endl;
		float RMSE = validate();
		cout << "finished" << endl;
		cout << "RMSE: " << RMSE << endl;
		//if (std::abs(oldRMSE-RMSE)<MINCHANGE){return;}
		//oldRMSE = RMSE;

	}

}
void SVD4::train_epoch(){ 
	float pred,err,uif,vjf;
	int i,j,Xij;
	sumIV.resize(NUM_USERS, vector<float>(NUM_FEATS, 0.0));
	// iterate through all training points
	cout << "Filling sumIV... ";
	for (int t=0; t< Xsz; t++){
		i = (*X)[t][0]-1;
		j = (*X)[t][1]-1;

		// fill out sumIV
		for (int f=0; f<NUM_FEATS; f++){
			sumIV[i][f] += IV[j][f];
		}
	}
	cout << "finished" << endl;
	std::clock_t c1, c2, c3, c4;
	cout << "SGD... " << endl;
	for (int t=0; t< Xsz; t++){
		c1 = std::clock();
		i = (*X)[o[t]][0]-1;
		j = (*X)[o[t]][1]-1;
		Xij = (*X)[o[t]][3];
		if (t%1000000==0){cout << "assignment: " << (std::clock() - c1) / (long double)(CLOCKS_PER_SEC) << endl;}
		c2 = std::clock();
		err = Xij - predict(i,j);
		if (t%1000000==0){cout << "predict: " << (std::clock() - c2) / (long double)(CLOCKS_PER_SEC) << endl;}

		// update Bu[i] and Bi[j]
		c3 = std::clock();
		bu[i] += bulrate*(err - bureg*bu[i]);
		bi[j] += bilrate*(err - bureg*bi[j]);
		if (t%1000000==0){cout << "baseline updates: " << (std::clock() - c3) / (long double)(CLOCKS_PER_SEC) << endl;}

		// update U[i][f] and V[j][f] at each feature f
		c4 = std::clock();
		for (int f=0; f<NUM_FEATS; f++){
			uif = U[i][f];
			vjf = V[j][f];
			U[i][f] += eta*(err * vjf - reg*uif);
			V[j][f] += eta*(err*(uif + 1.0/(float)sqrt(N[i])*sumIV[i][f])- reg*vjf);			
		}
		// update IV
		for (int f=0; f<NUM_FEATS; f++){
			IV[j][f] += eta*(err*(1.0/(float)sqrt(N[i])*sumIV[i][f]) - reg*IV[j][f]);
		}
		if (t%1000000==0){cout << "matrix updates: " << (std::clock() - c4) / (long double)(CLOCKS_PER_SEC) << endl;}

	}
	cout << "finished" << endl;
}

float SVD4::predict(int i, int j) {
	float dot = 0.0;
	for (int f=0; f<NUM_FEATS; f++){
		dot += V[j][f]* (U[i][f] + 1.0/(float)sqrt(N[i]) * sumIV[i][f]);
	}
	float pred = MEAN + bu[i] + bi[j] + dot;
	if (pred>5.0){pred=5.0;} // upper bound predictions at 5
	if (pred<1.0){pred=1.0;} // lower bound predictions at 1
	return pred;
}

float SVD4::validate() {
	float tot = 0.0;
	int i,j,truth;
	float pred;
	// looping through each training point
	for (int k=0; k<Xvsz;k++){
		// finding corresponding rows in U and V
		i = (*Xval)[k][0]-1;
		j = (*Xval)[k][1]-1;
		truth = (*Xval)[k][3]; // actual rating

		// dot product Ui and Vj for prediction rating
		pred = predict(i,j);
		tot += (truth-pred)*(truth-pred);
	}
	return sqrt(tot/(float)Xvsz);
}

void SVD4::gen_prediction(vector<vector<int> > * testdata, int tsz, string filename){
	ofstream pr; 
	pr.open(filename);
	int i,j;
	for (int k=0; k<tsz; k++){
		i = (*testdata)[k][0]-1;
		j = (*testdata)[k][1]-1;
		pr << predict(i,j) << "\n";
	}
	pr.close();
}