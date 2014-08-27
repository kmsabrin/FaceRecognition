#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define IM_SZ 132 * 132

void get_histogram_equalized_images( char a[], char b[])
{			 	
	unsigned char		image [IM_SZ + 10];
	int					F	  [256   + 10];
	int					CuF	  [256   + 10];
	int				    Feq	  [256   + 10];
	int					CuFeq [256   + 10];
	unsigned char		II	  [256   + 10];

/*	
	memset( F,     0, sizeof( F ) );

	memset( CuF,   0, sizeof( CuF ) );

	memset( Feq,   0, sizeof( Feq ) );

	memset( CuFeq, 0, sizeof( CuFeq ) );

	memset( II,    0, sizeof( II ) );
*/	

	char rr[100], ss[100];

	FILE *fp, *fpr, *fpw;

	int i, j, k, d, s, f, e;

	unsigned char c;

	int P;

	fp = fopen( "temp\\person_counter.txt", "r" );
	fscanf( fp, "%d", &P );
	fclose( fp );

	for ( i = 0; i < P; ++i )
	{
		for ( j = 0; j < 256; ++j )
		{
			F[j] = CuF[j] = Feq[j] = CuFeq[j] = II[j] = 0;
		}	

		sprintf( ss, "%s\\temp%d.bmp", a, i );
		sprintf( rr, "%s\\temp%d.bmp", b, i );
	
		fpr = fopen( ss, "rb" );
		fpw = fopen( rr, "wb" );

		for ( j = 0; j < 1078; ++j )
		{
			fscanf( fpr, "%c", &c );
			fprintf( fpw, "%c", c );
		}

		for ( j = 0; j < IM_SZ; ++j )
		{
			fscanf( fpr, "%c", &c );
			image[j] = c;	
			++F[c];
		}		

		for ( CuF[0] = F[0], j = 1; j < 256; ++j )
		{
			
			CuF[j] = CuF[j - 1] + F[j];		
		}

		s = CuF[256 - 1];

		f = s / 256;		
		
		for ( j = 0; j < 256; ++j )
		{
			Feq[j] = f;
		}		

		for ( e = s - f * 256, j = 128 - ( e / 2 ); j < 128 + ( e / 2 ); ++j )
		{
			++Feq[j];
		}
		
		for ( CuFeq[0] = Feq[0], j = 1; j < 256; ++j )
		{
			CuFeq[j] = CuFeq[j - 1] + Feq[j];
		}

		for ( j = 0; j < 256; ++j )
		{
			int min = 11111111, map;

			for ( k = 0; k < 256; ++k )
			{
				d = abs( CuF[j] - CuFeq[k] );

				if ( d < min )
				{
					min = d;
					
					map = k;
				}
			}
		
			II[j] = map;			
		}	
		
		for ( j = 0; j < IM_SZ; ++j )
		{

			fprintf( fpw, "%c", II[image[j]] );
		}

		fclose( fpr );
		fclose( fpw );
	}
}
