CC=gcc
CFLAGS=-Wall -pedantic -Wno-long-long -lpthread -pipe
GTKLIB=`pkg-config --cflags --libs gtk+-3.0`

all: hand

run : 	
	./hand	

compile: hand doc

doc: src/main.c src/lib/Hand.c 
	doxygen Doxyfile

hand: objs/main.o  objs/Hand.o
	$(CC) $(CFLAGS) $^ -o $@ $(GTKLIB) -export-dynamic



objs/main.o: src/main.c src/lib/Hand.c | objs
	$(CC) $(CFLAGS) -c $< $(GTKLIB) -o $@



objs/Hand.o: src/lib/Hand.c   | objs
	$(CC) $(CFLAGS) -c $< -o $@



objs: 
	mkdir objs

clean:	
	rm -rf objs/    hand 2>/dev/null

