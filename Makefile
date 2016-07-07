all: bfc

disassemble:
	objdump -b binary --start-address=0x54 -m i386  -Mintel,i386 -D ./a.out

bfc.o: bfc.c 
	gcc -c -g bfc.c

bfc: bfc.o
	gcc -g -o bfc bfc.o

clean:
	rm -f bfc bfc.o
