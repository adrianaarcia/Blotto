CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -g3 -lm

Blotto: blotto.o smap.o entry.o
	${CC} ${CFLAGS} -o $@ $^

Unit: smap_unit.c smap.o smap_test_functions.o
	${CC} ${CFLAGS} -o $@ $^

blotto.o: blotto.c
	${CC} ${CFLAGS} -c blotto.c

entry.o: entry.c entry.h
	${CC} ${CFLAGS} -c entry.c

smap.o: smap.h smap.c 
	${CC} ${CFLAGS} -c smap.c

smap_test_functions.o: smap_test_functions.h smap_test_functions.c 
	${CC} ${CFLAGS} -c smap_test_functions.c

