#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "mpistream.h"


/** ID to stream */

static pthread_mutex_t __factory_lock = PTHREAD_MUTEX_INITIALIZER;
static struct iMPI_Stream_factory __factory = {NULL, 0, 0, 0};

static inline int iMPI_Stream_factory_realloc( unsigned int array_size )
{
	__factory.streams = realloc( __factory.streams , array_size * sizeof(struct iMPI_Stream *) );

	if( !__factory.streams )
	{
		perror("Realloc");
		return MPI_ERR_INTERN;
	}

	unsigned int i;

	for( i = __factory.array_size  ; i < array_size ; i++ )
	{
		__factory.streams[i] = NULL;
	}

	__factory.array_size = array_size;

	return MPI_SUCCESS;
}


int iMPI_Stream_factory_register( struct iMPI_Stream *s, unsigned int *id )
{
	int res;

	pthread_mutex_lock(&__factory_lock );

	if( !__factory.streams )
	{
		res = iMPI_Stream_factory_realloc( 1 );

		if( res != MPI_SUCCESS )
			goto reg_err;
	}

	__factory.count++;

	if( __factory.array_size <= __factory.count )
	{
		res = iMPI_Stream_factory_realloc(  __factory.array_size + 4 );

		if( res != MPI_SUCCESS )
			goto reg_err;
	}


	unsigned int i;
	int done = 0;

	for( i = 0 ; i < __factory.array_size ; i++ )
	{
		if( __factory.streams[i] == NULL )
		{
			__factory.streams[i] = s;
			*id = i;
			done = 1;
		}

	}


	pthread_mutex_unlock(&__factory_lock );

	if( !done )
	{
		fprintf(stderr, "MPI_Stream: Error failed to register a stream\n");
		abort();
	}


	return MPI_SUCCESS;

reg_err:
	pthread_mutex_unlock(&__factory_lock );

	return res;
}


int iMPI_Stream_factory_unregister( unsigned int id )
{
	int res = MPI_ERR_INTERN;

	pthread_mutex_lock(&__factory_lock );

	if( (__factory.streams[id] == NULL) || (__factory.array_size <= id)  )
	{
		fprintf(stderr, "MPI Stream : Could not unregister %d\n", id);
		goto unreg_err;
	}

	__factory.streams[id] = NULL;

	pthread_mutex_unlock(&__factory_lock );

	return MPI_SUCCESS;

unreg_err:
	pthread_mutex_unlock(&__factory_lock );

	return res;

}


struct iMPI_Stream * iMPI_Stream_factory_resolve( unsigned int id )
{
	struct iMPI_Stream * ret = NULL; 

	pthread_mutex_lock(&__factory_lock );

	if( (__factory.streams[id] == NULL) || (__factory.array_size <= id)  )
	{
		fprintf(stderr, "MPI Stream : Could not unregister %d\n", id);
		goto unreg_err;
	}

	ret = __factory.streams[id];

	pthread_mutex_unlock(&__factory_lock );

	return ret;

unreg_err:
	pthread_mutex_unlock(&__factory_lock );

	return ret;
}



/** Internal MPI Stream Definitions */

struct iMPI_Stream * iMPI_Stream_new(  MPI_Comm comm, char * label , MPI_Count block_size )
{
	struct iMPI_Stream * s = malloc( sizeof(struct iMPI_Stream) );

	if( !s )
	{
		perror("malloc");
		return NULL;
	}

	MPI_Comm_dup( comm , &s->comm );

	s->label = strdup( label );

	s->block_size = block_size;
	s->is_root = 0;
	s->root = 0;
	s->mapped = 0;
	s->running = 0;
	s->fd = NULL;
	pthread_mutex_init(&s->lock, NULL);


	if( iMPI_Stream_factory_register( s, &s->id ) != MPI_SUCCESS )
	{
		abort();
	}

	return s;
}


int iMPI_Stream_release( struct iMPI_Stream * s )
{

	if( s->running )
	{
		s->running = 0;
		pthread_join( s->polling_th, NULL );
		fclose( s->fd );
	}

	MPI_Barrier(s->comm);

	if( iMPI_Stream_factory_unregister( s->id ) != MPI_SUCCESS )
	{
		return MPI_ERR_INTERN;
	}

	MPI_Comm_free( &s->comm );	

	free( s->label );
	s->label = NULL;

	s->mapped = 0;
	s->running = 0;
	s->block_size = 0;
	s->is_root = 0;
	s->root = 0;
	s->fd = NULL;
	pthread_mutex_destroy(&s->lock);

	free( s );

	return MPI_SUCCESS;
}


/** The Public MPI Stream Interface */


int MPIX_Stream_init( MPI_Comm comm, char * label , MPI_Count block_size,  MPI_Stream * s )
{
	MPI_Count cons_bsize;
	MPI_Allreduce( &block_size , &cons_bsize , 1 , MPI_COUNT , MPI_BOR , comm );

	if( cons_bsize != block_size )
	{
		return MPI_ERR_ARG;
	}

	struct iMPI_Stream * stream = iMPI_Stream_new( comm, label , block_size );

	if( !stream )
	{
		return MPI_ERR_INTERN;
	}

	*s = stream->id;

	return MPI_SUCCESS;
}

int MPIX_Stream_release(MPI_Stream * s )
{
	struct iMPI_Stream *st = iMPI_Stream_factory_resolve( *s );

	if( !st )
	{
		return MPI_ERR_ARG;
	}


	MPI_Barrier(st->comm);

	int rank, size;

	MPI_Comm_rank( st->comm , &rank );
	MPI_Comm_size( st->comm , &size );

	if( (1 < size) && (rank == 1) )
	{
		MPIX_Stream_printf(*s , "--STOP");
	}


	iMPI_Stream_release( st );

	*s = MPI_STREAM_NULL;
	return MPI_SUCCESS;
}

int MPIX_Stream_read( MPI_Stream s, void * data, MPI_Count size )
{
	struct iMPI_Stream *st = iMPI_Stream_factory_resolve( s );

	if( !st )
	{
		return MPI_ERR_ARG;
	}

	fprintf(stderr, "ERROR : Read is not here yet\n");

	return MPI_ERR_INTERN;
}

int MPIX_Stream_write( MPI_Stream s , void * data, MPI_Count size )
{
	struct iMPI_Stream *st = iMPI_Stream_factory_resolve( s );

	if( !st )
	{
		return MPI_ERR_ARG;
	}

	if( !st->mapped )
	{
		fprintf(stderr, "Error : Stream is not mapped\n");
		return MPI_ERR_ARG;
	}



	if( st->block_size <= size )
	{
		fprintf(stderr, "Error : Entry too large to fit buffer\n");
		return MPI_ERR_ARG;
	}

	int rank, csize;

	MPI_Comm_rank( st->comm , &rank );
	MPI_Comm_size( st->comm , &csize );

	if( (csize == 1) || (rank == st->root ))
	{
		pthread_mutex_lock( &st->lock );
		fwrite( data, sizeof(char), size, st->fd);
		//fflush( st->fd );
		pthread_mutex_unlock( &st->lock );
		return MPI_SUCCESS;
	}

	MPI_Send( data , size , MPI_CHAR , st->root , 123, st->comm );

	return MPI_SUCCESS;
}

/* UTILS */
static inline int hash_label( const char * label )
{
	int l = strlen(label);

	int ret = 13;

	int i;
	for (i = 0; i < l; ++i) {
		ret = ret * 3 + label[l];
	}

	return ret;
}


void * stream_reader( void * ps )
{
	struct iMPI_Stream *st = (struct iMPI_Stream *)ps;

	char * buffer = malloc( st->block_size );

	if(!buffer)
	{
		perror("malloc");
		abort();
	}

	int run = 1;

	while( run )
	{
		MPI_Status sta;
		MPI_Recv( buffer , st->block_size , MPI_CHAR , MPI_ANY_SOURCE , 123 , st->comm , &sta );
		
		int size;
		MPI_Get_count(&sta, MPI_CHAR, &size);


		buffer[size + 1 ] = '\0';
	
		if( buffer[0] == '-' 
		&&  buffer[1] == '-'
		&&  buffer[2] == 'S'
		&&  buffer[3] ==  'T'
		&&  buffer[4] ==  'O'
		&&  buffer[5] ==  'P'
		)
		{
			run = 0;
		}
		else
		{
			pthread_mutex_lock( &st->lock );
			fprintf( st->fd, "%s", buffer);
			//fflush( st->fd );
			pthread_mutex_unlock( &st->lock );
		}
		
	}

	return NULL;
}



int MPIX_Stream_map( MPI_Stream s , const char * label )
{
	struct iMPI_Stream *st = iMPI_Stream_factory_resolve( s );

	if( !st )
	{
		return MPI_ERR_ARG;
	}

	int hash = hash_label( label );
	int chash = 0;
	MPI_Allreduce( &hash , &chash , 1 , MPI_INT, MPI_BOR , st->comm );

	if( hash != chash )
	{
		fprintf(stderr, "Error only mapping with the same label are supported\n");
		return MPI_ERR_ARG;
	}

	int rank, size;

	MPI_Comm_rank( st->comm , &rank );
	MPI_Comm_size( st->comm , &size );


	/* First choose a root */
	st->root = 0;

	if( !rank )
	{
		st->root = rand()%size;
	}

	MPI_Bcast( &st->root , 1 , MPI_INT , 0 , st->comm );

	if( st->root == rank )
	{
		st->is_root = 1;
	}
	else
	{
		st->is_root = 0;
	}


	if( label[0] == '.' && label[1] == '/')
	{
		/* This is a file stream */
		if( st->is_root )
		{
			st->fd = fopen(label, "w");
		}
	}
	else if( !strcmp(label,"stdout"))
	{
		/* This is a stdout stream */
		if( st->is_root )
		{
			st->fd = fdopen(1,"w");
		}
	}
	else
	{
		fprintf(stderr, "ERROR : On this prototype only STDOUT or file streams are supported\n");
		return MPI_ERR_ARG;
	}

	if( st->is_root && (size != 1) )
	{
		pthread_create( &st->polling_th, NULL,stream_reader, (void*)st );
 		st->running = 1; 
	}


	st->mapped = 1;

	return MPI_SUCCESS;
}


int MPIX_Stream_printf( MPI_Stream s , const char * format , ... )
{
	struct iMPI_Stream *st = iMPI_Stream_factory_resolve( s );

	if( !st )
	{
		return MPI_ERR_ARG;
	}



	char static_buff[4096];

	char *buff = static_buff;

	if( 4096 < st->block_size )
	{
		buff = malloc( st->block_size + 1);
		if( !buff )
		{
			perror("malloc");
			abort();
		}
	}


	va_list args;
    
	va_start(args, format);
   
	vsnprintf( buff, st->block_size,  format, args);
	int len = strlen(buff);
	buff[len] = '\n';
	buff[len + 1] = '\0';
    
	va_end(args);

	
	MPIX_Stream_write( s, buff, len + 1);


	if( 4096 < st->block_size )
	{
		free(buff);
	}

	return MPI_SUCCESS;
}

