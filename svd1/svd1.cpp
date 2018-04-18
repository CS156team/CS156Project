

#include "svd1.hpp"
#include <vector>
#include <math.h>
#include <algorithm>    // std::shuffle
#include <iostream>
#include <fstream>


using namespace std;

	SVD1::SVD1(vector<vector<int> > * data, int size, float regularization, float learning_rate){
		X = data;
		Xsz = size;
		reg = regularization;
		eta = learning_rate;
		NUM_USERS = 458293;
		NUM_MOVIES = 17770;
		NUM_FEATS = 20;

	}
	SVD1::~SVD1(){
		//TODO
	}

	void SVD1::train(int max_epochs){
		// U: NUM_USERSxNUM_FEATS, initialized to random floats from -.5 to .5
		U.resize(NUM_USERS, vector<float>(NUM_FEATS, 0));
		for (int m=0; m<NUM_USERS;m++){
			for (int k=0;k<NUM_FEATS;k++){
				int pos = rand() % 2;
				if (pos==1) {U[m][k]= rand() / (float)RAND_MAX / 2.0;}
				else {U[m][k]= rand() / (float)RAND_MAX / -2.0;}
			}
		}
		// V: NUM_MOVIESxNUM_FEATS, initialized to random floats from -.5 to .5
		V.resize(NUM_MOVIES,vector<float>(NUM_FEATS, 0));
		for (int n=0; n<NUM_MOVIES;n++){
			for (int k=0;k<NUM_FEATS;k++){
				int pos = rand() % 2;
				if (pos==1) {V[n][k]= rand() / (float)RAND_MAX / 2.0;}
				else {V[n][k]= rand() / (float)RAND_MAX / -2.0;}
			}
		}
		cout << "U,V initialized"<<endl;
		// order to iterate through X
		vector<int> order;
		for (int o=0;o<Xsz;o++){
			order.push_back(o);
		}
		float old_err = 0;
		for (int n=0;n<max_epochs;n++){
			cout << "EPOCH: "<< n << endl;
			// shuffle order to go through X
			std::random_shuffle(order.begin(), order.end());
			for (int o=0;o<Xsz;o++){
				//progress update
				if (o % 200000 == 0) {cout<<(float)o/Xsz<<endl;}

				//update matrices for each training example
				int i = (*X)[order[o]][0]-1;
				int j = (*X)[order[o]][1]-1;
				int Xij = (*X)[order[o]][3];
				vector<float> gU = grad_U(i,j,Xij);
				vector<float> gV = grad_V(i,j,Xij);
				for (int f=0; f< NUM_FEATS;f++){ 
					U[i][f] = U[i][f] - gU[f];
					V[j][f] = V[j][f] - gV[f];
				}
			}
			float err = get_error();
			cout << "RMSE: " << err << endl;
		}
	}

	void SVD1::predict(vector<vector<int> > * testdata, int tsz){
		// prediction for user,movie = dot product of U[user][:], V[movie][:]
		ofstream pr; 
		pr.open("submission.dta");
		for (int k = 0; k < tsz; k++){
			int i = (*testdata)[k][0]-1;
			int j = (*testdata)[k][1]-1;
			float pred = 0;
			for (int f=0; f< NUM_FEATS;f++){
				pred += U[i][f] * V[j][f];
			}
			pr << pred << "\n";
		}
		pr.close();
		

	}
	vector<float> SVD1::grad_U(int i, int j, int Xij){
		// input:
			// i = row in U
			// j = row in V
			// (these also map training point X[i][j])
			// Xij = rating of user i on movie j
		vector<float> grad_U;
		float uidvj = 0.0;
		for (int f=0; f< NUM_FEATS;f++){
			uidvj += U[i][f] * V[j][f];
		}
		for (int f=0; f< NUM_FEATS; f++){
			//grad_U.push_back((1-reg*eta)*U[i][f]+eta*V[j][f]*(Xij-uidvj));
			grad_U.push_back(eta*(reg*U[i][f] - V[j][f]*(Xij-uidvj)));
		}
		return grad_U;
	}

	vector<float> SVD1::grad_V(int i, int j, int Xij){
		// input:
			// i = row in U
			// j = row in V
			// (these also map training point X[i][j])
			// Xij = rating of user i on movie j

		vector<float> grad_V;
		float uidvj = 0.0;
		for (int f=0; f< NUM_FEATS;f++){
			uidvj += U[i][f] * V[j][f];
		}
		for (int f=0; f< NUM_FEATS; f++){
			//grad_V.push_back((1-reg*eta)*V[j][f] + eta*U[i][f]*(Xij-uidvj));
			grad_V.push_back(eta*(reg*V[j][f] - U[i][f]*(Xij - uidvj)));
		}
		return grad_V;
	}

	float SVD1::get_error(){
		// RMSE = sqrt( sum( (truth-pred)^2 ) )
		float tot = 0.0;
		// looping through each training point
		for (int k = 0; k < Xsz; k++){
			// finding corresponding rows in U and V
			int i = (*X)[k][0]-1;
			int j = (*X)[k][1]-1;
			int truth = (*X)[k][3]; // actual rating
			// dot product Ui and Vj for prediction rating
			float pred = 0;
			for (int f=0; f< NUM_FEATS;f++){
				pred += U[i][f] * V[j][f];
			}
			//cout << truth << " ~ " << pred << endl;
			tot += (truth-pred)*(truth-pred);
		}
		return sqrt(tot)/Xsz;
	}
