/**
 * @file main.c
 * @author popovili@fit.cvut.cz
 * @brief  demo application for YouBionic Hand controll
 */

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include "./lib/Hand.h"

///Structure responsible for storing applicatin widgets pointers and variables
typedef struct {

	GtkWidget *g_menu_window;
	GtkWidget *g_glove_window;	
	GtkWidget *g_send_states_window;	
	GtkWidget *g_stand_window;	


    GtkWidget *g_ent_glove_win_txtIn;    	
	GtkWidget *g_lbl_glove_win_conn_state_glove;
	GtkWidget *g_lbl_glove_win_conn_state_hand;
	GtkWidget *g_lbl_glove_win_f[5];
	GtkWidget *g_lbl_glove_win_hand_data;
	GtkWidget *g_lbl_send_win_conn_state_hand;
	GtkWidget *g_lbl_stand_win_conn_state_hand;
	GtkWidget *g_lbl_stand_win_hand_data;
	GtkWidget *g_lbl_send_win_hand_data;

	GtkWidget *g_ent_send_win_f[6];
	GtkWidget *g_ent_send_win_textIn;
	GtkWidget *g_ent_stand_win_textIn;
	GtkWidget *g_ent_glove_win_thumbRot;



	SArdDev hand;
	SArdDev glove;
	SSerial term;

	bool hand_status;
	bool glove_status;	
	bool reading_glove_data;
	bool resending_data;
	bool wait_glove;
	bool chkbtn_states[6];
	bool chkbtn_glove;

} app_widgets;



bool is_comma (char c) {

	if (c == ',') 
		return true;

	return false;

}


// Glove control screen
//================================================================

/**
* @brief Display fingers positions from glove 
*/
void change_glove_screen_labels (app_widgets *app_wdgts, char *buff, int *states) {

	char tmp[5] = "";
	int i = 2;
	int j,cnt_lbl;

	const gchar *d_string;

			d_string = gtk_entry_get_text(GTK_ENTRY(app_wdgts->g_ent_glove_win_thumbRot));

			if ((buff[0] == 'S') && (buff[1] == ':')) {
				cnt_lbl = 0;
				while (buff[i] != ':') {
					j = 0;
					while (!is_comma(buff[i])) {
						tmp[j] = buff[i];
						j++;
						i++;
					}
					i++;
					gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_glove_win_f[cnt_lbl]), tmp);	
					states[cnt_lbl] = atoi( tmp );

					if (states[cnt_lbl] < 0) 
						states[cnt_lbl] = 0;

					if (states[cnt_lbl] > 250) 
						states[cnt_lbl] = 250;

					memset(tmp, 0, 5);
					cnt_lbl++;
					if (cnt_lbl == 5)
						break;
						}
					}

			if (app_wdgts->chkbtn_glove)
				states[5] = atoi( d_string );
			else
				states[5] = -1;


			for (int l =0; l<6;l++)
				printf("%d,", states[l]);

			printf("\n");

}

/**
* @brief Resends data from glove controller to robotic hand 
*/
static gboolean resend_data_from_glove (app_widgets *app_wdgts) {

	char buff[30] = {""};

	int states[6];

	if (app_wdgts->resending_data) {

		if(read_glove_position(app_wdgts->glove, buff, 30, app_wdgts->wait_glove))  {

			app_wdgts->wait_glove = false;
			change_glove_screen_labels(app_wdgts, buff, states);
			send_states_to_hand (&app_wdgts->hand, states);
			
		}
		else 
			app_wdgts->wait_glove = true;
		
		return true;
}
return false;

}


/**
* @brief Read fingers positions from glove
*/
static gboolean read_and_display_data_from_glove (app_widgets *app_wdgts) {
	char buff[30] = {""};
	int states[6];
	
	if (app_wdgts->reading_glove_data) {

		if(read_glove_position(app_wdgts->glove, buff, 30, app_wdgts->wait_glove))  {

			app_wdgts->wait_glove = false;
			change_glove_screen_labels (app_wdgts, buff, states);

		}
		else 
			app_wdgts->wait_glove = true;

		return true;

	}
	return false;
}


/**
* @brief Read servo positions from hand
*/
static gboolean read_hand_state (app_widgets *app_wdgts) {

	char buff[30] = {""};

	if (app_wdgts->hand_status) {
		if (read_from_device (app_wdgts->hand, buff, 30)) {

			gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_glove_win_hand_data), buff);	
			gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_stand_win_hand_data), buff);	
			gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_send_win_hand_data), buff);	

			}
		return true; 
	}
	return false;
}


/**
* @brief Controll checkbox 
*/
void on_chkbtn_glove_screen_toggled (GtkToggleButton *togglebutton, app_widgets *app_wdgts) {

	if (gtk_toggle_button_get_active(togglebutton)) {
		app_wdgts->chkbtn_glove = true;
	}
	else {
		app_wdgts->chkbtn_glove = false;
	}
}

/**
* @brief Starts/stops reading data from glove after cliking on button 
*/
void on_btn_glove_win_read_glove_clicked (GtkButton *button, app_widgets *app_wdgts) {
	if (app_wdgts->glove_status) {

		if (app_wdgts->reading_glove_data) {
			app_wdgts->reading_glove_data = false; 
		return;
	
		}
		else {
			app_wdgts->reading_glove_data = true; 
			g_timeout_add_full (G_PRIORITY_HIGH, 200,(GSourceFunc)read_and_display_data_from_glove, (gpointer*) app_wdgts, NULL);
		return;

		}
	}
}


/**
* @brief Glove device connection
*/
void on_btn_glove_win_conn_glove_clicked (GtkButton *button, app_widgets *app_wdgts) {

	const gchar *d_string;
	bool op_res;

	d_string = gtk_entry_get_text(GTK_ENTRY(app_wdgts->g_ent_glove_win_txtIn));


	op_res = open_device ( &app_wdgts->glove, d_string);

	if (op_res) { 

	gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_glove_win_conn_state_glove), "READY");

	set_io_speed (app_wdgts->glove.dev_addr, &app_wdgts->term);
	
	app_wdgts->glove_status = true;

	}

	else 
		gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_glove_win_conn_state_glove), "FAIL");
}

/**
* @brief Robotic hand connection
*/
void on_btn_glove_win_conn_hand_clicked (GtkButton *button, app_widgets *app_wdgts) {

	const gchar *d_string;
	bool op_res;

	d_string = gtk_entry_get_text(GTK_ENTRY(app_wdgts->g_ent_glove_win_txtIn));
	op_res = open_device ( &app_wdgts->hand, d_string);

	

	if (op_res) { 

	gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_glove_win_conn_state_hand), "READY");
	gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_send_win_conn_state_hand), "READY");
	gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_stand_win_conn_state_hand), "READY");

	if (!app_wdgts->hand_status)
		g_timeout_add_full (G_PRIORITY_HIGH, 200,(GSourceFunc)read_hand_state, (gpointer*) app_wdgts, NULL);

	set_io_speed (app_wdgts->hand.dev_addr, &app_wdgts->term);
	
	app_wdgts->hand_status = true;

	}

	else  {
		gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_glove_win_conn_state_hand), "FAIL");
		gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_send_win_conn_state_hand), "FAIL");
		gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_stand_win_conn_state_hand), "FAIL");
		app_wdgts->hand_status = false;
	}
}


/**
* @brief Starts/stops resending data from glove to hand after cliking on button 
*/
void on_btn_glove_win_start_resend_data_clicked (GtkButton *button, app_widgets *app_wdgts) {

 if (app_wdgts->glove_status && app_wdgts->hand_status) {
		if (app_wdgts->resending_data) {
			app_wdgts->resending_data = false; 
		return;
		}
		else {
			app_wdgts->resending_data = true; 
			app_wdgts->reading_glove_data = false; 
			g_timeout_add_full (G_PRIORITY_HIGH, 200,(GSourceFunc)resend_data_from_glove, (gpointer*) app_wdgts, NULL);
		return;
		}
	}
	return;
}


/**
* @brief Back to menu button
*/
void on_btn_glove_win_menu_clicked (GtkButton *button, app_widgets *app_wdgts) {

 	gtk_widget_hide(app_wdgts->g_glove_window);
    gtk_widget_show_all (app_wdgts->g_menu_window);    

}




// Send servo positions screen
//================================================================


void chkbtn_use (GtkToggleButton *togglebutton, app_widgets *app_wdgts, int id) {

	if (gtk_toggle_button_get_active(togglebutton)) {
		app_wdgts->chkbtn_states[id] = true;
	}
	else {
		app_wdgts->chkbtn_states[id] = false;
	}
}

/**
* @brief Controll checkbox 
*/
void on_chkbtn_states_screen_1_toggled (GtkToggleButton *togglebutton, app_widgets *app_wdgts) {

 chkbtn_use (togglebutton, app_wdgts, 0);

}

void on_chkbtn_states_screen_2_toggled (GtkToggleButton *togglebutton, app_widgets *app_wdgts) {

 chkbtn_use (togglebutton, app_wdgts, 1);

}

void on_chkbtn_states_screen_3_toggled (GtkToggleButton *togglebutton, app_widgets *app_wdgts) {

 chkbtn_use (togglebutton, app_wdgts, 2);

}

void on_chkbtn_states_screen_4_toggled (GtkToggleButton *togglebutton, app_widgets *app_wdgts) {

 chkbtn_use (togglebutton, app_wdgts, 3);

}

void on_chkbtn_states_screen_5_toggled (GtkToggleButton *togglebutton, app_widgets *app_wdgts) {

 chkbtn_use (togglebutton, app_wdgts, 4);

}

void on_chkbtn_states_screen_6_toggled (GtkToggleButton *togglebutton, app_widgets *app_wdgts) {

 chkbtn_use (togglebutton, app_wdgts, 5);

}

/**
* @brief Robotic hand connection
*/
void on_btn_send_win_connect_hand_clicked (GtkButton *button, app_widgets *app_wdgts) {

	const gchar *d_string;
	bool op_res;

	d_string = gtk_entry_get_text(GTK_ENTRY(app_wdgts->g_ent_send_win_textIn));
	op_res = open_device ( &app_wdgts->hand, d_string);

	if (op_res) { 

	gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_glove_win_conn_state_hand), "READY");
	gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_send_win_conn_state_hand), "READY");
	gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_stand_win_conn_state_hand), "READY");

	if (!app_wdgts->hand_status)
		g_timeout_add_full (G_PRIORITY_HIGH, 200,(GSourceFunc)read_hand_state, (gpointer*) app_wdgts, NULL);

	set_io_speed (app_wdgts->hand.dev_addr, &app_wdgts->term);
	
	app_wdgts->hand_status = true;

	}

	else  {
		gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_glove_win_conn_state_hand), "FAIL");
		gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_send_win_conn_state_hand), "FAIL");
		gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_stand_win_conn_state_hand), "FAIL");
		app_wdgts->hand_status = false;
	}

}

/**
* @brief Sends target servo positions to hand
*/
void on_btn_send_win_send_to_hand_clicked (GtkButton *button, app_widgets *app_wdgts) { 

	const gchar *d_string;
	int states[6];
		

	for (int i = 0; i < 6; i++) {

		d_string = gtk_entry_get_text(GTK_ENTRY(app_wdgts->g_ent_send_win_f[i]));
		states[i] = atoi( d_string );

		if(!app_wdgts->chkbtn_states[i])
			states[i] = -1;

	}

	if (app_wdgts->hand_status) {

		send_states_to_hand (&app_wdgts->hand,states);

	}

} 

/**
* @brief Back to menu button
*/
void on_btn_states_win_menu_clicked (GtkButton *button, app_widgets *app_wdgts) {

 	gtk_widget_hide(app_wdgts->g_send_states_window);
    gtk_widget_show_all (app_wdgts->g_menu_window);  

}



// Use standart movements  screen
//================================================================

/**
* @brief Back to menu button
*/
void on_btn_stand_win_menu_clicked (GtkButton *button, app_widgets *app_wdgts) {

 	gtk_widget_hide(app_wdgts->g_stand_window);
    gtk_widget_show_all (app_wdgts->g_menu_window);  

}

/**
* @brief Robotic hand connection
*/
void on_btn_stand_win_connect_hand_clicked (GtkButton *button, app_widgets *app_wdgts) {

	const gchar *d_string;
	bool op_res;

	d_string = gtk_entry_get_text(GTK_ENTRY(app_wdgts->g_ent_stand_win_textIn));
	op_res = open_device ( &app_wdgts->hand, d_string);

	if (op_res) { 

	gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_glove_win_conn_state_hand), "READY");
	gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_send_win_conn_state_hand), "READY");
	gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_stand_win_conn_state_hand), "READY");

	if (!app_wdgts->hand_status)
		g_timeout_add_full (G_PRIORITY_HIGH, 200,(GSourceFunc)read_hand_state, (gpointer*) app_wdgts, NULL);

	set_io_speed (app_wdgts->hand.dev_addr, &app_wdgts->term);
	
	app_wdgts->hand_status = true;


	}

	else  {
		gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_glove_win_conn_state_hand), "FAIL");
		gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_send_win_conn_state_hand), "FAIL");
		gtk_label_set_text(GTK_LABEL(app_wdgts->g_lbl_stand_win_conn_state_hand), "FAIL");
		app_wdgts->hand_status = false;
	}
}

/**
* @brief Sends command to hand (Open hand)
*/
void on_btn_stand_win_open_hand_clicked (GtkButton *button, app_widgets *app_wdgts) {

	if (app_wdgts->hand_status)
		open_hand (&app_wdgts->hand);

}

/**
* @brief Sends command to hand (Close hand)
*/
void on_btn_stand_win_close_hand_clicked (GtkButton *button, app_widgets *app_wdgts) {

	if (app_wdgts->hand_status)
		close_hand (&app_wdgts->hand);

}

/**
* @brief Sends command to hand (Palm grop v1)
*/
void on_btn_stand_win_grip_1_clicked (GtkButton *button, app_widgets *app_wdgts) {

int states[6] = {240,240,240,240,240,200};

	send_states_to_hand (&app_wdgts->hand, states);

}

/**
* @brief Sends command to hand (Palm grop v2)
*/
void on_btn_stand_win_grip_2_clicked (GtkButton *button, app_widgets *app_wdgts) {

int states[6] = {240,240,240,240,240,0};

	send_states_to_hand (&app_wdgts->hand, states);

}

/**
* @brief Sends command to hand (Finger grip)
*/
void on_btn_stand_win_grip_3_clicked (GtkButton *button, app_widgets *app_wdgts) {

int states[6] = {240,0,0,0,240,0};

	send_states_to_hand (&app_wdgts->hand, states);

}

/**
* @brief Sends command to hand (Central grip)
*/
void on_btn_stand_win_grip_4_clicked (GtkButton *button, app_widgets *app_wdgts) {

int states[6] = {140,100,50,40,140,100};

	send_states_to_hand (&app_wdgts->hand, states);

}

/**
* @brief Sends command to hand (Fist)
*/
void on_btn_stand_win_fist_clicked (GtkButton *button, app_widgets *app_wdgts) {

int states[6] = {0,0,0,0,0,170};

	send_states_to_hand (&app_wdgts->hand, states);

}

/**
* @brief Sends command to hand (Like)
*/
void on_btn_stand_win_like_clicked (GtkButton *button, app_widgets *app_wdgts) {

int states[6] = {0,0,0,0,220,150};

	send_states_to_hand (&app_wdgts->hand, states);

}

/**
* @brief Sends command to hand (Rock)
*/
void on_btn_stand_win_rock_clicked (GtkButton *button, app_widgets *app_wdgts) {

int states[6] = {220,0,0,220,220,130};

	send_states_to_hand (&app_wdgts->hand, states);

}

/**
* @brief Sends command to hand (Point)
*/
void on_btn_stand_win_point_clicked (GtkButton *button, app_widgets *app_wdgts) {

int states[6] = {240,0,0,0,0,200};

	send_states_to_hand (&app_wdgts->hand, states);

}

/**
* @brief Sends command to hand (OK)
*/
void on_btn_stand_win_ok_clicked (GtkButton *button, app_widgets *app_wdgts) {

int states[6] = {100,180,200,240,110,210};

	send_states_to_hand (&app_wdgts->hand, states);

}


// Menu window
//================================================================


/**
* @brief To "Use standart movements"  screen
*/
void on_btn_stand_move_clicked (GtkButton *button, app_widgets *app_wdgts) {

 	gtk_widget_hide(app_wdgts->g_menu_window);
    gtk_widget_show_all (app_wdgts->g_stand_window);  

}

/**
* @brief To "Send servo positions"  screen
*/
void on_btn_send_states_clicked (GtkButton *button, app_widgets *app_wdgts) {

 	gtk_widget_hide(app_wdgts->g_menu_window);
    gtk_widget_show_all (app_wdgts->g_send_states_window);      

}

/**
* @brief To "Glove control"  screen
*/
void on_btn_glove_contr_clicked (GtkButton *button, app_widgets *app_wdgts) {


 	gtk_widget_hide(app_wdgts->g_menu_window);
    gtk_widget_show_all (app_wdgts->g_glove_window);          

}

/**
* @brief Close app
*/
void on_window_menu_destroy(GtkWidget *widget, GdkEvent *event, gpointer data) {
    gtk_main_quit();

}


// Main app
//================================================================

void create_all_windows (int argc, char *argv[]) {


	GtkBuilder      *builder; 

    app_widgets     *widgets = g_slice_new(app_widgets);



	gtk_init(&argc, &argv);    

	init_serial(&widgets->term);

// Init all widgets
//================================================================

 	builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/window_menu.glade", NULL);
    gtk_builder_add_from_file (builder, "glade/window_glove.glade", NULL);
    gtk_builder_add_from_file (builder, "glade/window_send_states.glade", NULL);
    gtk_builder_add_from_file (builder, "glade/window_standart.glade", NULL);

    widgets->g_menu_window = GTK_WIDGET(gtk_builder_get_object(builder, "window_menu"));
	widgets->g_glove_window = GTK_WIDGET(gtk_builder_get_object(builder, "window_glove"));
	widgets->g_send_states_window = GTK_WIDGET(gtk_builder_get_object(builder, "window_send_states"));
	widgets->g_stand_window = GTK_WIDGET(gtk_builder_get_object(builder, "window_standart"));

	widgets->g_ent_glove_win_txtIn = GTK_WIDGET(gtk_builder_get_object(builder, "ent_glove_win_txtIn"));
	widgets->g_ent_send_win_textIn = GTK_WIDGET(gtk_builder_get_object(builder, "ent_send_win_textIn"));
	widgets->g_ent_stand_win_textIn = GTK_WIDGET(gtk_builder_get_object(builder, "ent_stand_win_textIn"));
	widgets->g_ent_glove_win_thumbRot = GTK_WIDGET(gtk_builder_get_object(builder, "ent_glove_win_thumbRot"));

	widgets->g_lbl_glove_win_conn_state_glove = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_glove_win_conn_state_glove"));
	widgets->g_lbl_glove_win_conn_state_hand = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_glove_win_conn_state_hand"));
	widgets->g_lbl_glove_win_f[0] = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_glove_win_f1"));
	widgets->g_lbl_glove_win_f[1] = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_glove_win_f2"));
	widgets->g_lbl_glove_win_f[2] = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_glove_win_f3"));
	widgets->g_lbl_glove_win_f[3] = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_glove_win_f4"));
	widgets->g_lbl_glove_win_f[4] = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_glove_win_f5"));
	widgets->g_lbl_glove_win_hand_data = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_glove_win_hand_data"));
	widgets->g_lbl_send_win_conn_state_hand = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_send_win_conn_state_hand"));
	widgets->g_lbl_stand_win_conn_state_hand = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_stand_win_conn_state_hand"));
	widgets->g_lbl_stand_win_hand_data = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_stand_win_hand_data"));
	widgets->g_lbl_send_win_hand_data = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_send_win_hand_data"));



	widgets->g_ent_send_win_f[0] = GTK_WIDGET(gtk_builder_get_object(builder, "ent_send_win_f1"));
	widgets->g_ent_send_win_f[1] = GTK_WIDGET(gtk_builder_get_object(builder, "ent_send_win_f2"));
	widgets->g_ent_send_win_f[2] = GTK_WIDGET(gtk_builder_get_object(builder, "ent_send_win_f3"));
	widgets->g_ent_send_win_f[3] = GTK_WIDGET(gtk_builder_get_object(builder, "ent_send_win_f4"));
	widgets->g_ent_send_win_f[4] = GTK_WIDGET(gtk_builder_get_object(builder, "ent_send_win_f5"));
	widgets->g_ent_send_win_f[5] = GTK_WIDGET(gtk_builder_get_object(builder, "ent_send_win_f6"));

// App starting states
//================================================================

	widgets->hand_status = false;
	widgets->glove_status = false;
	widgets->reading_glove_data = false;
	widgets->resending_data = false;
	widgets->wait_glove = false;
	widgets->chkbtn_glove = false;

	for (int i=0;i<6;i++)
	widgets->chkbtn_states[i] = true;

// Connect close button signals
//================================================================

    g_signal_connect(G_OBJECT(widgets->g_menu_window), "delete-event", G_CALLBACK(on_window_menu_destroy), NULL);

    g_signal_connect(G_OBJECT(widgets->g_glove_window), "delete-event", G_CALLBACK(on_window_menu_destroy), NULL);

    g_signal_connect(G_OBJECT(widgets->g_send_states_window), "delete-event", G_CALLBACK(on_window_menu_destroy), NULL);

    g_signal_connect(G_OBJECT(widgets->g_stand_window), "delete-event", G_CALLBACK(on_window_menu_destroy), NULL);

//================================================================

	gtk_builder_connect_signals(builder, widgets);

    g_object_unref(builder);

    gtk_widget_show_all (widgets->g_menu_window);  


// Run app 
//================================================================

	gtk_main();

// Close app 
//================================================================

	destr_serial (&widgets->term);
	g_slice_free(app_widgets, widgets);

}





int main (int argc, char *argv[]) {

    create_all_windows(argc, argv);

	return 0;
}

