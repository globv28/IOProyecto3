CC := gcc
CFLAGS := -Wall `pkg-config --cflags gtk+-3.0` -export-dynamic -rdynamic -lm
LIBS := `pkg-config --libs gtk+-3.0`

all: menu pending rutasMasCortas mochila seriesDeportivas

menu: menuController.c
	$(CC) -o menu menuController.c $(CFLAGS) $(LIBS)

pending: pendingController.c
	$(CC) -o pending pendingController.c $(CFLAGS) $(LIBS)
	
rutasMasCortas: rutasMasCortas.c
	$(CC) -o rutasMasCortas rutasMasCortas.c $(CFLAGS) $(LIBS)
	
mochila: mochila.c
	$(CC) -o mochila mochila.c $(CFLAGS) $(LIBS)
	
seriesDeportivas: seriesDeportivas.c
	$(CC) -o seriesDeportivas seriesDeportivas.c $(CFLAGS) $(LIBS)

clean:
	rm -f menu pending rutasMasCortas mochila seriesDeportivas
