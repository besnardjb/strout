MPICC=mpicc
CFLAGS=-O3 -fpic
LDFLAGS=

TARGET=libmpistream.so test_print

all : $(TARGET)


libmpistream.so : mpistream.c mpistream.h
	$(MPICC) $(CFLAGS) $(LDFLAGS) -shared mpistream.c -o $@

test_print : main.c libmpistream.so
	$(MPICC) $(CFLAGS) $^ -o $@ -Wl,-rpath,$(PWD) -L. -lmpistream

clean:
	rm -fr $(TARGET)


