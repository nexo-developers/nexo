#include<stdio.h>
#include<gconf/gconf-client.h>
#include<glib.h>
#include<gtk/gtk.h>

char *key = "/apps/mousecam/MyVal";
char *monitor = "/apps/mousecam";

void setKeys(GConfClient *client, guint cnxn_id, GConfEntry *entry,
		gpointer user_data) {
	printf("callback, nuevo valor: %s\n", gconf_value_get_string(gconf_entry_get_value(entry)));
	gconf_client_get_string(client, key, NULL);
}

int main(int argc, char* argv[]) {
	GConfClient *client;

	//char *key = "/apps/mousecam/MyVal";
	//char *monitor = "/apps/mousecam";
	char *valor;
	int clientId;

	client = gconf_client_get_default();
//	gconf_client_set_string(client, key, "COSA", NULL);
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

	valor = gconf_client_get_string(client, key, NULL);
	if (valor == NULL) {
		printf("valor es null\n");
		return 1;
	}
	printf("%s\n", valor);
	if (argc > 1) {
		gconf_client_set_string(client, key, argv[1], NULL);
		printf("Cambiado el fondo por %s\n", argv[1]);
	} else {
		printf("No me mandaste a cambiar nada\n");
	}

	gtk_main();

	return 0;
}
