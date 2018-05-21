// Autor
//		Lopez Santibañez Jimenez Luis Gerardo


/*
	Descripción:
		Utilizando el peor ajuste este programa aloja procesos en locaciones de 
		memoria contigua, cuando se acaba la memoria se busca un hueco para 
		alojar un nuevo proceso, este hueco debe ser el que tenga la mayor
		longitus de bits siguiendo el principio del peor ajuste.

		Cuando se alcanzan mas de 5 procesos en espera de alojamineto 
		se realiza la compactacion de la memoria para liberar espacio
		y poder asignar los procesos en

	Conclusiones
		Lopez Santibañez Jimenez Luis Gerardo
			Este programa ilustra muy bien la realizacion de el alojamiento de memoria
			en los diferentes sistemas operativos utilzzando una estrategia especifica
			pero que podria ser generica para utilizar otros tipos de ajuste
			Una de las complicaciones de esta practica fue el manejo de apuntadores
			el cual conlleva un nivel de abstraccion de los datos especial
			y mas cuando se ha perdido la practica por otros lenguajes de mas alto nivel

*/


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

void enqueue(Queue *q, Process *p, Memory *m, Huecos *h);
void addProcess(Memory *m, Process *p, Huecos *h, Queue *q);


/********** *Inicializar estructuras *********/

void initMemory(Memory *m){
	//Para tener 3000 unidades libres se termina en 4500
	m->mSize = 4500;
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



/********* Se muestra la informacion de las estrucutras ************/

void showProcesses(Memory *m){
	Process *cursor = (Process *)malloc(sizeof(Process)); 
	cursor = m->first;
	printf("\n\n -----------Estado de la memoria-------	 \n");
	while(cursor != NULL){
		printf("Nodo: %d, allocated: %d, ubicacion: %d, tamaño: %d \n", cursor->pId, cursor->pAllocated, cursor->pLocation, cursor->pSize);
		cursor = cursor->next;
	}
	printf("----------------\n");

	return;
}

void showHuecos(Huecos *h){
	Process *cursor = (Process *)malloc(sizeof(Process)); 
	cursor = h->first;
	while(cursor != NULL){
		printf("Se creo un hueco: %d, en ubicacion: %d. \n", cursor->pId, cursor->pLocation);
		cursor = cursor->next;
	}
	return;
}

/**********  Cola  ***********/

void showQueue(Queue *q){
	printf("\n\n -------- COLA ---------- \n");
	Process *cursor3 = (Process *)malloc(sizeof(Process)); 
	cursor3 = q->first;
	while (cursor3->next != NULL) {
    cursor3 = cursor3->next;
	printf("\n Siguiente en cola - pid %d", cursor3->pId);
  }
	printf("\n----------\n");
	return;
}




/**********  Asignacion de huecos  ***********/

void pushToHuecos(Huecos *h, Process *p){
	if(h->first == NULL && h->last == NULL){
		printf("Se va a crear el hueco en: %d\n", p->pId);
		h->first = p;
		h->last = p;

	}else{
		printf("Se va a crear el hueco en: %d \n", p->pId);
		h->last->next = p;
		h->last = p; 
	}
	return;
}

/**********  Buscqueda del hueco mas grande para peor ajuste  ***********/

Process* buscarPeorAjuste(Memory *m, Huecos *h){
	//printf("\nBuscando el peor ajuste\n");
	Process *cursor = (Process *)malloc(sizeof(Process)); 
	Process *maxHueco = (Process *)malloc(sizeof(Process)); 
	cursor = m->first;
	maxHueco->next = NULL;
	maxHueco->pId = 0;

	while(cursor != NULL){
		if(cursor->pAllocated == 0){
			if(maxHueco == NULL){
				maxHueco = cursor;
				// Primera asignacion
				return maxHueco;
			}
			if(cursor->pSize > maxHueco->pSize){
				maxHueco = cursor;
				// Si hay un hueco mas grande se reasigna
			}
		}
		cursor = cursor->next;
	}

	//Hueco con tamaño maximo
	return maxHueco;
}




/*************  Limpiamos la memoria  ***************/

void compactacion(Memory *m, Queue *q){
	Process *cursor = (Process *)malloc(sizeof(Process)); 
	cursor = m->first;
	while(cursor != NULL){
		if(cursor->pAllocated == 0){
			//Es un hueco
			if(cursor->prev == NULL){
				//Es el inicio de la memoria
				m->first = cursor->next;
				cursor->next->prev = NULL;
			}else{
				cursor->prev->next = cursor->next;
				cursor->next->prev = cursor->prev;
				cursor->next->pLocation = cursor->next->prev->pLocation + cursor->next->prev->pSize; 
				// Reasignacion
			}
			//Eliminar de la memoria real el cursor que ya no está referenciado (garbage collection)
		}
		cursor = cursor->next;
	}
	printf("\n ------- Se limpio la memoria de huecos ---- \n");
	return;
}

/*************  Limpiamos la cola  ***************/

void dequeue(Memory *m, Queue *q, Huecos *h){
	Process *cursor = (Process *)malloc(sizeof(Process)); 
	cursor = q->first;

	while(cursor != NULL){
		printf("Reinsertando de la cola %d\n", cursor->pId);
		addProcess(m, cursor, h, q);
		cursor = cursor->next;
	}
	
	q->length = 0;

}


/*************  Agregamos a la cola  ***************/

void enqueue(Queue *q, Process *p, Memory *m, Huecos *h){
	printf("\n No hay espacio poniendo en cola\n");
	if(q->first == NULL && q->last == NULL){
		//Es el primer nodo
		q->first = p;
		q->last = p;
	}
	else if(q->length > 4){
		printf("\n ------ Límite de a cola alcanzado, haciendo limpieza ------\n");
		compactacion(m, q);
		dequeue(m, q, h);
		// se limpia la memoria y la cola
		
		
		return;
	}else{

	Process *cursor = (Process *)malloc(sizeof(Process)); 
	cursor = q->first;
	while (cursor->next != NULL) {
    cursor = cursor->next;
  }
	cursor->next = p;
	p->next = NULL;
	q->length = q->length+1;

	}
}






/************* Se agrega a la memoria ***************/

void addProcess(Memory *m, Process *p, Huecos *h, Queue *q){
	if(m->first == NULL && m->last == NULL){
		p->pLocation = m->mOffset;
		//Es el primer nodo
		p->prev = NULL;
		p->next = NULL;
		m->first = p;
		m->last = p;

	}else{
		if((m->mSize - (m->last->pLocation + m->last->pSize)) < p->pSize){
			printf("\n -----Ya no cabe en lista, buscando en huecos -----");
			Process *px = (Process *)malloc(sizeof(Process)); 
		 	px = buscarPeorAjuste(m, h);	

			// Se valida si cabe en el maximo hueco
			if(px->pSize >= p->pSize){
				int newGapSize = px->pSize - p->pSize;
				px->pId = p->pId;
				px->pAllocated = 1;
				px->pSize = p->pSize;
				printf("\n---Se inserto id: %d un hueco ----\n",px->pId );

				//Si el proceso tiene fragmentacion interna
				// se crea un hueco en ese espacio
				if(newGapSize > 0){
					Process *hueco = (Process*)malloc(sizeof(Process));
					hueco = initProcess(hueco, -11, newGapSize);
					hueco->pAllocated = 0;
					hueco->pLocation = px->pSize + px->pLocation;
					hueco->next = px->next;
					px->next = hueco;
					hueco->prev  = px;
				}
			}else{
				printf("\n ************ \n No cabe en ningun hueco, agregando a la cola \n ************ \n");
				enqueue(q, p, m, h);
			}
			
		}else{
				//Aun hay espacio en la memoria
				p->next = NULL;
				m->last->next = p;
				p->prev = m->last;
				p->pLocation = m->last->pLocation + m->last->pSize; 
				m->last = p; 
		}
	}
	showProcesses(m);
	return;
}

/************* Se elimina a la memoria ***************/

void popFromMemory(Memory *m, int id){
	Process *cursor = (Process *)malloc(sizeof(Process)); 
	cursor = m->first;
	while(cursor != NULL){
		if(cursor->pId == id){
			cursor->pAllocated = 0;
			printf("Se libero el nodo: %d\n", cursor->pId);
			return;
		}
		cursor = cursor->next;
	}
	return;
}



/***********  Main  *************/

int main(int argc, char ** argv){
	//se lee de un archivo
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

	//Inicializacion

    if(fptr == NULL){
      printf("Error al abrir el archivo.");   
      exit(1);             
   }

   
   while(fscanf(fptr, "%d%d", &pId, &pSize) == 2){
   		printf("Proceso creado: %d\n", pId);
   		Process *p = (Process*)malloc(sizeof(Process));
		p = initProcess(p, pId, pSize);
   		if (pSize == 0){
   			popFromMemory(m, pId);
   		}else{
			addProcess(m, p, h, q);
   		}
		i++;
   }
   printf("\n\n Fin del programa \n\n");
    return 0;
}

