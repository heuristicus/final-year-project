CC=gcc
GEN_EXEC=generator
EST_EXEC=estimator
CFLAGS=-Wall -g -I/usr/include/muParser
SRCDIR=./src/
BUILDDIR=./build/
LIBDIR=./lib/
LIBS=-lm -lmuparser

GEN_SRC=math_functions.c generator.c file_util.c paramlist.c start.c
EST_SRC=estimatorIWLS.c util.c math_functions.c

GEN_OBJECTS=$(PSRC:.c=.o)
EST_OBJECTS=$(ESRC:.c=.o)

# Append the directory to the filenames
C_GEN_SRC=$(addprefix $(SRCDIR),$(GEN_SRC))
C_EST_SRC=$(addprefix $(SRCDIR),$(EST_SRC))
C_GEN_OBJECTS=$(addprefix $(BUILDDIR),$(GEN_OBJECTS))
C_EST_OBJECTS=$(addprefix $(BUILDDIR),$(EST_OBJECTS))

all: generator estimator

generator: $(GEN_OBJECTS)
	$(CC) $(CFLAGS) $(C_GEN_SRC) $(LIBS) -o $(GEN_EXEC) -I$(LIBDIR)

estimator: $(EST_OBJECTS)
	$(CC) $(CFLAGS) $(C_EST_SRC) $(LIBS) -o $(EST_EXEC) -I$(LIBDIR)

clean:
	rm -rf $(C_EST_OBJECTS) $(C_GEN_OBJECTS) $(GEN_EXEC) $(EST_EXEC)