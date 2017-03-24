#pragma once
#include "query.h"
#include <iostream>

class BM25 {
private:
	Query query;
	map<int, double> page_rank = map<int, double>();
	map<int, DocInfo> *DocList = new map<int, DocInfo>();
	map<string,int> RelList = map<string,int>();
	int R;
	int N, ni;
	double k1, k2, b, avdl;

	ifstream iiFile;
public:
	BM25();
	BM25(Query query);

	//set variable
	void set_doc(map<int, DocInfo>* doc_list);
	void set_rel(vector<RelInfo> *RelList);
	void set_NR(int N, int R);
	void set_var(double k1, double k2, double b, double avdl);

	//eval_weight
	double eval_K(int dl);
	void eval_weight(IndexList* index_list);
	void read_iindex(unsigned int ii_ptr, int index_id, double q_weight);
	void insert_rank(int doc_id, double result);

	//eval pre_value;
	int eval_ri(vector<string>* doc_name);
	double eval_pre(int R,int ri,int N,int ni);

	//show result
	void show_result(map<int, DocInfo>* doclist);
	//print txt
	void print_result(map<int, DocInfo>* doclist);
};