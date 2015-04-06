CC = g++
CFLAGS = -g -DGL_GLEXT_PROTOTYPES -Iglut-3.7.6-bin
LDFLAGS = -lglut -lGLU -lGL

FILES = 

RM = /bin/rm -f 

all: main 
main: main.o 
	$(CC) $(CFLAGS) -o as3 main.o $(LDFLAGS) 
main.o: main.cpp $(FILES)
	$(CC) $(CFLAGS) -c main.cpp $(FILES) -o main.o 
clean: 
	$(RM) *.o *~ as3
