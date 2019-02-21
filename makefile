all: writer.c reader.c
	clear
	gcc -Wall writer.c -o writer
	gcc -Wall reader.c -o reader
