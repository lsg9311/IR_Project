#include "VSM.h"

VSM::VSM() {
	this->query = Query();
	this->page_rank = map<int, double>();
}
VSM::VSM(Query query) {
	this->query = query;
	this->page_rank = map<int, double>();
}

void VSM::eval_cos(IndexList* index_list) {
	this-> iiFile = ifstream("Inverted_index.dat");
	map<string, double> val_list = this->query.get_val();
	map<string, double>::iterator val_node;
	Index cur_index;
	unsigned int ii_ptr;
	for (val_node = val_list.begin(); val_node != val_list.end(); val_node++) {
		cur_index=index_list->get_index(val_node->first);
		ii_ptr = cur_index.iIndex;
		this->read_iindex(ii_ptr,cur_index.index_id,val_node->second);
	}
	this->iiFile.close();
}

void VSM::read_iindex(unsigned int ii_ptr,int index_id, double q_weight) {
	string iiLine;
	int cur_index=10;
	int cur_doc=10;
	double cur_weight=0;
	double result=0;
	this->iiFile.seekg(ii_ptr, ios::beg);
	while (getline(this->iiFile, iiLine)) {
		//check index id
		cur_index = stoi(iiLine.substr(0, iiLine.find_first_of("\t")));
		if (cur_index != index_id) {
			break;
		}
		iiLine = iiLine.substr(iiLine.find_first_of("\t") + 1);

		//check doc id
		cur_doc = stoi(iiLine.substr(0, iiLine.find_first_of("\t")));
		iiLine = iiLine.substr(iiLine.find_first_of("\t") + 1);

		//skip tf
		iiLine = iiLine.substr(iiLine.find_first_of("\t") + 1);

		//eval weight
		cur_weight = stod(iiLine);
		result = cur_weight*q_weight;

		//insert rank
		this->insert_rank(cur_doc, result);
	}
}

void VSM::insert_rank(int doc_id, double result) {
	map<int, double>::iterator rank_node;
	rank_node = this->page_rank.find(doc_id);
	//new doc
	if (rank_node == this->page_rank.end()) {
		this->page_rank.insert(pair<int, double>(doc_id, result));
	}
	//has doc
	else {
		rank_node->second += result;
	}
	return;
}

bool comp_rank(std::pair<int, double> first, std::pair<int, double> second) {
	return first.second > second.second;
}

void VSM::show_result(map<int,DocInfo>* doclist) {
	cout << "Topic Number : " << this->query.get_num() << endl;
	cout << "Query : " << this->query.get_title() << endl;
	vector<pair<int, double>> sort_rank(this->page_rank.begin(),this->page_rank.end());
	sort(sort_rank.begin(), sort_rank.end(), comp_rank);

	string doc_name;
	int doc_id;
	for (int i = 0; i < 100; i++) {
		doc_id = sort_rank.at(i).first;
		doc_name = doclist->find(doc_id)->second.doc_name;
		cout << doc_name << "\t" << sort_rank.at(i).second << endl;
	}
	return;
}

void VSM::print_result(map<int, DocInfo>* doclist) {
	ofstream outFile = ofstream("VSM_result.txt", std::ios_base::out | std::ios_base::app);
	outFile << "Topic Number : " << this->query.get_num() << endl;
	outFile << "Query : " << this->query.get_title() << endl;
	vector<pair<int, double>> sort_rank(this->page_rank.begin(), this->page_rank.end());
	sort(sort_rank.begin(), sort_rank.end(), comp_rank);

	string doc_name;
	int doc_id;
	for (int i = 0; i < 100; i++) {
		doc_id = sort_rank.at(i).first;
		doc_name = doclist->find(doc_id)->second.doc_name;
		outFile << doc_name << "\t" << sort_rank.at(i).second << endl;
	}
	outFile.close();
	return;
}