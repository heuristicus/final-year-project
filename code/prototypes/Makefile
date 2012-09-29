CC=gcc
CFLAGS=-Wall -g -I/usr/include/muParser
SOURCES=math_functions.c p_func.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=poisson

all: poisson

poisson: $(OBJECTS)
	$(CC) $(CFLAGS) $(SOURCES) -lm -lmuparser

math: math_functions.o
	$(CC) $(CFLAGS) math_functions.c


output: output_data.o
	$(CC) $(CFLAGS) output_data.c

pfunc: p_func.o
	$(CC) $(CFLAGS) p_func.c

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)