#include <mpi.h>

#include "mpistream.h"

int main( int argc, char *argv[])
{
	int l;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &l);

	MPI_Stream s;
	
	MPIX_Stream_init( MPI_COMM_WORLD, "proc" , 1024,  &s );

	MPIX_Stream_map(s, "stdout");

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int i;

	for (i = 0; i < 4096; ++i) {
		MPIX_Stream_printf( s , "Hello %d from %d" , i , rank );
	}

	
	MPIX_Stream_release( &s );

	MPI_Finalize();

	return 0;
}
