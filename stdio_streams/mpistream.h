#ifndef MPI_STREAM_H
#define MPI_STREAM_H

#include <mpi.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

/** The Public MPI Stream Interface */


#define MPI_STREAM_NULL (-1)

typedef int MPI_Stream;

int MPIX_Stream_init( MPI_Comm comm, char * label , MPI_Count block_size,  MPI_Stream * s );
int MPIX_Stream_release(MPI_Stream * s );

int MPIX_Stream_map( MPI_Stream s , const char * label );

int MPIX_Stream_read( MPI_Stream s, void * data, MPI_Count size );

int MPIX_Stream_write( MPI_Stream s , void * data, MPI_Count size );

int MPIX_Stream_printf( MPI_Stream s , const char * format , ... );



/*
 * INTERNAL
 */

/** Internal MPI Stream Definitions */
struct iMPI_Stream
{
	/* MPI Stream Duped Comm */
	MPI_Comm comm;
	/* MPI Stream local Label */
	char * label;
	/* MPI Stream Block size (same on all) */
	MPI_Count block_size;
	/* MPI Stream id (matching local handle) */
	unsigned int id;
	/* Root CTX */
	int is_root;
	int root;
	pthread_t polling_th;
	pthread_mutex_t lock;
	FILE* fd;
	int mapped;
	int running;
};


struct iMPI_Stream * iMPI_Stream_new(  MPI_Comm comm, char * label , MPI_Count block_size);
int iMPI_Stream_release( struct iMPI_Stream * s );


/** ID to stream */

struct iMPI_Stream_factory
{
	struct iMPI_Stream ** streams;
	unsigned int array_size;
	unsigned int count;
	unsigned int current_id;
};

int iMPI_Stream_factory_register( struct iMPI_Stream *s, unsigned int *id );
int iMPI_Stream_factory_unregister( unsigned int id );
struct iMPI_Stream * iMPI_Stream_factory_resolve( unsigned int id );




#endif /* end of include guard: MPI_STREAM_H */
