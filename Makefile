CC=gccx

all: encode\
	decode\
   
encode: encode.c
	$(CC) -o encode encode.c  
decode: decode.c
	$(CC) -o decode decode.c

clean:
	rm -f decode *.o
	rm -f encode *.o
