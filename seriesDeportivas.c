//gcc -o prueba  seriesDeportivas.c -Wall `pkg-config --cflags --libs gtk+-3.0` -export-dynamic -rdynamic
//Ecuación de Bellman: Tabla[i][j] = p*tabla[i-1][j]* q*tabla[i][i-j]
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>

//variables de gtk
GtkWidget* window;

GtkWidget* cantJuegos;
GtkWidget* GanarCasa;
GtkWidget* GanarVisita;
GtkWidget* siguiente;

GtkGrid* gameLocation;
GtkGrid* probabilidades;

//variables para manejar la vara
int games;
float probGanarCasa;
float probPerderCasa;

float probGanarVisita;
float probPerderVisita;
float mejorDe;


//matrices de valores and shit
gboolean button_clicked = FALSE;

const float *tablaProbabilidades[51][51]; //tabla en la que se van a trabajar los resultados de probabilidades

const int juegos[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25,
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
    46, 47, 48, 49, 50, 51
};

int juegoEnCasa[] = { //0 = falso, no es juego en casa de a, 1 = verdadero, juegan en casa de A
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0
};


void exit_app(){
    gtk_main_quit();
}

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
    g_object_unref(provider);
}

void actualizarLocalizacion(GtkToggleButton *checkbox, int juego) {
    gboolean active = gtk_toggle_button_get_active(checkbox);
    
    // Actualiza la matriz juegoEnCasa en la posición correspondiente
    if (active) {
        juegoEnCasa[juego] = 1;
    } else {
        juegoEnCasa[juego] = 0;
    }
}


//crea la tabla inicial para ingresar si los juegos seran en casa o no
void createTable1(){
    // Crear títulos para la primera fila
    GtkWidget *labelTitulo1 = gtk_label_new("N. Juego");
    GtkWidget *labelTitulo2 = gtk_label_new("En Casa (A)");

    // Agregar títulos a la primera fila y centrarlos
    gtk_widget_set_name(labelTitulo1, "neutro");
    gtk_widget_set_name(labelTitulo2, "neutro");
    gtk_widget_set_halign(labelTitulo1, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(labelTitulo2, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(gameLocation), labelTitulo1, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(gameLocation), labelTitulo2, 1, 0, 1, 1);


    for (int i = 0; i < games; i++) {
        // Etiqueta con el número en la primera columna
        GtkWidget *labelNumero = gtk_label_new(g_strdup_printf("%d", i+1));
        gtk_widget_set_name(labelNumero, "neutro");
        gtk_grid_attach(GTK_GRID(gameLocation), labelNumero, 0, i + 1, 1, 1);
        gtk_widget_set_halign(labelNumero, GTK_ALIGN_CENTER);

        // Checkbox en la segunda columna y centrado
        GtkWidget *checkbox = gtk_check_button_new();
        gtk_grid_attach(GTK_GRID(gameLocation), checkbox, 1, i + 1, 1, 1);
        gtk_widget_set_halign(checkbox, GTK_ALIGN_CENTER);

        g_signal_connect(checkbox, "toggled", G_CALLBACK(actualizarLocalizacion), GINT_TO_POINTER(i));        
    }
}

void createTable2(){
    mejorDe = games/2 + 1;
    //crear etiquetas para las filas y las columnas
    GtkWidget *labelTitulo1 = gtk_label_new(" P.A "); //probabilidad de que gane a en x momento
    gtk_widget_set_name(labelTitulo1, "neutro");
    gtk_grid_attach(GTK_GRID(probabilidades), labelTitulo1, 0, 0, 1, 1);


    for(int i = 0; i < mejorDe+1; i++){
        GtkWidget *labelNumeroA = gtk_label_new(g_strdup_printf(" %d ", i));
        GtkWidget *labelNumeroB = gtk_label_new(g_strdup_printf(" %d ", i));
        gtk_widget_set_name(labelNumeroA, "neutro");
        gtk_widget_set_name(labelNumeroB, "neutro");
        gtk_grid_attach(GTK_GRID(probabilidades), labelNumeroA, 0, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(probabilidades), labelNumeroB, i+1, 0, 1, 1);
        gtk_widget_set_halign(labelNumeroA, GTK_ALIGN_CENTER);
        gtk_widget_set_halign(labelNumeroB, GTK_ALIGN_CENTER);
    }

    for (int i = 1; i <= mejorDe+1; i++) {
        for (int j = 1; j <= mejorDe+1; j++) {
            // Crear una etiqueta con el texto "n.c" y agregarla a la celda correspondiente
            if(i == 1 && j ==1){
                GtkWidget *temp = gtk_label_new(" G.O "); //TERMINO EL JUEGO
                gtk_widget_set_name(temp, "neutro");
                gtk_grid_attach(GTK_GRID(probabilidades), temp, i, j, 1, 1);
                gtk_widget_set_halign(temp, GTK_ALIGN_CENTER);
            } else if(i == 1){
                GtkWidget *temp = gtk_label_new(g_strdup_printf(" %d ", i-1)); //TERMINO EL JUEGO
                gtk_widget_set_name(temp, "neutro");
                gtk_grid_attach(GTK_GRID(probabilidades), temp, i, j, 1, 1);
                gtk_widget_set_halign(temp, GTK_ALIGN_CENTER);
            } else if( j == 1){
                GtkWidget *temp = gtk_label_new(g_strdup_printf(" %d ", 1)); //TERMINO EL JUEGO
                gtk_widget_set_name(temp, "neutro");
                gtk_grid_attach(GTK_GRID(probabilidades), temp, i, j, 1, 1);
                gtk_widget_set_halign(temp, GTK_ALIGN_CENTER);
            } else{
                GtkWidget *temp = gtk_label_new(" n.c ");
                gtk_widget_set_name(temp, "neutro");
                gtk_grid_attach(GTK_GRID(probabilidades), temp, i, j, 1, 1);
                gtk_widget_set_halign(temp, GTK_ALIGN_CENTER);
            }
        }
    }
}

void cleanTable1(){
	for (int i = 0; i < games + 1; i++) { //limpia la tabla que se va a ctualizando
		gtk_grid_remove_row(gameLocation, 0);
    }
}
void cleanTable2(){ //limpia la tabla del display de la respuesta final
	for (int i = 0; i < games + 2; i++) {
		gtk_grid_remove_row(probabilidades, 0);
    }
}

void actualizarTablaRespuesta() {
    // Implementa la lógica para trabajar con checkbox e índices aquí
    for (int i = 1; i < mejorDe+1; i++) {
        for(int j = 1; j < mejorDe+1; j++){
            char *endptr;
            char *endptr1;
        // Accede a la variable juegoEnCasa para saber si el juego i es en casa o no
        GtkWidget *widgetArriba = gtk_grid_get_child_at(probabilidades, i, j-1);
        GtkLabel *labelArriba = GTK_LABEL(widgetArriba);
        const gchar *textoLabelArriba = gtk_label_get_text(labelArriba);    
        double arriba = strtod(textoLabelArriba, endptr); 

        GtkWidget *widgetIzquierda = gtk_grid_get_child_at(probabilidades, i-1, j);
        GtkLabel *labelIzquierda = GTK_LABEL(widgetIzquierda);
        const gchar *textoLabelIzquierda = gtk_label_get_text(labelIzquierda); 
        double izquierda = strtod(textoLabelIzquierda, endptr1); 

        int esEnCasa = juegoEnCasa[i];
        float probabilidadGanar;


        if(esEnCasa == 1){
            //el juego es en casa   
            probabilidadGanar = probGanarCasa*arriba + probPerderCasa*izquierda;

        } else{
            //el juego es de visita y se usan las probabilidades de visita
            probabilidadGanar = probGanarVisita*arriba + probPerderVisita*izquierda;
        }

        g_print("Juego %d es en casa: %d\n", i + 1, esEnCasa);
        }
    }
}

gboolean actualizarCantJuego( GtkWidget *widget, GdkEventButton *event, gpointer object )
{
    button_clicked = TRUE;
	cleanTable1();
    cleanTable2();
    games = gtk_spin_button_get_value_as_int((GtkSpinButton*) cantJuegos);
    g_print("cantJuegos: %d\n", games);
	createTable1();
	createTable2();
    gtk_widget_show_all(window);

    return FALSE;
}

gboolean actualizarGanarCasa (GtkWidget *widget, GdkEventButton *event, gpointer object )
{
    button_clicked = TRUE;
    probGanarCasa = gtk_spin_button_get_value((GtkSpinButton*) GanarCasa);
    probPerderCasa = 1 - probGanarCasa;
    
    // Imprime los valores en pantalla
    g_print("probGanarVisita: %f\n", probGanarCasa);
    g_print("probPerderVisita: %f\n", probPerderCasa);
    
    gtk_widget_show_all(window);

    return FALSE;
}

gboolean actualizarGanarVisita (GtkWidget *widget, GdkEventButton *event, gpointer object )
{
    button_clicked = TRUE;
    probGanarVisita = gtk_spin_button_get_value((GtkSpinButton*) GanarVisita);
    probPerderVisita = 1 - probGanarVisita;

    // Imprime los valores en pantalla
    g_print("probGanarVisita: %f\n", probGanarVisita);
    g_print("probPerderVisita: %f\n", probPerderVisita);

    gtk_widget_show_all(window);

    return FALSE;
}


int main(int argc, char *argv[]){
    GtkBuilder *builder; //GTK builder
    gtk_init(&argc, &argv); //start gtk

    builder = gtk_builder_new(); //create gtk ui builder
    gtk_builder_add_from_file(builder, "seriesDeportivas.glade", NULL); //LOAD UI FILE
    
    load_css();
    //ventana
    window = GTK_WIDGET(gtk_builder_get_object(builder, "MyWindow")); //load window named MyWindow

    //botones
    cantJuegos = GTK_WIDGET(gtk_builder_get_object(builder, "cant_juegos")); 
	GanarCasa = GTK_WIDGET(gtk_builder_get_object(builder, "home"));
    GanarVisita = GTK_WIDGET(gtk_builder_get_object(builder, "visit"));
    siguiente = GTK_WIDGET(gtk_builder_get_object(builder, "next_button"));

    

    //tablas
    gameLocation = GTK_GRID(gtk_builder_get_object(builder, "games_location"));
    probabilidades = GTK_GRID(gtk_builder_get_object(builder, "answer_table"));
    
    //ASIGN VARIABLES
    games = 3;
    probGanarCasa = 0;
    probGanarVisita = 0;

    //connect signals
    g_signal_connect( cantJuegos, "activate", G_CALLBACK(actualizarCantJuego), NULL );
    g_signal_connect( GanarCasa, "activate", G_CALLBACK(actualizarGanarCasa), NULL );
    g_signal_connect( GanarVisita, "activate", G_CALLBACK(actualizarGanarVisita), NULL );
    g_signal_connect( siguiente, "clicked", G_CALLBACK(actualizarTablaRespuesta), NULL );

    gtk_builder_connect_signals(builder, NULL);
    

    g_object_unref(builder);



    gtk_widget_show_all(window); //show window
    gtk_main(); //run

    return 0;
}


