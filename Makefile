CFLAGS=-ansi -Wpedantic -Wall -Werror -D_THREAD_SAFE -D_REENTRANT -D_POSIX_C_SOURCE=200112L
LIBRARIES=-lpthread 
LFLAGS=

all: legionari.exe

legionari.exe: legionari.o DBGpthread.o
	gcc ${LFLAGS} -o legionari.exe legionari.o DBGpthread.o ${LIBRARIES}

legionari.o: legionari.c DBGpthread.h
	gcc -c ${CFLAGS} legionari.c 

DBGpthread.o: DBGpthread.c printerror.h
	gcc -c ${CFLAGS} DBGpthread.c

.PHONY: clean run

clean: 
	rm -f *.exe *.o *~ core

run: legionari.exe
	./legionari.exe   


