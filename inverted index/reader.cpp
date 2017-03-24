#include "reader.h"
#include <io.h>
#include <iostream>

DocReader::DocReader() {
	this->file = ifstream();
	this->doc_info = { 0,"",0 };
	this->tf_list = map<string, TF_info>();
}

void DocReader::set_id(int id) {
	this->doc_id = id;
	this->doc_info.doc_id = id;
}

bool DocReader::file_open(string filepath) {
	bool result = true;
	this->file.open(filepath);
	if (!this->file.is_open()) result = false;
	return result;
}
bool DocReader::file_close() {
	bool result = true;
	this->file.close();
	return result;
}

Doc_info DocReader::get_doc() {
	return this->doc_info;
}

void DocReader::parse() {
	string tempLine=string();
	while (getline(this->file, tempLine)) {
		if (tempLine.find("[DOCNO] : ") == 0) {
			string start = "[DOCNO] : ";
			tempLine.erase(tempLine.find(start), start.length());
			
			this->doc_info.doc_name = tempLine;
		}
		if (tempLine.compare("[STEM] : ")==0) {
			while (getline(this->file, tempLine)) {
				this->parse_word(tempLine);
			}
		}
	}
}

void DocReader::parse_word(string word) {
	if (word == "") {
		return;
	}
	string temp_word = "";
	string temp_char = "";
	int iter = 0;
	int length = 0;

	for (int i = 0; i < word.length(); i++) {
		temp_char = word.at(i);
		//is char
		if ((temp_char == "'") || (temp_char >= "a"&&temp_char <= "z") || (temp_char >= "A"&&temp_char <= "Z") || (temp_char >= "0"&&temp_char <= "9")) {
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
					length++;
					this->insert_tf(temp_word);
					temp_word = ""; //empty temp_word
				}
			}
			//is special character
			else
				continue;
		}
	}
	this->doc_info.length = length;
	return;
}

//insert TF and index
void DocReader::insert_tf(string word) {
	TF_info new_tf;

	auto exist_tf = this->tf_list.find(word);
	if (exist_tf != this->tf_list.end()) {
		exist_tf->second.tf++;
	}
	else{
		new_tf.doc_id = this->doc_id;
		new_tf.index = word;
		new_tf.tf = 1;
		this->tf_list.insert(pair<string, TF_info>(word, new_tf));
	}
	return;
}

map <string, TF_info> DocReader::get_TF() {
	return this->tf_list;
}

/*
void DocReader::write_tf() {
	ofstream file = ofstream("TF.txt", std::ios_base::out | std::ios_base::app);
	for (int iter = 0; iter < this->tf_list.size(); iter++) {
		file << this->tf_list.find(iter)->second.doc_id << "\t" << this->tf_list.find(iter)->second.index << "\t" << this->tf_list.find(iter)->second.tf << endl;
	}
	return;
}*/

//DOCS READER
DocsReader::DocsReader() {
	this->doc_id = 0;
	this->root = "";
	this->filepath_list = vector<string>();
	this->pre_index_list = map<string, Index>();
	this->doc_info = unordered_map<int, Doc_info>();
	this->tf_info = list<TF_info>();
	this->fileTF = ofstream("TF.dat");
}

void DocsReader::set_root(string root) {
	this->root = root;
	return;
}

void DocsReader::crawl_list() {
	/*
	string head[2] = { "APW","NYT" };
	int year = 1998;
	int month = 1;
	int day = 1;
	int docNum = 1;
	string file_path="";

	for (int iter = 0; iter < 2; iter++) {
		for (year = 1998; year <= 2000; year++) {
			for (month = 1; month <= 12; month++) {
				for (day = 1; day <= 31; day++) {
					for (docNum = 1; docNum < 2000; docNum++) {
						file_path = "";
						file_path.append(head[iter]);
						file_path.append(to_string(year));
						file_path.append(to_string(month / 10) + to_string(month % 10));
						file_path.append(to_string(day / 10) + to_string(day % 10));
						file_path.append("."+to_string(docNum / 1000) + to_string((docNum % 1000)/100) + to_string((docNum % 100) / 10) + to_string(docNum % 10));
						file_path.append(".txt");

						this->filepath_list.push_back(file_path);
					}
				}
			}
		}
	}*/
	_finddata_t fd;
	long handle;
	int result = 1;
	string file_root = this->root + "\\*.*";
	handle = _findfirst(file_root.c_str(), &fd);
	if (handle == -1)
	{
		printf("There were no files.\n");
		return;
	}

	while (result != -1)
	{
		//cout << "Push " << fd.name << endl;
		this->filepath_list.push_back(fd.name);
		result = _findnext(handle, &fd);
	}

	_findclose(handle);
	return;
	
}

void DocsReader::read_files() {
	string filepath = "";
	int failcount = 0;
	
	for (int iter = 0; iter < this->filepath_list.size(); iter++) {
		DocReader docReader = DocReader();
		filepath = this->root + this->filepath_list.at(iter);
		cout << filepath << endl;
		if (docReader.file_open(filepath) == false) continue;
		this->doc_id++;
		docReader.set_id(this->doc_id);
		//TF in doc
		docReader.parse();
		map<string, TF_info> tf_list = docReader.get_TF();
		this->doc_info.insert(pair<int, Doc_info>(this->doc_id, docReader.get_doc()));
		this->add_tf(&tf_list);
		
		//write
		int max_N = 10000000;
		if (this->tf_info.size() > max_N) {
			cout << "write..." << endl;
			this->insert_list(&this->tf_info);
			this->write_tf();
			this->tf_info = list<TF_info>();
		}
		docReader.file_close();
	}
	//write
	this->insert_list(&this->tf_info);
	this->write_tf();
	this->write_doc_info();
}

void DocsReader::tf_close() {
	this->fileTF.close();
}

void DocsReader::add_tf(map<string, TF_info>* new_tf) {
	map<string, TF_info>::iterator new_node;
	for (new_node = new_tf->begin(); new_node != new_tf->end(); new_node++) {
		this->tf_info.push_back(new_node->second);
	}
}

map<string,Index> DocsReader::get_list() {
	return this->pre_index_list;
}

void DocsReader::insert_list(list<TF_info>* tf_list) {
	int iter=0;
	Index temp_index;
	TF_info temp_tf;
	list<TF_info> ::iterator node;
	for(node=tf_list->begin();node!=tf_list->end();node++){
		temp_tf.doc_id = node->doc_id;
		temp_tf.index = node->index;
		temp_tf.tf = node->tf;

		auto exist_index = this->pre_index_list.find(temp_tf.index);
		//if exists
		if (exist_index != this->pre_index_list.end() ) {
			exist_index->second.cf += temp_tf.tf;
			exist_index->second.df++;
		}
		//otherwise(new word)
		else {
			temp_index.word = temp_tf.index;
			temp_index.df = 1;
			temp_index.cf = temp_tf.tf;
			this->pre_index_list.insert(pair<string, Index>(temp_tf.index, temp_index));
		}	
	}
}

void DocsReader::write_tf() {
	list<TF_info>::iterator node;
	for (node = this->tf_info.begin(); node != this->tf_info.end(); node++) {
		this->fileTF << node->doc_id << "\t" << node->index << "\t" << node->tf << endl;
	}
	/*
	for (int iter = 0; iter < this->tf_info.size(); iter++) {
		this->fileTF << this->tf_info.find(iter)->second.doc_id << "\t" << this->tf_info.find(iter)->second.index << "\t" << this->tf_info.find(iter)->second.tf << endl;
	}*/
	return;
}

void DocsReader::write_doc_info() {
	ofstream file = ofstream("Document.dat");
	Doc_info temp_doc;
	for (int iter = 1; iter <= this->doc_info.size(); iter++) {
		temp_doc = this->doc_info.find(iter)->second;
		file << temp_doc.doc_id << "\t" << temp_doc.doc_name << "\t" << temp_doc.length << endl;
	}
	return;
}