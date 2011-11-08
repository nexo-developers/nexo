#include <stdio.h>
#include <pthread.h>
#include <gconf/gconf-client.h>
#include <glib.h>
#include <gtk/gtk.h>


#define N 8
#define M 16

/* constantes */
const char *mensaje[2] = {"Hola Mundo", "Mundo Hola"};
const int cantidad[2] = {N, M};
const int repeticiones = 20;

/* variables globales */ 
int x = 0;
//useconds_t pausa = 999999;
unsigned int pausa = 999999;
char *key = "/apps/mousecam/MyVal";
char *monitor = "/apps/mousecam";

/* funciones declaracion*/
void imprime_mensaje(void *ptr);
void listenerGconf(void *ptr);
void setKeys(GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer user_data);

int main(void)
{
	pthread_t hilo0, hilo1;
	int id0=0, id1=1;

	pthread_create(&hilo0, NULL, (void *) &imprime_mensaje, (void *) &id0);	
	pthread_create(&hilo1, NULL, (void *) &listenerGconf, (void *) &id1);	
	
	pthread_join(hilo0, NULL);
	pthread_join(hilo1, NULL);

	return 0;
}

void imprime_mensaje(void *ptr)
{
	int i=0, id=0;

	id = *(int *) ptr;
	//for(i=0; i<repeticiones; i++) {
    for(;;) {
        usleep(pausa);/*
        if (-1 == usleep(pausa)) {
            printf("(%d) Error: usleep\n",id);
            pthread_exit(-1);
        }
*/
		printf("(%d) imprime_mensaje: %s\n",id,mensaje[id]);
    }
	return;
}

void listenerGconf(void *ptr)
{
    GConfClient *client;

	//char *key = "/apps/mousecam/MyVal";
	//char *monitor = "/apps/mousecam";
	int valor;
	int clientId;

	client = gconf_client_get_default();
	gconf_client_add_dir(client, monitor, GCONF_CLIENT_PRELOAD_NONE, NULL);

	gconf_client_notify_add(client,
			  	  	  	  	  key,
	                          setKeys,
	                          NULL,
	                          NULL,
	                          NULL);

	//valor =  gconf_client_get_int(client, key, NULL);
    //printf("(%d) listenerGconf valor %d\n",id,valor);
    
   
	gtk_main();
}

void setKeys(GConfClient *client, guint cnxn_id, GConfEntry *entry,
		gpointer user_data) {
	printf("Callback, nuevo valor: %d\n", gconf_value_get_int(gconf_entry_get_value(entry)));
	gconf_client_get_int(client, key, NULL);
}
