CC=gcc
CFLAGS=-Wall -g
SOURCES=math_functions.c p_func.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=poisson

all: poisson

poisson: $(OBJECTS)
	$(CC) $(CFLAGS) $(SOURCES) -lm

math: math_functions.o
	$(CC) $(CFLAGS) math_functions.c -lm


output: output_data.o
	$(CC) $(CFLAGS) output_data.c

pfunc: p_func.o
	$(CC) $(CFLAGS) p_func.c -lm

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)