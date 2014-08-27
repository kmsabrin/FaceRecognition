#include <math.h>
#include <stdio.h>

#define ONE 1
#define TWO 2
#define SZ 200
#define ZERO 0
#define SQRT( a )  sqrt(a)
#define ABS( a ) (((a) < 0) ? (-a) : (a))

//double zmax;
//int I;
//int IERR;
//int J;
//int Nn;

//FILE *fp;

double Sign(double a, double b) 
{
	if ( b < ZERO ) 
	{
		return ( -ABS( a ) );
	}
	else 
	{
		return ( ABS( a ) );
	}
}

void TQL2( int Nn, double D[], double E[], double Z[][SZ + 10], int *IER )  
{	
	double B, C, F, G, H, pP, R, S, EPS, EPS1;
	int I, J, K, L, M, JM;
	
	EPS = ZERO; 
	JM = 30;
	*IER = 0;
	
	if ( Nn == 1 )  
	{
		return;
	}
			
	if ( EPS != ZERO ) 
	{
		goto e12;
	}
	
	EPS = 1.0;

e10:  
	EPS = EPS / TWO;
    EPS1 = ONE + EPS;

    if ( EPS1 > ONE ) 
	{
		goto e10;
	}
	  
e12:  
	for ( I = 2; I <= Nn; I++ )  
	{
		E[I - 1] = E[I];
	}
    
	E[Nn] = ZERO;
    F = ZERO;
    B = ZERO;
	  
    for ( L = 1; L <= Nn; L++ ) 
	{
		J = 0;
		H = EPS * ( ABS( D[L] ) + ABS( E[L] ) );
		if ( B < H )  
		{
			B = H;
		}
		  		  
		for ( M = L; M <= Nn; M++ )
		{
			if ( ABS(E[M]) <= B ) 
			{
				goto e18;
			}
		}
			  
e18:    
		if ( M == L )  
		{
			goto e26;
		}
						
e20:	
		if ( J == JM ) 
		{
			goto e36;
		}

		J = J + 1;
				
		G = D[L];
        pP = ( D[L + 1] - G ) / ( TWO * E[L] );
        R = SQRT( pP * pP + ONE );
        D[L] = E[L] / ( pP + Sign( R, pP ) );
        H = G - D[L];
        
		for ( I = L + 1; I <= Nn; I++ )  
		{
			D[I] = D[I] - H;
		}

        F = F + H;
				
        pP = D[M];
        C = ONE;
        S = ZERO;
          
		for ( I = M - 1; I >= L; I-- ) 
		{
			G = C * E[I];
			H = C * pP;
				
			if ( ABS( pP ) >= ABS( E[I] ) ) 
			{				
				C = E[I] / pP;
				R = SQRT( C * C + ONE );
				E[I + 1] = S * pP * R;
				S = C / R;
				C = ONE / R;
			}
			else 
			{
				C = pP / E[I];
				R = SQRT( C * C + ONE );
				E[I + 1] = S * E[I] * R;
				S = ONE / R;
				C = C * S;
			}

			pP = C * D[I] - S * G;
			D[I + 1] = H + S * ( C * G + S * D[I] );
						
			for ( K = 1; K <= Nn; K++ ) 
			{
				H = Z[K][I + 1];
				Z[K][I + 1] = S * Z[K][I] + C * H;
				Z[K][I] = Z[K][I] * C - S * H;
			}
		}

		E[L] = S * pP;
		D[L] = C * pP;
		
		if ( ABS( E[L] ) > B )  
		{
			goto e20;
		}
						
e26:    
		D[L] = D[L] + F;
      }
	  
	  for ( L = 2; L <= Nn; L++ ) 
	  {
		  I = L - 1;
		  K = I;
		  pP = D[I];
		 
		  for ( J = L; J <= Nn; J++ ) 
		  {
			  if ( D[J] >= pP ) 
			  {
				  goto e30;
			  }

			  K = J;
			  pP = D[J];

e30:		
		  ;}

		  if ( K == I )  
		  {
			  goto e34;
		  }

		  D[K] = D[I];
		  D[I] = pP;

		  for ( J = 1; J <= Nn; J++ ) 
		  {
			  pP = Z[J][I];
			  Z[J][I] = Z[J][K];
			  Z[J][K] = pP;
		  }

e34: 
	  ;}
      return;
	  	  
e36:  
	  *IER = L;
} 
   
void TRED2( double a[][SZ + 10], int Nn, double d[], double e[] )  
{
	int l, k, j, i;
	double scale, hh, h, g, f;
	   
	for ( i = Nn; i >= 2; i-- )  
	{
		l = i - 1;
		h = scale = 0.0;

		if ( l > 1 )  
		{
			for ( k = 1; k <= l; k++  )
			{
				scale += fabs(a[i][k]);
			}

			if ( scale == 0.0 )
			{
				e[i] = a[i][l];
			}
			else  
			{
				for ( k = 1; k <= l; k++ ) 
				{
					a[i][k] /= scale;
					h += a[i][k] * a[i][k];
				}

				f = a[i][l];
				g = ( f > 0 ? -sqrt( h ) : sqrt( h ) );
				e[i] = scale * g;
				h -= f * g;
				a[i][l] = f - g;
				f = 0.0;

				for ( j = 1; j <= l; j++ ) 
				{
					a[j][i] = a[i][j] / h;
					g = 0.0;

					for ( k = 1; k <= j; k++ )
					{
						g += a[j][k] * a[i][k];
					}

					for ( k = j+1; k <= l; k++ )
					{
						g += a[k][j] * a[i][k];
					}

					e[j] = g / h;
					f += e[j] * a[i][j];
				}

				hh = f / ( h + h );

				for ( j = 1; j <= l; j++ )  
				{
					f = a[i][j];
					e[j] = g = e[j] - hh * f;

					for ( k = 1; k <= j; k++ )
					{
						a[j][k] -= ( f * e[k] + g * a[i][k] );
					}
				}
			}
		}	
		else
		{
		   e[i] = a[i][l];
		}

		d[i] = h;
	}
	
	d[1] = 0.0;
	e[1] = 0.0;
	   
	for ( i = 1; i <= Nn; i++ )
	{
		l = i - 1;
		
		if ( d[i] )
		{
			for ( j = 1; j <= l; j++ )
			{
				g = 0.0;
				
				for ( k = 1; k <= l; k++ )
				{
					g += a[i][k] * a[k][j];
				}
				   
				for ( k = 1; k <= l; k++ )
				{
					a[k][j] -= g * a[k][i];
				}
			 }
		}
		 
		d[i] = a[i][i];
		a[i][i] = 1.0;
		
		for ( j = 1; j <= l; j++ )
		{
			a[j][i] = a[i][j] = 0.0;	  
		}
	}
} 
  
  

