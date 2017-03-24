#pragma once
#include <map>
#include <fstream>
#include "article.h"
using namespace std;

class Parser {
	protected:
		ifstream file;
	public:
		Parser();
		bool file_open(string filepath);
		bool file_close();
};

class DocParser : public Parser {
	private:
		map<int, Article> article_map;
	public:
		DocParser();
		void parse();
		map<int, Article> result();
};

class WordParser : public Parser {
	private:
		map<int, string> word_map;
	public:
		WordParser();
		void parse();
		map<int, string> result();
};