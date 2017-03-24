#include "BM25.h"

BM25::BM25() {
	this->query = Query();
	this->page_rank = map<int, double>();
}
BM25::BM25(Query query) {
	this->query = query;
	this->page_rank = map<int, double>();
}

//set CONSTANT
void BM25::set_doc(map<int, DocInfo>* doc_list) {
	this->DocList = doc_list;
}

void BM25::set_rel(vector<RelInfo> *RelList) {
	int query_num = this->query.get_num();
	bool find = 0;
	RelInfo temp_rel;
	for (int i = 0; i < RelList->size(); i++) {
		temp_rel = RelList->at(i);
		if (temp_rel.query_num != query_num) {
			if (find == 0) continue;
			else if (find == 1) return;
		}
		else {
			if(find==0) find = 1;
			this->RelList.insert(pair<string, int>(temp_rel.doc_name, temp_rel.query_num));
		}
	}
	return;
}

void BM25::set_NR(int N, int R) {
	this->N = N;
	this->R = R;
}

void BM25::set_var(double k1, double k2, double b, double avdl) {
	this->k1 = k1;
	this->k2 = k2;
	this->b = b;
	this->avdl = avdl;
}

//eval weight
double BM25::eval_K(int dl) {
	double result = this->k1*((1 - this->b) + this->b*((double)dl / this->avdl));
	return result;
}

void BM25::eval_weight(IndexList* index_list) {
	this->iiFile = ifstream("Inverted_index.dat");
	map<string, double> val_list = this->query.get_val();
	map<string, double>::iterator val_node;
	Index cur_index;
	unsigned int ii_ptr;
	for (val_node = val_list.begin(); val_node != val_list.end(); val_node++) {
		cur_index = index_list->get_index(val_node->first);
		ii_ptr = cur_index.iIndex;
		this->read_iindex(ii_ptr, cur_index.index_id, val_node->second);
	}
	this->iiFile.close();
}

void BM25::read_iindex(unsigned int ii_ptr, int index_id, double q_weight) {
	string iiLine;
	int cur_index = 0;
	int cur_doc = 0;
	int cur_tf = 0;
	double result = 0;
	map<int, double> temp_rank = map<int, double>();
	vector<string> occur_doc=vector<string>();
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

		//eval tf
		cur_tf= stoi(iiLine.substr(iiLine.find_first_of("\t") + 1));
		
		//eval result
		int dl = this->DocList->find(cur_doc)->second.length;
		double K = this->eval_K(dl);
		result = ((this->k1 + 1)*(double)cur_tf / (K + (double)cur_tf))*((this->k2 + 1)*q_weight / (this->k2 + q_weight));
		
		//insert rank
		temp_rank.insert(pair<int, double>(cur_doc, result));
		//ocuur count
		occur_doc.push_back(this->DocList->find(cur_doc)->second.doc_name);
	}
	//eval pre_value
	int ri = this->eval_ri(&occur_doc);
	this->R = this->RelList.size();
	double pre_value = this->eval_pre(this->R, ri, this->N, occur_doc.size());
	map<int, double>::iterator node;
	for (node = temp_rank.begin(); node != temp_rank.end(); node++) {
		this->insert_rank(node->first, pre_value*node->second);
	}
}

int BM25::eval_ri(vector<string>* occur_doc) {
	int ri = 0;
	for (int i = 0; i < occur_doc->size(); i++) {
		string doc_name = occur_doc->at(i);
		map<string, int>::iterator node;
		node = this->RelList.find(doc_name);
		if (node!=this->RelList.end()) {
			ri++;
		}
	}
	return ri;
}
double BM25::eval_pre(int R, int ri, int N, int ni) {
	double pre;
	double r1 = ((double)ri + 0.5) / ((double)(R - ri) + 0.5);
	double r2 = ((double)(ni - ri) + 0.5) / ((double)(N - ni - R + ri) + 0.5);
	pre = log(r1/r2);
	return pre;
}

void BM25::insert_rank(int doc_id, double result) {
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

//result
bool comp_rank_bm(std::pair<int, double> first, std::pair<int, double> second) {
	return first.second > second.second;
}

void BM25::show_result(map<int, DocInfo>* doclist) {
	cout << "Topic Number : " << this->query.get_num() << endl;
	cout << "Query : " << this->query.get_title() << endl;
	vector<pair<int, double>> sort_rank(this->page_rank.begin(), this->page_rank.end());
	sort(sort_rank.begin(), sort_rank.end(), comp_rank_bm);

	string doc_name;
	int doc_id;
	for (int i = 0; i < 100; i++) {
		doc_id = sort_rank.at(i).first;
		doc_name = doclist->find(doc_id)->second.doc_name;
		cout << doc_name << "\t" << sort_rank.at(i).second << endl;
	}
	return;
}

void BM25::print_result(map<int, DocInfo>* doclist) {
	ofstream outFile = ofstream("BM25_result.txt", std::ios_base::out | std::ios_base::app);
	outFile << "Topic Number : " << this->query.get_num() << endl;
	outFile << "Query : " << this->query.get_title() << endl;
	vector<pair<int, double>> sort_rank(this->page_rank.begin(), this->page_rank.end());
	sort(sort_rank.begin(), sort_rank.end(), comp_rank_bm);

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