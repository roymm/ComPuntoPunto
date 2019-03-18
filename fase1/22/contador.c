#include <assert.h>
#include <stdio.h>
#include <unistd.h> //ftruncate

#define BUFF_SIZE 256
#define SEPARATORS_NUM 10
#define TRUE 1
#define FALSE !TRUE

int word_count(const char * string){
	char separators[] = {' ', '.', ',', ':', '!', '?', ';', '\t', '\n', '\0'};
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

	FILE * finput = NULL;	//finput = va a leer las cadenas que le envie el lector, foutput va a responder con la longitud de la cadena leida
	FILE * foutput = NULL;
	
	char buffer[BUFF_SIZE];
	int cur = 0;
	int count;
	char aux;

	while(TRUE){
		while(!finput){ 
			finput = fopen("comm_strings.txt", "r+");
		}
		aux = getc(finput);

		if(aux == '1'){					//si la banderilla en el comunicador es 1
			while(!feof(finput)){			//lee la hilera en el archivo y cuenta sus caracteres
				buffer[cur++] = getc(finput);
			}
			buffer[cur-1] = '\0';
			count = word_count((const char *) buffer);
			if(count == -1){
				printf("ERROR! La frase tiene caracteres no reconocidos\n");
			} else {
				//printf("%s -> %d\n", buffer, count);
				if(!foutput)
					foutput = fopen("comm_sizes.txt", "w");
				//introducir retardo de 5 segundos aqui
				assert(foutput);
				fputc('1', foutput);	//le indica al lector que ya hay respuesta
				fprintf(foutput, "%d", count);	//escribe la respuesta en el comunicador de sizes
				fclose(foutput); 
			}
			ftruncate(fileno(finput), 0);	//borra el archivo
			rewind(finput);					//se coloca al inicio
			fprintf(finput, "0");			//le imprime un 0
		} 
		if(finput)
			fclose(finput);

		finput = NULL;
		cur = 0;
	}
}
