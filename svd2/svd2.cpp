

#include "svd2.hpp"
#include <vector>
#include <math.h>
#include <algorithm>    // std::shuffle
#include <iostream>
#include <fstream>


using namespace std;

	SVD2::SVD2(vector<vector<int> > * data, int size, float regularization, float learning_rate){
		X = data;
		Xsz = size;
		reg = regularization;
		eta = learning_rate;
		NUM_USERS = 458293;
		NUM_MOVIES = 17770;
		NUM_FEATS = 40;

	}
	SVD2::~SVD2(){
		//TODO
	}

	void SVD2::train(int max_epochs){
		// U: NUM_USERSxNUM_FEATS, initialized to 0.1
		U.resize(NUM_USERS, vector<float>(NUM_FEATS, 0.1));
		// V: NUM_MOVIESxNUM_FEATS, initialized to 0.1
		V.resize(NUM_MOVIES,vector<float>(NUM_FEATS, 0.1));
		cout << "U,V initialized"<<endl;

		// order to iterate through X
		vector<int> order;
		for (int o=0; o<Xsz;o++){
			order.push_back(o);
		}
		float old_err = 0;
		for (int n=0;n<max_epochs;n++){
			cout << "EPOCH: "<< n << endl;

			// iterate through each feature
			for (int f=0; f< NUM_FEATS; f++){
				// shuffle order to go through X
				std::random_shuffle(order.begin(), order.end()); 
				cout << "Feature: " << f << " of " << NUM_FEATS << endl;
				for (int o=0;o<Xsz;o++){
					//progress update
					// if (o % 200000 == 0) {cout<<(float)o/Xsz<<endl;}

					// training example data:
					int i = (*X)[order[o]][0]-1;
					int j = (*X)[order[o]][1]-1;
					int Xij = (*X)[order[o]][3];
					float pred = 0;
					for (int g=0; g<NUM_FEATS;g++){
						pred+= U[i][g]*V[j][g];
					}
					float err = eta * (Xij - pred);
					float uuf = U[i][f];
					U[i][f] += err*V[j][f];
					V[j][f] += err* uuf;
				}
			}
			float err = get_error();
			cout << "RMSE: " << err << endl;
		}
	}

	void SVD2::predict(vector<vector<int> > * testdata, int tsz){
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
			if (pred>5.0){pred=5.0;} // upper bound predictions at 5
			if (pred<1.0){pred=1.0;} // lower bound predictions at 1
			pr << pred << "\n";
		}
		pr.close();
		

	}
	vector<float> SVD2::grad_U(int i, int j, int Xij){
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

	vector<float> SVD2::grad_V(int i, int j, int Xij){
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

	float SVD2::get_error(){
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
		return sqrt(tot/Xsz);
	}
