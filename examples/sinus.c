#include <stdio.h>
#include <math.h>
#include <sys/time.h>


char * now()
{
	static char tmp[150];
	static unsigned long int cnt = 0;

	snprintf( tmp, 150, "%lu" , cnt++ );

	return tmp;

}



int main(int argc, char ** argv )
{
	
	double i;

	printf("%s:o1: new:LOG label:main markdown:1\n", now() );
	printf("%s:o1: commit:\n", now() );
	printf("%s:p1: new:PLOT x:time y:sin label:sinus\n", now() );
	printf("%s:t1: new:TABLE x:time y:sin label:sinus\n", now() );

	for( i = 0.0 ; i < 10.0 ; i= i + 0.05 )
	{
		printf("%s:p1: x:%g y:%g\n", now(), i, sin(i));
		printf("%s:t1: x:%g y:%g\n", now(), i, sin(i));
	}

	printf("%s:o1: log:\"# Plot of Sinus\"\n", now());
	printf("%s:o1: emit:p1\n", now() );
	
	printf("%s:o1: log:\"# Table of Sinus\"\n", now());
	printf("%s:o1: emit:t1\n", now() );


	return 0;
}
