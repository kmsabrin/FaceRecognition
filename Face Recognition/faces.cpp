#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "eigen_vector.cpp"
#include "contrast_stretching.cpp"
#include "histogram_equalization.cpp"

#define SIZE	200 // maximum number of training image

#define IMW		132

#define IMH		132

#define N		IMW * IMH  // size of one dimension data image

int				P, temp_P;     // number of training image [start from 0]

double			training_images					[SIZE + 10][N + 10];

double			mean_image						[N    + 10];

double			centered_images					[SIZE + 10][N + 10];

double			data_matrix						[N    + 10][SIZE + 10];

double			transpose_data_matrix			[SIZE + 10][N + 10];
	
double			covariance_matrix				[SIZE + 10][SIZE + 10];

double			eigen_vectors					[SIZE + 10][SIZE + 10];
	
double			eigen_values					[SIZE + 10];

double			normalized_original_eigenvectors[SIZE + 10][N + 10];

double			feature_vectors					[SIZE + 10][N + 10];

double			test_image						[N    + 10];

double			test_feature_vector				[N    + 10];


int				zero_eigenvalues;

int				n_eigen_vectors;

void get_training_images() // Reads Input From File
{	
	char s[100];

	FILE *fp;

	int i, j;

	unsigned char c;	

	for ( i = 0; i < P; ++i )
	//for ( i = 0; i < temp_P; ++i )
	{
		sprintf( s, "histogram_images\\temp%d.bmp", i );

		fp = fopen( s, "rb" );

		fseek( fp, 1078, 0 );

		j = 0;				

		while ( 1 )
		{
			fscanf( fp, "%c", &c );

			if ( feof( fp ) ) break;

			training_images[i][j++] = c;						
		}

		fclose ( fp );
	}	
}

void get_mean_image() // Writes Output To File
{
	int i, j;

	double k;

	FILE *fp = fopen( "temp\\mean_image.txt", "w" );

	fprintf( fp, "%d\n", N );

	for ( j = 0; j < N; ++j )
	{
		for ( i = 0, k = 0; i < temp_P; ++i )
		//for ( i = 0, k = 0; i < P; ++i )
		{
			k += training_images[i][j];
		}

		//mean_image[j] = k / P;		
		mean_image[j] = k / temp_P;		
		fprintf( fp, "%lf\n", mean_image[j] );
	}

	fclose( fp );
}

void get_centered_images()
{
	int i, j;

	for ( i = 0; i < P; ++i )
	//for ( i = 0; i < temp_P; ++i )
	{
		for ( j = 0; j < N; ++j )
		{
			centered_images[i][j] = training_images[i][j] - mean_image[j];
		}
	}
}

void get_data_matrix()
{
	int i, j;

	for ( i = 0; i < N; ++i )
	{
		//for ( j = 0; j < P; ++j )
		for ( j = 0; j < temp_P; ++j )
		{
			data_matrix[i][j] = centered_images[j][i]; // transpose matrix opearation
		}
	}
}

void get_transpose_data_matrix()
{
	int i, j;

	//for ( i = 0; i < P; ++i )
	for ( i = 0; i < temp_P; ++i )
	{
		for ( j = 0; j < N; ++j )
		{
			transpose_data_matrix[i][j] = centered_images[i][j]; // since they are equivalent
		}
	}
}

void get_covariance_matrix()
{
	int i, j, k;

	// DT X D = (PXN) x (NXP) = (PXP)
	//for ( i = 0; i < P; ++i )
	for ( i = 0; i < temp_P; ++i )
	{
		//for ( j = 0; j < P; ++j )
		for ( j = 0; j < temp_P; ++j )
		{
			covariance_matrix[i][j] = 0;

			for ( k = 0; k < N; ++k )
			{
				covariance_matrix[i][j] += transpose_data_matrix[i][k] * data_matrix[k][j];
			}
		}
	}
}

void get_eigenvalues_eigenvectors_of_covariance_matrix()
{
	int ERR;

	int i, j;	

	double Z[SIZE + 10][SIZE + 10];

	double D[SIZE + 10];

	double E[SIZE + 10];

	//for ( i = 1; i <= P; ++i )
	for ( i = 1; i <= temp_P; ++i )
	{
		//for ( j = 1; j <= P; ++j )
		for ( j = 1; j <= temp_P; ++j )
		{
			Z[i][j] = covariance_matrix[i - 1][j - 1];
		}
	}

	//TRED2( Z, P, D, E );
	TRED2( Z, temp_P, D, E );

	//TQL2( P, D, E, Z, &ERR );
	TQL2( temp_P, D, E, Z, &ERR );

	//for( i = 1; i <= P; ++i )
	for( i = 1; i <= temp_P; ++i )
	{
		eigen_values[i - 1] = D[i]; // in increasing order

		if ( D[i] < 0.05 ) 
		{
			++zero_eigenvalues;
		}

		for ( j = 1; j <= temp_P; ++j )
		//for ( j = 1; j <= P; ++j )
		{			
			eigen_vectors[i - 1][j - 1] = Z[j][i];
		}	
	}	

	/*====================my changes===================*/
	//n_eigen_vectors = P - zero_eigenvalues; // eliminating zero eigen values
	//n_eigen_vectors = (int) P * 0.5; // taking 40% of eigenfaces
	n_eigen_vectors = 50;
/*	FILE *zp;
	zp = fopen("error.txt","w");
	fprintf( zp, "%d\n", zero_eigenvalues);
	fclose(zp);
	*/
}

void get_original_eigenvectors()
{
	int i, j, k;

	//for ( k = 0; k < P; ++k )
	for ( k = 0; k < temp_P; ++k )
	{
		double normal = 0;

		for ( i = 0; i < N; ++i )
		{
			double sum = 0;					

			//for ( j = 0; j < P; ++j )
			for ( j = 0; j < temp_P; ++j )
			{
				sum += data_matrix[i][j] * eigen_vectors[k][j];				
			}			

			normalized_original_eigenvectors[k][i] = sum;

			normal += sum * sum;
		}

		normal = sqrt( normal );

		for ( i = 0; i < N; ++i )
		{
			normalized_original_eigenvectors[k][i] /= normal;			
		}
	}
}

void write_final_eigenvectors() // Write Output To File
{
	int i, j;

	FILE *fp = fopen( "temp\\eigen_vectors.txt", "w" );

	fprintf( fp, "%d\n", n_eigen_vectors );
		
	//for( i = P - 1; i >= zero_eigenvalues; --i ) // process in decreasing order
	//for( i = P - 1; i >= P - n_eigen_vectors; --i ) // process in decreasing order
	for( i = temp_P - 1; i >= temp_P - n_eigen_vectors; --i ) // process in decreasing order
	{	
		for ( j = 0; j < N; ++j )
		{			
			fprintf( fp, "%lf\n", normalized_original_eigenvectors[i][j] );
		}	
	}

	fclose( fp );
}

void get_feature_vectors_by_projecting_images_onto_eigenspace() // Write Output To File
{
	int i, j, k, t;

	FILE *fp = fopen( "temp\\feature_vectors.txt", "w" );

	for ( i = 0; i < P; ++i ) // for each persons image
	{
		//for ( j = P - 1, t = 0; j >= zero_eigenvalues; --j, ++t ) // for each eigenvector find dot product // let's go decreasing order
		//for ( j = P - 1, t = 0; j >= P - n_eigen_vectors; --j, ++t ) // for each eigenvector find dot product // let's go decreasing order
		for ( j = temp_P - 1, t = 0; j >= temp_P - n_eigen_vectors; --j, ++t ) // for each eigenvector find dot product // let's go decreasing order
		{
			double dot_product_sum = 0; // for each entry of eigenvector 

			for ( k = 0; k < N; ++k )
			{
				dot_product_sum += centered_images[i][k] * normalized_original_eigenvectors[j][k];
			}

			feature_vectors[i][t] = dot_product_sum;			

			fprintf( fp, "%lf\n", feature_vectors[i][t] );
		}		
	}

	fclose( fp );
}

void face()
{
	FILE *fp = fopen( "temp\\person_counter.txt", "r" );

	fscanf( fp, "%d", &P );

	fclose( fp );

	temp_P = P;	

	get_contrast_stretched_images ( "captured_images", "contrast_images" );

	get_histogram_equalized_images( "contrast_images", "histogram_images" );	

	//get_contrast_stretched_images( "captured_images", "histogram_images" );	

	get_training_images();

	get_mean_image();

	get_centered_images();

	get_data_matrix();

	get_transpose_data_matrix();

	get_covariance_matrix();	

	zero_eigenvalues = 0;

	get_eigenvalues_eigenvectors_of_covariance_matrix();	

	get_original_eigenvectors();	

	write_final_eigenvectors();

	get_feature_vectors_by_projecting_images_onto_eigenspace();
}