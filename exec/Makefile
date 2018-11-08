all: pgm1 pgm2 exec_pgm

pgm1.o: pgm1.c
	gcc -c pgm1.c
pgm1: pgm1.o
	gcc -o pgm1 pgm1.o
pgm2.o: pgm2.c
	gcc -c pgm2.c
pgm2: pgm2.o
	gcc -o pgm2 pgm2.o
exec_pgm.o: exec_pgm.c
	gcc -c exec_pgm.c
exec_pgm: exec_pgm.o
	gcc -o exec_pgm exec_pgm.o
clean:
	rm pgm1 pgm2 exec_pgm *.o
