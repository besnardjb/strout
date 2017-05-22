#include <stdio.h>
#include <math.h>
#include <sys/time.h>

int main(int argc, char ** argv )
{
	
	double i;

	MPIX_Stream_printf("o1: new:LOG label:main markdown:1" );
	MPIX_Stream_printf("o1: commit:" );
	MPIX_Stream_printf("p1: new:PLOT x:time y:sin label:sinus" );
	MPIX_Stream_printf("t1: new:TABLE x:time y:sin label:sinus" );

	for( i = 0.0 ; i < 10.0 ; i= i + 0.05 )
	{
		MPIX_Stream_printf("p1: x:%g y:%g", i, sin(i));
		MPIX_Stream_printf("t1: x:%g y:%g", i, sin(i));
	}

	MPIX_Stream_printf("o1: log:\"# Plot of Sinus\"");
	MPIX_Stream_printf("o1: emit:p1" );
	
	MPIX_Stream_printf("o1: log:\"# Table of Sinus\"");
	MPIX_Stream_printf("o1: emit:t1" );

	return 0;
}
