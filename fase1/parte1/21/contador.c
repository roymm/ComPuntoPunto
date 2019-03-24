#include "stdio.h"
#include "unistd.h" //ftruncate

#define BUFF_SIZE 256
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

int main(){

	FILE * fi = NULL;
	char buffer[BUFF_SIZE];
	int cur = 0;
	int count;
	char aux;

	while(1){
		while(!fi){ 
			fi = fopen("comm", "r+");
		}
		aux = getc(fi);

		if(aux == '1'){
			while(!feof(fi)){
				buffer[cur++] = getc(fi);
			}
			buffer[cur-1] = '\0';
			count = word_count((const char *) buffer);
			if(count == -1){
				printf("ERROR! La frase tiene caracteres no reconocidos\n");
			} else {
				printf("%s -> %d\n", buffer, count);
			}
			ftruncate(fileno(fi), 0);
			rewind(fi);
			fprintf(fi, "0");
		} 
		fclose(fi);

		fi = NULL;
		cur = 0;
	}
}
