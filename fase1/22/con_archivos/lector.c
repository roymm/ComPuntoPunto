#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#define BUFF_SIZE 256
#define DEFAULT_STRINGS 32
#define NUMBER_SIZE 8
#define TRUE 1
#define FALSE !TRUE

int main(){
	
	FILE * fi = NULL;	//fcount: archivo para leer las respuestas del contador
	FILE * fcount = NULL;
	char buffer[BUFF_SIZE];
	int* sizes = (int *) calloc(DEFAULT_STRINGS, sizeof(int));	//arreglo de longitudes de las cadenas
	char** string_buffer = (char**) calloc(DEFAULT_STRINGS, sizeof(char*));	//arreglo de cadenas
	char c = getchar();	//lee el primer caracter del archivo
	int last_size_read = 0;	//numero de la ultima respuesta obtenida del contador
	int cur = 0;
	int current_string = 0;	//contador de hileras
	char counter_answer[NUMBER_SIZE]; //buffer temporal para almacenar la cantidad de palabras, de maximo 7 digitos
	bool answered = TRUE;
	bool finished = FALSE;
	int last_sent_string = 0;
	

	while(c != EOF){	//mientras que se obtengan caracteres por la entrada estandar
		if(last_size_read == last_sent_string && finished)	//condiciones para salir del programa
			break;
		char aux = '0'; 	
		
		if(!fcount && current_string == 0){
			fcount = fopen("comm_sizes.txt", "wr");
			fputc('0', fcount);
		}else{
			fcount = fopen("comm_sizes.txt", "r+");
		}
		assert(fcount);
		aux = fgetc(fcount);	//obtiene el primer caracter del archivo de contador
		if(aux != EOF && aux == '1'){ //1: significa que hay una respuesta lista del contador
			int read = 0;
			answered = TRUE;
			while(!feof(fcount)){		//obtiene la respuesta del contador
				counter_answer[read++] = fgetc(fcount);
			} 
			counter_answer[read] = '\0';
			sizes[last_size_read++] = atoi(counter_answer); //almacena la respuesta
			memset(counter_answer, '\0', NUMBER_SIZE);			//limpia la cadena del numero
			ftruncate(fileno(fcount), 0);	//borra el archivo
			rewind(fcount);					//se coloca al inicio
			fprintf(fcount, "0");			//le imprime un 0 para indicar que no hay mas respuesta
		}
		fclose(fcount);
		do{
			buffer[cur++] = c;	//obtenga los caracteres
		}while((c = getchar()) != EOF && c != '\n'); //hasta encontrar un fin de linea o fin de archivo (ctrl-D)
		buffer[cur] = '\0';	//agrega el fin de hilera
		
		if(strcmp(buffer, "1") == 0){	//si se lee un "1", el programa termina
			finished = TRUE;
			continue;		//se salta el resto puesto que ya no se debe hacer nada mas
		}
		else{
			string_buffer[current_string++] = strdup(buffer);	//almacena la cadena en el arreglo de cadenas
		}
		//si el contador ya respondio la ultima peticion, puede proceder a enviar otra
		if(answered){
			while(!fi){	//se abre o se crea el archivo comunicador de hileras
				fi = fopen("comm_strings.txt", "w");
			}
			fprintf(fi, "1");
			fwrite(string_buffer[last_sent_string], sizeof(char), strlen(string_buffer[last_sent_string]) + 1, fi); //imprime lo obtenido en la entrada
			++last_sent_string;
			fclose(fi);
			answered = FALSE;
		}
		fi = NULL;
		fcount = NULL;
		cur = 0;	//reinicia el contador
		if(c != EOF){	//continua leyendo
			c = getchar();
		}
	}

	for(int i = 0; i < current_string; ++i){
		printf("%s -> %d\n", string_buffer[i], sizes[i]);	//imprime todas las parejas de oraciones con su cantidad de palabras
		free(string_buffer[i]);
	}
	free(string_buffer);		//liberacion de memoria
	free(sizes);
	remove("comm_strings.txt");
	remove("comm_sizes.txt");
	return 0;
}
