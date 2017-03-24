#include "reader.h"
#include "info.h"
#include <fstream>
#include <iostream>


int main() {
	IndexList index_list = IndexList();
	Index temp_index;
	int iter = 0;

	int menu = 0;
	while(1){
		std::cout << "메뉴를 선택하세요\n"
			<<"0. Load index\n"
			<< "1. Indexing\n"
			<< "2. Inverted Index"<<endl;
		
		std::cin >> menu;
		if (menu == 0) {
			ifstream indexFile = ifstream("index_list.dat");
			string line;
			Index new_index;
			while (getline(indexFile,line)) {
				new_index.index_id = stoi(line.substr(0, line.find_first_of("\t")));
				line = line.substr(line.find_first_of("\t") + 1);

				new_index.word = line.substr(0, line.find_first_of("\t"));
				line = line.substr(line.find_first_of("\t") + 1);

				new_index.df = stoi(line.substr(0, line.find_first_of("\t")));
				line = line.substr(line.find_first_of("\t") + 1);

				new_index.cf = stoi(line.substr(0, line.find_first_of("\t")));
				line = line.substr(line.find_first_of("\t") + 1);

				new_index.iIndex = stoi(line.substr(0, line.find_first_of("\t")));

				index_list.set_index(new_index.word, new_index);
			}
		}
		if(menu==1){
			DocsReader docsReader = DocsReader();
			docsReader.set_root("sample/");
			docsReader.crawl_list();
			docsReader.read_files();
			index_list.set_data(&docsReader.get_list());
			index_list.write_to_txt();
		}
		if (menu == 2) {
			IindexList iIndexList = IindexList();
			iIndexList.eval_all (&index_list);
			iIndexList.write_to_txt();

			index_list.write_to_txt();
			iIndexList.close_tf();

		}
	}
	return 0;
}