#include "remover.h"

//remover
Remover::Remover() {
	this->words=map<int,string>();
}

void Remover::set_word(map<int,string> words) {
	this->words = words;
}
map<int,string> Remover::get_result() {
	return this->words;
}

WordRemover::WordRemover() {
	this->stopword_list = map<int, string>();
}
void WordRemover::setWordList(map<int, string> stopword_list) {
	this->stopword_list = stopword_list;
}


void WordRemover::remove_stopword() {
	bool is_stopword = false;
	string word = "";
	int i,j,iter = 0;
	map<int, string> temp_map=map<int,string>();

	for (i = 0; i < this->words.size(); i++) {
		word = this->words.find(i)->second;
		
		is_stopword = false;
		//check if it is stopword or not
		for (j = 0; j < this->stopword_list.size(); j++) {
			if (word.compare(this->stopword_list.find(j)->second) == 0) {
				is_stopword = true;
				break;
			}
		}
		//if it is not stopword, then append to the result
		if (is_stopword == false) {
			temp_map.insert(pair<int, string>(iter, word));
			iter++;
		}
	}
	this->words = temp_map;
	return;
}


//DocStemmer
DocStemmer::DocStemmer() {
	return;
}
void DocStemmer::stem_doc() {

	 //[porter2 based stemmer]
	string* word=new string();
	for (int i = 0; i < this->words.size(); i++) {
		*word = this->words.find(i)->second;
		Porter2Stemmer::stem(*word);
		this->words.find(i)->second = *word;
	}
	return;
}