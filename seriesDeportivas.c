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
GtkWidget* informacion;
GtkWidget* respuestaFinal;

GtkGrid* gameLocation;
GtkGrid* probabilidades;
GtkFileFilter *filter;
GtkWidget *FileChooserWindow;

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

        g_signal_connect(checkbox, "toggled", G_CALLBACK(actualizarLocalizacion), GINT_TO_POINTER(i+1));        
    }
}

void getLocationChecks(){
    for (int i = 1; i < games+1; i++)
    {
        GtkWidget *checkBox = gtk_grid_get_child_at(gameLocation, 1,i); 
        gboolean active = gtk_toggle_button_get_active(checkBox);
        if(active){
            juegoEnCasa[i] == 1;
        }else{
            juegoEnCasa[i] == 0;
        }
    }
    gtk_widget_show_all(window);
}

void setLocationChecks(){
    for (int i = 1; i < games+1; i++)
    {
        GtkWidget *checkBox = gtk_grid_get_child_at(gameLocation, 1,i);
        if(juegoEnCasa[i] == 1){
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkBox), TRUE);
        }else{
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkBox), FALSE);
        }
    }
    gtk_widget_show_all(window);
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
                gtk_widget_set_halign(temp, GTK_ALIGN_FILL);
            }
             else if(i == 1){
                GtkWidget *temp = gtk_label_new(g_strdup_printf(" %d ", i-1)); //TERMINO EL JUEGO
                gtk_widget_set_name(temp, "rojo");
                gtk_grid_attach(GTK_GRID(probabilidades), temp, i, j, 1, 1);
                gtk_widget_set_halign(temp, GTK_ALIGN_FILL);
            } else if( j == 1){
                GtkWidget *temp = gtk_label_new(g_strdup_printf(" %d ", 1)); //TERMINO EL JUEGO
                gtk_widget_set_name(temp, "azul");
                gtk_grid_attach(GTK_GRID(probabilidades), temp, i, j, 1, 1);
                gtk_widget_set_halign(temp, GTK_ALIGN_FILL);
            } else{
                GtkWidget *temp = gtk_label_new(" n.c ");
                gtk_widget_set_name(temp, "neutro");
                gtk_grid_attach(GTK_GRID(probabilidades), temp, i, j, 1, 1);
                gtk_widget_set_halign(temp, GTK_ALIGN_FILL);
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

float extraerNumero(const char *cadena) {
    float numero;
    if (sscanf(cadena, "%f VISITA", &numero) == 1) {
        return numero;
    } else if (sscanf(cadena, "%f CASA", &numero) == 1) {
        return numero;
    } else {
        // Manejar el caso en el que la cadena no tiene el formato correcto.
        // Por ejemplo, puedes devolver un valor especial o mostrar un mensaje de error.
        printf("Formato no válido: %s\n", cadena);
        exit(1);
    }
}

char *f_to_s(float numero) {
    char *buffer = malloc(20); 
    if (buffer == NULL) {
        perror("Error al asignar memoria para la cadena");
        exit(EXIT_FAILURE);
    }

    snprintf(buffer, 20, "%.2f", numero); // Formatear el número como una cadena

    return buffer;
}

char* concatenarTresCadenas(const char* str1, const char* str2, const char* str3) {
    size_t longitud_total = strlen(str1) + strlen(str2) + strlen(str3) + 1;
    char* resultado = (char*)malloc(longitud_total * sizeof(char));
    if (resultado == NULL) {
        perror("Error en la asignación de memoria");
        exit(EXIT_FAILURE);
    }

    strcpy(resultado, str1);
    strcat(resultado, str2);
    strcat(resultado, str3);

    return resultado;
}

void actualizarTablaRespuesta() {
    //Ecuación de Bellman: Tabla[i][j] = p*tabla[i-1][j]* q*tabla[i][i-j]
    // Implementa la lógica para trabajar con checkbox e índices aquí
        //connect signals
    actualizarCantJuego();
    actualizarGanarCasa();
    actualizarGanarVisita();
    getLocationChecks();
    for (int i = 1; i < mejorDe + 1; i++) {
        for(int j = 1; j < mejorDe + 1; j++){
        // Accede a la variable juegoEnCasa para saber si el juego i es en casa o no
        GtkWidget *widgetArriba = gtk_grid_get_child_at(probabilidades, j+1,i);
        GtkLabel *labelArriba = GTK_LABEL(widgetArriba);
        const gchar *textoLabelArriba = gtk_label_get_text(labelArriba);    

        GtkWidget *widgetIzquierda = gtk_grid_get_child_at(probabilidades, j,i+1);
        GtkLabel *labelIzquierda = GTK_LABEL(widgetIzquierda);
        const gchar *textoLabelIzquierda = gtk_label_get_text(labelIzquierda); 

        double arriba = strtod(textoLabelArriba, NULL);
        double izquierda = strtod(textoLabelIzquierda, NULL);

        int juegoNum = (mejorDe-i)+(mejorDe-j)+1;

        int esEnCasa = juegoEnCasa[juegoNum];
        float probabilidadGanar;
        if (esEnCasa == 1) {
                // El juego es en casa   
                probabilidadGanar = probGanarCasa * arriba + probPerderCasa * izquierda;
                
                // Create a new label with the updated probability and replace the existing label
                GtkWidget *temp = gtk_label_new(g_strdup_printf(" %f CASA", probabilidadGanar));
                if(probabilidadGanar == 1){
                    gtk_widget_set_name(temp, "azul");
                }else if(probabilidadGanar > 0.5){
                    gtk_widget_set_name(temp, "verde");
                }
                else if(probabilidadGanar == 0.5){
                    gtk_widget_set_name(temp, "amarillo");
                }
                else if(probabilidadGanar > 0){
                    gtk_widget_set_name(temp, "naranja");
                }
                else if(probabilidadGanar == 0){
                    gtk_widget_set_name(temp, "rojo");
                }

                // Remove the existing label from the grid
                GtkWidget *existingLabel = gtk_grid_get_child_at(probabilidades, j+1, i+1);
                gtk_container_remove(GTK_CONTAINER(probabilidades), existingLabel);
                
                gtk_grid_attach(GTK_GRID(probabilidades), temp, j+1, i+1, 1, 1);
                gtk_widget_set_halign(temp, GTK_ALIGN_FILL);
                gtk_widget_show(temp);  // Show the newly created label
            } else if (esEnCasa == 0) {
                // El juego es de visita y se usan las probabilidades de visita
                
                probabilidadGanar = probGanarVisita * arriba + probPerderVisita * izquierda;

                // Create a new label with the updated probability and replace the existing label
                GtkWidget *temp = gtk_label_new(g_strdup_printf(" %f VISITA", probabilidadGanar));
                if(games%2 == 0 && i == 1 && j == 1){
                    probabilidadGanar = 0.5;
                    temp = gtk_label_new(g_strdup_printf(" %f EMPATE", probabilidadGanar));
                }
                if(probabilidadGanar == 1){
                    gtk_widget_set_name(temp, "azul");
                }else if(probabilidadGanar > 0.5){
                    gtk_widget_set_name(temp, "verde");
                }
                else if(probabilidadGanar == 0.5){
                    gtk_widget_set_name(temp, "amarillo");
                }
                else if(probabilidadGanar > 0){
                    gtk_widget_set_name(temp, "naranja");
                }
                else if(probabilidadGanar == 0){
                    gtk_widget_set_name(temp, "rojo");
                }
                

                // Remove the existing label from the grid
                GtkWidget *existingLabel = gtk_grid_get_child_at(probabilidades, j+1, i+1);
                gtk_container_remove(GTK_CONTAINER(probabilidades), existingLabel);
                
                gtk_grid_attach(GTK_GRID(probabilidades), temp, j+1, i+1, 1, 1);
                gtk_widget_set_halign(temp, GTK_ALIGN_CENTER);
                gtk_widget_show(temp);  // Show the newly created label
            }
        //g_print("Juego %d es en casa: %d arriba: %f  izquierda %f i: %d j: %d\n", juegoNum, esEnCasa, arriba, izquierda, i+1, j);
        }
    }
    GtkWidget *widgetRespuesta = gtk_grid_get_child_at(probabilidades, games/2+2, games/2+2);
    GtkLabel *labelRespuesta = GTK_LABEL(widgetRespuesta);
    const gchar *textoLabelRespuesta = gtk_label_get_text(labelRespuesta);
    char* string = f_to_s(extraerNumero(textoLabelRespuesta)*100);
    string = concatenarTresCadenas("La probabilidad de que A gane es del ", string, "\%");
    gtk_label_set_text(respuestaFinal, string);
    setLocationChecks();
}



char *i_to_s(int numero) {
    char *cadena = malloc(12); // Suficiente espacio para un int (10 dígitos) y el carácter nulo
    if (cadena == NULL) {
        perror("Error al asignar memoria para la cadena");
        exit(EXIT_FAILURE);
    }

    snprintf(cadena, 12, "%d", numero); // Convertir el entero a una cadena

    return cadena;
}




gboolean actualizarCantJuego( GtkWidget *widget, GdkEventButton *event, gpointer object )
{
    button_clicked = TRUE;
	cleanTable1();
    cleanTable2();
    games = gtk_spin_button_get_value_as_int((GtkSpinButton*) cantJuegos);
    if(games % 2 == 0){
        games++;
    }
    //g_print("cantJuegos: %d\n", games);
    char* string = concatenarTresCadenas("Ganará el mejor de ", i_to_s(games), " juegos \n");
    string = concatenarTresCadenas(string, "El primero en ganar ", i_to_s(((int)games/2)+1));
    string = concatenarTresCadenas(string, " partidos, gana \n","p_h= ");
    string = concatenarTresCadenas(string, f_to_s(probGanarCasa), " (Probabilidad de A de ganar en casa)\n");
    string = concatenarTresCadenas(string, "p_v= ", f_to_s(probGanarVisita));
    string = concatenarTresCadenas(string, " (Probabilidad de A de ganar de visita)\n", "q_h=");
    string = concatenarTresCadenas(string, f_to_s(1-probGanarVisita), " (Probabilidad de A de ganar en casa)\np_v=");
    string = concatenarTresCadenas(string, f_to_s(1-probGanarCasa), "(Probabilidad de B de ganar de visita)");
    gtk_label_set_text((GtkLabel*)informacion, string);

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
    
    char* string = concatenarTresCadenas("Ganará el mejor de ", i_to_s(games), " juegos \n");
    string = concatenarTresCadenas(string, "El primero en ganar ", i_to_s(((int)games/2)+1));
    string = concatenarTresCadenas(string, " partidos, gana \n","p_h= ");
    string = concatenarTresCadenas(string, f_to_s(probGanarCasa), " (Probabilidad de A de ganar en casa)\n");
    string = concatenarTresCadenas(string, "p_v= ", f_to_s(probGanarVisita));
    string = concatenarTresCadenas(string, " (Probabilidad de A de ganar de visita)\n", "q_h=");
    string = concatenarTresCadenas(string, f_to_s(1-probGanarVisita), " (Probabilidad de A de ganar en casa)\np_v=");
    string = concatenarTresCadenas(string, f_to_s(1-probGanarCasa), "(Probabilidad de B de ganar de visita)");
    gtk_label_set_text((GtkLabel*)informacion, string);

    // Imprime los valores en pantalla
    //g_print("probGanarVisita: %f\n", probGanarCasa);
    //g_print("probPerderVisita: %f\n", probPerderCasa);
    
    gtk_widget_show_all(window);

    return FALSE;
}

gboolean actualizarGanarVisita (GtkWidget *widget, GdkEventButton *event, gpointer object )
{
    button_clicked = TRUE;
    probGanarVisita = gtk_spin_button_get_value((GtkSpinButton*) GanarVisita);
    probPerderVisita = 1 - probGanarVisita;
    char* string = concatenarTresCadenas("Ganará el mejor de ", i_to_s(games), " juegos \n");
    string = concatenarTresCadenas(string, "El primero en ganar ", i_to_s(((int)games/2)+1));
    string = concatenarTresCadenas(string, " partidos, gana \n","p_h= ");
    string = concatenarTresCadenas(string, f_to_s(probGanarCasa), " (Probabilidad de A de ganar en casa)\n");
    string = concatenarTresCadenas(string, "p_v= ", f_to_s(probGanarVisita));
    string = concatenarTresCadenas(string, " (Probabilidad de A de ganar de visita)\n", "q_h=");
    string = concatenarTresCadenas(string, f_to_s(1-probGanarVisita), " (Probabilidad de A de ganar en casa)\np_v=");
    string = concatenarTresCadenas(string, f_to_s(1-probGanarCasa), "(Probabilidad de B de ganar de visita)");
    gtk_label_set_text((GtkLabel*)informacion, string);
    // Imprime los valores en pantalla
    //g_print("probGanarVisita: %f\n", probGanarVisita);
    //g_print("probPerderVisita: %f\n", probPerderVisita);

    gtk_widget_show_all(window);

    return FALSE;
}

float s_to_f(char *str) {
    char *endptr;
    float result = strtof(str, &endptr);


    if (*endptr != '\0' && *endptr != '\n') {
        printf("Error: No se pudo convertir la cadena a float.\n");
    }

    return result;
}

gboolean cargar()
{
    button_clicked = TRUE;
	cleanTable1();
	cleanTable2();
	createTable1();
    gtk_widget_show_all(window);
    gtk_spin_button_set_value((GtkSpinButton*)cantJuegos, games);
    gtk_spin_button_set_value((GtkSpinButton*)GanarCasa, probGanarCasa);
	gtk_spin_button_set_value((GtkSpinButton*)GanarVisita, probGanarVisita);
	setLocationChecks();
    gtk_widget_show_all(window);
	//createTable2();
	//setear_informacion();
    return FALSE;
}

void cargar_estado(){
	char *filename;
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(FileChooserWindow));
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen(filename, "r");
    if (fp == NULL){
        exit(EXIT_FAILURE);
	}
	int n = -1;
    while ((read = getline(&line, &len, fp)) != -1) {
		char*arrays = strtok(line, " ");
		int j = 0;
		while (arrays != NULL){
			if(n==-1){
			    games = atoi(arrays);
    			probGanarCasa = s_to_f(strtok(NULL, " "));
                probGanarVisita = s_to_f(strtok(NULL, " "));
				break;
			}
			juegoEnCasa[j+1] = atoi(arrays);
			arrays = strtok(NULL, " ");
			j++;
		}
		n++;
    }
	cargar();
    fclose(fp);
    if(line){
        free(line);
	}
	gtk_widget_set_visible(FileChooserWindow, false);
}

void open_fileChooser(GtkWidget* widget, gpointer data)
{
	filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.std");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(FileChooserWindow), filter);
    gtk_dialog_run (GTK_DIALOG (FileChooserWindow));
}

void guardar_estado(){
	char *string = concatenarTresCadenas(i_to_s(games), " ", f_to_s(probGanarCasa));
    string = concatenarTresCadenas(string, " ", f_to_s(probGanarVisita));
    string = concatenarTresCadenas(string, "\n", "");
    int esEnCasa;
    for(int i=0; i < games; i++){
        esEnCasa = juegoEnCasa[i];
        string = concatenarTresCadenas(string, i_to_s(esEnCasa), " ");
    }
	FILE *fp;
	fp = fopen ("estado.std", "w");
	for (int i = 0; i < strlen(string); i++) {
            fputc(string[i], fp);
    }
	fclose(fp);
}


int main(int argc, char *argv[]){
    GtkBuilder *builder; //GTK builder
    gtk_init(&argc, &argv); //start gtk

    builder = gtk_builder_new(); //create gtk ui builder
    gtk_builder_add_from_file(builder, "seriesDeportivas.glade", NULL); //LOAD UI FILE
    
    load_css();
    //ventana
    window = GTK_WIDGET(gtk_builder_get_object(builder, "MyWindow")); //load window named MyWindow
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 500);


    //botones
    cantJuegos = GTK_WIDGET(gtk_builder_get_object(builder, "cant_juegos")); 
	GanarCasa = GTK_WIDGET(gtk_builder_get_object(builder, "home"));
    GanarVisita = GTK_WIDGET(gtk_builder_get_object(builder, "visit"));
    siguiente = GTK_WIDGET(gtk_builder_get_object(builder, "next_button"));

    

    //tablas
    gameLocation = GTK_GRID(gtk_builder_get_object(builder, "games_location"));
    probabilidades = GTK_GRID(gtk_builder_get_object(builder, "answer_table"));
    informacion = GTK_WIDGET(gtk_builder_get_object(builder, "problem_information"));
    respuestaFinal = GTK_WIDGET(gtk_builder_get_object(builder, "final_answer"));
    FileChooserWindow = GTK_WIDGET(gtk_builder_get_object(builder, "FileChooserWindow"));
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


