
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define MEMORY_SIZE = 3000
#define MEMORY_OFFSET = 1500



/*********************  Estructuras de Datos *********************/

typedef struct Process{
	int pSize, pId, pLocation, pAllocated;
	struct Process *prev;
	struct Process *next;
}Process;


typedef struct Memory{
	int mSize, mOffset;
	Process *last;
	Process *first;
}Memory;


typedef struct Huecos{
	int size;
	Process *last;
	Process *first;
}Huecos;


typedef struct Queue{
	int pending;
	Process *last;
	Process *first;
}Queue;



/********** *Inicializar estructuras *********/
void initMemory(Memory *m){
	m->mSize = 3000;
	m->mOffset = 1500;
	m->last = NULL;
	m->first = NULL;
}


void initHuecos(Huecos *h){
	h->last = NULL;
	h->first = NULL;
}


Process* initProcess(Process* p, int pId, int pSize){
	p->pId = pId; 
	p->pSize = pSize;
	p->pAllocated = 1;
	return p;
}



/**********PRINTS************/

void printMemoryInfo(Memory *m){
	Process *cursor = (Process *)malloc(sizeof(Process)); 
	cursor = m->first;
	while(cursor != NULL){
		printf("Nodo: %d, allocated: %d, ubicacion: %d. \n", cursor->pId, cursor->pAllocated, cursor->pLocation);
		cursor = cursor->next;
	}
	return;
}


void printHuecosInfo(Huecos *h){
	Process *cursor = (Process *)malloc(sizeof(Process)); 
	cursor = h->first;
	while(cursor != NULL){
		printf("Hueco: %d, allocated: %d, ubicacion: %d. \n", cursor->pId, cursor->pAllocated, cursor->pLocation);
		cursor = cursor->next;
	}
	return;
}



/**********  Huecos  ***********/

void pushToHuecos(Huecos *h, Process *p){
	if(h->first == NULL && h->last == NULL){
		printf("pushing first hueco: %d\n", p->pId);
		h->first = p;
		h->last = p;

	}else{
		printf("pushing to hueco: %d \n", p->pId);
		h->last->next = p;
		h->last = p; 
	}
	return;
}


Process* lookForBiggestGap(Memory *m, Huecos *h){
	printf("\nBuscando el peor ajuste\n");
	Process *cursor = (Process *)malloc(sizeof(Process)); 
	Process *maxHueco = (Process *)malloc(sizeof(Process)); 
	cursor = m->first;
	//Volvemos NULL para no regresar basura
	maxHueco->next = NULL;
	maxHueco->pId = 0;

	while(cursor != NULL){
		//POR AQUÏ ENTRA A UN LOOP INFINITO
		printf("\nwhile %d\n", cursor->pId);
		if(cursor->pAllocated == 0){
			if(maxHueco == NULL){
				maxHueco = cursor;
				printf("\nprimera asignacion de hueco: %d, con tamaño: %d\n ", maxHueco->pId, maxHueco->pSize);
				return maxHueco;
			}
			if(cursor->pSize > maxHueco->pSize){
				maxHueco = cursor;
				printf("\nSe reasigna maxhueco, el máximo es: %d con tamaño %d\n",  maxHueco->pId, maxHueco->pSize);
			}
			// printf("Hueco en :: %d, con tamaño: %d\n", cursor->pId, cursor->pSize);
			//pushToHuecos(h, cursor);
		}
		cursor = cursor->next;
	}

	//Hueco con tamaño maximo
	printf("returning maxHueco Pid: %d\n", maxHueco->pId);
	return maxHueco;
}



/*************  Memory  ***************/

void pushToMemory(Memory *m, Process *p, Huecos *h){
	if(m->first == NULL && m->last == NULL){
		p->pLocation = m->mOffset;
		//Es el primer nodo
		p->prev = NULL;
		p->next = NULL;
		m->first = p;
		m->last = p;
		/// la borro :v m->mSize = m->mSize - p->pSize;

	}else{
		printf("\n Pushing to memory in else se quieren %d \n", p->pSize);
		if((m->mSize - (m->last->pLocation + m->last->pSize)) < p->pSize){
			printf("\n mSize: %d, m->last->pLocation: %d, m->last->pSize->: %d, p->size: %d \n", m->mSize, m->last->pLocation,  m->last->pSize, p->pSize);
			printf("Ya no cabe en la lista, buscando en los huecos\n");
			Process *px = (Process *)malloc(sizeof(Process)); 
		 	px = lookForBiggestGap(m, h);	

			printf("\n----xxx--\n");


			
			printf("\n--- px->pSize %d >= p->pSize %d  ---\n", px->pSize, p->pSize);
			if(px->pSize >= p->pSize){
				printf("\n---Se inserta proceso en hueco----\n");
				getchar();
				int newGapSize = px->pSize - p->pSize;
				px->pId = p->pId;
				px->pAllocated = 1;
				px->pSize = p->pSize;
				printf("\n---Success id: %d, psize %d----\n",px->pId,px->pSize );

				getchar();
				if(newGapSize > 0){
					Process *hueco = (Process*)malloc(sizeof(Process));
					hueco = initProcess(hueco, 9999, newGapSize);
					//Vuelvo el proceso un hueco con el allocated = 0
					printf("\n****\n Reasignacion de apuntadores \n****\n " );
					getchar();
					hueco->pAllocated = 0;
					hueco->pLocation = px->pSize + px->pLocation;
					hueco->next = px->next;
					printf("\n****\n hueco->pLocation: %d \n****\n ", hueco->pLocation );
					//printf("hueco->next->pId: %d \n", hueco->next->pId );
					px->next = hueco;
					hueco->prev  = px;
					printf("px->next: %d \n", px->next->pId );
					printf("hueco->prev: %d \n", hueco->prev->pId );
					printf("px->prev: %d \n", px->prev->pId );
					


				}
				//lookForBiggestGap(m, h);
			}else{
				printf("\n ************ \n No cabe en ningun hueco \n ************ \n");
			}
			
		}else{
			printf("Si te cabe en la lista, asignando al final de la lista.\n\n");
				p->next = NULL;
				m->last->next = p;
				p->prev = m->last;
				p->pLocation = m->last->pLocation + m->last->pSize; 
				printf("Allocated in: %d\n", p->pLocation);
				m->last = p; 
		}
	}
	printMemoryInfo(m);
}


void popFromMemory(Memory *m, int id){
	Process *cursor = (Process *)malloc(sizeof(Process)); 
	cursor = m->first;
	while(cursor != NULL){
		if(cursor->pId == id){
			cursor->pAllocated = 0;
			//Se agrega a huecos
			printf("Nodo dealocado: %d\n", cursor->pId);
			return;
		}
		cursor = cursor->next;
	}
	return;
}



/***********  Main  *************/

int main(int argc, char ** argv){
    char * filePath = argv[1];
    int pId, pSize, i;
    FILE *fptr;
    fptr = fopen(filePath,"r");

    Memory *m = (Memory*)malloc(sizeof(Memory));
    Huecos *h = (Huecos*)malloc(sizeof(Huecos));
    initMemory(m);
	initHuecos(h);

    if(fptr == NULL){
      printf("Error al abrir el archivo.");   
      exit(1);             
   }

   
   while(fscanf(fptr, "%d%d", &pId, &pSize) == 2){
   		printf("PROCESO: %d\n", pId);
   		Process *p = (Process*)malloc(sizeof(Process));
		p = initProcess(p, pId, pSize);
   		if (pSize == 0){
   			printf("se va a deallocar\n");
   			popFromMemory(m, pId);
   		}else{
			pushToMemory(m, p, h);
   		}
		i++;
		getchar();   		
   }
    return 0;
}














