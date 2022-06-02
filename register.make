register: register.o socket-utils.o
	cc register.o socket-utils.o -o register

register.o: register.c register.h socket-utils.h
	cc -c register.c

socket-utils.o: socket-utils.c socket-utils.h
	cc -c socket-utils.c
