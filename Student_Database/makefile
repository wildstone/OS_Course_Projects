all: load query change print clean

load: header.c load.c
	gcc -o load header.c load.c
query: header.c query.c
	gcc -o query header.c query.c
change: header.c change.c
	gcc -o change header.c change.c
print: header.c print.c
	gcc -o print header.c print.c
clean: header.c print.c
	gcc -o clean header.c clean.c
