#ifndef SVD4H
#define SVD4H

#include <vector>
#include <string>

using namespace std;
class SVD4{

public:
	SVD4(vector<vector<int> > * tdata, int tsize, 
			vector<vector<int> > * vdata, int vsize, 
			float regularization, float learning_rate);
	~SVD4();
	void train_main(int max_epochs);
	void train_epoch();
	float predict(int i, int j);
	float validate();
	void gen_prediction(vector<vector<int> > * testdata, int tsz,string filename);
private:
	vector<vector<int> >* X;
	int Xsz;
	vector<vector<int> >* Xval;
	int Xvsz;
	double reg;
	double eta;
	int NUM_USERS;
	int NUM_MOVIES;
	int NUM_FEATS;
	float MINCHANGE;
	float MEAN;
	vector<vector<float> > U;
	vector<vector<float> > V;
	vector<float> bu;
	vector<float> bi;
	vector<vector<float> > IV;
	vector<vector<float> > sumIV;
	void fill_N();
	vector<int> N;
	vector<int> o;
	float bulrate;
	float bureg;
	float bilrate;
	float bireg;

	
};

#endif