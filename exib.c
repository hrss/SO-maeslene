#include <stdio.h>
#include <stdlib.h>

#define N 25
typedef enum {E, S, C, P, T} estado_t; /*Esperando, Sofa, Cadeira, Pagando, Tentando entrar na barbearia*/
estado_t estado[N];

void exibe_barbearia(){
  int i,p = 0,c=0,s=0;
  for(i = 0; i < N; i++){ /*exibe o caixa de pagamento*/
    if(estado[i] == 'P'){
      p = 1;
      printf("B II %d ",i);
    }
  }
  if(p == 0){
    printf("  II   ");
  }
  for(i = 0; i < N; i++){ /* exibe a fila de espera do caixa*/
    if(estado[i] == 'R'){
      printf("%d ",i);
    }
  }
  for(i = 0;i<N;i++){ /*exibe os clientes que estao cortando cabelo no momento*/
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
  for(i = 0;i<N;i++){ /*exibe os clientes que estao sentados no sofa*/
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
  for(i = 0;i<N;i++){ /*exibe os clientes que estao esperando em pe*/
    if(estado[i] == 'E'){
      printf("%d ",i);
    }   
  }
  printf(" || ");
  
  for(i = 0;i<N;i++){ /*exibe os clientes que estao tentando entrar na barbearia*/
    if(estado[i] == 'T'){
      printf("%d ",i);
    }   
  }

  printf("\n");
}
int main(){
	int i;
	estado[0] = 'P';
	for(i = 1;i<4;i++){
		estado[i] = 'R';
	}
	for(i = 4;i<7;i++){
		estado[i] = 'C';
	}
	for(i = 7;i<11;i++){
		estado[i] = 'S';
	}
	for(i = 11;i<N;i++){
		estado[i] = 'E';
	}
	for(i = 20;i<25;i++){
		estado[i] = 'T';
	}
	exibe_barbearia();
	return 0;
}