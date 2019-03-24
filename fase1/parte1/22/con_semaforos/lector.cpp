#include "semaphore.h"
#include "shared.h"

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

int main(){

	Semaphore s(0xb40703, 1);
	SharedMem sm;
	char * shared_mem = (char *) sm.attach();

	std::string buffer;
	std::string helper;

	std::stringstream ss;

	std::vector<std::string> sentences;

	std::vector<int> word_counts;
	int int_helper;

	int temp_buffer[256];
	int used = 0;

	bool validator;
	int print;

	do{
		std::getline(std::cin, buffer);	
		print = buffer.compare("1");
		s.wait();
		if('0' == shared_mem[0]){
			helper.assign((const char *) shared_mem + 2);
			std::cout << "HELPER: " << helper << std::endl;
			ss.str(helper);

			/*
			while(ss >> int_helper){
				word_counts.push_back(int_helper);
			}
			ss.clear();
			*/
			
			while(ss){
				ss >> temp_buffer[used++];
				std::cout << "TP: " << used-1 << " " << temp_buffer[used-1] << std::endl;
			}
			ss.str("");
			ss.clear();
			for(int i = 0; i < used-1; ++i){
				word_counts.push_back(temp_buffer[i]);
			}
			used = 0;
		}
		if(print != 0){
			sentences.push_back(buffer);
			shared_mem[0] = '1';
			for(int i = 0; i < buffer.length(); ++i){
				shared_mem[i+1] = buffer[i];
			}
			shared_mem[buffer.length()+1] = '\0';
		}
		s.signal();
		if(print == 0){
			for(int i = 0; i < sentences.size() && i < word_counts.size(); ++i){
				if(word_counts[i] != -1){
					std::cout << sentences[i] << " -> " << word_counts[i] << std::endl;
				} else {
					std::cout << "La oracion: " << sentences[i] << " contiene caracteres ilegales y no se puede procesar" << std::endl;
				}
			}
		}
	}while(!buffer.empty());
	
	return 0;
}
