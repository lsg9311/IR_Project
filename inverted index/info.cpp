#include "info.h"
#include <iostream>
#include <math.h>

using namespace std;
IndexList::IndexList() {
	this->list = map<string, Index>();
}

int IndexList::get_size() {
	return this->list.size();
}

map<string, Index> IndexList::get_list() {
	return this->list;
}

Index IndexList::get_index(string word) {
	Index result= this->list.find(word)->second;
	return result;
}

void IndexList::set_iIndex(string word, int iIndex) {
	this->list.find(word)->second.iIndex=iIndex;
	return;
}

void IndexList::set_index(string word, Index index) {
	this->list.insert(pair<string, Index>(word, index));
	return;
}

void IndexList::set_data(map<string, Index>* pre_list) {
	map<string, Index>::iterator node;
	Index new_Index;
	int iter=0;
	for (node = pre_list->begin(); node != pre_list->end(); node++) {
		new_Index = node->second;
		new_Index.index_id = iter;
		this->list.insert(pair<string, Index>(new_Index.word, new_Index));
		iter++;
	}
}

void IndexList::write_to_txt() {
	ofstream outFile = ofstream("Index_list.dat");
	map<string, Index>::iterator node;

	for (node = this->list.begin(); node != this->list.end(); node++) {
		outFile << node->second.index_id << "\t" << node->second.word << "\t" << node->second.df << "\t" << node->second.cf << "\t" << node->second.iIndex << endl;
	}
	outFile.close();
}

/// INVERTED INDEX ///
IindexList::IindexList() {
	this->sorted_list = map<Invert_id, Invert_val>();
	this->tfFile = ifstream("TF.dat");
	
	ifstream docFile = ifstream("Document.dat");
	string temp;
	int cnt=0;
	while (getline(docFile, temp)) cnt++;
	docFile.close();
	
	this->N = cnt;
}
void IindexList::close_tf(){
	this->tfFile.close();
}

void IindexList::eval_all(IndexList* index_list) {
	//set Iindex
	map<Invert_id, Invert_val>::iterator node;
	int iIndex=0;
	string cur_index="";
	int id = 1;

	for (id = 1; id <= this->N; id++) {
		cout << id << "번째 문서 진행 중..." << endl;
		this->eval_doc(index_list, id);
	}
	for (node = this->sorted_list.begin(); node != this->sorted_list.end(); node++) {
		if (node->second.secret_key.compare(cur_index)!=0) {
			cur_index = node->second.secret_key;
			index_list->set_iIndex(cur_index, iIndex);
		}
		iIndex++;
	}
}

void IindexList::eval_doc(IndexList* index_list, int doc_id) {
	string tf_line;
	TF_info temp_tf;
	//bool find = 0;
	unordered_map<int, TF_info> doc_TFs = unordered_map<int, TF_info>();
	int iter = 0;
	//Invert temp_invert;
	Invert_id temp_Iid;
	Invert_val temp_Ival;
	int prev_pt=0;
	//load doc_tf
	while (getline(this->tfFile, tf_line)) {
		temp_tf.doc_id=stoi(tf_line.substr(0,tf_line.find_first_of("\t")));
		if (temp_tf.doc_id != doc_id) {
			this->tfFile.seekg(prev_pt, ios::beg);
			break;
		}
		prev_pt = this->tfFile.tellg();
		tf_line = tf_line.substr(tf_line.find_first_of("\t") + 1);

		//find = 1;
		temp_tf.index = tf_line.substr(0, tf_line.find_first_of("\t"));
		tf_line = tf_line.substr(tf_line.find_first_of("\t")+1);

		temp_tf.tf= stoi(tf_line.substr(0, tf_line.find_first_of("\t")));
		doc_TFs.insert(pair<int, TF_info>(iter, temp_tf));
		iter++;

	}
	double index_df;
	double normal_weight=0.0;
	//set value
	for (int i = 0; i < doc_TFs.size(); i++) {
		temp_tf = doc_TFs.find(i)->second;

		temp_Iid.doc_id = doc_id;
		temp_Iid.index_id = index_list->get_index(temp_tf.index).index_id;
		temp_Ival.secret_key = temp_tf.index;
		temp_Ival.tf = temp_tf.tf;

		index_df = index_list->get_index(temp_tf.index).df;

		temp_Ival.weight = (log((double)temp_Ival.tf) + 1)*(log(this->N / index_df));
		normal_weight += pow(temp_Ival.weight,2);
		this->sorted_list.insert(pair<Invert_id,Invert_val>(temp_Iid,temp_Ival));
	}
	//eval weight
	normal_weight = sqrt(normal_weight);
	map<Invert_id, Invert_val>::iterator node;
	for (node = this->sorted_list.begin(); node != this->sorted_list.end(); node++) {
		if (node->first.doc_id == doc_id)
			node->second.weight /= normal_weight;
	}
}

void IindexList::print_list() {
	map<Invert_id, Invert_val>::iterator node;

	for (node = this->sorted_list.begin(); node != this->sorted_list.end(); node++) {
		cout << node->first.index_id << "\t" << node->first.doc_id << "\t" << node->second.tf << "\t" << node->second.weight << endl;
	}
}

void IindexList::write_to_txt() {
	ofstream outFile = ofstream("Inverted_index.dat");
	map<Invert_id, Invert_val>::iterator node;
	for (node = this->sorted_list.begin(); node != this->sorted_list.end(); node++) {
		outFile << node->first.index_id << "\t" << node->first.doc_id << "\t" << node->second.tf << "\t" << node->second.weight << endl;
	}
	outFile.close();
}