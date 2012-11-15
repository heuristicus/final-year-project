INCLUDE_DIR=/usr/include/muParser
LIBDIR=libs/
CFLAGS=-Wall -g -I$(INCLUDE_DIR)
CC=gcc $(CFLAGS)
GEN_SOURCES=generator.c start.c
EST_SOURCES=estimatorIWLS.c
UTIL_SOURCES=file_util.c paramlist.c util.c math_util.c
LIBS=-lm -lmuparser -lgmp
CUST_LIBS=-lfile_util -lparamlist -lmath_util -lgeneral_util
UTIL_OBJECTS=$(UTIL:.c=.o)
GEN_OBJECTS=$(GEN_SOURCES:.c=.o)
EST_OBJECTS=$(EST_SOURCES:.c=.o)

all: generator estimator estimator_piecewise

rungen: generator
	LD_LIBRARY_PATH=. ./generator

runest: estimator
	LD_LIBRARY_PATH=. ./estimator

generator: generator.o start.o libs
	$(CC) generator.o start.o -L. $(CUST_LIBS) $(LIBS) -o $@

generator.o: generator.c generator.h
	$(CC) -c $< -o $@

start.o: start.c generator.h
	$(CC) -c $< -o $@

estimator: estimator_IWLS.o libs
	$(CC) estimator_IWLS.o -L. $(CUST_LIBS) $(LIBS) -o $@

estimator_IWLS.o: estimator_IWLS.c estimator.h
	$(CC) -c $< -o $@

estimator_piecewise: estimator_piecewise.o estimator_IWLS.o libs
	$(CC) estimator_piecewise.o estimator_IWLS.o -L. $(CUST_LIBS) $(LIBS) -o $@

estimator_piecewise.o: estimator_piecewise.c estimator.h
	$(CC) -c $< -o $@

libs: libfile_util.so libparamlist.so libmath_util.so libgeneral_util.so

libfile_util.so: file_util.o
	$(CC) -shared -o $@ $<

libparamlist.so: paramlist.o
	$(CC) -shared -o $@ $<

libmath_util.so: math_util.o
	$(CC) -shared -o $@ $<

libgeneral_util.so: general_util.o
	$(CC) -shared -o $@ $<

file_util.o: file_util.c file_util.h
	$(CC) -c $< -o $@

paramlist.o: paramlist.c paramlist.h
	$(CC) -c $< -o $@

math_util.o: math_util.c math_util.h
	$(CC) -c $< -o $@

general_util.o: general_util.c general_util.h
	$(CC) -c $< -o $@

clean:
	rm -f $(EST_OBJECTS) $(GEN_OBJECTS) $(UTIL_OBJECTS) *.so generator estimator estimator_piecewise