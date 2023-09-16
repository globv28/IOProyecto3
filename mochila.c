#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>

GtkWidget *window;
GtkWidget *cantidadElementos;
GtkWidget *capacidadMochila;
GtkWidget *siguiente; //boton para avanzar las operaciones
GtkGrid *gridPaths;
GtkGrid *gridShown; //tabla final
GtkEntry ***inputs; //matriz para almacenar los punteros a los gtk entry  
GtkFileFilter *filter;
GtkWidget *informacion;
GtkWidget *respuesta;

GtkWidget *cargarButton;

int nElementos;
int nCapacidad;

int xTomada;

gboolean button_clicked = FALSE;
int drawingAreaHeight = 500;
int drawingAreaWidth = 500;
int nodes = 0;
gchar *textNodes;
int nodeSize = 20;//Disminuya para hacer más grande

char *rowLabels[] = {
		"A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
		"K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
		"U", "V", "W", "X", "Y", "Z"
	};

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
int values[50][50];

int valuesP[10][10] = {{0,0,0,0,0,0,0,0}, //matriz de valores para realizar las operaciones
					{0,0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0,0}};

struct {int matrizD[10][10]; int matrizP[10][10];} state [10];

const char *elementos[] = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
    "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
    "U", "V", "W", "X", "Y", "Z",
    "A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1", "I1", "J1",
    "K1", "L1", "M1", "N1", "O1", "P1", "Q1", "R1", "S1", "T1",
    "U1", "V1", "W1", "X1", "Y1", "Z1",
    "A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2", "I2", "J2",
    "K2", "L2", "M2", "N2", "O2", "P2", "Q2", "R2", "S2", "T2",
    "U2", "V2", "W2", "X2", "Y2", "Z2",
    "A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3", "I3", "J3",
    "K3", "L3", "M3", "N3", "O3", "P3", "Q3", "R3", "S3", "T3",
    "U3", "V3", "W3", "X3", "Y3", "Z3"
};

char *capacidades[] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
    "60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
    "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
    "80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
    "90", "91", "92", "93", "94", "95", "96", "97", "98", "99"
	};

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
    g_object_unref(provider);
}

void actualizarLetras(GtkEntry *entry, gpointer user_data) {
    int row = GPOINTER_TO_INT(user_data);

    const gchar* colName = gtk_entry_get_text(entry);
    char* col = g_strdup(colName);
	elementos[row] = col;
	GtkWidget *nameCell = gtk_grid_get_child_at(GTK_GRID(gridShown), row, 0);
	gtk_label_set_text(GTK_LABEL(nameCell), col);
}


void entry_changed(GtkEntry * entry, gpointer user_data) {
	const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    int len = strlen(text);
    int decimal_count = 0;
    gchar filtered_text[len + 1];
    int filtered_index = 0;



    for (int i = 0; i < len; i++) {
        if (isdigit(text[i]) || (text[i] == '.' && decimal_count < 1)) {
            filtered_text[filtered_index++] = text[i];
            if (text[i] == '.') {
                decimal_count++;
            }
        }
    }

    filtered_text[filtered_index] = '\0';
    gtk_entry_set_text(GTK_ENTRY(entry), filtered_text);
}

void cleanTable1(){
	for (int i = 0; i < nElementos + 1; i++) { //limpia la tabla que se va a ctualizando
		gtk_grid_remove_row(gridPaths, 0);
    }
}
void cleanTable2(){ //limpia la tabla del display de la respuesta final
	for (int i = 0; i < nCapacidad + 2; i++) {
		gtk_grid_remove_row(gridShown, 0);
    }
}

void createTable2() {
	GtkWidget *label = gtk_label_new ("Capacidad");
	gtk_widget_set_name(label, "neutro");

	gtk_grid_attach(gridShown, label, 0, 0, 1, 1);
	for (int i = 1; i < nElementos + 1; i++) {
		GtkWidget *label = gtk_label_new (elementos[i - 1]);
		gtk_widget_set_name(label, "neutro");
		gtk_grid_attach(gridShown, label, i, 0, 1, 1);
    }
    for (int i = 1; i < nCapacidad + 2; i++) {
		GtkWidget *label = gtk_label_new (capacidades[i - 1]);
		gtk_widget_set_name(label, "neutro");
		gtk_grid_attach(gridShown, label, 0, i, 1, 1);
		for (int j = 1; j < nElementos + 1; j++) {
			GtkWidget *entry = gtk_button_new();
			gtk_widget_set_name(entry, "neutro");
			gtk_grid_attach(gridShown, entry, j, i, 1, 1);
		}
    }
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

//crea la tabla inicial para ingresar las distancias iniciales entre los nodos
void createTable1(){
	char *columnLabels[] = {"Elemento", "Valor", "Costo", "Cantidad disponible"};
	for (int i = 0; i < 4; i++) {
		GtkWidget *label = gtk_label_new (columnLabels[i]);
		gtk_widget_set_name(label, "neutro");
		gtk_grid_attach(gridPaths, label, i, 0, 1, 1);
    }	

    for (int i = 1; i < nElementos + 1; i++) {
		GtkWidget *entry = gtk_entry_new();
		gtk_widget_set_name(entry, "neutro");
		gtk_entry_set_text((GtkEntry*)entry, elementos[i-1]);
		gtk_grid_attach(gridPaths, entry, 0, i, 1, 1);
		g_signal_connect(entry, "changed", G_CALLBACK(actualizarLetras), GINT_TO_POINTER(i));
		for (int j = 1; j < 4; j++) {
			GtkWidget *entry = gtk_entry_new();
			gtk_widget_set_name(entry, "neutro");
			gtk_grid_attach(gridPaths, entry, j, i, 1, 1);
			g_signal_connect(entry, "changed", G_CALLBACK(entry_changed), &j);
		}
    }

	
}

void setear_informacion();


gboolean cargar()
{
    button_clicked = TRUE;
	cleanTable1();
	cleanTable2();
	createTable1();
    gtk_widget_show_all(window);
	for (int i = 1; i <= nElementos; i++) {
		for (int j = 1; j <= 3; j++) {
			GtkWidget *cell = gtk_grid_get_child_at(GTK_GRID(gridPaths), j, i);
			gtk_entry_set_text(GTK_ENTRY(cell), i_to_s(values[i-1][j-1]));
		}
	}
    gtk_widget_show_all(window);
	gtk_spin_button_set_value((GtkSpinButton*)cantidadElementos, nElementos);
	gtk_spin_button_set_value((GtkSpinButton*)capacidadMochila, nCapacidad);
	
	gtk_widget_set_sensitive(GTK_WIDGET(capacidadMochila), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(cantidadElementos), FALSE);
	//aqui
	gtk_widget_set_sensitive(cargarButton, FALSE);
	//----
	createTable2();
	setear_informacion();
    return FALSE;
}

char* matrizToString();

void guardar_tabla(){
	char *string = matrizToString();
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
    if (fp == NULL){
        exit(EXIT_FAILURE);
	}
	int n = -1;
    while ((read = getline(&line, &len, fp)) != -1) {
		char*arrays = strtok(line, " ");
		int j = 0;
		while (arrays != NULL){
			if(n==-1){
			    nElementos = atoi(arrays);
    			nCapacidad = atoi(strtok(NULL, " "));
				break;
			}
			values[n][j] = atoi(arrays);
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
    gtk_file_filter_add_pattern(filter, "*.tbl");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(FileChooserWindow), filter);
    gtk_dialog_run (GTK_DIALOG (FileChooserWindow));
}



void exit_app(){
    gtk_main_quit();
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

char* matrizToString() {
    char* resultado = concatenarTresCadenas(i_to_s(nElementos), " ", i_to_s(nCapacidad));
	resultado = concatenarTresCadenas(resultado, "\n", "");
	for(int i = 1; i < nElementos + 1; i++){
		GtkWidget *costCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), 2, i);
		const char* constTextCost = gtk_entry_get_text(GTK_ENTRY(costCell));
		char* textCost = g_strdup(constTextCost);

		GtkWidget *valueCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), 1, i);
		const gchar* constTextValue2 = gtk_entry_get_text(GTK_ENTRY(valueCell));
		char* textValue2 = g_strdup(constTextValue2);

		GtkWidget *availableCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), 3, i);
		const gchar* availableConst = gtk_entry_get_text(GTK_ENTRY(availableCell));
		char* available = g_strdup(availableConst);

		resultado = concatenarTresCadenas(resultado, textValue2, " ");
		resultado = concatenarTresCadenas(resultado, textCost, " ");
		resultado = concatenarTresCadenas(resultado, available, " ");
		if(i != nElementos){
			resultado = concatenarTresCadenas(resultado, "", "\n");
		}
	}
    return resultado;
}

float s_to_f(char *str) {
    char *endptr;
    float result = strtof(str, &endptr);


    if (*endptr != '\0' && *endptr != '\n') {
        printf("Error: No se pudo convertir la cadena a float.\n");
    }

    return result;
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

int obtener_numero_entero(const char *cadena) {

    const char *flecha = strstr(cadena, " -> ");

    if (flecha == NULL) {
        return atoi(cadena);
    }


    int longitud_numero = flecha - cadena;


    char numero_str[longitud_numero + 1]; 
    strncpy(numero_str, cadena, longitud_numero);
    numero_str[longitud_numero] = '\0'; 


    int numero = atoi(numero_str);

    return numero;
}


float obtenerValorCelda(int columna, int capacidadActual){
	int fila = capacidadActual + 1;
	GtkWidget *cell = gtk_grid_get_child_at(GTK_GRID(gridShown), columna-1, fila);
	const gchar* constTextValue = gtk_button_get_label(GTK_BUTTON(cell));
	char* textValue = i_to_s(obtener_numero_entero(g_strdup(constTextValue)));
	float valorViejo = s_to_f(textValue);

	GtkWidget *availableCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), 3, columna);
	const char* constTextAvailable = gtk_entry_get_text(GTK_ENTRY(availableCell));
	char* textAvailable = g_strdup(constTextAvailable);
	float disponibles = s_to_f(textAvailable);

	GtkWidget *costCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), 2, columna);
	const char* constTextCost = gtk_entry_get_text(GTK_ENTRY(costCell));
	char* textCost = g_strdup(constTextCost);
	float costo = s_to_f(textCost);

	GtkWidget *valueCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), 1, columna);
	const gchar* constTextValue2 = gtk_entry_get_text(GTK_ENTRY(valueCell));
	char* textValue2 = g_strdup(constTextValue2);
	float valorNuevo = s_to_f(textValue2);

	xTomada = 0;

	float valorMayor = valorViejo;

	for(int i = 1; i <= disponibles && i*costo <= capacidadActual; i++){
		GtkWidget *cell3 = gtk_grid_get_child_at(GTK_GRID(gridShown), columna - 1, fila - costo*i);
		const gchar* constTextValue3 = gtk_button_get_label(GTK_BUTTON(cell3));
		char* textValue3 = i_to_s(obtener_numero_entero(g_strdup(constTextValue3)));
		float restante = s_to_f(textValue3);

		float valor = valorNuevo * i;

		if(valorMayor < valor + restante){
			valorMayor = valor + restante;
			xTomada = i;
		}
	}

	return valorMayor;
}


//rellena la tabla final
void updateTable() {
	numTabla++;


	GtkWidget *nameCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), 0, numTabla);
	const gchar* constName = gtk_entry_get_text(GTK_ENTRY(nameCell));
	char* name = g_strdup(constName);

	GtkWidget *valueCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), 1, numTabla);
	const gchar* constTextValue = gtk_entry_get_text(GTK_ENTRY(valueCell));
	char* textValue = g_strdup(constTextValue);
	float valor = s_to_f(textValue);

	GtkWidget *costCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), 2, numTabla);
	const char* constTextCost = gtk_entry_get_text(GTK_ENTRY(costCell));
	char* textCost = g_strdup(constTextCost);
	float costo = s_to_f(textCost);

	GtkWidget *availableCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), 3, numTabla);
	const char* constTextAvailable = gtk_entry_get_text(GTK_ENTRY(availableCell));
	char* textAvailable = g_strdup(constTextAvailable);
	float disponibles = s_to_f(textAvailable);


	if (numTabla == 1)
	{
		for(int i = 1; i < nCapacidad + 2; i++){
			valor = s_to_f(textValue);
			GtkWidget *cell = gtk_grid_get_child_at(GTK_GRID(gridShown), numTabla, i);
			if((int)((i-1)/costo) == 0){
				char* valorAEscribir = concatenarTresCadenas("0 -> X", name, " = 0");
				gtk_button_set_label(GTK_BUTTON(cell), valorAEscribir);
				gtk_widget_set_name(cell, "rojo");
			}
			else{
				int multiplicado = (((int)((i-1) / costo)) <= disponibles)? ((int)((i-1) / costo)): disponibles;
				valor = valor * multiplicado;
				char* valorAEscribir = concatenarTresCadenas(f_to_s(valor)," -> X", name);
				valorAEscribir = concatenarTresCadenas(valorAEscribir," = ", i_to_s(multiplicado));
				gtk_button_set_label(GTK_BUTTON(cell), valorAEscribir);
				gtk_widget_set_name(cell, "verde");
			}
		}
	}
	else{
		for(int i = 1; i < nCapacidad + 2; i++){
			GtkWidget *cell = gtk_grid_get_child_at(GTK_GRID(gridShown), numTabla, i);
			if(costo > i-1){
				GtkWidget *cell2 = gtk_grid_get_child_at(GTK_GRID(gridShown), numTabla - 1, i);
				const char* constValorNuevo = gtk_button_get_label(GTK_BUTTON(cell2));
				char* valorNuevo = i_to_s(obtener_numero_entero(g_strdup(constValorNuevo)));
				char* valorAEscribir = concatenarTresCadenas(valorNuevo," -> X", name);
				valorAEscribir = concatenarTresCadenas(valorAEscribir," = ", i_to_s(0));
				gtk_button_set_label(GTK_BUTTON(cell), valorAEscribir);
				gtk_widget_set_name(cell, "rojo");
			}else{
				char* valorNuevo = f_to_s(obtenerValorCelda(numTabla, i-1));
				char* valorAEscribir = concatenarTresCadenas(valorNuevo," -> X", name);
				valorAEscribir = concatenarTresCadenas(valorAEscribir," = ", i_to_s(xTomada));
				gtk_button_set_label(GTK_BUTTON(cell), valorAEscribir);
				if(xTomada != 0){
					gtk_widget_set_name(cell, "verde");
				}
				else{
					gtk_widget_set_name(cell, "rojo");
				}
			}
		}
	}
	


    gtk_widget_show_all(window);
}

char *transformar_cadena(const char *cadena) {

    const char *x = strchr(cadena, 'X');

    if (x == NULL) {
        printf("Error: Formato incorrecto de cadena.\n");
        exit(EXIT_FAILURE);
    }

    x++;

    const char *igual = strstr(cadena, " = ");

    if (igual == NULL) {
        printf("Error: Formato incorrecto de cadena.\n");
        exit(EXIT_FAILURE);
    }

    int longitud_i = igual - x;

    char i[50]; 
    strncpy(i, x, longitud_i);
    i[longitud_i] = '\0'; 

    igual += 3;


    char resultado[100]; 
	snprintf(resultado, sizeof(resultado), "%s = %s", i, igual);

    char *cadena_transformada = malloc(strlen(resultado) + 1);
    if (cadena_transformada == NULL) {
        perror("Error al asignar memoria para la cadena");
        exit(EXIT_FAILURE);
    }

    strcpy(cadena_transformada, resultado);
	cadena_transformada = concatenarTresCadenas("X", cadena_transformada,"");

    return cadena_transformada;
}

int obtener_valor_j(const char *cadena) {

    const char *igual = strstr(cadena, " = ");


    if (igual == NULL) {
        printf("Error: Formato incorrecto de cadena.\n");
        exit(EXIT_FAILURE);
    }

    igual += 3;


    int j = atoi(igual);

    return j;
}

void obtenerRespuesta(){
	int filaActual = nCapacidad + 1;
	int columnaActual = nElementos;
	bool z = false;
	char* r = "";
	while(filaActual > 0 && columnaActual > 0){
		GtkWidget* cell = gtk_grid_get_child_at(GTK_GRID(gridShown), columnaActual, filaActual);
		const char* constValorNuevo = gtk_button_get_label(GTK_BUTTON(cell));
		char*valorNuevo = g_strdup(constValorNuevo);
		const char* constCeldaActual = gtk_widget_get_name(cell);
		char*celdaActual = g_strdup(constCeldaActual);
		if(strcmp(celdaActual,"rojo") == 0){
			columnaActual--;
		}else{
		    GtkWidget *costCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), 2, columnaActual);
			const char* constTextCost = gtk_entry_get_text(GTK_ENTRY(costCell));
			char* textCost = g_strdup(constTextCost);
			float costo = s_to_f(textCost);
			filaActual -= costo * obtener_valor_j(valorNuevo);
			if(!z){
				z = true;
				r = concatenarTresCadenas("Z = ", i_to_s(obtener_numero_entero(valorNuevo)),"\n");
			}
			columnaActual--;
			valorNuevo = transformar_cadena(valorNuevo);
			r = concatenarTresCadenas(r, valorNuevo, "\n");
		}
	}
	gtk_label_set_text(GTK_LABEL(respuesta), r);
	gtk_widget_set_name(respuesta, "neutro");
}

void setear_informacion(){
	char* restricciones = "\n Sujeto a:\n\t" ;
	char* valores = "Maximizar\n\tZ = ";
	char* xs = "\n";
	for (int i = 1; i < nElementos+1; i++)
	{
		GtkWidget *nameCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), 0, i);
		const gchar* constName = gtk_entry_get_text(GTK_ENTRY(nameCell));
		char* name = g_strdup(constName);

		GtkWidget *costCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), 2, i);
		const char* constTextCost = gtk_entry_get_text(GTK_ENTRY(costCell));
		char* textCost = g_strdup(constTextCost);

		GtkWidget *valueCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), 1, i);
		const gchar* constTextValue2 = gtk_entry_get_text(GTK_ENTRY(valueCell));
		char* textValue2 = g_strdup(constTextValue2);

		GtkWidget *availableCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), 3, i);
		const gchar* availableConst = gtk_entry_get_text(GTK_ENTRY(availableCell));
		char* available = g_strdup(availableConst);

		restricciones = concatenarTresCadenas(restricciones, textCost, "X");
		restricciones = concatenarTresCadenas(restricciones, name, "");


		valores = concatenarTresCadenas(valores, textValue2, "X");
		valores = concatenarTresCadenas(valores, name, "");

		xs = concatenarTresCadenas(xs, "\t0 <= X", name);
		xs = concatenarTresCadenas(xs, " <= ", available);
		xs = concatenarTresCadenas(xs, "\n","");

		if(i != nElementos){
			valores = concatenarTresCadenas(valores, "", " + ");
			restricciones = concatenarTresCadenas(restricciones, "", " + ");
		}
	}
	restricciones = concatenarTresCadenas(restricciones, " <= ", i_to_s(nCapacidad));
	gtk_label_set_text(GTK_LABEL(informacion), concatenarTresCadenas(valores, restricciones, xs));
	gtk_widget_set_name(informacion, "neutro");
	
}

void deactivateTable1(){
    for (int i = 1; i < nElementos + 1; i++) {
		for (int j = 0; j < 4; j++) {
			GtkWidget *inputCell = gtk_grid_get_child_at(GTK_GRID(gridPaths), j, i);
			gtk_widget_set_sensitive(inputCell, FALSE);
		}
	}
}
void on_siguiente_clicked(){
	if(numTabla == 0){
		deactivateTable1();
		gtk_widget_set_sensitive(GTK_WIDGET(capacidadMochila), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(cantidadElementos), FALSE);
	}
	if(numTabla < nElementos){
		setear_informacion();
		updateTable();
	}
}

void mostrarRespuestaFinal(){
	for (int i = 0; i < nElementos; i++)
	{
		on_siguiente_clicked();
	}
	obtenerRespuesta();
}

void returnTable() {
    gtk_widget_show_all(window);
}

void on_anterior_clicked(){
	if(numTabla >= 1){
		returnTable();
	}
}



gboolean actualizarFilas( GtkWidget *widget, GdkEventButton *event, gpointer object )
{
    button_clicked = TRUE;
	cleanTable1();
    cleanTable2();
    nCapacidad = gtk_spin_button_get_value_as_int((GtkSpinButton*) capacidadMochila);
    nElementos = gtk_spin_button_get_value_as_int((GtkSpinButton*) cantidadElementos);
	createTable1();
	createTable2();
    gtk_widget_show_all(window);
    return FALSE;
}

gboolean actualizarColumnas( GtkWidget *widget, GdkEventButton *event, gpointer object )
{
    button_clicked = TRUE;
	cleanTable1();
    cleanTable2();
    nCapacidad = gtk_spin_button_get_value_as_int((GtkSpinButton*) capacidadMochila);
    nElementos = gtk_spin_button_get_value_as_int((GtkSpinButton*) cantidadElementos);
	createTable1();
	createTable2();
    gtk_widget_show_all(window);

    return FALSE;
}

int main(int argc, char *argv[]){
    GtkBuilder *builder; //GTK builder
    gtk_init(&argc, &argv); //start gtk

    builder = gtk_builder_new(); //create gtk ui builder
    gtk_builder_add_from_file(builder, "mochila.glade", NULL); //LOAD UI FILE
    
    load_css();
    
    window = GTK_WIDGET(gtk_builder_get_object(builder, "MyWindow")); //load window named MyWindow
	siguiente = GTK_WIDGET(gtk_builder_get_object(builder, "next_button"));

    
    //ASIGN VARIABLES
    gridPaths = GTK_GRID(gtk_builder_get_object(builder, "paths"));
	gridShown = GTK_GRID(gtk_builder_get_object(builder, "tablaValues"));
    
    cantidadElementos = GTK_WIDGET(gtk_builder_get_object(builder, "CantidadElementos")); 
	capacidadMochila = GTK_WIDGET(gtk_builder_get_object(builder, "Capacidad"));


	informacion = GTK_WIDGET(gtk_builder_get_object(builder, "Informacion")); 
	respuesta = GTK_WIDGET(gtk_builder_get_object(builder, "Respuesta")); 
    
	nElementos = 1;
	nCapacidad = 1;
	
	cargarButton = GTK_WIDGET(gtk_builder_get_object(builder, "Cargar")); 

    g_signal_connect( cantidadElementos, "activate", G_CALLBACK(actualizarColumnas), NULL );
	g_signal_connect( capacidadMochila, "activate", G_CALLBACK(actualizarFilas), NULL );

    gtk_builder_connect_signals(builder, NULL);
    
    FileChooserWindow = GTK_WIDGET(gtk_builder_get_object(builder, "FileChooserWindow"));

    g_object_unref(builder);



    gtk_widget_show_all(window); //show window
    gtk_main(); //run

    return 0;
}


