#include <stdio.h>
#include <stdlib.h>

#define N 20
typedef enum {E, S, C, P} estado_t; /*Esperando, Sofa, Cadeira, Pagando*/
estado_t estado[N];

void exibe_barbearia() {
	int i,p = 0,c=0,s=0;
	for(i = 0; i < N; i++){
		if(estado[i] == 'P'){
			p = 1;
			printf("B II %d  ",i);
		}
	}
	if(p == 0){
		printf(" II ");
	}
	for(i = 0;i<N;i++){
		if(estado[i] == 'C'){
			c++;
			printf("|B %d| ",i);
		}		
	}
	switch(c){
		case 0: printf("|B | |B | |B | ");
		break;
		case 1: printf("|B | |B | ");
		break;
		case 2: printf("|B | ");
		break;
	}
	printf(" |");
	for(i = 0;i<N;i++){
		if(estado[i] == 'S'){
			s++;
			if(s < 4){
				printf("%d ",i);
			}
			else
				printf("%d",i);
		}		
	}
	switch(s){
		case 0: printf("        ");
		break;
		case 1: printf("      ");
		break;
		case 2: printf("    ");
		break;
		case 3: printf("  ");
	}
	printf("|  |");
	for(i = 0;i<N;i++){
		if(estado[i] == 'E'){
			printf("%d ",i);
		}		
	}

	printf("\n");
}

int main(){
	int i;
	estado[0] = 'E';
	for(i = 1;i<4;i++){
		estado[i] = 'E';
	}
	for(i = 4;i<8;i++){
		estado[i] = 'S';
	}
	for(i = 8;i<N;i++){
		estado[i] = 'E';
	}
	exibe_barbearia();
	return 0;
}