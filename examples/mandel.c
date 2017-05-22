#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>

#include "mpistream.h"

#define SIZEX 1000
#define SIZEY 1000
#define ITER 64

static int cnt = 0;


int main( int argc ,char ** argv )
{
	int l;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &l );

	MPI_Stream s;
	MPIX_Stream_init( MPI_COMM_WORLD, "m", 1024, &s);
	MPIX_Stream_map( s, "stdout");


	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank );
	MPI_Comm_size(MPI_COMM_WORLD, &size );
	int x,y;

	int qr = (SIZEX/size);
	const double qx = 3.0/SIZEX, qy = 2.0/SIZEY;

	if(rank == 0)
	{
		MPIX_Stream_printf(s,"im1: new:IMG sx:%d sy:%d label:mandel",  SIZEX, SIZEY );
	}

	MPI_Barrier(MPI_COMM_WORLD);

	for( x = qr * rank ; x < qr*(rank+1) ; x++ ) {
		for(y = 0 ; y < SIZEY; y++ ) {
			double it = 0;
			double complex c = (-2.0 + x*qx) + (-1.0 + y*qy)*I, z = 0;
			while( (++it  < ITER) && (cabs(z) < 2.0 ) ) {
				z = z*z + c;
			}
			MPIX_Stream_printf(s,"im1: x:%d y:%d gs:%d", 
					  x, y, 255 - (int)(255.0*(it/ITER)) );

		}

	}

	MPI_Barrier(MPI_COMM_WORLD);
	
	if( !rank ){
		MPIX_Stream_printf(s,"out: new:LOG markdown:1 label:Mandelbrot" );
		MPIX_Stream_printf(s,"out: commit:" );
		MPIX_Stream_printf(s,"out: log:\"# The Mandelbrot Set\"" );
		MPIX_Stream_printf(s,"out: emit:im1");
	}

	MPIX_Stream_release(&s);

	MPI_Finalize();

	return 0;
}
