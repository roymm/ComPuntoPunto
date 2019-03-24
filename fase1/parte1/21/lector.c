#include "stdio.h"

#define BUFF_SIZE 256

int main(){
	
	FILE * fi = NULL;
	char buffer[BUFF_SIZE];
	char c = getchar();
	char aux;
	int cur = 0;

	while(c != EOF){
		do{
			buffer[cur++] = c;	
		}while((c = getchar()) != EOF && c != '\n');

		buffer[cur] = '\0';

		while(!fi){
			fi = fopen("comm", "w");
		}
		fprintf(fi, "1");
		fprintf(fi, (const char *) buffer);
		fclose(fi);
		fi = NULL;
		cur = 0;
		if(c != EOF){
			c = getchar();
		}
	}
	return 0;
}
