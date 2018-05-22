#include "svd5.hpp"
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>     /* abs */
#include <string>
#include <algorithm>    // std::shuffle
#include <cmath>
#include <ctime>

using namespace std;

///////////////////////////////////////////////////////
/* Things to do:
-shuffle
-regularization parameters
-randomize initialization
-cache sumIV
*/
//////////////////////////////////////////////////////
SVD5::SVD5(vector<vector<int> > * tdata, int tsize, 
			vector<vector<int> > * vdata, int vsize) {
	// training and validation data
	X = *tdata;
	Xsz = tsize;
	Xval = *vdata;
	Xvsz = vsize;

	// matrix factorization
	NUM_USERS = 458293;
	NUM_MOVIES = 17770;
	NUM_DAYS = 2243;
	NUM_FEATS = 40;

	MEAN = 3.6095;


	MINCHANGE = 0.0005;
	U.resize(NUM_USERS, vector<float>(NUM_FEATS, 0.0));
	V.resize(NUM_MOVIES, vector<float>(NUM_FEATS, 0.0));
	eta = 0.008;
	reg = 0.0015;

	for (int i=0;i<NUM_USERS;i++){
		for (int j=0;j<NUM_FEATS;j++){
			U[i][j]= static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/0.3));
		}
	}

	for (int i=0;i<NUM_MOVIES;i++){
		for (int j=0;j<NUM_FEATS;j++){
			V[i][j]= static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/0.3));
		}
	}

	// implicit user bias
	IV.resize(NUM_MOVIES, vector<float>(NUM_FEATS, 0.0));
	sumIV.resize(NUM_USERS, vector<float>(NUM_FEATS, 0.0));
	N.resize(NUM_USERS,0);

	// shuffling order
	for (int a=0; a<Xsz;a++){
		o.push_back(a);
	}

	// resize baseline predictors
	bu_vec.resize(NUM_USERS, 0.0);
	but_vec.resize(NUM_USERS, vector<float>(NUM_DAYS, 0.0));
	au_vec.resize(NUM_USERS, 0.0);
	dev_t_vec.resize(NUM_USERS, vector<float>(NUM_DAYS, 0.0));
	bi_vec.resize(NUM_MOVIES, 0.0);
	bi_bin_vec.resize(NUM_MOVIES, vector<float>(30, 0.0));
	cu_vec.resize(NUM_USERS, 1.0);
	cut_vec.resize(NUM_USERS, vector<float>(NUM_DAYS, 0.0));
	bi_f_vec.resize(NUM_MOVIES, vector<float>(5, 0.0));
	mean_date_rating.resize(NUM_USERS, 0.0);
	mean_date_rating_count.resize(NUM_USERS, 0.0);

	Fui_vec.resize(NUM_USERS, vector<int>(NUM_DAYS, 1)); 
	fui_vec.resize(NUM_USERS, vector<int>(NUM_DAYS, 0)); 

	// baseline constants

	lamda_bu = 0.0255;
  	lamda_but = 0.00231;
  	lamda_au = 3.95;
   	lamda_bi = 0.0255;
    lamda_bi_bin = 0.0929;
    lamda_cu = 0.0476;
    lamda_cut = 0.019;
    lamda_bi_f = 1.1E-8;

    rate_bu = 0.00267;
    rate_but = 0.00257;
    rate_au = 3.11E-6;
    rate_bi = 0.00048;
    rate_bi_bin = 0.000115;
    rate_cu = 0.00564;
    rate_cut = 0.00103;
    rate_bi_f = 0.00236;

    a = 6.76; // log base
    loga = log(a);
}

SVD5::~SVD5() {
	// TODO
}

void SVD5::baseline_init(){
	int u,d;
	for (int j=0; j< Xsz; j++){
		u = (X)[o[j]][0];
		d = (X)[o[j]][2];

		mean_date_rating[u] = mean_date_rating[u] + d + 1; // + 1 because d is index
		mean_date_rating_count[u] = mean_date_rating_count[u] + 1;
		Fui_vec[u][d] = Fui_vec[u][d] + 1;
	}

	for (int j=0; j<NUM_USERS; j++){
		if (mean_date_rating_count[j] != 0) {
			mean_date_rating[j] = mean_date_rating[j] / (float) mean_date_rating_count[j];
		}
	}
	for (int j=0; j<NUM_USERS; j++){
		for (int l=0; l<NUM_DAYS; l++){
			fui_vec[j][l] = int(log(Fui_vec[j][l])/loga);
	    //if (fui_vec[j][l] !=0) {cout << fui_vec[j][l] <<endl;}
		}
	}
	for (int j=0; j<Xsz; j++){
		u = (X)[o[j]][0];
		d = (X)[o[j]][2];
		if ((d+1 - mean_date_rating[u]) > 0){
			dev_t_vec[u][d] = pow((d+1 - mean_date_rating[u]), 0.4);
		}
		else{
			dev_t_vec[u][d] = -1*pow((mean_date_rating[u] - (d+1)), 0.4);
		}
	}
}

void SVD5::fill_N(){
	int i;
	for (int t=0; t< Xsz; t++){
		i = (X)[t][0];
		N[i] += 1;
	}
	for (int i=0; i<N.size(); i++){
		if (N[i] == 0){ cout << "cries" << endl;}
	}
}


void SVD5::train_main(int max_epochs) {

	cout << "Filling N... " << endl;
	fill_N();
	cout << "Initializing baseline factors... " << endl;
	baseline_init();
	cout << "finished" << endl;

	float oldRMSE = 1000;
	for (int ep = 0; ep < max_epochs; ep++){
		cout << "Epoch: " << ep << endl;
		if (ep % 2 == 0){
			cout << "Shuffling order" <<endl;
			std::random_shuffle(o.begin(), o.end()); 
		}
		train_epoch();
		eta=eta*0.99;
		cout << "Validating... "<<endl;
		float RMSE = validate();
		cout << "finished" << endl;
		cout << "RMSE: " << RMSE << endl;
		//if (std::abs(oldRMSE-RMSE)<MINCHANGE){return;}
		//oldRMSE = RMSE;

	}

}
void SVD5::train_epoch(){ 

	float pred, err, uuf, vmf;
	int u, m, d, bin_idx, Xij;
	sumIV.resize(NUM_USERS, vector<float>(NUM_FEATS, 0.0));
	// iterate through all training points
	cout << "Filling sumIV... ";
	for (int t=0; t< Xsz; t++){
		u = (X)[t][0];
		m = (X)[t][1];

		// fill out sumIV
		for (int f=0; f<NUM_FEATS; f++){
			sumIV[u][f] += IV[m][f];
		}
	}
	cout << "finished" << endl;

	//std::clock_t c1, c2, c3, c4;
	cout << "SGD... " << endl;
	for (int t=0; t< Xsz; t++){
		if (t%20000000==0){cout << "  -progress: " << t/(float)Xsz << endl;}
		//c1 = std::clock();
		u = (X)[o[t]][0];
		m = (X)[o[t]][1];
		d = (X)[o[t]][2];
		bin_idx = (int)(d/75);
		Xij = (X)[o[t]][3];
		//if (t%1000000==0){cout << "assignment: " << (std::clock() - c1) / (long double)(CLOCKS_PER_SEC) << endl;}
		//c2 = std::clock();
		err = Xij - predict(u, m, d);
		//if (t%1000000==0){cout << "predict: " << (std::clock() - c2) / (long double)(CLOCKS_PER_SEC) << endl;}

		// baseline updates
		//c3 = std::clock();
		bu_vec[u] += rate_bu*(err - lamda_bu*bu_vec[u]);

		but_vec[u][d] += rate_but*(err - lamda_but*but_vec[u][d]);

		au_vec[u] += rate_au*(dev_t_vec[u][d] * err - lamda_au*au_vec[u]);

		bi_vec[m] += rate_bi*((cu_vec[u] + cut_vec[u][d])*err - lamda_bi*bi_vec[m]);

		bi_bin_vec[m][bin_idx] += rate_bi_bin*((cu_vec[u] + cut_vec[u][d])*err - lamda_bi_bin*bi_bin_vec[m][bin_idx]);

		cu_vec[u] += rate_cu*((bi_vec[m] + bi_bin_vec[m][bin_idx])*err - lamda_cu*(cu_vec[u]-1));	      

		cut_vec[u][d] += rate_cut*((bi_vec[m] + bi_bin_vec[m][bin_idx])*err - lamda_cut*cut_vec[u][d]);	  
		
    	bi_f_vec[m][fui_vec[u][d]] += rate_bi_f*(err - lamda_bi_f*bi_f_vec[m][fui_vec[u][d]]);	     
		//if (t%1000000==0){cout << "baseline updates: " << (std::clock() - c3) / (long double)(CLOCKS_PER_SEC) << endl;}

		//if (t%100000==0){cout << bu_vec[u] << but_vec[u][d] << au_vec[u] << bi_vec[m] << bi_bin_vec[m][bin_idx] << cu_vec[u]
		//<< cut_vec[u][d] << bi_f_vec[m][fui_vec[u][d]] << endl;}
		// update U[i][f] and V[j][f] at each feature f
		//c4 = std::clock();
		for (int f=0; f<NUM_FEATS; f++){
			uuf = U[u][f];
			vmf = V[m][f];
			U[u][f] += eta*(err * vmf - reg*uuf);
			V[m][f] += eta*(err*(uuf + 1.0/sqrt(N[u])*sumIV[u][f])- reg*vmf);
			// update IV
			IV[m][f] += eta*(err*(1.0/sqrt(N[u])*sumIV[u][f]) - reg*IV[m][f]);			
		}
		//if (t%1000000==0){cout << "matrix updates: " << (std::clock() - c4) / (long double)(CLOCKS_PER_SEC) << endl;}

	}
	cout << "finished" << endl;
}

float SVD5::predict(int u, int m, int d) {
	float dot = 0.0;
	for (int f=0; f<NUM_FEATS; f++){
		dot += V[m][f]* (U[u][f] + 1.0/sqrt(N[u]) * sumIV[u][f]);
	}
	int bin_idx = int(d/75);
	float bui = MEAN + bu_vec[u] + au_vec[u] * dev_t_vec[u][d] + 
		  but_vec[u][d] + (bi_vec[m] + bi_bin_vec[m][bin_idx]) * 
		  (cu_vec[u] + cut_vec[u][d]) + bi_f_vec[m][fui_vec[u][d]];

	float pred = bui + dot;
	if (pred>5.0){pred=5.0;} // upper bound predictions at 5
	if (pred<1.0){pred=1.0;} // lower bound predictions at 1
	return pred;
}

float SVD5::validate() {
	float tot = 0.0;
	int i, j, d, truth;
	float pred;
	// looping through each training point
	for (int k=0; k<Xvsz;k++){
		// finding corresponding rows in U and V
		i = (Xval)[k][0];
		j = (Xval)[k][1];
		d = (Xval)[k][2];
		truth = (Xval)[k][3]; // actual rating

		// dot product Ui and Vj for prediction rating
		pred = predict(i,j,d);
		tot += (truth-pred)*(truth-pred);
	}
	return sqrt(tot/(float)Xvsz);
}

void SVD5::gen_prediction(vector<vector<int> > * testdata, int tsz, string filename){
	ofstream pr; 
	pr.open(filename);
	int i, j, d;
	for (int k=0; k<tsz; k++){
		i = (*testdata)[k][0];
		j = (*testdata)[k][1];
		d = (*testdata)[k][2];
		pr << predict(i,j,d) << "\n";
	}
	pr.close();
}