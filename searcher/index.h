#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <map>

using namespace std;
//Relevant List
struct RelInfo {
	int query_num;
	string doc_name;
};

//Doc List
struct DocInfo {
	string doc_name;
	int length;
};

//TF_info//
typedef struct TF_info {
	int doc_id=0;
	string index;
	int tf;
}TF_info;

//INDEX LIST//
typedef struct Index {
	string word;
	int index_id = 0;
	int df;
	int cf;
	unsigned int iIndex = 0;
} Index ;

class IndexList {
private:
	map<string, Index> list;
public :
	IndexList();

	map<string, Index> get_list();
	Index get_index(string word);

	int get_size();

	void set_iIndex(string word, int iIndex);
	void set_index(string word, Index index);
	void set_data(map<string, Index>* pre_list);
};