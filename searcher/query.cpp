#include "query.h"

#define N 507062

Query::Query() {
	this->num = 0;
	this->title = "";
	this->desc = "";
	this->narr = "";
	this->words = vector<string>();
	this->sw_list = vector<string>();
	this->val_list = map<string, double>();
}

void Query::set_stopword(vector<string>* sw_list) {
	this->sw_list = *sw_list;
}

void Query::set_num(int num) {
	this->num = num;
	return;
}
void Query::set_title(string title) {
	this->title = title;
	return;
}
void Query::set_desc(string desc) {
	this->desc = desc;
	return;
}
void Query::set_narr(string narr) {
	this->narr = narr;
	return;
}

int Query::get_num() {
	return this->num;
}
string Query::get_title() {
	return this->title;
}
string Query::get_desc() {
	return this->desc;
}
string Query::get_narr() {
	return this->narr;
}

map<string, double> Query::get_val() {
	return this->val_list;
}

void Query::parse_word(string text) {
	string temp_word;
	string temp_char;
	char real_char;
	for (unsigned int i = 0; i < text.size(); i++) {
		real_char = text.at(i);
		real_char=tolower(real_char);
		if ((real_char >= 'a'&& real_char <= 'z') || (real_char >= '0'&& real_char <= '9')) {
			temp_char = real_char;
			temp_word.append(temp_char);
		}
		else {
			temp_char = real_char;
			if (temp_char.compare(" ") == 0 || temp_char.compare("-") == 0 || temp_char.compare("/") == 0) {
				if (temp_word.compare("") == 0) continue;
				if (is_stopword(&this->sw_list, temp_word) == false) {
					Porter2Stemmer::stem(temp_word);
					this->words.push_back(temp_word);
				}
				temp_word = "";
				continue;
			}
		}
		
	}
}

void Query::parse_all() {
	//title
	this->parse_word(this->title);
	//desc
	this->parse_word(this->desc);
	//narr
	this->parse_word(this->narr);
}


bool Query::is_stopword(vector<string>* sw_list, string word) {
	if (find(sw_list->begin(), sw_list->end(), word) != sw_list->end()) {
		return true; //is stopword
	}
	return false;
}

void Query::eval_tf() {
	string word;
	map<string, double>::iterator tf_node;
	for (int i = 0; i < this->words.size(); i++) {
		word=this->words.at(i);
		tf_node = this->val_list.find(word);
		if (tf_node == this->val_list.end()) {
			this->val_list.insert(pair<string, double>(word, 1.0));
		}
		else {
			tf_node->second++;
		}
	}
}

void Query::eval_weight(IndexList* index_list) {
	map<string, double>::iterator node;
	Index cur_index;
	double weight;
	for (node = this->val_list.begin(); node != this->val_list.end(); node++) {
		cur_index=index_list->get_index(node->first);
		if (cur_index.word.compare(" ") == 0) {
				continue;
		}
		weight = log(node->second + 1)*log(N/cur_index.df);
		node->second = weight;
	}
}

//QuerySetter
QuerySetter::QuerySetter() {
	this->Qfile = ifstream("topics25.txt");
}
void QuerySetter::query_set(vector<Query>* query_list) {
	//stopword set
	ifstream swFile = ifstream("stopword.txt");
	vector<string> sw_list = vector<string>();
	string swLine;

	while (getline(swFile, swLine)) {
		if (swLine.compare("") == 0) continue;
		sw_list.push_back(swLine);
	}
	swFile.close();

	//set query
	string q_line;
	while (getline(this->Qfile, q_line)) {
		if (q_line.compare("<top>") == 0) {
			Query new_query = Query();
			new_query.set_stopword(&sw_list);
			while (getline(this->Qfile, q_line)) {
				if (q_line.compare("</top>") == 0) {
					new_query.parse_all();
					new_query.eval_tf();
					query_list->push_back(new_query);
					break;
				}
				//num
				if (q_line.find("<num> Number: ") != string::npos) {
					q_line = q_line.substr(14);
					new_query.set_num(stoi(q_line));
					continue;
				}
				//title
				if (q_line.find("<title> ") != string::npos) {
					q_line = q_line.substr(8);
					new_query.set_title(q_line);
					continue;
				}
				//desc
				if (q_line.find("<desc> Description: ") != string::npos) {
					string new_desc = "";
					while (getline(this->Qfile, q_line)) {
						if (q_line.compare(" ") == 0 || q_line.compare("") == 0 || q_line.compare("  ") == 0) break;
						new_desc.append(q_line + " ");
					}
					new_query.set_desc(new_desc);
					continue;
				}
				//narr
				if (q_line.find("<narr> Narrative: ") != string::npos) {
					string new_narr = "";
					while (getline(this->Qfile, q_line)) {
						if (q_line.compare(" ") == 0 || q_line.compare("") == 0 || q_line.compare("  ") == 0) break;
						new_narr.append(q_line + " ");
					}
					new_query.set_narr(new_narr);
					continue;
				}
			}
		}
	}
	return;
}


void QuerySetter::finish() {
	this->Qfile.close();
}