
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
	int length;
	Process *last;
	Process *first;
}Queue;

void pushToQueue(Queue *q, Process *p, Memory *m, Huecos *h);
void pushToMemory(Memory *m, Process *p, Huecos *h, Queue *q);


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


void initQueue(Queue *q){
	q->last = NULL;
	q->first = NULL;
	q->length = 0;
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
	printf("\n\n -----------Estado de la memoria-------	 \n");
	while(cursor != NULL){
		printf("Nodo: %d, allocated: %d, ubicacion: %d, tamaño: %d \n", cursor->pId, cursor->pAllocated, cursor->pLocation, cursor->pSize);
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


void printQueueInfo(Queue *q){
	//malloc problem
	printf("\n\n -------- COLA ---------- \n");
	Process *cursor3 = (Process *)malloc(sizeof(Process)); 
	cursor3 = q->first;
	printf("Nodo: %d en la cola \n", cursor3->pId);
	//printf("NodoNEXT: %d en la cola \n", q->first->next->pId);

	//EL LOOP INFINITO ERA PORQUE NO HABIA UN NEXT PERO EL PROCESS SI EXISTIA SIEMPRE 
	while (cursor3->next != NULL) {
    cursor3 = cursor3->next;
	printf("\n cursor->next-pid ");
  }
	printf("----------\n");
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
	//printf("\nBuscando el peor ajuste\n");
	Process *cursor = (Process *)malloc(sizeof(Process)); 
	Process *maxHueco = (Process *)malloc(sizeof(Process)); 
	cursor = m->first;
	//Volvemos NULL para no regresar basura
	maxHueco->next = NULL;
	maxHueco->pId = 0;

	while(cursor != NULL){
		if(cursor->pAllocated == 0){
			if(maxHueco == NULL){
				maxHueco = cursor;
				//printf("\nprimera asignacion de hueco: %d, con tamaño: %d\n ", maxHueco->pId, maxHueco->pSize);
				return maxHueco;
			}
			if(cursor->pSize > maxHueco->pSize){
				maxHueco = cursor;
				//printf("\nSe reasigna maxhueco, el máximo es: %d con tamaño %d\n",  maxHueco->pId, maxHueco->pSize);
			}
			// printf("Hueco en :: %d, con tamaño: %d\n", cursor->pId, cursor->pSize);
			//pushToHuecos(h, cursor);
		}
		cursor = cursor->next;
	}

	//Hueco con tamaño maximo
	printf("\n Maximo hueco devuelto Pid: %d\n", maxHueco->pId);
	return maxHueco;
}




/*************  Queue  ***************/

void cleanMemory(Memory *m, Queue *q){
	Process *cursor = (Process *)malloc(sizeof(Process)); 
	cursor = m->first;
	while(cursor != NULL){
		if(cursor->pAllocated == 0){
			//Es un hueco, A POR ÉL
			if(cursor->prev == NULL){
				//Es el inicio de la memoria
				m->first = cursor->next;
				cursor->next->prev = NULL;
			}else{
				cursor->prev->next = cursor->next;
				cursor->next->prev = cursor->prev;
				cursor->next->pLocation = cursor->next->prev->pLocation + cursor->next->prev->pSize; // está bien esto???
			}
			//Eliminar de la memoria real el cursor que ya no está referenciado (garbage collection)
		}
		printf("Hueco con id %d eliminado\n", cursor->pId);
		cursor = cursor->next;
	}
	return;
}


void cleanQueue(Memory *m, Queue *q, Huecos *h){
	Process *cursor = (Process *)malloc(sizeof(Process)); 
	cursor = q->first;
	while(cursor != NULL){
		printf("INTENTANDO REINSERTAR PROCESO: %d\n", cursor->pId);
		pushToMemory(m, cursor, h, q);
		cursor = cursor->next;
	}
	printQueueInfo(q);
	q->length = 0;

}



void pushToQueue(Queue *q, Process *p, Memory *m, Huecos *h){
	printf("Process %d queued\n", p->pId);
	if(q->first == NULL && q->last == NULL){
		//Es el primer nodo
		q->first = p;
		q->last = p;
		printf("\n***** \n");
		printf("\nPRIMER NODO DE LA COLA \n");
		printf("\n q->first->pid  %d \n", p->pId);
		printf("\n q->first->pid  %d \n", q->first->pId);

		printf("\n***** \n");

		return;
	}
	else if(q->length > 4){
		printf("Límite de a cola alcanzado, haciendo limpieza \n");
		cleanMemory(m, q);
		printf("AQUÍ VA A MOSTRAR ESTA WEADA \n");
		//printQueueInfo(q);
		Process *cursor = (Process *)malloc(sizeof(Process)); 
		cursor = q->first;
		while (cursor->next != NULL) {

			printf("\n actual :%d, ursor->next-pid %d ",cursor->pId, cursor->next->pId);
			cursor = cursor->next;
		}
		printf("\n AQUI SI SE VE LA QUEYE BIEN ALV getchar() ");
		getchar();
		cleanQueue(m, q, h);
		//printQueueInfo(q);
		
		return;
	}else{
	//Apunta al primer nodo, pues se encuentran en una lista diferente a la de la memoria. No hay problema con eso
	//FALTA: Agregar referencia a next porque no está siendo una cola
	printf("\n***** REASIGNANDO COLA *** \n");
	// //gerry
	Process *cursor = (Process *)malloc(sizeof(Process)); 
	cursor = q->first;
	while (cursor->next != NULL) {
    cursor = cursor->next;
	printf("\n cursor->next-pid ");
  }
	cursor->next = p;
	p->next = NULL;
	printf("\n NEW :: cursor->pid : %d cursor->next  %d \n", cursor->pId,cursor->next->pId);

	// p->pLocation = m->last->pLocation + m->last->pSize; ?? location not currently knowed
	q->length = q->length+1;
	printf("Procesos encolados: %d\n", q->length);



	}

	//printQueueInfo(q);

}






/*************  Memory  ***************/

void pushToMemory(Memory *m, Process *p, Huecos *h, Queue *q){
	if(m->first == NULL && m->last == NULL){
		p->pLocation = m->mOffset;
		//Es el primer nodo
		p->prev = NULL;
		p->next = NULL;
		m->first = p;
		m->last = p;
		/// la borro :v m->mSize = m->mSize - p->pSize;

	}else{
		if((m->mSize - (m->last->pLocation + m->last->pSize)) < p->pSize){
			printf("\n ---Ya no cabe en lista, buscando en huecos ---");
			Process *px = (Process *)malloc(sizeof(Process)); 
		 	px = lookForBiggestGap(m, h);	

			// printf("\n--- px->pSize %d >= p->pSize %d  ---\n", px->pSize, p->pSize);
			if(px->pSize >= p->pSize){
				//printf("\n---Se encontro un hueco----\n");
				int newGapSize = px->pSize - p->pSize;
				px->pId = p->pId;
				px->pAllocated = 1;
				px->pSize = p->pSize;
				printf("\n---Se inserto el proc id: %d  con psize: %d en un hueco ----\n",px->pId,px->pSize );

				if(newGapSize > 0){
					Process *hueco = (Process*)malloc(sizeof(Process));
					hueco = initProcess(hueco, 9999, newGapSize);
					//Vuelvo el proceso un hueco con el allocated = 0
					//printf("\n****\n Reasignacion de apuntadores \n****\n " );
					hueco->pAllocated = 0;
					hueco->pLocation = px->pSize + px->pLocation;
					hueco->next = px->next;
					//printf("\n****\n hueco->pLocation: %d \n****\n ", hueco->pLocation );
					//printf("hueco->next->pId: %d \n", hueco->next->pId );
					px->next = hueco;
					hueco->prev  = px;
				}
				//lookForBiggestGap(m, h);
			}else{
				printf("\n ************ \n No cabe en ningun hueco, agregando a la cola \n ************ \n");
				pushToQueue(q, p, m, h);
			}
			
		}else{
				p->next = NULL;
				m->last->next = p;
				p->prev = m->last;
				p->pLocation = m->last->pLocation + m->last->pSize; 
				printf("Agregado al final de la lista en Allocated in: %d\n", p->pLocation);
				m->last = p; 
		}
	}
	printMemoryInfo(m);
	return;
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
    Queue *q = (Queue*)malloc(sizeof(Queue));

    initMemory(m);
	initHuecos(h);
	initQueue(q);

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
			pushToMemory(m, p, h, q);
   		}
		i++;
		getchar();   		
   }
    return 0;
}
