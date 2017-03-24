#include "query.h"
#include "VSM.h"
#include "BM25.h"
#include <iostream>

using namespace std;

int main() {
	
	map<int, DocInfo> DocList = map<int, DocInfo>();
	vector<RelInfo> RelList = vector<RelInfo>();

	IndexList index_list = IndexList();
	vector<Query> *query_list = new vector<Query>();
	QuerySetter q_str = QuerySetter();
	
	//query setting
	q_str.query_set(query_list);
	q_str.finish();
	cout << "Query ready" << endl;

	//load index list
	ifstream indexFile = ifstream("index_list.dat");
	string line;
	Index new_index;
	while (getline(indexFile, line)) {
		new_index.index_id = stoi(line.substr(0, line.find_first_of("\t")));
		line = line.substr(line.find_first_of("\t") + 1);

		new_index.word = line.substr(0, line.find_first_of("\t"));
		line = line.substr(line.find_first_of("\t") + 1);

		new_index.df = stoi(line.substr(0, line.find_first_of("\t")));
		line = line.substr(line.find_first_of("\t") + 1);

		new_index.cf = stoi(line.substr(0, line.find_first_of("\t")));
		line = line.substr(line.find_first_of("\t") + 1);

		new_index.iIndex = stoul(line.substr(0, line.find_first_of("\t")));
		index_list.set_index(new_index.word, new_index);
	}
	cout << "Index list ready" << endl;
	indexFile.close();

	//load doc list
	ifstream docFile = ifstream("Document.dat");
	int key;
	DocInfo doc_info;
	double sumdl=0.0;
	while (getline(docFile, line)) {
		key = stoi(line.substr(0, line.find_first_of("\t")));
		line = line.substr(line.find_first_of("\t") + 1);

		doc_info.doc_name = line.substr(0, line.find_first_of("\t"));
		line = line.substr(line.find_first_of("\t") + 1);

		doc_info.length = stoi(line.substr(0, line.find_first_of("\t")));
		sumdl += doc_info.length;
		DocList.insert(pair<int,DocInfo>(key,doc_info));
	}
	double avdl = sumdl / key;
	cout << "Doc list ready" << endl;
	docFile.close();
	//Relevant List
	ifstream relFile = ifstream("relevant_document.txt");
	RelInfo rel_info;
	while (getline(relFile, line)) {
		rel_info.query_num = stoi(line.substr(0, line.find_first_of("\t")));
		line = line.substr(line.find_first_of("\t") + 1);

		rel_info.doc_name = line.substr(0, line.find_first_of("\t"));
		RelList.push_back(rel_info);
	}
	cout << "Relevant list ready" << endl;
	docFile.close();

	//select model
	int menu = 0;
	while (1) {
		cout << "Select model" << endl;
		cout << "1. Vector Space Model" << endl;
		cout << "2. BM25 Model" << endl;
		cin >> menu;
		if (menu == 1) {
			//evaluate Query weight
			for (int i = 0; i < query_list->size(); i++) {
				query_list->at(i).eval_weight(&index_list);
			}
			cout << "Evaluating query weight complete" << endl;
			for (int i = 0; i < query_list->size(); i++) {
				VSM vs_model = VSM(query_list->at(i));
				vs_model.eval_cos(&index_list);
				vs_model.print_result(&DocList);
			}
		}
		else if (menu == 2) {
			for (int i = 0; i < query_list->size(); i++) {
				BM25 bm_model = BM25(query_list->at(i));
				bm_model.set_doc(&DocList);
				bm_model.set_rel(&RelList);
				bm_model.set_NR(DocList.size(), RelList.size());
				bm_model.set_var(1.2, 500, 0.75, avdl);
				bm_model.eval_weight(&index_list);
				bm_model.print_result(&DocList);
			}
		}
	}

	/*
	//check query list
	for (int i = 0; i < query_list->size(); i++) {
		Query temp_query = Query();
		temp_query = query_list->at(i);
		cout << "*********"<< i << "¹øÂ° Äõ¸®" <<"*********" << endl;
		cout << "Num : " << temp_query.get_num() << endl;
		cout << "Title : " << temp_query.get_title() << endl;
		cout << "Desc : " << temp_query.get_desc() << endl;
		cout << "Narr : " << temp_query.get_narr() << endl;

	}*/
	return 0;
}