train: train.o socket-utils.o
	cc train.o socket-utils.o -o train

train.o: train.c train.h socket-utils.h
	cc -c train.c

socket-utils.o: socket-utils.c socket-utils.h
	cc -c socket-utils.c
