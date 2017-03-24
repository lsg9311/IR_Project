#pragma once
#include "info.h"
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>

using namespace std;

typedef struct doc_info {
	int doc_id;
	string doc_name;
	int length;
}Doc_info;

class DocReader {
private:
	ifstream file;
	int doc_id;
	Doc_info doc_info;
	map <string,TF_info> tf_list;
public:
	DocReader();

	void set_id(int id);
	
	bool file_open(string filepath);
	bool file_close();

	map <string, TF_info> get_TF();
	Doc_info get_doc();

	void parse();
	void parse_word(string text);
	void insert_tf(string word);
};

class DocsReader {
private:
	string root;
	vector<string> filepath_list;
	list<TF_info> tf_info;
	unordered_map<int, Doc_info> doc_info;
	map<string, Index> pre_index_list;
	int doc_id;
	ofstream fileTF;
public:
	DocsReader();

	void set_root(string root);
	void crawl_list();

	void read_files();
	void insert_list(list <TF_info>* tf_list);
	map<string, Index> get_list();

	void add_tf(map<string, TF_info>* new_tf);

	void write_tf();
	void write_doc_info();

	void tf_close();
};