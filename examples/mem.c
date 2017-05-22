#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <stdio.h>
#include <sys/sysinfo.h>
#include <sys/time.h>


#include "mpistream.h"

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


struct statm_t{
	unsigned long size;
	unsigned long resident;
	unsigned long share;
	unsigned long text;
	unsigned long lib;
	unsigned long data;
	unsigned long dt;
};

void read_off_memory_status(struct statm_t * result)
{
  unsigned long dummy;
  const char* statm_path = "/proc/self/statm";

  FILE *f = fopen(statm_path,"r");
  if(!f){
    perror(statm_path);
    abort();
  }
  if(7 != fscanf(f,"%ld %ld %ld %ld %ld %ld %ld", 
		   &result->size,
                   &result->resident,
		   &result->share,
                   &result->text,
                   &result->lib,
                   &result->data,
                   &result->dt) )
		  {
		    perror(statm_path);
		    abort();
		  }
  fclose(f);
  
  /* Multiply by page size */
  int page_size = sysconf(_SC_PAGESIZE);
  
  result->size *= page_size;
  result->resident *= page_size;
  result->share *= page_size;
  result->text *= page_size;
  result->lib *= page_size;
  result->data *= page_size;
  result->dt *= page_size;
  
}

static double getmem()
{
	struct statm_t mem= {};
	read_off_memory_status( & mem );

	return mem.resident / (1024.0 * 1024.0);
}


void memstream_log()
{
	double lmem = getmem();
	double mem = 0;
	double now = getts();

	MPI_Reduce( &lmem , &mem , 1 , MPI_DOUBLE , MPI_SUM , 0 , MPI_COMM_WORLD );

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

	for (i = 0; i < 8192; ++i) {
		
		double s =  fabs( 5 * 1024 * 1024.0 * 1024.0 * sin(i/64.0) );
		void * data = malloc( s );

		memset( data , 0 , s );

		memstream_log();

		free( data );

		usleep(500);
	}




	memstream_release();


	MPI_Finalize();

	return 0;
}
