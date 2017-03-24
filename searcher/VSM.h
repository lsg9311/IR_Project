#pragma once
#include "query.h"
#include <iostream>

class VSM {
private:
	Query query;
	map<int, double> page_rank = map<int, double>();
	ifstream iiFile;
public:
	VSM();
	VSM(Query query);

	//evaluate cosine simillarity
	void eval_cos(IndexList* index_list);
	void read_iindex(unsigned int ii_ptr,int index_id, double q_weight);
	void insert_rank(int doc_id, double result);

	//show result
	void print_result(map<int, DocInfo>* doclist);
	void show_result(map<int, DocInfo>* doclist);
};