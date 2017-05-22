#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>

#define SIZEX 1000
#define SIZEY 1000
#define ITER 64

static int cnt = 0;


int main( int argc ,char ** argv )
{
	MPI_Init(&argc, &argv );

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank );
	MPI_Comm_size(MPI_COMM_WORLD, &size );
	int x,y;

	int qr = (SIZEX/size);
	const double qx = 3.0/SIZEX, qy = 2.0/SIZEY;

	if(rank == 0)
	{
		MPIX_Stream_printf("im1: new:IMG sx:%d sy:%d label:mandel\n",  SIZEX, SIZEY );
	}

	MPI_Barrier(MPI_COMM_WORLD);

	for( x = qr * rank ; x < qr*(rank+1) ; x++ ) {
		MPIX_Stream_printf("%d:p%d: new:PLOT x:time y:val label:mandel%d\n", cnt++, x, x );
		for(y = 0 ; y < SIZEY; y++ ) {
			double it = 0;
			double complex c = (-2.0 + x*qx) + (-1.0 + y*qy)*I, z = 0;
			while( (++it  < ITER) && (cabs(z) < 2.0 ) ) {
				z = z*z + c;
			}
			MPIX_Stream_printf("im1: x:%d y:%d gs:%d", 
					  x, y, 255 - (int)(255.0*(it/ITER)) );

			MPIX_Stream_printf("p%d: x:%d y:%d", x,   y, 255 - (int)(255.0*(it/ITER)) );
		}

	}

	MPI_Barrier(MPI_COMM_WORLD);
	
	if( !rank ){
		MPIX_Stream_printf("out: new:LOG markdown:1 label:Mandelbrot" );
		MPIX_Stream_printf("outs: new:LOG markdown:1 label:\"Mandelbrot Slice\"" );
		MPIX_Stream_printf("out: commit:" );
		MPIX_Stream_printf("out: log:\"# The Mandelbrot Set\"" );
		MPIX_Stream_printf("out: emit:im1");
		
		
		for( x = 0 ; x < SIZEX ; x++ ) {
			MPIX_Stream_printf("outs: emit:p%d");
		}
	}


	MPI_Finalize();

	return 0;
}
