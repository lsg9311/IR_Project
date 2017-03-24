#include <iostream>
#include <fstream>
#include "article.h"

Article::Article(string docNo, string headline, string text) {
	this->docNo = docNo;
	this->headline = headline;
	this->text = text;
}
string Article::get_docNo() {
	return this->docNo;
}
string Article::get_headline() {
	return this->headline;
}
string Article::get_text() {
	return this->text;
}
string Article::get_stem_text() {
	return this->stem_text;
}
map<int, string> Article::get_words() {
	return this->words;
}

void Article::set_docNo(string docNo) {
	this->docNo = docNo;
}
void Article::set_headline(string headline) {
	this->headline = headline;
}
void Article::set_text(string text) {
	this->text = text;
	refresh_words();
}
void Article::set_stem_text(string stem_text) {
	this->stem_text = stem_text;
	refresh_words();
}
void Article::set_words(map<int, string> words) {
	this->words = words;
	merge_words();
	return;
}

void Article::merge_words() {
	this->stem_text = "";
	for (int i = 0; i < this->words.size(); i++) {
		this->stem_text.append(this->words.find(i)->second + " ");
	}
	return;
}

void Article::refresh_words() {
	if (this->text == "") {
		return;
	}
	string temp_word = "";
	string temp_char = "";
	int iter = 0;
	

	for (int i = 0; i < this->text.length(); i++) {
		temp_char = this->text.at(i);
		//is char
		if ((temp_char=="'")||(temp_char >= "a"&&temp_char <= "z") || (temp_char >= "A"&&temp_char <= "Z") || (temp_char >= "0"&&temp_char <= "9")) {
			temp_word.append(temp_char);
		}

		//not char
		else {
			//is seperator
			if (temp_char == "\n" || temp_char == " " || temp_char == "\t") {
				//is empty temp_word
				if (temp_word == "") continue;
				//has temp_word
				else {
					//lower case
					for (int j = 0; j < temp_word.size(); j++)
					{
						char character = tolower(temp_word[j]);
						temp_word[j] = character;
					}
					this->words.insert(pair<int, string>(iter, temp_word));
					iter++;
					temp_word = ""; //empty temp_word
				}
			}
			//is special character
			else
				continue;
		}
	}
	return;
}
void Article::print_article() {
	std::cout << "[DOCNO] : " << this->docNo << endl;
	std::cout << "[HEADLINE] : " << this->headline << endl;
	std::cout << "[TEXT] : \n" << this->text << endl;
	std::cout << "[STEM] : \n" << this->stem_text << endl;

	return;
}

void Article::article_to_txt() {
	string file_name = "article_stems/"+this->docNo + ".txt";
	std::ofstream outFile(file_name);
	outFile << "[DOCNO] : " << this->docNo << endl;
	outFile << "[HEADLINE] : " << this->headline << endl;
	outFile << "[TEXT] : \n" << this->text << endl;
	outFile << "[STEM] : \n" << this->stem_text << endl;
}