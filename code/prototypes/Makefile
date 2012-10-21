CC=gcc
CFLAGS=-Wall -g -I/usr/include/muParser
SOURCES=math_functions.c generator.c file_util.c paramlist.c start.c
LIBS=-lm -lmuparser
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=poisson

all: poisson $(EXECUTABLE)

poisson: $(OBJECTS)
	$(CC) $(CFLAGS) $(SOURCES) $(LIBS) -o $@

math: math_functions.o
	$(CC) $(CFLAGS) math_functions.c


pfunc: p_func.o
	$(CC) $(CFLAGS) poisson_generator.c

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)