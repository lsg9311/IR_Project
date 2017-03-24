#include <iostream>
#include <map>
#include <vector>
#include "parser.h"
#include "article.h"
#include "remover.h"
//crawl all file
vector<string> crawl_files() {
	int iter=0;
	vector<string> file_path_list = vector<string>();
	string path[6] = {"ir_corpus/APW/1998/","ir_corpus/APW/1999/","ir_corpus/APW/2000/","ir_corpus/NYT/1998/","ir_corpus/NYT/1999/","ir_corpus/NYT/2000/"};
	string path_end[2] = { "_APW_ENG","_NYT" }; //012 => 0 345=>1 path iter/3 = path_end iter
	int year, month, day;
	string s_year, s_month, s_day;
	string file_path;

	for (iter = 0; iter < 6; iter++) {
		for (year = 1998; year <= 2000; year++) {
			for (month = 1; month <= 12; month++) {
				for (day = 1; day <= 31; day++) {
					s_year = to_string(year);
					if (month / 10 < 1) s_month = "0" + to_string(month);
					else s_month = to_string(month);
					if (day / 10 < 1) s_day = "0" + to_string(day);
					else s_day = to_string(day);
					file_path = path[iter] + s_year + s_month + s_day + path_end[iter / 3];

					file_path_list.push_back(file_path);
				}
			}
		}

		std::cout << "Push complete" << endl;
	}
	return file_path_list;
}


int main() {
	///////////////Variable////////////////
	//Parser
	DocParser docParser = DocParser();
	WordParser stopwordParser = WordParser();

	//List
	vector<string> doc_list;
	map<int, Article> article_list;
	map<int, string> stopword_list;

	//Wordremover
	WordRemover wordremover=WordRemover();

	//Stemmer
	DocStemmer docstemmer = DocStemmer();
	
	//temporal variable
	bool isFile = true;
	Article* temp_article = new Article();
	map<int, string> result_word = map<int, string>();
	//iterator
	int iter = 0;
	int doc_iter = 0;
	///////////////////////////////////////
	using namespace std;

	//document Parsing
	doc_list = crawl_files();

	for (doc_iter = 0; doc_iter < doc_list.size(); doc_iter++) {
		docParser = DocParser();
		std::cout << "Read " << doc_list.at(doc_iter) << endl;
		isFile = docParser.file_open(doc_list.at(doc_iter));
		if (!isFile) continue;
		docParser.parse();
		article_list = docParser.result();
		docParser.file_close();
	

		//stopword txt Parsing
		stopwordParser.file_open("Stopword List.txt");
		stopwordParser.parse();
		stopword_list = stopwordParser.result();

		//remove stopword
		std::cout << "\n\n\n" << "Removing Stopword"<<endl;
		wordremover.setWordList(stopword_list);
		for (iter = 0; iter < article_list.size(); iter++) {
			std::cout << "Removing stopwords in " << article_list.find(iter)->second.get_docNo() << endl;
			result_word = map<int, string>();
			//no text
			if (article_list.find(iter)->second.get_text() == "") continue;

			wordremover.set_word(article_list.find(iter)->second.get_words());
			wordremover.remove_stopword();
			result_word=wordremover.get_result();
		
			article_list.find(iter)->second.set_words(result_word);
		}
	
		std::cout << "\n\n\n" << "Stemming";
		//stemming
		for (iter = 0; iter < article_list.size(); iter++) {
			std::cout << "Stemming " << article_list.find(iter)->second.get_docNo() << endl;

			result_word = map<int, string>();
		
			docstemmer.set_word(article_list.find(iter)->second.get_words());
			docstemmer.stem_doc();
			result_word=docstemmer.get_result();

			article_list.find(iter)->second.set_words(result_word);
		}

		std::cout << "\n\n\n" << "Writing"<<endl;
		//print article
		for (iter = 0; iter < article_list.size(); iter++) {
			std::cout <<"Write " <<article_list.find(iter)->second.get_docNo() << endl;
			article_list.find(iter)->second.article_to_txt();
		}
	}
	std::cout << "\n\n\n" << "Done.";
	return 0;
}