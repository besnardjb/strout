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
		printf("%d:im1: new:IMG sx:%d sy:%d label:mandel\n", cnt++,  SIZEX, SIZEY );		
	}

	MPI_Barrier(MPI_COMM_WORLD);

	for( x = qr * rank ; x < qr*(rank+1) ; x++ ) {
		printf("%d:p%d: new:PLOT x:time y:val label:mandel%d\n", cnt++, x, x );		
		for(y = 0 ; y < SIZEY; y++ ) {
			double it = 0;
			double complex c = (-2.0 + x*qx) + (-1.0 + y*qy)*I, z = 0;
			while( (++it  < ITER) && (cabs(z) < 2.0 ) ) {
				z = z*z + c;
			}
			printf("%d:im1: x:%d y:%d gs:%d\n", 
					cnt++,  x, y, 255 - (int)(255.0*(it/ITER)) );

			printf("%d:p%d: x:%d y:%d\n", cnt++, x,   y, 255 - (int)(255.0*(it/ITER)) ); 
		}

	}

	MPI_Barrier(MPI_COMM_WORLD);
	
	if( !rank ){
		printf("%d:out: new:LOG markdown:1 label:Mandelbrot\n", cnt++,  SIZEX, SIZEY );		
		printf("%d:outs: new:LOG markdown:1 label:\"Mandelbrot Slice\"\n", cnt++,  SIZEX, SIZEY );		
		printf("%d:out: commit:\n", cnt++,  SIZEX, SIZEY );		
		printf("%d:out: log:\"# The Mandelbrot Set\"\n", cnt++,  SIZEX, SIZEY );		
		printf("%d:out: emit:im1\n", cnt++);		
		
		
		for( x = 0 ; x < SIZEX ; x++ ) {
			printf("%d:outs: emit:p%d\n", cnt++, x);
		}
	}


	MPI_Finalize();

	return 0;
}
