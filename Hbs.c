#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 20 //número de clientes que tentam entrar na barbearia
#define MAX 20 //máximo de clientes na barbearia
#define CP N-7 //clientes em peh

int Totalclientes = 0; //total de clientes na barbearia
sem_t mutex; // protege o contador de clientes na loja e a listas de espera
sem_t sofa; //clientes sentados no sofa
sem_t barbeiro; //havera um semáforo para cada barbeiro,ou seja, 3
sem_t pagamento;//sinaliza que o cliente pagou

//threads que os clientes fazem
void entraBarbearia();
void sentaSofa();
void temCabeloCortado();
void paga();

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
