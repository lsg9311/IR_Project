#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <math.h>
#include "index.h"
#include "final_stemmer.h"

using namespace std;

class Query {
private:
	int num;
	string title;
	string desc;
	string narr;
	vector<string> words;
	vector<string> sw_list;
	map<string,double> val_list; //tf list->weight list
public:
	Query();

	void set_num(int num);
	void set_title(string title);
	void set_desc(string desc);
	void set_narr(string narr);

	int get_num();
	string get_title();
	string get_desc();
	string get_narr();

	map<string, double> get_val();

	//set stopword list
	void set_stopword(vector<string>* sw_list);

	//remove stopword and stemming
	void parse_all();
	void parse_word(string text);
	bool is_stopword(vector<string>* sw_list,string word);
	
	//eval tf and weight
	void eval_tf();
	void eval_weight(IndexList* index_list);
};

class QuerySetter {
private:
	ifstream Qfile;
public:
	QuerySetter();
	void query_set(vector<Query>* query_list);
	void rm_word(vector<Query>* query_list);
	void finish();
};