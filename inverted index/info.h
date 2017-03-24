#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <map>

using namespace std;

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
	int iIndex = 0;
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

	void write_to_txt();
};

//INVERTED INDEX LIST//
typedef struct Invert {
	int index_id;
	int doc_id;
	int tf;
	double weight;
}Invert;
typedef struct Invert_id {
	int index_id;
	int doc_id;

	bool operator==(const Invert_id &Iid) const{
		return index_id == Iid.index_id && doc_id == Iid.doc_id;
	}

	bool operator<(const Invert_id &Iid) const{
		if (index_id == Iid.index_id)
			return doc_id < Iid.doc_id;
		else
			return index_id < Iid.index_id;
	}
}Invert_id;

typedef struct Invert_val {
	int tf;
	double weight;
	string secret_key = "";
}Invert_val;


class IindexList {
private:
	map<Invert_id, Invert_val> sorted_list;
	ifstream tfFile;
	int N;

	void eval_doc(IndexList* index_list, int doc_id);
	
public:
	IindexList();

	void close_tf();
	void eval_all(IndexList* index_list);
	void print_list();

	void write_to_txt();
};