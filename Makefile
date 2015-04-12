CC = g++
CFLAGS = -g -DGL_GLEXT_PROTOTYPES -Iglut-3.7.6-bin
LDFLAGS = -lglut -lGLU -lGL -lGLEW

FILES = shader.cpp keylistener.cpp mesh.cpp

RM = /bin/rm -f 

all: main

main: clean main.cpp $(FILES)
	$(CC) $(CFLAGS) -o as3 main.cpp $(FILES) $(LDFLAGS) 

clean: 
	$(RM) *.o *~ as3
