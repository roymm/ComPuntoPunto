#include "semaphore.h"
#include "shared.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <chrono>
#include <cstdlib>
#define SEPARATORS_NUM 10
int word_count(const char * string){
	char separators[] = {' ', '.', ',', ':', '!', '?', ';', '\t', '\n', '\0'};
	int i = 0;
	int j;
	int count = 0;
	int in_word = 0;
	int no_error = 1;
	int found_separator = 0;
	for(int i = 0; string[i] != '\0' && no_error; ++i){
		if((string[i] >= '0' && string[i] <= '9') || (string[i] >= 'A' && string[i] <= 'Z') || (string[i] >= 'a' && string[i] <= 'z')){
			if(!in_word){
				in_word = 1;
				++count;
			}
		} else {
				for(j = 0; j < SEPARATORS_NUM && !found_separator; ++j){
					if(string[i] == separators[j]){
						//found_separator = 1;
						break;
					}
				}
				if(j == SEPARATORS_NUM){
					no_error = 0;
					count = -1;
				} else {
					in_word = 0;
					found_separator = 0;
				}
		}
	}
	return count;
}

int main(int argc, char * argv[]){
	int lag = 5;
	if(argc > 1){
		lag = std::atoi(argv[1]);
	}
	Semaphore s(0xb40703, 1);
	SharedMem sm;
	char * shared_mem = (char *) sm.attach();	
	std::string buffer;
	std::string helper;
	std::stringstream ss;
	std::vector<int> word_counts;
	auto start = std::chrono::steady_clock::now();	
	bool read = false;
	bool empty = true;
	while(true){
		s.wait();
		if('1' == shared_mem[0]){
			buffer = (const char *) shared_mem + 1;
			read = true;
			shared_mem[0] = '3';
		}

		if(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() > lag){
			start = std::chrono::steady_clock::now();
			helper = ss.str();
			ss.str("");
			shared_mem[0] = '0';
			for(int i = 0; i < helper.length(); ++i){
				shared_mem[i+1] = helper[i];
				empty = false;
			}
			shared_mem[helper.length()+1] = '\0';
			if(empty){
				shared_mem[helper.length()+2] = '\0';
			}
			empty = true;
		}
		s.signal();

		if(read){
			read = false;
			ss << " " << word_count(buffer.c_str());
		}
	}
}
