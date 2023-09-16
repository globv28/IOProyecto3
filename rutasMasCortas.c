#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>

GtkWidget *window;
GtkWidget *myArea;
GtkWidget *nodeEntry;
gboolean button_clicked = FALSE;
int drawingAreaHeight = 500;
int drawingAreaWidth = 500;
int nodes = 0;
gchar *textNodes;
int nodeSize = 20;//Disminuya para hacer más grande
static struct { int posX; int posY; } nodesPos[10];
GtkWidget *siguiente; //boton para avanzar las operaciones
GtkGrid *gridPaths;
GtkGrid *gridShown; //tabla final
GtkEntry ***inputs; //matriz para almacenar los punteros a los gtk entry  
int anchoFlecha = 5;
int largoFlecha = 10;
GtkFileFilter *filter;
const char *rowLabels[8] = {"A", "B", "C", "D", "E", "F", "G", "H"};
int estadoActual = 0;
struct {double r; double g; double b;} color [10] = {{0.88, 0.38, 0.35},
													 {1, 0, 1},
													 {0.92, 0.66, 0.22},
													 {0.58, 0.66, 0.023},
													 {0.19, 0.84, 0.78},
													 {0.87, 0.55, 0.94},
													 {0.22, 0, 0.5},
													 {0.68, 0.04, 0.992}};
int numTabla = 0;
int values[10][10] = {{0,-1,-1,-1,-1,-1,-1,-1,-1}, //matriz de valores para realizar las operaciones
					{-1,0,-1,-1,-1,-1,-1,-1,-1},
					{-1,-1,0,-1,-1,-1,-1,-1,-1},
					{-1,-1,-1,0,-1,-1,-1,-1,-1},
					{-1,-1,-1,-1,0,-1,-1,-1,-1},
					{-1,-1,-1,-1,-1,0,-1,-1,-1},
					{-1,-1,-1,-1,-1,-1,0,-1,-1},
					{-1,-1,-1,-1,-1,-1,-1,0,-1},
					{-1,-1,-1,-1,-1,-1,-1,-1,0}};

int valuesP[10][10] = {{0,0,0,0,0,0,0,0}, //matriz de valores para realizar las operaciones
					{0,0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0,0}};

struct {int matrizD[10][10]; int matrizP[10][10]} state [10];
/*struct {int matrizD[10][10]; int matrizP[10][10]} state1;
struct {int matrizD[10][10]; int matrizP[10][10]} state2;
struct {int matrizD[10][10]; int matrizP[10][10]} state3;
struct {int matrizD[10][10]; int matrizP[10][10]} state4;
struct {int matrizD[10][10]; int matrizP[10][10]} state5;
struct {int matrizD[10][10]; int matrizP[10][10]} state6;
struct {int matrizD[10][10]; int matrizP[10][10]} state7;*/



/*int node0Connections[10] = {0,1,2,3,4,5,6,7};
int node1Connections[10] = {0,1,2,3,4,5,6,7};
int values[3][10] = {0,1,2,3,4,5,6,7};
int values[4][10] = {0,1,2,3,4,5,6,7};
int values[5][10] = {0,1,2,3,4,5,6,7};
int values[6][10] = {0,1,2,3,4,5,6,7};
int values[7][10] = {0,1,2,3,4,5,6,7};
int values[8][10] = {0,1,2,3,4,5,6,7};*/

GtkWidget *FileChooserWindow;
GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
gint res;

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

char* matrizToString() {
    // Calculamos el tamaño necesario para la cadena resultante
    int buffer_size = nodes * nodes * 5;  // Espacio suficiente para los números y separadores
    char* resultado = (char*)malloc(buffer_size * sizeof(char));
    if (resultado == NULL) {
        perror("Error en la asignación de memoria");
        exit(EXIT_FAILURE);
    }

    // Construimos la cadena con la matriz
    int offset = 0;
    for (int i = 1; i < nodes + 1; i++) {
        for (int j = 1; j < nodes + 1; j++) {
            offset += snprintf(resultado + offset, buffer_size - offset, "%d", values[i][j]);
            if (j < nodes) {
                offset += snprintf(resultado + offset, buffer_size - offset, " ");
            }
        }
        if (i < nodes) {
            offset += snprintf(resultado + offset, buffer_size - offset, "\n");
        }
    }

    return resultado;
}

void guardar_tabla(){
	printf("entra");
	char *string = matrizToString();
	/*char *string = "";
	for (int i = 1; i < nodes + 1; i++)	{
		for (int j = 1; j < nodes + 1; j++)
		{
			char *num;
			sprintf(num,"%d",values[i][j]);
			strcat(string, num);
			strcat(string," ");
		}
		strcat(string, "\n");
	}*/
	printf(string);
	FILE *fp;
	fp = fopen ("matriz.tbl", "w");
	for (int i = 0; i < strlen(string); i++) {
            fputc(string[i], fp);
    }
	fclose(fp);
}

void cargar_tabla(){
	char *filename;
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(FileChooserWindow));
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen(filename, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    nodes = 0;
	char *array[20][20];
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        //printf("%s", line);
		char*arrays = strtok(line, " ");
		int j = 0;
		while (arrays != NULL){
			//printf("%s\n", arrays);
			array[nodes][j] = arrays;
			values[nodes+1][j+1] = atoi(arrays);
			//printf("%d ", values[nodes][j]);
			arrays = strtok(NULL, " ");
			j++;
		}
		//printf("\n");
		nodes++;
		//printf("nodos: %d", nodes);
    }
	//printf("nodos: %d", nodes);
	/*for(int i = 0; i < nodes; i++){
		for(int j = 0; j < nodes; j++){
			//printf("%d %d", i, j);
			printf("%s ", values[i][j]);
			//values[i][j] = atoi(array[i-1][j-1]);
		}
		printf("\n");
	}*/
	printf("/%d/", nodes);
	cargar();
    fclose(fp);
    if (line)
        free(line);
	gtk_widget_set_visible(FileChooserWindow, false);
    //exit(EXIT_SUCCESS);
}

void open_fileChooser(GtkWidget* widget, gpointer data)
{
	filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.tbl");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(FileChooserWindow), filter);
    gtk_dialog_run (GTK_DIALOG (FileChooserWindow));
}



void exit_app(){
    gtk_main_quit();
}

void cleanTable1(){
	for (int i = 0; i < 9; i++) { //limpia la tabla que se va a ctualizando
		gtk_grid_remove_row(gridPaths, 0);
    }
}
void cleanTable2(){ //limpia la tabla del display de la respuesta final
	for (int i = 0; i < 9; i++) {
		gtk_grid_remove_row(gridShown, 0);
    }
}

// Función para manejar la acción de ingresar en un GtkEntry
// cuando se ingresa algo a los entrys activa esta señal
// esta funcion almacena y actualiza los datos que se ingresan en la tabla.
//los datos ingresados se ingresan en n*n
void entry_changed(GtkEditable *editable, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(editable);
    const gchar *text = gtk_entry_get_text(entry);
    g_print("Valor ingresado: %s\n", text);

// Obtener la posición (fila y columna) del GtkEntry
    gint row, col;
    gboolean found = FALSE;
    for (int i = 0; i <= nodes; i++) {
        for (int j = 0; j <= nodes; j++) {
            if (inputs[i][j] == entry) {
                row = i;
                col = j;
                found = TRUE;
                break;
            }
        }
        if (found) {
            break;
        }
    }

    if (found) {
        // Actualizar el valor en la matriz inputs
        printf("Valor ingresado en (%d, %d): %s\n", row, col, text);
        int value = strtol(text, NULL, 10); //convertir a numerico el texto
		values[row][col] = value;
		gtk_widget_queue_draw (myArea);
    } else {
        g_print("GtkEntry no encontrado en la matriz inputs\n");
    }
}


//crea la tabla inicial para ingresar las distancias iniciales entre los nodos
void createTable1(int cargada){
	int n = nodes + 1; //cuadricula nxn el + 1 representa las labels
	inputs = g_new(GtkEntry**, n);
	cleanTable1();
    
	GtkWidget *label = gtk_label_new (NULL);
	char labelText[5];  // Definimos un buffer para el texto
	snprintf(labelText, sizeof(labelText), "D(%d)", numTabla);

	gtk_label_set_text(label, labelText);
	gtk_grid_attach(gridPaths, label, 0, 0, 1, 1); // Adjunta el label a la posición (i, j) en el grid	
    
	for (int i = 0; i < n ; i++) {
		inputs[i] = g_new(GtkEntry*, n); //se le crea una row a inputs
        
		for (int j = 0; j < n; j++) {
			if(i == 0){
				GtkLabel *label2 = gtk_label_new (rowLabels[j-1]);
				gtk_grid_attach(gridPaths, label2, 0, j, 1, 1); // Adjunta el label a la posición (i, j) en el grid
			} 
			else if (j == 0){
				GtkLabel *label = gtk_label_new (rowLabels[i-1]);
				gtk_grid_attach(gridPaths, label, i, 0, 1, 1); // Adjunta el label a la posición (i, j) en el grid

			} else {
            
			inputs[i][j] = GTK_ENTRY(gtk_entry_new()); // Crea un nuevo GtkEntry
			
			gtk_entry_set_width_chars(GTK_WIDGET(inputs[i][j]), 5);
            gtk_grid_attach(gridPaths, GTK_WIDGET(inputs[i][j]), j, i, 1, 1); // Adjunta el entry  a la posición (i, j) en el grid
			gchar* valor;
			if (cargada == 0){
				valor = (i == j)? "0": "-1";
			}else{
				valor = g_strdup_printf("%d", values[i][j]);
			}
			gtk_entry_set_text (inputs[i][j], valor);
			g_signal_connect(inputs[i][j], "changed", G_CALLBACK(entry_changed), NULL);
			
			}
        }
    }
	
    gtk_widget_show_all(window);
}

char* valor(int nodo1, int nodo2){
	char* stri = rowLabels[nodo1];
	if(valuesP[nodo1][nodo2] == 0){
		return stri;
	}else{
		return valor(valuesP[nodo1][nodo2], nodo2);
	}

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

void mostrarRutasOptimas() {
    GtkWidget *ventana = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(ventana), "Tabla NxN");
    gtk_container_set_border_width(GTK_CONTAINER(ventana), 10);
    gtk_widget_set_size_request(ventana, 400, 400);
    g_signal_connect(ventana, "destroy", G_CALLBACK(gtk_main_quit), false);

    GtkWidget *tabla = gtk_table_new(nodes, nodes, TRUE);
    gtk_container_add(GTK_CONTAINER(ventana), tabla);

    for (int i = 0; i < nodes + 1; i++) {
        for (int j = 0; j < nodes + 1; j++) {
			GtkWidget *celda;// = gtk_label_new("R");
			if(j == 0 || i == 0){
				celda = (j==0)? gtk_label_new(rowLabels[i-1]): gtk_label_new(rowLabels[j-1]);
			}else{
				celda = gtk_button_new_with_label(rowLabels[i-1]);
			}
			gtk_table_attach_defaults(GTK_TABLE(tabla), celda, j, j + 1, i, i + 1);
        }
    }

    gtk_widget_show_all(ventana);
}

void on_siguiente_clicked(){
	printf("estado: %d", numTabla);
	if(numTabla < nodes){
		updateTable1();
	}
	else{
		mostrarRutasOptimas();
	}
}

void on_anterior_clicked(){
	printf("estado: %d", numTabla);
	if(numTabla >= 1){
		returnTable();
	}
}

void returnTable() {
	numTabla--;
    int n = nodes + 1; // Tamaño de la cuadrícula nxn (incluyendo las etiquetas)
    cleanTable1(); // Limpia cualquier contenido anterior en la ventana
	//ACTUALIZAR EL D(NUMTABLA)
	GtkLabel *label = gtk_label_new (NULL);
	char labelText[5];  // Definimos un buffer para el texto
    snprintf(labelText, sizeof(labelText), "D(%d)", numTabla);
	gtk_label_set_text(label, labelText);
	gtk_label_set_width_chars(label, 10);
	gtk_grid_attach(gridPaths, label, 0, 0, 1, 1); // Adjunta el label a la posición (i, j) en el grid		

    for (int i = 1; i < n; i++) {
		GtkLabel *label = gtk_label_new (rowLabels[i-1]);
		gtk_grid_attach(gridPaths, label, i, 0, 1, 1); // Adjunta el label a la posición (i, j) en el grid
        for (int j = 1; j < n; j++) {
			GtkLabel *label2 = gtk_label_new (rowLabels[j-1]);
			gtk_grid_attach(gridPaths, label2, 0, j, 1, 1); // Adjunta el label a la posición (i, j) en el grid
			values[i][j] = state[numTabla+1].matrizD[i][j];
			valuesP[i][j] = state[numTabla+1].matrizP[i][j];
			createTableOutputFinal();
			//este codigo rellena el valor para mostrarlo en la tabla
			char value_str[10];
			sprintf(value_str, "%d", values[i][j]); // Convierte el valor numérico a una cadena
			GtkLabel *label = gtk_label_new(value_str);
			gtk_label_set_width_chars(label, 10);
			gtk_grid_attach(gridPaths, label, j, i, 1, 1);
            }
        }
	
    gtk_widget_show_all(window);
}

//rellena la tabla final
void updateTable1() {
	numTabla++;
    int n = nodes + 1; // Tamaño de la cuadrícula nxn (incluyendo las etiquetas)
    cleanTable1(); // Limpia cualquier contenido anterior en la ventana
	//ACTUALIZAR EL D(NUMTABLA)
	GtkLabel *label = gtk_label_new (NULL);
	char labelText[5];  // Definimos un buffer para el texto
    snprintf(labelText, sizeof(labelText), "D(%d)", numTabla);
	gtk_label_set_text(label, labelText);
	gtk_label_set_width_chars(label, 10);
	gtk_grid_attach(gridPaths, label, 0, 0, 1, 1); // Adjunta el label a la posición (i, j) en el grid		

    for (int i = 1; i < n; i++) {
		GtkLabel *label = gtk_label_new (rowLabels[i-1]);
		gtk_grid_attach(gridPaths, label, i, 0, 1, 1); // Adjunta el label a la posición (i, j) en el grid
        for (int j = 1; j < n; j++) {
			GtkLabel *label2 = gtk_label_new (rowLabels[j-1]);
			gtk_grid_attach(gridPaths, label2, 0, j, 1, 1); // Adjunta el label a la posición (i, j) en el grid

				//esta seccion del codigo actualiza el valor de la tabla
				//aqui es donde deberia implementarse el algoritmo de floyd
				//values[i][j] = values[i][j] + 1;
				state[numTabla].matrizD[i][j] = values[i][j];
				state[numTabla].matrizP[i][j] = valuesP[i][j];
				int a, b1, b2, b, oldValue;
				oldValue = values[i][j];
				a = (values[i][j] == -1) ? 10000: values[i][j];
				b1 = (values[i][numTabla] == -1) ? 10000: values[i][numTabla];
				b2 = (values[numTabla][j] == -1) ? 10000: values[numTabla][j];
				b = b1 + b2;
				values[i][j] = (a > b) ? values[i][numTabla] + values[numTabla][j] : values[i][j];
				if(oldValue != values[i][j]){
					valuesP[i][j] = numTabla;
					createTableOutputFinal();
					//printf("I: %d J: %d Valor: %d\n NumTabla: %d\n", i, j, valuesP[i][j], rowLabels[numTabla]);	
				}
				//printf("I: %d J: %d Valor: %d\n", i, j, values[i][j]);	
			
				//este codigo rellena el valor para mostrarlo en la tabla
                char value_str[10];
                sprintf(value_str, "%d", values[i][j]); // Convierte el valor numérico a una cadena
                GtkLabel *label = gtk_label_new(value_str);
				gtk_label_set_width_chars(label, 10);
                gtk_grid_attach(gridPaths, label, j, i, 1, 1);
            }
        }
	
    gtk_widget_show_all(window);
}

void createTableOutputFinal() {
    int n = nodes + 1; // Tamaño de la cuadrícula nxn (incluyendo las etiquetas)
    cleanTable2(); // Limpia cualquier contenido anterior en la ventana
	GtkLabel *label = gtk_label_new ("P");
	gtk_label_set_width_chars(label, 10);
	gtk_grid_attach(gridShown, label, 0, 0, 1, 1); // Adjunta el label a la posición (i, j) en el grid		

    for (int i = 1; i < n; i++) {
		GtkLabel *label = gtk_label_new (rowLabels[i-1]);
		gtk_grid_attach(gridShown, label, i, 0, 1, 1); // Adjunta el label a la posición (i, j) en el grid
		
		GtkLabel *label2 = gtk_label_new (rowLabels[i-1]);
		gtk_grid_attach(gridShown, label2, 0, i, 1, 1); // Adjunta el label a la posición (i, j) en el grid
        for (int j = 1; j < n; j++) {

            char value_str[10];
            sprintf(value_str, "%d", valuesP[i][j]); // Convierte el valor numérico a una cadena
            GtkWidget *label = gtk_label_new(value_str);
			gtk_label_set_width_chars(label, 10);
            gtk_grid_attach(gridShown, label, j, i, 1, 1);
            
        }
    }

    gtk_widget_show_all(window);
}


void add_line_value(cairo_t *cr, int value, int x, int y, int pcolor){
	cairo_move_to(cr, x, y);
	//printf("%f", color[pcolor].r);
	cairo_set_source_rgb(cr, color[pcolor].r, color[pcolor].g, color[pcolor].b);
	cairo_set_font_size (cr,15);
	char buf[20];
	sprintf(buf,"%d",value);
	cairo_show_text(cr,buf);
	cairo_stroke(cr);
	cairo_set_source_rgb(cr, 0,0,0);
}

void add_node_name(cairo_t *cr, int x, int y, int pos){
	//double radius = drawingAreaHeight/nodeSize;
	cairo_save (cr);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_translate(cr, -6, 6);
	cairo_set_font_size (cr,20);
	char buf[20];
	sprintf(buf,"%d",pos);
    cairo_show_text(cr,rowLabels[pos]);
    cairo_stroke(cr);
    cairo_restore (cr);
    cairo_set_source_rgb(cr, 0, 0, 0);
}

void draw_arrow0(cairo_t *cr, int nodo, int tipoY){ // vertical
	cairo_set_line_width(cr,3);
	double radius = drawingAreaHeight/nodeSize + 1;
	cairo_move_to(cr, nodesPos[nodo].posX, nodesPos[nodo].posY - radius*tipoY);
	cairo_line_to(cr, nodesPos[nodo].posX - anchoFlecha, nodesPos[nodo].posY - radius*tipoY - largoFlecha*tipoY);
	cairo_move_to(cr, nodesPos[nodo].posX, nodesPos[nodo].posY - radius*tipoY);
	cairo_line_to(cr, nodesPos[nodo].posX + anchoFlecha, nodesPos[nodo].posY - radius*tipoY  - largoFlecha*tipoY);
	cairo_stroke(cr);
	cairo_set_line_width(cr,1);
	cairo_stroke(cr);
}

void draw_arrow1(cairo_t *cr, int node, int direccion){
	cairo_set_line_width(cr,3);
	double radius = drawingAreaHeight/nodeSize + 1;
	cairo_move_to(cr, nodesPos[node].posX - radius*direccion, nodesPos[node].posY);
	cairo_line_to(cr, nodesPos[node].posX - radius*direccion - largoFlecha *direccion, nodesPos[node].posY + anchoFlecha*direccion);
	cairo_move_to(cr, nodesPos[node].posX - radius*direccion, nodesPos[node].posY);
	cairo_line_to(cr, nodesPos[node].posX - radius*direccion - largoFlecha *direccion, nodesPos[node].posY - anchoFlecha*direccion);
	cairo_stroke(cr);
	cairo_set_line_width(cr,1);
	cairo_stroke(cr);
	}

void draw_arrow2(cairo_t *cr, int node, int direccion, int direccionY){
	cairo_set_line_width(cr,3);
	double restaY = 20;
	double restaX = 20;
	cairo_move_to(cr, nodesPos[node].posX - restaX*direccion, nodesPos[node].posY + restaY * direccionY);
	cairo_line_to(cr, nodesPos[node].posX - restaX*direccion - largoFlecha*direccion, nodesPos[node].posY + restaY * direccionY + anchoFlecha*direccionY);
	cairo_move_to(cr, nodesPos[node].posX - restaX*direccion, nodesPos[node].posY + restaY * direccionY);
	cairo_line_to(cr, nodesPos[node].posX - restaX*direccion - anchoFlecha*direccion, nodesPos[node].posY + restaY * direccionY + largoFlecha*direccionY);
	cairo_stroke(cr);
	cairo_set_line_width(cr,1);
	cairo_stroke(cr);
	}

void draw_arrow3(cairo_t *cr, int node, int direccion, int direccionY){
	cairo_set_line_width(cr,3);
	double restaY = 25.5;
	double restaX = 10;
	cairo_move_to(cr, nodesPos[node].posX - restaX*direccion, nodesPos[node].posY + restaY * direccionY);
	cairo_line_to(cr, nodesPos[node].posX - restaX*direccion - largoFlecha*direccion, nodesPos[node].posY + restaY * direccionY + largoFlecha*direccionY);
	cairo_move_to(cr, nodesPos[node].posX - restaX*direccion, nodesPos[node].posY + restaY * direccionY);
	cairo_line_to(cr, nodesPos[node].posX - restaX*direccion - (anchoFlecha-5)*direccion, nodesPos[node].posY + restaY * direccionY + (largoFlecha+3)*direccionY);
	cairo_stroke(cr);
	cairo_set_line_width(cr,1);
	cairo_stroke(cr);
	}

void draw_arrow4(cairo_t *cr, int node, int direccion, int direccionY){
	cairo_set_line_width(cr,3);
	double restaY = 10;
	double restaX = 25;
	cairo_move_to(cr, nodesPos[node].posX - restaX*direccion, nodesPos[node].posY + restaY * direccionY);
	cairo_line_to(cr, nodesPos[node].posX - restaX*direccion - (largoFlecha+3)*direccion, nodesPos[node].posY + restaY * direccionY);
	cairo_move_to(cr, nodesPos[node].posX - restaX*direccion, nodesPos[node].posY + restaY * direccionY);
	cairo_line_to(cr, nodesPos[node].posX - restaX*direccion - largoFlecha*direccion, nodesPos[node].posY + restaY * direccionY + largoFlecha*direccionY);
	cairo_stroke(cr);
	cairo_set_line_width(cr,1);
	cairo_stroke(cr);
}

void set_node_connections(cairo_t * cr, int node){
	int node2;
	for(int i = 1; i < 9; i++){
		if(i != node && values[node][i] != -1){
			node2 = i-1;
			cairo_move_to(cr, nodesPos[node - 1].posX, nodesPos[node - 1].posY);
			cairo_line_to(cr, nodesPos[node2].posX, nodesPos[node2].posY);
			cairo_stroke(cr);
		}
	}
}

void set_node1_arrows(cairo_t *cr){
	int node2;
	int node = 0;
	if(values[1][2] != -1){
		node2 = 1;
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		draw_arrow1(cr, node2, 1);
		cairo_stroke(cr);
		add_line_value(cr, values[1][node2 + 1], nodesPos[node2].posX - 55, nodesPos[node2].posY - 3, 0);
		cairo_stroke(cr);
	}
	if(values[1][3] != -1){
		node2 = 2;
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		draw_arrow2(cr,node2, 1,1);
		cairo_stroke(cr);
		add_line_value(cr, values[1][node2+1], nodesPos[node2].posX - 45, nodesPos[node2].posY + 35, 0);
		cairo_stroke(cr);
	}
	if(values[1][4] != -1){
		node2 = 3;
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		draw_arrow2(cr,node2,1,-1);
		cairo_stroke(cr);
		add_line_value(cr, values[1][node2+1], nodesPos[node2].posX - 45, nodesPos[node2].posY - 23, 0);
		cairo_stroke(cr);
	}
	if(values[1][5] != -1){
		node2 = 4;
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		draw_arrow4(cr,node2,1,1);
		cairo_stroke(cr);
		add_line_value(cr, values[1][node2+1], nodesPos[node2].posX - 63, nodesPos[node2].posY + 23, 0);
		cairo_stroke(cr);
	}
	if(values[1][6] != -1){
		node2 = 5;
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		draw_arrow3(cr,node2,1,1);
		add_line_value(cr, values[1][node2+1], nodesPos[node2].posX - 30, nodesPos[node2].posY + 50, 0);
		cairo_stroke(cr);
	}
	if(values[1][7] != -1){
		node2 = 6;
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		draw_arrow3(cr,node2,1,-1);
		add_line_value(cr, values[1][node2+1], nodesPos[node2].posX - 30, nodesPos[node2].posY - 35, 0);
		cairo_stroke(cr);
	}
	if(values[1][8] != -1){
		node2 = 7;
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		draw_arrow4(cr,node2,1,-1);
		add_line_value(cr, values[1][node2+1], nodesPos[node2].posX - 55, nodesPos[node2].posY - 25, 0);
		cairo_stroke(cr);
	}
}

void set_node2_arrows(cairo_t *cr){
	int node2;
	int node = 1;
	if(values[2][1] != -1){
		node2 = 0;
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		draw_arrow1(cr, node2, -1);
		add_line_value(cr, values[2][node2+1], nodesPos[node2].posX + 45, nodesPos[node2].posY - 3, 1);
		cairo_stroke(cr);
	}
	
	if(values[2][3] != -1){
		node2 = 2;
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		draw_arrow2(cr, node2, -1,1);
		add_line_value(cr, values[2][node2+1], nodesPos[node2].posX + 35, nodesPos[node2].posY + 32, 1);
		cairo_stroke(cr);
	}
	
	if(values[2][4] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 3;
		draw_arrow2(cr,node2,-1,-1);
		add_line_value(cr, values[2][node2+1], nodesPos[node2].posX + 35 , nodesPos[node2].posY - 43, 1);
		cairo_stroke(cr);
	}
	
	if(values[2][5] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 4;
		draw_arrow3(cr,node2,-1,1);
		add_line_value(cr, values[2][node2+1], nodesPos[node2].posX + 20, nodesPos[node2].posY + 45, 1);
		cairo_stroke(cr);
	}
	
	if(values[2][6] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 5;
		draw_arrow4(cr,node2,-1,1);
		add_line_value(cr, values[2][node2+1], nodesPos[node2].posX + 55, nodesPos[node2].posY + 20, 1);
		cairo_stroke(cr);
	}
	
	if(values[2][7] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 6;
		draw_arrow4(cr,node2,-1,-1);
		add_line_value(cr, values[2][node2+1], nodesPos[node2].posX + 55, nodesPos[node2].posY - 30, 1);
		cairo_stroke(cr);
	}
	
	if(values[2][8] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 7;
		draw_arrow3(cr,node2,-1,-1);
		add_line_value(cr, values[2][node2+1], nodesPos[node2].posX + 20, nodesPos[node2].posY - 35, 1);
		cairo_stroke(cr);
	}
}

void set_node3_arrows(cairo_t *cr){
	int node2;
	int node = 2;
	if(values[3][1] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 0;
		draw_arrow2(cr, node2, -1,-1);
		add_line_value(cr, values[3][node2+1], nodesPos[node2].posX + 23, nodesPos[node2].posY - 34, 2);
		cairo_stroke(cr);
	}
	if(values[3][2] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 1;
		draw_arrow2(cr, node2, 1,-1);
		add_line_value(cr, values[3][node2+1], nodesPos[node2].posX - 35, nodesPos[node2].posY - 35, 2);
		cairo_stroke(cr);
	}
	if(values[3][4] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 3;
		draw_arrow0(cr, node2, 1);
		add_line_value(cr, values[3][node2+1], nodesPos[node2].posX + 3, nodesPos[node2].posY - 45, 2);
		cairo_stroke(cr);
	}
	if(values[3][5] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 4;
		draw_arrow4(cr, node2, 1,-1);
		add_line_value(cr, values[3][node2+1], nodesPos[node2].posX - 40, nodesPos[node2].posY - 25, 2);
		cairo_stroke(cr);
	}
	if(values[3][6] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 5;
		draw_arrow4(cr, node2, -1,-1);
		add_line_value(cr, values[3][node2+1], nodesPos[node2].posX + 35, nodesPos[node2].posY - 25, 2);
		cairo_stroke(cr);
	}
	if(values[3][7] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 6;
		draw_arrow3(cr, node2, -1,-1);
		add_line_value(cr, values[3][node2+1], nodesPos[node2].posX + 10, nodesPos[node2].posY - 50, 2);
		cairo_stroke(cr);
	}
	if(values[3][8] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 7;
		draw_arrow3(cr, node2, 1,-1);
		add_line_value(cr, values[3][node2+1], nodesPos[node2].posX - 20, nodesPos[node2].posY - 50, 2);
		cairo_stroke(cr);
	}
}

void set_node4_arrows(cairo_t *cr){
	int node2;
	int node = 3;
	if(values[4][1] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 0;
		draw_arrow2(cr, node2, -1,1);
		add_line_value(cr, values[4][node2+1], nodesPos[node2].posX + 22, nodesPos[node2].posY + 43, 3);
		cairo_stroke(cr);
	}
	if(values[4][2] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 1;
		draw_arrow2(cr, node2, 1,1);
		add_line_value(cr, values[4][node2+1], nodesPos[node2].posX - 28, nodesPos[node2].posY + 43, 3);
		cairo_stroke(cr);
	}
	if(values[4][3] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 2;
		draw_arrow0(cr, node2,-1);
		add_line_value(cr, values[4][node2+1], nodesPos[node2].posX + 1, nodesPos[node2].posY + 55, 3);
		cairo_stroke(cr);
	}
	if(values[4][5] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 4;
		draw_arrow3(cr, node2, 1,1);
		add_line_value(cr, values[4][node2+1], nodesPos[node2].posX - 35, nodesPos[node2].posY + 55, 3);
		cairo_stroke(cr);
	}
	if(values[4][6] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 5;
		draw_arrow3(cr, node2, -1,1);
		add_line_value(cr, values[4][node2+1], nodesPos[node2].posX + 21, nodesPos[node2].posY + 51, 3);
		cairo_stroke(cr);
	}
	if(values[4][7] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 6;
		draw_arrow4(cr, node2, -1,1);
		add_line_value(cr, values[4][node2+1], nodesPos[node2].posX + 33, nodesPos[node2].posY + 33, 3);
		cairo_stroke(cr);
	}
	if(values[4][8] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 7;
		draw_arrow4(cr, node2, 1,1);
		add_line_value(cr, values[4][node2+1], nodesPos[node2].posX - 45, nodesPos[node2].posY + 33, 3);
		cairo_stroke(cr);
	}
}

void set_node5_arrows(cairo_t *cr){
	int node2;
	int node = 4;
	if(values[5][1] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 0;
		draw_arrow4(cr, node2, -1,-1);
		add_line_value(cr, values[5][node2+1], nodesPos[node2].posX + 42, nodesPos[node2].posY - 22, 4);
		cairo_stroke(cr);
	}
	if(values[5][2] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 1;
		draw_arrow3(cr, node2, 1,-1);
		add_line_value(cr, values[5][node2+1], nodesPos[node2].posX - 10, nodesPos[node2].posY - 35, 4);
		cairo_stroke(cr);
	}
	if(values[5][3] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 2;
		draw_arrow4(cr, node2, -1,1);
		add_line_value(cr, values[5][node2+1], nodesPos[node2].posX + 40, nodesPos[node2].posY + 10, 4);
		cairo_stroke(cr);
	}
	if(values[5][4] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 3;
		draw_arrow3(cr, node2, -1,-1);
		add_line_value(cr, values[5][node2+1], nodesPos[node2].posX + 14, nodesPos[node2].posY - 58, 4);
		cairo_stroke(cr);
	}
	if(values[5][6] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 5;
		draw_arrow1(cr, node2, -1);
		add_line_value(cr, values[5][node2+1], nodesPos[node2].posX + 38, nodesPos[node2].posY - 2, 4);
		cairo_stroke(cr);
	}

	if(values[5][7] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 6;
		draw_arrow2(cr, node2, -1,-1);
		add_line_value(cr, values[5][node2+1], nodesPos[node2].posX + 37, nodesPos[node2].posY - 48, 4);
		cairo_stroke(cr);
	}
	if(values[5][8] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 7;
		draw_arrow0(cr, node2, 1);
		add_line_value(cr, values[5][node2+1], nodesPos[node2].posX, nodesPos[node2].posY - 40, 4);
		cairo_stroke(cr);
	}
}

void set_node6_arrows(cairo_t *cr){
	int node2;
	int node = 5;
	if(values[6][1] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 0;
		draw_arrow3(cr,node2,-1,-1);
		add_line_value(cr, values[6][node2+1], nodesPos[node2].posX, nodesPos[node2].posY - 35, 5);
		cairo_stroke(cr);
	}
	if(values[6][2] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 1;
		draw_arrow4(cr,node2,1,-1);
		add_line_value(cr, values[6][node2+1], nodesPos[node2].posX - 55, nodesPos[node2].posY - 25, 5);
		cairo_stroke(cr);
	}
	if(values[6][3] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 2;
		draw_arrow4(cr, node2, 1,1);
		add_line_value(cr, values[6][node2+1], nodesPos[node2].posX - 48, nodesPos[node2].posY + 10, 5);
		cairo_stroke(cr);
	}
	if(values[6][4] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 3;
		draw_arrow3(cr, node2, 1,-1);
		add_line_value(cr, values[6][node2+1], nodesPos[node2].posX - 18, nodesPos[node2].posY - 50, 5);
		cairo_stroke(cr);
	}
	if(values[6][5] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 4;
		draw_arrow1(cr, node2, 1);
		add_line_value(cr, values[6][node2+1], nodesPos[node2].posX - 50, nodesPos[node2].posY - 2, 5);
		cairo_stroke(cr);
	}

	if(values[6][7] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 6;
		draw_arrow0(cr, node2, 1);
		add_line_value(cr, values[6][node2+1], nodesPos[node2].posX - 10, nodesPos[node2].posY - 40, 5);
		cairo_stroke(cr);
	}
	if(values[6][8] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 7;
		draw_arrow2(cr, node2, 1,-1);
		add_line_value(cr, values[6][node2+1], nodesPos[node2].posX -40, nodesPos[node2].posY - 40, 5);
		cairo_stroke(cr);
	}
}

void set_node7_arrows(cairo_t *cr){
	int node2;
	int node = 6;
	if(values[7][1] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 0;
		draw_arrow3(cr,node2,-1,1);
		add_line_value(cr, values[7][node2+1], nodesPos[node2].posX + 3, nodesPos[node2].posY + 52, 6);
		cairo_stroke(cr);
	}
	if(values[7][2] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 1;
		draw_arrow4(cr,node2,1,1);
		add_line_value(cr, values[7][node2+1], nodesPos[node2].posX - 55, nodesPos[node2].posY + 16, 6);
		cairo_stroke(cr);
	}
	if(values[7][3] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 2;
		draw_arrow3(cr, node2, 1,1);
		add_line_value(cr, values[7][node2+1], nodesPos[node2].posX -18, nodesPos[node2].posY + 60, 6);
		cairo_stroke(cr);
	}
	if(values[7][4] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 3;
		draw_arrow4(cr, node2, 1,-1);
		add_line_value(cr, values[7][node2+1], nodesPos[node2].posX - 48, nodesPos[node2].posY, 6);
		cairo_stroke(cr);
	}
	if(values[7][5] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 4;
		draw_arrow2(cr, node2, 1,1);
		add_line_value(cr, values[7][node2+1], nodesPos[node2].posX - 50, nodesPos[node2].posY + 40, 6);
		cairo_stroke(cr);
	}

	if(values[7][6] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 5;
		draw_arrow0(cr, node2, -1);
		add_line_value(cr, values[7][node2+1], nodesPos[node2].posX - 10, nodesPos[node2].posY + 53, 6);
		cairo_stroke(cr);
	}
	if(values[7][8] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 7;
		draw_arrow1(cr, node2, 1);
		add_line_value(cr, values[7][node2+1], nodesPos[node2].posX - 55, nodesPos[node2].posY + 15, 6);
		cairo_stroke(cr);
	}
}

void set_node8_arrows(cairo_t *cr){
	int node2;
	int node = 7;
	if(values[8][1] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 0;
		draw_arrow4(cr,node2,-1,1);
		add_line_value(cr, values[8][node2+1], nodesPos[node2].posX + 45, nodesPos[node2].posY + 15, 7);
		cairo_stroke(cr);
	}
	if(values[8][2] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 1;
		draw_arrow3(cr,node2,1,1);
		add_line_value(cr, values[8][node2+1], nodesPos[node2].posX - 10, nodesPos[node2].posY + 50, 7);
		cairo_stroke(cr);
	}
	if(values[8][3] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 2;
		draw_arrow3(cr, node2, -1, 1);
		add_line_value(cr, values[8][node2+1], nodesPos[node2].posX + 25, nodesPos[node2].posY + 55, 7);
		cairo_stroke(cr);
	}
	if(values[8][4] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 3;
		draw_arrow4(cr, node2, -1,-1);
		add_line_value(cr, values[8][node2+1], nodesPos[node2].posX + 40, nodesPos[node2].posY - 3, 7);
		cairo_stroke(cr);
	}
	if(values[8][5] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 4;
		draw_arrow0(cr, node2, -1);
		add_line_value(cr, values[8][node2+1], nodesPos[node2].posX - 10, nodesPos[node2].posY + 50, 7);
		cairo_stroke(cr);
	}

	if(values[8][6] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 5;
		draw_arrow2(cr, node2, -1,1);
		add_line_value(cr, values[8][node2+1], nodesPos[node2].posX + 42, nodesPos[node2].posY + 40, 7);
		cairo_stroke(cr);
	}
	if(values[8][7] != -1){
		cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
		node2 = 6;
		draw_arrow1(cr, node2, -1);
		add_line_value(cr, values[8][node2+1], nodesPos[node2].posX + 40, nodesPos[node2].posY - 3, 7);
		cairo_stroke(cr);
	}
}


void draw_node(cairo_t *cr, int node){
	int x = nodesPos[node].posX;
	int y = nodesPos[node].posY;
	cairo_save (cr);
	cairo_translate (cr, x,y);
	//cairo_set_source_rgb(cr, 1, 1, 0);
	//cairo_set_font_size (cr, 20);
    //cairo_show_text(cr,"a");
    //cairo_set_source_rgb(cr, 0.226, 0.96, 0.88);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_arc (cr, 0., 0., drawingAreaHeight/nodeSize+1, 0., 2 * M_PI);
    cairo_stroke(cr);
    cairo_set_source_rgb(cr, color[node].r, color[node].g, color[node].b);
	cairo_arc (cr, 0., 0., drawingAreaHeight/nodeSize, 0., 2 * M_PI);
	cairo_fill(cr);
	cairo_stroke(cr);
	add_node_name(cr,x,y, node);
	cairo_restore (cr);
}

void set_position(int x, int y, int pos){
	nodesPos[pos].posX = x;
    nodesPos[pos].posY = y;
}

/*void connect_nodes(cairo_t *cr, int node1, int node2){
	//cairo_translate (cr,drawingAreaWidth / 2., drawingAreaHeight / 2.);
	double radius = drawingAreaHeight/nodeSize;
	cairo_save (cr);
	cairo_translate (cr, nodesPos[node1].posX, nodesPos[node1].posY);
	cairo_move_to(cr,-radius/4,-radius/4);
	cairo_line_to(cr, nodesPos[node2].posX - nodesPos[node1].posX -radius/4, nodesPos[node2].posY-nodesPos[node1].posY -radius/4);
	cairo_arc (cr, 0., 0., drawingAreaHeight/nodeSize, 0., 2 * M_PI);
	cairo_stroke(cr);
	cairo_restore (cr);
	//cairo_stroke(cr);
	//cairo_save (cr);
	//cairo_set_source_rgb(cr, 1, 0, 0);
	//cairo_translate (cr, nodesPos[node1].posX, nodesPos[node1].posY);
	//draw_node(cr,0,0);
	//cairo_set_source_rgb(cr, 1, 1, 0);
    //cairo_line_to(cr, nodesPos[node2].posX, nodesPos[node2].posY);
    //cairo_stroke_preserve(cr);
    //cairo_restore (cr);
}*/

/*void draw_arrow(cairo_t *cr, int x1, int y1, int x2, int y2){
	cairo_move_to(cr, 0, 0);
	cairo_set_line_width(cr,8);
	double arrow_length = 0.45;
	double arrow_angle = M_PI/3;
	double arrowhead_angle = M_PI/4;
 	double arrowhead_length = 45;
	//cairo_line_to(cr,arrow_length * cos(arrow_angle), arrow_length * sin(arrow_angle) + 50);
	//cairo_move_to(cr,-arrowhead_length * cos(arrow_angle - arrowhead_angle), -arrowhead_length * sin(arrow_angle - arrowhead_angle));
	cairo_line_to(cr,arrowhead_length * cos(arrow_angle - arrowhead_angle), arrowhead_length * sin(arrow_angle - arrowhead_angle));
	cairo_line_to(cr,-arrowhead_length * cos(arrow_angle + arrowhead_angle), -arrowhead_length * sin(arrow_angle + arrowhead_angle));
    cairo_stroke(cr);
}*/

void connect_nodes(cairo_t *cr, int node1, int node2, int value){
	cairo_move_to(cr, nodesPos[node1].posX, nodesPos[node1].posY);
	cairo_line_to(cr, nodesPos[node2].posX, nodesPos[node2].posY);
	cairo_move_to(cr, nodesPos[node2].posX *0.5 - nodesPos[node1].posX * 0.5, nodesPos[node2].posY * 0.5 - nodesPos[node1].posY * 0.5);
	cairo_set_font_size (cr,20);
	char buf[20];
	sprintf(buf,"%d",value);
    cairo_show_text(cr,buf);
	//draw_arrow(cr, node2);
	cairo_stroke(cr);
}

/*void draw_line(cairo_t *cr, int node1, int node2){
  // coordinates for the center of the window
  int xc, yc;
  xc = drawingAreaWidth / 2;
  yc = drawingAreaHeight / 2;

  cairo_set_line_width(cr,10.0);

  // draw red lines out from the center of the window
  cairo_set_source_rgb(cr, 0.8, 0.0, 0.0);
  cairo_move_to(cr,0, 0);
  cairo_move_to(cr,10, 50);
  //cairo_line_to(cr, xc, yc);
  //cairo_line_to(cr, 0, drawingAreaHeight);
  //cairo_move_to(cr, xc, yc);
  cairo_line_to(cr,drawingAreaWidth, yc);
  
  cairo_stroke(cr);

}*/

gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_translate (cr, drawingAreaWidth / 2., drawingAreaHeight / 2.);
    int distance1 = 140;
    int distance2 = 100;
	//cairo_scale (cr, drawingAreaWidth / 100., drawingAreaHeight / 100.); 
	if(nodes == 1){
		set_position(0,0,0);
		//draw_node(cr,0,0,0);
	}else{
	if(nodes >= 2){
		set_position(-distance1,0,0);
		set_position(distance1,0,1);
		//draw_node(cr,-distance1,0,0);
		//draw_node(cr,distance1,0,1);
	}
	if(nodes >= 3){
		set_position(0,-distance1,2);
		//draw_node(cr,0,-distance1,2);
	}
	if(nodes >= 4){
		set_position(0,distance1,3);
		//draw_node(cr,0,distance1,3);
	}
	if(nodes >= 5){
		set_position(distance2,-distance2,4);
		//draw_node(cr,distance2,-distance2,4);
	}
	if(nodes >= 6){
		set_position(-distance2,-distance2,5);
		//draw_node(cr,-distance2,-distance2,5);
	}
	if(nodes >= 7){
		set_position(-distance2,distance2,6);
		//draw_node(cr,-distance2,distance2,6);
	}
	if(nodes == 8){
		set_position(distance2,distance2,7);
		//draw_node(cr,distance2,distance2,7);
	}}
	set_node_connections(cr,1);
	set_node_connections(cr,2);
	set_node_connections(cr,3);
	set_node_connections(cr,4);
	set_node_connections(cr,5);
	set_node_connections(cr,6);
	set_node_connections(cr,7);
	set_node_connections(cr,8);
	for(int i = 0; i < nodes; i++){
		draw_node(cr,i);
	}
	set_node1_arrows(cr);
	set_node2_arrows(cr);
	set_node3_arrows(cr);
	set_node4_arrows(cr);
	set_node5_arrows(cr);
	set_node6_arrows(cr);
	set_node7_arrows(cr);
	set_node8_arrows(cr);
	//draw_arrow0(cr, 2, 1);
	//draw_arrow0(cr, 3, -1);

	//cairo_move_to(cr, 0, 0);
	//cairo_save (cr);
	//printf("%d",2);
	//cairo_scale (cr, drawingAreaWidth / 8., drawingAreaHeight / 8.); 
	//cairo_arc (cr, 0., 0., 1., 0., 2 * M_PI);
	//cairo_translate (cr, 0,0);
	//cairo_set_font_size (cr,1);
	//cairo_show_text(cr,"a");
	//cairo_restore (cr);
	//cairo_set_source_rgb(cr, 1, 0, 0);
	//draw_node(cr,0,0);
	//cairo_line_to(cr, drawingAreaWidth, drawingAreaHeight);
	//cairo_stroke(cr);
//draw_line(cr,0,0);
//draw_arrow(cr,-75,0,0,0);

    return FALSE;
}

gboolean button_cb( GtkWidget *widget, GdkEventButton *event, gpointer object )
{
    button_clicked = TRUE;
    nodes = gtk_spin_button_get_value_as_int((GtkSpinButton*)nodeEntry);
    //printf("You entered: %d", i);
    // Explicitly notify the drawing area it should redraw itself
    gtk_widget_queue_draw (myArea);
    //createTable();
	createTable1(0);
	createTableOutputFinal();
    return FALSE;
}

gboolean cargar()
{
    button_clicked = TRUE;
    //printf("You entered: %d", i);
    // Explicitly notify the drawing area it should redraw itself
    gtk_widget_queue_draw (myArea);
    //createTable();
	createTable1(1);
	for (int i = 1; i < nodes + 1; i++) {
		for (int j = 1; j < nodes + 1; j++) {
			state[numTabla].matrizD[i][j] = values[i][j];
			state[numTabla].matrizP[i][j] = valuesP[i][j];
			//g_print("Número: %d\nCadena: %s\n", values[i][j], cadena);
			//gtk_entry_set_text(inputs[0][0], cadena);
		}
	}
	createTableOutputFinal();
    return FALSE;
}

int main(int argc, char *argv[]){
    GtkBuilder *builder; //GTK builder
    gtk_init(&argc, &argv); //start gtk

    builder = gtk_builder_new(); //create gtk ui builder
    gtk_builder_add_from_file(builder, "rutasMasCortas.glade", NULL); //LOAD UI FILE
    
    load_css();
    
    window = GTK_WIDGET(gtk_builder_get_object(builder, "MyWindow")); //load window named MyWindow
	siguiente = GTK_WIDGET(gtk_builder_get_object(builder, "next_button"));
    
    //ASIGN VARIABLES
    gridPaths = GTK_GRID(gtk_builder_get_object(builder, "paths"));
	gridShown = GTK_GRID(gtk_builder_get_object(builder, "tablaValues"));
    
    myArea = GTK_WIDGET(gtk_builder_get_object(builder, "MyArea")); 
    
    nodeEntry = GTK_WIDGET(gtk_builder_get_object(builder, "NodeEntry")); 
    
    gtk_widget_add_events( myArea, GDK_BUTTON_PRESS_MASK );
    gtk_widget_set_size_request( myArea, drawingAreaWidth, drawingAreaHeight);
    g_signal_connect( myArea, "draw", G_CALLBACK(draw_cb), NULL );
    g_signal_connect( nodeEntry, "activate", G_CALLBACK(button_cb), NULL );
    gtk_builder_connect_signals(builder, NULL);
    
    FileChooserWindow = GTK_WIDGET(gtk_builder_get_object(builder, "FileChooserWindow"));

    g_object_unref(builder);

    gtk_widget_show_all(window); //show window
    gtk_main(); //run

    return 0;
}


