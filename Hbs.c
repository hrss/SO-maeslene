#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 20 //número de clientes que tentam entrar na barbearia
#define MAX 20 //máximo de clientes na barbearia
#define CP N-7 //clientes em peh

typedef enum {E, S, C, P, T} estado_t; /*Esperando, Sofa, Cadeira, Pagando, Tentando entrar na barbearia*/
estado_t estado[N];
int tc = 0; //total de clientes na barbearia
sem_t mutex; // protege o contador de clientes na loja e a listas de espera
sem_t sofa; //clientes sentados no sofa
sem_t barbeiro; //havera um semáforo para cada barbeiro,ou seja, 3
sem_t pagamento;//sinaliza que o cliente pagou


void exibe_barbearia() {
  int i,p = 0,c=0,s=0;
  for(i = 0; i < N; i++){
    if(estado[i] == 'P'){
      p = 1;
      printf("B II %d ",i);
    }
  }
  if(p == 0){
    printf("  II   ");
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
  printf(" || ");
  
  for(i = 0;i<N;i++){
    if(estado[i] == 'T'){
      printf("%d ",i);
    }   
  }

  printf("\n");
}
//threads que os clientes fazem
void entraBarbearia(int cliente_id){
  int entrou = 0;
  sem_wait(&mutex);
  if(tc < MAX){
    tc++;
    entrou = 1;
  }
  sem_post(&mutex);
  if(entrou){
    sentaSofa();
  }
}
void sentaSofa(int cliente_id){
  sem_wait(&sofa);
  temCabeloCortado(cliente_id);
}
void temCabeloCortado(int cliente_id){
  sem_wait(&barbeiro);
  sem_post(&sofa);
  paga(cliente_id);
}
void paga(int cliente_id){
  sem_wait()
}

//threads que os barbeiros fazem
void cortarCabelo();
void recebePagamento();
void dorme();

void barbeiro(int cliente_id){

}
void* cliente(void *v){
  int id_cliente = *(int *) v;
  while(1){
    entraBarbearia(id_cliente);
    sentaSofa(id_cliente);
    temCabeloCortado(id_cliente);
    paga(id_cliente);
  }
}





int main() {
  pthread_t thr[N];
  int i,id_cliente[N];
  for (i = 0; i < N; i++) {
    id_cliente[i] = i; 
  }
  sem_init(&mutex, 0, 1); 
  sem_init(&sofa, 0, 4); 
  sem_init(&barbeiro, 0, 3);
  sem_init(&pagamento, 0, 1);   

  for (i = 0; i < N; i++) 
    pthread_create(&thr[i], NULL, cliente, (void*) &id_cliente[i]);

  for (i = 0; i < N; i++) 
    pthread_join(thr[i], NULL);

  for (i = 0; i < N; i++) 
    sem_destroy(&sem_garfo[i]);
  sem_destroy(&sem_mesa);
  sem_destroy(&sem_central);
  return 0;
}
