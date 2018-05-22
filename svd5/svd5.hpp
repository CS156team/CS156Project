#ifndef SVD5H
#define SVD5H

#include <vector>
#include <string>

using namespace std;
class SVD5{

public:
	SVD5(vector<vector<int> > * tdata, int tsize, 
			vector<vector<int> > * vdata, int vsize);
	~SVD5();
	void train_main(int max_epochs);
	void train_epoch();
	float predict(int i, int j, int d);
	float validate();
	void gen_prediction(vector<vector<int> > * testdata, int tsz,string filename);
private:
	vector<vector<int> > X;
	int Xsz;
	vector<vector<int> > Xval;
	int Xvsz;
	double reg;
	double eta;
	int NUM_USERS;
	int NUM_MOVIES;
	int NUM_DAYS;
	int NUM_FEATS;
	float MINCHANGE;
	float MEAN;
	vector<vector<float> > U;
	vector<vector<float> > V;

	vector<vector<float> > IV;
	vector<vector<float> > sumIV;
	void fill_N();
	void baseline_init();
	vector<int> N;
	vector<int> o;

	// baseline datastructures
	vector<float> bu_vec;
	vector<vector<float>> but_vec;
	vector<float> au_vec;
	vector<vector<float>>dev_t_vec;
	vector<float>bi_vec;
	vector<vector<float>>bi_bin_vec;
	vector<float>cu_vec;
	vector<vector<float>>cut_vec;
	vector<vector<float>>bi_f_vec;
	vector<float>mean_date_rating;
	vector<float>mean_date_rating_count;
	vector<vector<int>>fui_vec;
	vector<vector<int>>Fui_vec;

	// baseline constants
	float lamda_bu;
  	float lamda_but;
    float lamda_au;
    float lamda_bi;
    float lamda_bi_bin;
    float lamda_cu;
    float lamda_cut;
    float lamda_bi_f;
  
    float rate_bu;
    float rate_but;
    float rate_au;
    float rate_bi;
    float rate_bi_bin;
    float rate_cu;
    float rate_cut;
    float rate_bi_f;
  
    float a; // log base
    float loga;

};

#endif