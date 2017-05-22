MPICC=mpicc

CFLAGS=-O3 -I./stdio_streams/ -Wl,-rpath,$(PWD)/stdio_streams/
LDFLAGS= -lm -L$(PWD)/stdio_streams/  -lmpistream

TARGET= bin/mandel bin/mem bin/sinus

all : $(TARGET)

libmpistream.so :
	cd ./stdio_streams/ && make

bin/mandel : ./examples/mandel.c libmpistream.so
	$(MPICC) $(CFLAGS) $(LDFLAGS) ./examples/mandel.c -o $@


bin/mem : ./examples/mem.c libmpistream.so
	$(MPICC) $(CFLAGS) $(LDFLAGS) ./examples/mem.c -o $@


bin/sinus : ./examples/sinus.c libmpistream.so
	$(MPICC) $(CFLAGS) $(LDFLAGS) ./examples/sinus.c -o $@

clean:
	rm -fr $(TARGET)
	cd ./stdio_streams/ && make clean
