#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

void get_contrast_stretched_images( char a[], char b[])
{	
	char r[100], s[100];

	FILE *fp, *fpr, *fpw;

	int i, j;

	unsigned char c;

	int P;

	fp = fopen( "temp\\person_counter.txt", "r" );
	fscanf( fp, "%d", &P );
	fclose( fp );

	for ( i = 0; i < P; ++i )
	{
		sprintf( s, "%s\\temp%d.bmp", a, i );
		sprintf( r, "%s\\temp%d.bmp", b, i );
	
		fpr = fopen( s, "rb" );
		fpw = fopen( r, "wb" );

		for ( j = 0; j < 1078; ++j )
		{
			fscanf( fpr, "%c", &c );
			fprintf( fpw, "%c", c );
		}

		int MAX = -1;
		int MIN = 1000;

		j = 0;

		while ( 1 )
		{
			fscanf( fpr, "%c", &c );

			if ( feof( fpr ) ) break;
	
			if ( c > MAX ) MAX = c;

			if ( c < MIN ) MIN = c;
			
			++j;
		}		
		
		fseek( fpr, 1078, 0 );

		// j = 0;

		while ( j-- )
		{			
			fscanf( fpr, "%c", &c );

			if ( feof( fpr ) ) 
			{							
				break;
			}		

			c = ((double)( c - MIN ) / (double)( MAX - MIN )) * 255.0;		

			fprintf( fpw, "%c", c );
		}		

		fclose( fpr );
		fclose( fpw );
	}
}
