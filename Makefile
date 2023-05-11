CC = gcc
WALL = -Wall 

all: system_functions cpu_functions user_functions print

print: print.o
	${CC} ${WALL} -o print print.o -lm

print.o: print.c
	${CC} ${WALL} -c -o print.o print.c 

cpu_functions: cpu_functions.o
	${CC} ${WALL} -o cpu_functions cpu_functions.o

cpu_functions.o: cpu_functions.c cpu_functions.h
	${CC} ${WALL} -c -o cpu_functions.o cpu_functions.c

system_functions: system_functions.o
	${CC} ${WALL} -o system_functions system_functions.o

system_functions.o: system_functions.c system_functions.h
	${CC} ${WALL} -c -o system_functions.o system_functions.c

user_functions: user_functions.o
	${CC} ${WALL} -o user_functions user_functions.o

user_functions.o: user_functions.c user_functions.h
	${CC} ${WALL} -c -o user_functions.o user_functions.c

clean:
	rm -f system_functions.o system_functions user_functions.o user_functions cpu_functions.o cpu_functions print.o print