#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#include "mpistream.h"

int main(int argc, char ** argv )
{
	int l;
	MPI_Init_thread( &argc, &argv, MPI_THREAD_MULTIPLE, &l);

	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size );
	
	if( size != 1 )
	{
		fprintf(stderr, "Run this test on a single process\n");
		return 1;
	}


	MPI_Stream s;
	MPIX_Stream_init( MPI_COMM_WORLD, "out",  1024, &s );

	MPIX_Stream_map( s, "stdout");

	
	double i;

	MPIX_Stream_printf(s,"o1: new:LOG label:main markdown:1" );
	MPIX_Stream_printf(s,"o1: commit:" );
	MPIX_Stream_printf(s,"p1: new:PLOT x:time y:sin label:sinus" );
	MPIX_Stream_printf(s,"t1: new:TABLE x:time y:sin label:sinus" );

	for( i = 0.0 ; i < 10.0 ; i= i + 0.05 )
	{
		MPIX_Stream_printf(s,"p1: x:%g y:%g", i, sin(i));
		MPIX_Stream_printf(s,"t1: x:%g y:%g", i, sin(i));
	}

	MPIX_Stream_printf(s,"o1: log:\"# Plot of Sinus\"");
	MPIX_Stream_printf(s,"o1: emit:p1" );
	
	MPIX_Stream_printf(s,"o1: log:\"# Table of Sinus\"");
	MPIX_Stream_printf(s,"o1: emit:t1" );

	MPIX_Stream_release(&s);

	MPI_Finalize();

	return 0;
}
