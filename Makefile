all: bfc

bfc.o: bfc.c 
	gcc -c bfc.c

bfc: bfc.o
	gcc -o bfc bfc.o

clean:
	rm -f bfc bfc.o
