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
int barbeirosAC = 0; /*barbeiros que acabaram de cortar cabelo*/ 
int sofa = 4;
int tc = 0; /*total de clientes na barbearia*/
sem_t mutexBarbearia; /* os mutex garantem exclusao mutua quando necessario*/
sem_t mutexBarbeiro;
sem_t mutexGeral;
sem_t mutexSofa;
sem_t vagaSofa; /*caso nao tenha vaga no sofa, esse semaforo eh utilizado para o proximo conseguir sentar quando houver vaga*/
sem_t barbeiro; /*garante que os clientes sejam atendido na ordem certa, apesar de um barbeiro poder chamar o cliente por vez*/
sem_t clientesEsp; /*avisa o barbeiro que existe cliente*/
sem_t pagamento; /*avisa ao cliente que o barbeiro vai pegar seu pagamento*/
sem_t mutexCaixa;
int barbeirosV[3] = {0, 0, 0};


void exibe_barbearia(){
  int i,p = 0,c=0,s=0, v[N];
  for(i = 0; i < N; i++){ /*exibe o caixa de pagamento*/
	v[i] = 0;
    if(estado[i] == 'P'){
      p = 1;
      printf("B II %d ", i);
    }
  }
  if(p == 0){
    printf("  II   ");
  }
  for(i = 0; i < N; i++){ /* exibe a fila de espera do caixa*/
    if(estado[i] == 'R'){
      printf("%d ", i);
      v[i] = 1;
    }
  }
  
  for (i = 0; i < 3; i++) {
	  if (barbeirosV[i] == 0) {
		printf("|B | ");
	  } else if (barbeirosV[i] > 0 && v[barbeirosV[i] - 1] == 0) {
		printf("|B %d| ", barbeirosV[i] - 1);
	  } else {
		printf("| | ");
	  }
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
  sem_wait(&mutexBarbearia); /* garante exclusao mutua na hora de verificar a lotacao da barbearia*/
  if(tc < MAX){ /*caso a barbearia nao esteja lotada*/
    tc++;
    entrou = 1;
  }
  sem_post(&mutexBarbearia);
  if(entrou){
    sentaSofa(cliente_id);
  }
  else
    estado[cliente_id] ='X';/*vai embora*/
}


void cortaCabelo(int cliente_id) {
	  /*libera uma vaga no sofá*/
	int i;
	sem_post(&vagaSofa);
	sem_wait(&mutexGeral);
	sofa = sofa + 1;  
	estado[cliente_id] = 'C'; /*começa a cortar o cabelo*/
	
	for (i = 0; i < 3; i++) {
		if(barbeirosV[i] == 0){
			barbeirosV[i] = cliente_id + 1;
			break;
		}
	}
	
	sem_post(&mutexGeral);
	sleep(1);
	sem_wait(&mutexGeral);
	exibe_barbearia();
	sem_post(&mutexGeral);
	paga(cliente_id);
	
}

/*ao conseguir entrar na barbearia, verifica se tem lugar no sofa*/
void sentaSofa(int cliente_id){
  sem_wait(&mutexSofa); /*apenas um cliente por vez pode sentar no sofa*/
  if(sofa>0){ /*caso tenha lugar no sofa,senta, se nao espera*/
    sofa = sofa - 1;
    estado[cliente_id] = 'S';
    sleep(1);
    sem_wait(&mutexGeral);
    exibe_barbearia();
    sem_post(&mutexGeral);
    sem_post(&mutexSofa); /*libera outra pessoa para tentar sentar no sofa*/
    sem_post(&clientesEsp);
    
    sem_wait(&barbeiro);
    cortaCabelo(cliente_id);
    
  }
  else{
    estado[cliente_id] = 'E';
    sem_wait(&mutexGeral);
    exibe_barbearia();
    sem_post(&mutexGeral);
    Esperando(cliente_id);
  }

}
/*espera ateh liberar algum lugar no sofa*/
void Esperando(int cliente_id){
  sem_wait(&vagaSofa); /*espera uma vaga no sofa*/
  sofa = sofa - 1; /*logo que liberar a vaga, ja senta*/
  estado[cliente_id] = 'S';
  sem_post(&mutexSofa);/*libera para alguem tentar sentar no sofa*/
  sleep(1);
  sem_wait(&mutexGeral);
  exibe_barbearia();
  sem_post(&mutexGeral);
  sem_post(&clientesEsp);/*entra na lista de espera para ser atendido*/
  sem_wait(&barbeiro);
  cortaCabelo(cliente_id);
}

void paga(int cliente_id){
  int i;
  
  estado[cliente_id] = 'R';
  sleep(1);
  sem_wait(&mutexGeral);
  exibe_barbearia();
  sem_post(&mutexGeral);

  sem_post(&pagamento);  /*espera o barbeiro receber o pagamento*/
  
  sem_wait(&mutexGeral);
	for (i = 0; i < 3; i++) {
		if(barbeirosV[i] == cliente_id+1){
			barbeirosV[i] = -1;
			
			break;
		}
	}
estado[cliente_id] = 'P'; /*paga o barbeiro*/
  sleep(1);
  exibe_barbearia();
sem_post(&mutexGeral);

  sem_wait(&mutexGeral);
  estado[cliente_id] = 'X'; /*sai da barbearia*/
  sem_post(&mutexCaixa);
  sem_post(&mutexGeral);
}


void* F_barbeiro(void *v){
	int j;
	for(j = 0; j < 2; j++){
	int i;
  sem_wait(&mutexBarbeiro);/*um barbeiro recebo o cliente de cada vez*/
  sem_wait(&clientesEsp);/*caso um cliente chegue, o barbeiro acorda*/
  sem_post(&barbeiro);
  
  sem_post(&mutexBarbeiro);/*libera outro barbeiro para receber o proximo cliente*/
  sleep(1);
  sem_wait(&mutexGeral);
  exibe_barbearia();
  sem_post(&mutexGeral);
  sem_wait(&pagamento);
  
  
  
  barbeirosAC = 1;
  sem_wait(&mutexCaixa); /*recebe o pagamento do cliente*/
  sem_wait(&mutexGeral);
  for (i = 0; i < 3; i++) {
		if(barbeirosV[i] == -1){
			barbeirosV[i] = 0;
			break;
		}
	}
	sem_post(&mutexGeral);
  barbeirosAC = 0;
  sem_wait(&mutexGeral);
  exibe_barbearia();
  sem_post(&mutexGeral);
  }
  exit(0);

}

void* cliente(void *v){

  int id_cliente = *(int *) v;
  while(1){
    entraBarbearia(id_cliente);
    sentaSofa(id_cliente);
    //temCabeloCortado(id_cliente);
    
  }
}





int main() {
  pthread_t thrC[N];
  pthread_t thrB[3];
  int i,id_cliente[N], id_barbeiro[3];
  for (i = 0; i < N; i++) {
    id_cliente[i] = i;
    estado[i] = 'T';
  }
  sem_init(&vagaSofa,0,0);
  sem_init(&mutexBarbearia, 0, 1);
  sem_init(&mutexBarbeiro, 0, 1);
  sem_init(&clientesEsp,0,0);
  sem_init(&mutexGeral,0,1);
  sem_init(&mutexCaixa, 0,1);
  sem_init(&mutexSofa, 0, 1);
  sem_init(&pagamento, 0, 0);
  sem_init(&barbeiro, 0, 0);
  exibe_barbearia();
  for (i = 0; i < 3; i++) /*tem 3 barbeiros trabalhando ou dormindo*/
    pthread_create(&thrB[3], NULL, F_barbeiro, &id_barbeiro[i]);
  for (i = 0; i < N; i++) /*tem N clientes querendo cortar cabelo*/
    pthread_create(&thrC[i], NULL, cliente, (void*) &id_cliente[i]);

  for (i = 0; i < N; i++){ /*garante que o processo nao acabe antes das threads*/
    pthread_join(thrC[i], NULL);
    pthread_join(thrB[i], NULL);  
  }

  return 0;
}
