CFLAGS = -Wall
LDFLAGS = -lm

#Allegro directives for compilation
#`pkg-config --cflags --libs allegro-5.0 allegro_primitives-5.0 allegro_font-5.0 allegro_audio-5.0 allegro_ttf-5.0 allegro_image-5.0 allegro_acodec-5.0` -lm
CALLEGRO = `pkg-config --libs --cflags allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5`


#Object files
objects = boulderdash.o libgamech2.o libentities2.o
#source files
cfiles = boulderdash.c libgamech2.c libentities2.c
#header files
hfiles = libgamech2.h libentities2.h

all:
	clear
	gcc -c $(CFLAGS) $(cfiles)
	gcc $(objects) $(CFLAGS) $(LDFLAGS) -o bdash $(CALLEGRO)
clean:
	-rm -f $(objects) *~

purge:
	-rm -f $(objects) bdash *~
