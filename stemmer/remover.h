#pragma once
#include <string>
#include <map>
//final
#include "final_stemmer\/final_stemmer.h"
using namespace std;

class Remover {
protected:
	map<int,string> words;

public:
	Remover();

	void set_word(map<int,string> words);
	map<int,string> get_result();
};

class WordRemover : public Remover {
private:
	map<int, string> stopword_list;
public:
	WordRemover();
	void setWordList(map<int, string> stopword_list);
	void remove_stopword();
};

class DocStemmer : public Remover {
private:
public:
	DocStemmer();
	void stem_doc();
};