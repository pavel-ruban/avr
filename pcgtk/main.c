#include <sys/time.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>

#include <string.h>
#include <math.h>

static GtkBuilder *builder;
GtkWidget *window;
GtkTreeStore *store;
        GtkStatusIcon *tray_icon;

int sockfd,n;
struct sockaddr_in servaddr,cliaddr;
socklen_t len;
char mesg[1000];

#undef GTK_DISABLE_DEPRECATED

	G_MODULE_EXPORT void
quit_activate (GtkAction *action)
{
	GtkWidget *window;

	window = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));
	gtk_widget_destroy (window);
}

	G_MODULE_EXPORT void
about_activate (GtkAction *action)
{
	GtkWidget *about_dlg;

	about_dlg = GTK_WIDGET (gtk_builder_get_object (builder, "aboutdialog1"));
	gtk_dialog_run (GTK_DIALOG (about_dlg));
	gtk_widget_hide (about_dlg);
}

	G_MODULE_EXPORT void
help_activate (GtkAction *action)
{
	g_print ("Help not available\n");
}

void tray_icon_on_click(GtkStatusIcon *status_icon,
                        gpointer user_data)
{
	memcpy(mesg, "command: open", 13);
	send_udp_packet();

        printf("Clicked on tray icon\n");
	gtk_status_icon_set_from_file (tray_icon, "door_open.png");
}

void tray_icon_on_menu(GtkStatusIcon *status_icon, guint button,
                       guint activate_time, gpointer user_data)
{
	static uint8_t hidden = 0;

	gtk_status_icon_set_from_file (tray_icon, "door.png");
	if (hidden) {
		gtk_window_deiconify(window);
		hidden = 0;
	}
	else
	{
		gtk_window_iconify(window);
		hidden = 1;
	}
        printf("Popup menu\n");
}

static GtkStatusIcon *create_tray_icon() {

        tray_icon = gtk_status_icon_new_from_file("door.png");
        g_signal_connect(G_OBJECT(tray_icon), "activate",
                         G_CALLBACK(tray_icon_on_click), NULL);
        g_signal_connect(G_OBJECT(tray_icon),
                         "popup-menu",
                         G_CALLBACK(tray_icon_on_menu), NULL);
        //gtk_status_icon_set_from_icon_name(tray_icon,
          //                                 GTK_STOCK_MEDIA_STOP);
       // gtk_status_icon_set_tooltip(tray_icon,
         //                           "Example Tray Icon");
        gtk_status_icon_set_visible(tray_icon, TRUE);

        return tray_icon;
}

	GtkWidget *
do_builder (GtkWidget *do_widget)
{
	static GtkWidget *window = NULL;
	GError *err = NULL;
	GtkWidget *toolbar;

	if (!window)
	{
		builder = gtk_builder_new ();
		gtk_builder_add_from_file (builder, "window.ui", &err);
		if (err)
		{
			g_error ("ERROR: %s\n", err->message);
			return NULL;
		}
		gtk_builder_connect_signals (builder, NULL);
		window = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));
		gtk_window_set_screen (GTK_WINDOW (window),
				gtk_widget_get_screen (do_widget));
		g_signal_connect (window, "destroy",
				G_CALLBACK (gtk_widget_destroyed), &window);
		toolbar = GTK_WIDGET (gtk_builder_get_object (builder, "toolbar1"));
		gtk_style_context_add_class (gtk_widget_get_style_context (toolbar),
				"primary-toolbar");
	}

	if (!gtk_widget_get_visible (window))
	{
		gtk_widget_show_all (window);
	}
	else
	{
		gtk_widget_destroy (window);
		window = NULL;
	}


	return window;
}

enum
{
	ID_COLUMN,
	TIME_COLUMN,
	SRC_COLUMN,
	DEST_COLUMN,
	PROTOCOL_COLUMN,
	DATA_COLUMN,
	N_COLUMNS
};

GtkTreeIter iter;
unsigned int row_id = 0;

	void
populate_tree_model(GtkTreeStore *store)
{
struct timeval tv;
gettimeofday(&tv, NULL);

double time_in_mill =
         (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;

	for (int i = 0; i < 1; ++i)
	{
		gtk_tree_store_append(store, &iter, NULL);  /* Acquire an iterator */
		gtk_tree_store_set (store, &iter,
				ID_COLUMN, row_id,
				TIME_COLUMN, (double) time_in_mill,
				SRC_COLUMN, "00:13:BC:4D:F2:C1",
				DEST_COLUMN, "00:13:BC:4D:F2:C1",
				PROTOCOL_COLUMN, "UDP",
				DATA_COLUMN, mesg,
				-1);

		++row_id;
	}
}

void send_udp_packet()
{
	unsigned long NonBlock = 1; //ioctlsocket(sockfd, FIONBIO, &NonBlock);
	gtk_status_icon_set_from_file (tray_icon, "door.png");

	struct timeval timeout;

	// Setup fd_set structure
	fd_set fds;
	while (1)
	{
		timeout.tv_sec = 0;
		timeout.tv_usec = 300000;

		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);
		// Return value:
		// -1: error occurred
		// 0: timed out
		// > 0: data ready to be read

		sendto(sockfd, mesg, strlen(mesg), 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
		g_print("UDP packet is sent\n");

		int retval = select(sockfd + 1, &fds, NULL, NULL, &timeout);
		if(retval == -1){
			printf("Error");
		}
		else if(retval == 0){
			printf("Timeout");
		}
		else {
			unsigned int len = sizeof(cliaddr);
			n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
			printf("-------------------------------------------------------\n");
			mesg[n] = 0;
			printf("Received the following:\n");
			printf("%s",mesg);
			printf("-------------------------------------------------------\n");
			if (!strcmp("ignored", mesg))
			{
				break;
			}
			if (!strcmp("ok", mesg))
			{
				break;
			}
		}

	}
}

void
setup_tree (GtkWidget *window)
{
	GtkWidget *tree;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;

	/* Create a model.  We are using the store model for now, though we
	 * could use any other GtkTreeModel */
	store = gtk_tree_store_new (N_COLUMNS,
			G_TYPE_INT,
			G_TYPE_DOUBLE,
			G_TYPE_STRING,
			G_TYPE_STRING,
			G_TYPE_STRING,
			G_TYPE_STRING);


	/* custom function to fill the model with data */
	populate_tree_model(store);

	/* Create a view */
	tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));

	/* The view now holds a reference.  We can get rid of our own
	 * reference */
	g_object_unref (G_OBJECT (store));

	/* Create a cell render and arbitrarily make it red for demonstration
	 * purposes */
	renderer = gtk_cell_renderer_text_new ();
	g_object_set (G_OBJECT (renderer),
			"background", "grey",
			NULL);

	/* Create a column, associating the "text" attribute of the
	 * cell_renderer to the first column of the model */
	column = gtk_tree_view_column_new_with_attributes ("id", renderer,
			"text", ID_COLUMN,
			NULL);

	/* Add the column to the view. */
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);
	column = gtk_tree_view_column_new_with_attributes ("time", renderer,
			"text", TIME_COLUMN,
			NULL);

	/* Add the column to the view. */
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);
	column = gtk_tree_view_column_new_with_attributes ("source", renderer,
			"text", SRC_COLUMN,
			NULL);

	/* Add the column to the view. */
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

	/* Second column.. title of the book. */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("destination",
			renderer,
			"text", DEST_COLUMN,
			NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

	/* Last column.. whether a book is checked out. */
	//renderer = gtk_cell_renderer_toggle_new ();
	column = gtk_tree_view_column_new_with_attributes ("protocol",
			renderer,
			"text", PROTOCOL_COLUMN,
			NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);
	//renderer = gtk_cell_renderer_toggle_new ();
	column = gtk_tree_view_column_new_with_attributes ("data",
			renderer,
			"text", DATA_COLUMN,
			NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

	/* Now we can manipulate the view just like any other GTK widget */
	gtk_container_add(window, tree);
}

	void
send_open_udp_packet()
{
	memcpy(mesg, "command: open", 13);
	send_udp_packet();
	gtk_status_icon_set_from_file (tray_icon, "door_open.png");
}

void send_button_handler() {
	memcpy(mesg, "test message", 12);
	send_udp_packet();
}

	static void
activate (GtkApplication* app,
		gpointer        user_data)
{
	GtkWidget *tree_window;
	GtkWidget *box;
	GtkWidget *label;
	GtkWidget *send_button;
	GtkWidget *open_button;

	GtkWidget *toolbar;

	GtkToolItem *toolbar_send_button;


	window = gtk_application_window_new (app);
	/*label = gtk_label_new ("Hello GNOME!");
	  gtk_container_add (GTK_CONTAINER (window), label);
	  */
	gtk_window_set_title (GTK_WINDOW (window), "Welcome to GNOME");
	gtk_window_set_default_size (GTK_WINDOW (window), 200, 100);
	gtk_widget_show_all (window);
	gtk_container_set_border_width(GTK_CONTAINER(window), 2);

	box = gtk_box_new(TRUE, 1);
	gtk_box_set_homogeneous((GtkBox *) box, 0);
	gtk_container_add(GTK_CONTAINER(window), box);

	/* Here we begin to create the toolbar */
	toolbar = gtk_toolbar_new();

	send_button = gtk_button_new_with_label("check");
	g_signal_connect (send_button, "clicked",
			G_CALLBACK (send_button_handler), window);

	gtk_box_pack_start(GTK_BOX(box), send_button, TRUE, TRUE, 1);
	open_button = gtk_button_new_with_label("open");
	g_signal_connect (open_button, "clicked",
			G_CALLBACK (send_open_udp_packet), window);

	gtk_box_pack_start(GTK_BOX(box), open_button, TRUE, TRUE, 1);

	//tree_window = gtk_scrolled_window_new(NULL, NULL);
	//gtk_box_pack_start(GTK_BOX(box), tree_window, TRUE, TRUE, 0);
	//gtk_widget_show (button);
	//setup_tree(tree_window);
 	gtk_widget_show_all(window);
}

	int
main (int argc, char **argv)
{

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(26100);
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_addr.s_addr = inet_addr("192.168.0.11");
	cliaddr.sin_port = htons(26100);

	sockfd=socket(PF_INET,SOCK_DGRAM,0);

	bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

	GtkApplication *app;
	int status;

	GtkStatusIcon *tray_icon;

	gtk_init(&argc, &argv);
	tray_icon = create_tray_icon();
	//       gtk_main();
	app = gtk_application_new (NULL, G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);

	return status;
}
