#include "index.h"
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
	Index fail_index;
	fail_index.word = " ";
	map<string, Index>::iterator node;
	node = this->list.find(word);
	if (node == this->list.end()) {
		return fail_index;
	}
	else{
		Index result= node->second;
		return result;
	}
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