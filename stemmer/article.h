#pragma once
#include <string>
#include <map>

using namespace std;

class Article {
	private:
		string docNo;
		string headline;
		string text;
		string stem_text;

		map<int, string> words;
	public:
		Article(string docNo="",string headline="",string text="");

		string get_docNo();
		string get_headline();
		string get_text();
		string get_stem_text();
		map<int,string> get_words();

		void set_docNo(string docNo);
		void set_headline(string headline);
		void set_text(string text);
		void set_stem_text(string stem_text);
		void set_words(map<int, string>);

		void merge_words();
		void refresh_words();
		void print_article();

		void article_to_txt();
};