all: writer.c reader.c
	gcc -Wall writer.c -o writer
	gcc -Wall reader.c -o reader
