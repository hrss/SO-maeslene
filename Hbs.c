#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 20 //número de clientes que tentam entrar na barbearia
#define MAX 20 //máximo de clientes na barbearia
#define CP N-7 //clientes em peh

typedef enum {E, S, C, P, T , X, R} estado_t; /*Esperando, Sofa, Cortado cabelo, Pagando, Tentando entrar na barbearia , sai da barbearia,
esperando pagamento*/
estado_t estado[N];
int tc = 0; /*total de clientes na barbearia*/
sem_t mutexBarbearia; /* os mutex garantem exclusao mutua quando necessario*/
sem_t mutexBarbeiro;
sem_t mutexCadeira;
sem_t mutexSofa; 
sem_t vagaSofa; /*caso nao tenha vaga no sofa, esse semaforo eh utilizado para o proximo conseguir sentar quando houver vaga*/
sem_t barbeiro; /*garante que os clientes sejam atendido na ordem certa, apesar de um barbeiro poder chamar o cliente por vez*/
sem_t clientesEsp; /*avisa o barbeiro que existe cliente*/
sem_t pagamento; /*avisa ao cliente que o barbeiro vai pegar seu pagamento*/


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
/*threads que os clientes fazem*/
void entraBarbearia(int cliente_id){
  int entrou = 0;
  sem_wait(&tentaBarbearia); /* garante exclusao mutua na hora de verificar a lotacao da barbearia*/
  if(tc < MAX){ /*caso a barbearia nao esteja lotada*/
    tc++;
    entrou = 1;
  sem_post(&tentaBarbearia);
  if(entrou){
    sentaSofa(cliente_id);
  }
  else
    estado[cliente_id] ='X';/*vai embora*/
}
/*ao conseguir entrar na barbearia, verifica se tem lugar no sofa*/
void sentaSofa(int cliente_id){
  sem_wait(&mutexSofa); /*apenas um cliente por vez pode sentar no sofa*/
  if(sofa>0){ /*caso tenha lugar no sofa,senta, se nao espera*/
    sofa = sofa - 1;
    estado[cliente_id] = 'S';
    exibe_barbearia();
    sem_post(&mutexSofa); /*libera outra pessoa para tentar sentar no sofa*/
    sem_post(&clientesEsp);
    sem_wait(&barbeiro);
  }
  else{
    estado[cliente_id] = 'E';
    exibe_barbearia();
    Esperando(cliente_id);
  }
  
}
/*espera ateh liberar algum lugar no sofa*/
void Esperando(int cliente_id){
  sem_wait(&vagaSofa); /*espera uma vaga no sofa*/
  sofa = sofa - 1; /*logo que liberar a vaga, ja senta*/
  estado[cliente_id] = 'S';
  sem_post(&mutexSofa);/*libera para alguem tentar sentar no sofa*/
  exibe_barbearia();
  sem_post(&clientesEsp);/*entra na lista de espera para ser atendido*/
  sem_wait(&barbeiro);
}

void paga(int cliente_id){
  estado[i] = 'R';
  exibe_barbearia();
  sem_wait(&pagamento);  /*espera o barbeiro receber o pagamento*/
  estado[cliente_id] = 'P'; /*paga o barbeiro*/
  exibe_barbearia(); 
  estado[cliente_id] = 'X'; /*sai da barbearia*/
}


void F_barbeiro(int cliente_id){
  sem_wait(&mutexBarbeiro);/*um barbeiro recebo o cliente de cada vez*/
  sem_wait(&clientesEsp);/*caso um cliente chegue, o barbeiro acorda*/
  sem_post(&barbeiro);
  sofa = sofa + 1;    /*libera uma vaga no sofá*/
  sem_post(&vagaSofa);  
  estado[cliente_id] = 'C'; /*começa a cortar o cabelo*/
  exibe_barbearia();
  sem_post(&mutexBarbeiro);/*libera outro barbeiro para receber o proximo cliente*/
  sem_post(&pagamento); /*recebe o pagamento do cliente*/

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
    estado[i] = 'T';
  }
  sem_init(&vagaSofa,0,0);
  sem_init(&mutexBarbearia, 0, 1); 
  sem_init(&clientesEsp,0,0);
  sem_init(&mutexCadeira,0,1);
  sem_init(&mutexSofa, 0, 1); 
  sem_init(&pagamento, 0, 0);   
  sem_init(&barbeiro, 0, 0);
  exibe_barbearia();
  for (i = 0; i < 3; i++) /*tem 3 barbeiros trabalhando ou dormindo*/
    pthread_create(&thr[i], NULL, F_barbeiro, NULL;
  for (i = 0; i < N; i++) /*tem N clientes querendo cortar cabelo*/
    pthread_create(&thr[i], NULL, cliente, (void*) &id_cliente[i]);

  for (i = 0; i < N; i++) /*garante que o processo nao acabe antes das threads*/
    pthread_join(thr[i], NULL);

  return 0;
}
