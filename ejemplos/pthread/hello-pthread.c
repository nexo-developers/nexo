#include <stdio.h>
#include <pthread.h>


#define N 8
#define M 16

/* constantes */
const char *mensaje[2] = {"Hola Mundo", "Mundo Hola"};
const int cantidad[2] = {N, M};

/* variables globales */ 
int x = 0;

void imprime_mensaje(void *ptr);

int main(void)
{
	pthread_t hilo0, hilo1;
	int id0=0, id1=1;

    /* Se crean dos hilos donde uno recibe el id 0 y otro el 1 para saber qué mensaje debe imprimir */
	pthread_create(&hilo0, NULL, (void *) &imprime_mensaje, (void *) &id0);	
	pthread_create(&hilo1, NULL, (void *) &imprime_mensaje, (void *) &id1);	
	
	pthread_join(hilo0, NULL);
	pthread_join(hilo1, NULL);

	return 0;
}

void imprime_mensaje(void *ptr) {
	int i=0, id=0;

	id = *(int *) ptr;
	for(i=0; i<cantidad[id]; i++) {
        usleep(100000);
        x++;
		printf("%d - %s\n",x,mensaje[id]);
    }
	return;
}
