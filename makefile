main: main.c main.h
	cc main.c -o main

register: register.o socket-utils.o 
	cc register.o socket-utils.o -o register

register.o: register.c register.h socket-utils.h
	cc -c register.c

train: train.o socket-utils.o
	cc train.o socket-utils.o -o train

train.o: train.c train.h socket-utils.h
	cc -c train.c

socket-utils.o: socket-utils.c socket-utils.h
	cc -c socket-utils.c
