#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpistream.h"
#include <sys/resource.h>
#include <stdio.h>
#include <sys/sysinfo.h>
#include <sys/time.h>

double _getts()
{
	struct timeval tv;
	gettimeofday( &tv, NULL );

	return (double)tv.tv_sec + (double)tv.tv_usec * 1e-6;
}


double getts()
{
	static double ref = 0.0;
	if( ref == 0.0 )
		ref = _getts();

	return _getts() - ref;
}




static MPI_Stream mem_s;
int rank;

void memstream_init() {
	MPIX_Stream_init( MPI_COMM_WORLD, "mem", 1024,  &mem_s );
	MPIX_Stream_map( mem_s, "stdout");

	if( !rank ) {
		MPIX_Stream_printf( mem_s , "mem: new:LOG label:Mem markdown:1");
		MPIX_Stream_printf( mem_s , "mem: commit:");
		MPIX_Stream_printf( mem_s , "memtable: new:TABLE label:Mem time:TS mem:Free_Mem");
		MPIX_Stream_printf( mem_s , "memplot: new:PLOT x:time y:Free_Memory(MB) label:\"Memory over Time\"");
	}
}

void memstream_release() {

	if( !rank ) {
		MPIX_Stream_printf( mem_s , "mem: log:\"# Memory Plot\"");
		MPIX_Stream_printf( mem_s , "mem: emit:memplot");
		MPIX_Stream_printf( mem_s , "mem: log:\"# Memory Table\"");
		MPIX_Stream_printf( mem_s , "mem: emit:memtable");
	}
	
	MPIX_Stream_release( &mem_s );
}

static double getmem()
{
	struct sysinfo si;
 	sysinfo (&si);

	return si.freeram / (1024.0 * 1024.0);
}


void memstream_log()
{
	double mem = getmem();
	double now = getts();


	if( !rank )
	{
		MPIX_Stream_printf( mem_s , "memtable: time:%g mem:%g", now, mem);
		MPIX_Stream_printf( mem_s , "memplot: x:%g y:%g", now, mem);
	}
}




int main( int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank );


	memstream_init();


	int i;

	for (i = 0; i < 1024; ++i) {
		
		double s =  fabs( 5* 1024 * 1024.0 * 1024.0 * sin(i/64.0) );
		void * data = malloc( s );

		memset( data , 0 , s );

		memstream_log();

	fprintf(stderr, "%d\n", i);
		free( data );

		usleep(500);
	}




	memstream_release();


	MPI_Finalize();

	return 0;
}
