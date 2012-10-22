CC=gcc
CFLAGS=-Wall -g -I/usr/include/muParser
P_SOURCES=math_functions.c generator.c file_util.c paramlist.c start.c
E_SOURCES=estimatorIWLS.c util.c math_functions.c
LIBS=-lm -lmuparser
P_OBJECTS=$(P_SOURCES:.c=.o)
E_OBJECTS=$(E_SOURCES:.c=.o)
GENERATOR=generator
ESTIMATOR=estimator

all: generator estimator

generator: $(P_OBJECTS)
	$(CC) $(CFLAGS) $(P_SOURCES) $(LIBS) -o $(GENERATOR)

estimator: $(E_OBJECTS)
	$(CC) $(CFLAGS) $(E_SOURCES) $(LIBS) -o $(ESTIMATOR)
clean:
	rm -rf $(E_OBJECTS) $(P_OBJECTS) $(GENERATOR) $(ESTIMATOR)