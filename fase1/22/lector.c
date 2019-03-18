#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#define BUFF_SIZE 256
#define DEFAULT_STRINGS 32

int main(){
	
	FILE * fi = NULL;	//fcount: archivo para leer las respuestas del contador
	FILE * fcount = NULL;
	char buffer[BUFF_SIZE];
	int* sizes = (int *) calloc(DEFAULT_STRINGS, sizeof(int));	//arreglo de longitudes de las cadenas
	char** string_buffer = (char**) calloc(DEFAULT_STRINGS, sizeof(char*));	//arreglo de cadenas
	char c = getchar();	//lee el primer caracter del archivo
	//char aux;
	int last_size_read = 0;	//numero de la ultima respuesta obtenida del contador
	int cur = 0;
	int current_string = 0;	//contador de hileras
	char counter_answer[8];

	while(c != EOF){	//mientras que se obtengan caracteres por la entrada estandar
		do{
			buffer[cur++] = c;	//obtenga los caracteres
		}while((c = getchar()) != EOF && c != '\n'); //hasta encontrar un fin de linea o fin de archivo (ctrl-D)
		buffer[cur] = '\0';	//agrega el fin de hilera
		
		if(strcmp(buffer, "1") == 0)	//si se lee un "1", el programa termina
			break;

		while(!fi){	//se abre o se crea el archivo comunicador de hileras
			fi = fopen("comm_strings.txt", "w");
		}
		fprintf(fi, "1");
		fprintf(fi, (const char *) buffer); //imprime lo obtenido en la entrada
		string_buffer[current_string++] = strdup(buffer);	//almacena la cadena en el arreglo de cadenas
		fclose(fi);
		
		if(!fcount){
			fcount = fopen("comm_sizes.txt", "r+");
		}
		char aux = '0'; 	
		aux = fgetc(fcount);	//obtiene el primer caracter del archivo de contador
		
		if(aux != EOF && aux == '1'){ //1: significa que hay una respuesta lista del contador
			int read = 0;
			while(!feof(fcount)){		//obtiene la respuesta del contador
				counter_answer[read++] = getc(fcount);
			}
			counter_answer[read] = '\0';
			sizes[last_size_read++] = atoi(counter_answer); //almacena la respuesta
			memset(counter_answer, '\0', 8);			//limpia la cadena del numero
			ftruncate(fileno(fcount), 0);	//borra el archivo
			rewind(fcount);					//se coloca al inicio
			fprintf(fcount, "0");			//le imprime un 0 para indicar que no hay mas respuestas
			fclose(fcount);
		}
		
		fi = NULL;
		fcount = NULL;
		cur = 0;	//reinicia el contador
		if(c != EOF){	//continua leyendo
			c = getchar();
		}
	}
	if(fi)
		fclose(fi);
	for(int i = 0; i < current_string; ++i){
		printf("%s -> %d\n", string_buffer[i], sizes[i]);	//imprime todas las parejas de oraciones con su cantidad de palabras
	}
	free(string_buffer);
	free(sizes);
	return 0;
}
