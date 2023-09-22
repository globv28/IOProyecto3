//Main menu
//by Gloriana y Viviana

//gcc -o menu menuController.c -Wall `pkg-config --cflags --libs gtk+-3.0` -export-dynamic -rdynamic
// ./menu

//Libraries

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX_PROCESSES 100

pid_t process_pids[MAX_PROCESSES];
int num_processes = 0;


//variables
GtkWidget *window;
GtkWidget *floydButton;
GtkWidget *tbdButton1;
GtkWidget *tbdButton2;
GtkWidget *tbdButton3;
GtkWidget *tbdButton4;
GtkWidget *exit_button;
GList * windows = NULL;

static void load_css(void){
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen * screen;
    
    const gchar *css_style_file = "styles.css";
    
    GFile *css_fp = g_file_new_for_path(css_style_file);
    
    GError * error = 0;
    
    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    
    gtk_style_context_add_provider_for_screen(screen,GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_file(provider,css_fp,&error);
    printf("entra");
    g_object_unref(provider);
}

//Main
int main(int argc, char *argv[]){
    GtkBuilder *builder; //GTK builder
    gtk_init(&argc, &argv); //start gtk

    builder = gtk_builder_new(); //create gtk ui builder
    gtk_builder_add_from_file(builder, "Menu_UI.glade", NULL); //LOAD UI FILE

    load_css();

    

    //ASIGN VARIABLES
    window = GTK_WIDGET(gtk_builder_get_object(builder, "MyWindow")); //load window named MyWindow
    //window = gtk_window_new(GTK_WINDOW_TOPLEVEL);


    floydButton = GTK_WIDGET(gtk_builder_get_object(builder, "floyd_button"));
    gtk_widget_set_name(floydButton, "menuButton");
    tbdButton1 = GTK_WIDGET(gtk_builder_get_object(builder, "tbd_button1"));
    gtk_widget_set_name(tbdButton1, "menuButton");
    tbdButton2 = GTK_WIDGET(gtk_builder_get_object(builder, "tbd_button2"));
    gtk_widget_set_name(tbdButton2, "menuButton");
    tbdButton3 = GTK_WIDGET(gtk_builder_get_object(builder, "tbd_button3"));
    gtk_widget_set_name(tbdButton3, "menuButton");
    tbdButton4 = GTK_WIDGET(gtk_builder_get_object(builder, "tbd_button4"));
    gtk_widget_set_name(tbdButton4, "menuButton");
    exit_button = GTK_WIDGET(gtk_builder_get_object(builder, "exit_button"));
    gtk_widget_set_name(exit_button, "menuButton");

    

    //gtk essentials
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    gtk_widget_show_all(window); //show window
    gtk_main(); //run

    return 0;
}

void exit_app(){
    gtk_main_quit();
    //exit(0);
}

char* concat(char* s1, char* s2){
    char *result = malloc(strlen(s1) + strlen(s2)+1);
    strcpy(result,s1);
    strcat(result,s2);
    return result;

}

void tbd_button_clicked(){
    popen("./pending","r");
}

void knapsack_button_clicked(){
    popen("./mochila","r");
}

void series_deportivas_clicked(){
    popen("./seriesDeportivas","r");
}

void floyd_button_clicked(){
	popen("./rutasMasCortas","r");
    //printf("Algoritmo de las rutas más cortas ejecutandose");
}

/*char integerstr[32];
    sprintf(integerstr,"%d",instancias);
    const char* comp1 = "gcc -o pending";
    const char* comp3 = "gcc -o pending pendingController.c -Wall `pkg-config --cflags --libs gtk+-3.0` -export-dynamic -rdynamic";
    const char* comp2 = concat(comp1, integerstr);
    char* full = concat(comp2, comp3);
    char* name = concat("./pending", integerstr);
    //strcpy(full,comp1);
    //strcat(comp1, numero);
    //strcat(full, comp2);
    //free(full);
    printf(full);
    system(full);
    //char* name = strcat("./pending", numero);
    system(name);
    instancias++;
    
    //printf("Archivo pending ejecutandose");*/
