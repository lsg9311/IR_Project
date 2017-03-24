#include <iostream>
#include <fstream>
#include "parser.h"
#include "article.h"

using namespace std;
Parser::Parser() {
	this->file = ifstream();
}
bool Parser::file_open(string filepath) {
	bool result = true;
	this->file.open(filepath);
	if (!this->file.is_open()) result=false;
	return result;
}
bool Parser::file_close() {
	bool result = true;
	this->file.close();
	return result;
}

//DocParser
DocParser::DocParser() {
	this->article_map = map<int, Article>();
}
void DocParser::parse() {
	char ctempLine[200];
	string tempLine;
	Article temp_article;
	int iter = 0;

	while (this->file.getline(ctempLine, sizeof(ctempLine))) {
		tempLine = ctempLine; 
		if (tempLine.compare("<DOC>")==0) {
			temp_article = Article();
			while (this->file.getline(ctempLine, sizeof(ctempLine))) {
				tempLine = ctempLine;
				//end of the DOC
				if (tempLine.compare("</DOC>")==0) break; 
				//store DOCNO
				if (tempLine.find("<DOCNO>") == 0) {
					string start = "<DOCNO> ";
					string end = " </DOCNO>";
					tempLine.erase(tempLine.find(start), start.length());

					tempLine.erase(tempLine.find(end), end.length());
					temp_article.set_docNo(tempLine);
				}
				//store HEADLINE
				else if (tempLine.find("<HEADLINE>") == 0) {
					string headline;
					if(tempLine.compare("<HEADLINE>")==0){
						while (this->file.getline(ctempLine, sizeof(ctempLine))) {
							tempLine = ctempLine;
							if (tempLine.compare("</HEADLINE>") == 0) break;
							else headline.append(tempLine);
						}
					}
					else {
						string start = "<HEADLINE> ";
						string end = " </HEADLINE>";
						tempLine.erase(tempLine.find(start), start.length());

						tempLine.erase(tempLine.find(end), end.length());
						headline = tempLine;
					}
					temp_article.set_headline(headline);
				}
				//store TEXT
				else if (tempLine.find("<TEXT>") == 0) {
					string text;
					while (this->file.getline(ctempLine, sizeof(ctempLine))) {
						tempLine = ctempLine;
						if (tempLine.compare("</TEXT>") == 0) break;
						if (tempLine.find("<P>") == 0) continue;
						if (tempLine.find("</P>") == 0) {
							text.append("\n");
							continue;
						}
						else text.append(tempLine+" ");
					}
					temp_article.set_text(text);
				}
				//ignore else
				else
					continue;
			}
			//store article
			article_map.insert(std::pair<int,Article>(iter,temp_article));
			iter++;
		}
	}
	return;
}

map<int, Article> DocParser::result() {
	return article_map;
}

//WordParser
WordParser::WordParser() {
	this->word_map = map<int, string>();
}

void WordParser::parse() {
	char ctempLine[200];
	string tempLine;
	int iter = 0;
	
	while (this->file.getline(ctempLine, sizeof(ctempLine))) {
		tempLine = ctempLine;
		if (tempLine.compare("") == 0) continue;
		word_map.insert(std::pair<int, string>(iter, tempLine));
		iter++;
	}
}

map<int, string> WordParser::result() {
	return word_map;
}