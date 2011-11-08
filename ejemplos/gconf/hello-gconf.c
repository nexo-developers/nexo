#include<stdio.h>
#include<gconf/gconf-client.h>
#include<glib.h>
#include<gtk/gtk.h>

char *key = "/apps/mousecam/MyVal";
char *monitor = "/apps/mousecam";

void setKeys(GConfClient *client, guint cnxn_id, GConfEntry *entry,
		gpointer user_data) {
	printf("callback, nuevo valor: %d\n", gconf_value_get_int(gconf_entry_get_value(entry)));
	gconf_client_get_int(client, key, NULL);
}

int main(int argc, char* argv[]) {
	GConfClient *client;

	char *valor;
    int intValor;
	int clientId;

	client = gconf_client_get_default();
	gconf_client_add_dir(client, monitor, GCONF_CLIENT_PRELOAD_NONE, NULL);

	/* Connect the check_button to a callback that can toggle it when the option
	   * is changed by some outside program.
	   */
	  gconf_client_notify_add(client,
			  	  	  	  	  key,
	                          setKeys,
	                          NULL,
	                          NULL,
	                          NULL);

    //valor = gconf_client_get_string(client, key, NULL);
	intValor = gconf_client_get_int(client, key, NULL);
    

	if (argc > 1) {
		gconf_client_set_int(client, key, atoi(argv[1]), NULL);
		printf("Seteado valor %s recibido por parametro.\n", argv[1]);
	} else {
        gconf_client_set_int(client, key, 1000, NULL);
		printf("Seteado valor por defecto 1000\n");
	}

	gtk_main();

	return 0;
}
