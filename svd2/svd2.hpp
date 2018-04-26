#ifndef SVD2H
#define SVD2H

#include <vector>

using namespace std;
class SVD2{

public:
	SVD2(vector<vector<int> > * data, int size, float regularization, float learning_rate);
	~SVD2();
	void train(int max_epochs);
	void predict(vector<vector<int> > * testdata, int tsz);
	vector<float> grad_U(int i, int j, int Xij);
	vector<float> grad_V(int i, int j, int Xij);
	float get_error();
private:
	vector<vector<int> >* X;
	int Xsz;
	double reg;
	double eta;
	int NUM_USERS;
	int NUM_MOVIES;
	int NUM_FEATS;
	vector<vector<float> > U;
	vector<vector<float> > V;
	
	
};

#endif