
VPATH = src:obj:bin

all: dir main bin/train bin/register bin/rbc

dir: 
	mkdir -p bin obj log

main: main.c main.h
	cc $< -o $@

bin/train: train.o socket-utils.o dir
	cc obj/train.o obj/socket-utils.o -o $@

bin/register: register.o socket-utils.o dir 
	cc obj/register.o obj/socket-utils.o -o $@

bin/rbc: rbc.o socket-utils.o dir 
	cc obj/rbc.o obj/socket-utils.o -o $@

obj/train.o: train.c train.h socket-utils.c dir
	cc -c $< -o $@

obj/register.o: register.c register.h socket-utils.c dir
	cc -c $< -o $@

obj/rbc.o: rbc.c rbc.h socket-utils.c dir
	cc -c $< -o $@

obj/socket-utils.o: socket-utils.c socket-utils.h dir
	cc -c $< -o $@

clean:
	rm -rdf main obj bin
	rm -rdf assets/MA* assets/log



