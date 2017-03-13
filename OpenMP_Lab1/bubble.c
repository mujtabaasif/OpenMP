#include <stdio.h>
#include<omp.h>

#include <x86intrin.h>
typedef int bool;
#define true 1
#define false 0

#define NBEXPERIMENTS    7
static long long unsigned int experiments [NBEXPERIMENTS] ;

/* 
  bubble sort -- sequential, parallel -- 
*/

static   unsigned int N ;

typedef  int  *array_int ;

/* the X array will be sorted  */

static   array_int X  ;

long long unsigned int average (long long unsigned int *exps)
{
  unsigned int i ;
  long long unsigned int s = 0 ;

  for (i = 2; i < (NBEXPERIMENTS-2); i++)
	{
	  s = s + exps [i] ;
	}

  return s / (NBEXPERIMENTS-2) ;
}

void init_array (array_int T)
{
  register int i ;

  for (i = 0 ; i < N ; i++)
	{
	  T [i] = N - i ;
	}
}

void print_array (array_int T)
{
  register int i ;

  for (i = 0 ; i < N ; i++)
	{
	  printf ("%d ", T[i]) ;
	}
  printf ("\n") ;
}

/*
  test if T is properly sorted
 */
int is_sorted (array_int T)
{
  register int i ;
  
  for (i = 1 ; i < N ; i++)
	{
	  if (T[i-1] > T [i])
	return 0 ;
	}
  return 1 ;
}

void sequential_bubble_sort (int *T, const int size)
{
	/* TODO: sequential implementation of bubble sort */ 

	register int i;
	register int k;
	while(is_sorted(T) == 0){
	  for(i = 0; i < size - 1; i++){
		if(T[i] > T[i + 1]){
		  k = T[i+1];
		  T[i+1] = T[i];
		  T[i] = k;
		}
	  }
   }
	return ;
}

void parallel_bubble_sort (int *T, const int size)
{
	/* TODO: parallel implementation of bubble sort */
  
	register int i;
	register int j;
	register int k;
	int chunksize = 16;
	for(j = 0; j < size; j++){
	  
	  #pragma omp parallel for schedule (static) private (i,k)
	  for(i = 0; i < size - 1; i++){
		if(T[i] > T[i + 1]){
		  k = T[i+1];
		  T[i+1] = T[i];
		  T[i] = k;
		}
	  }
	  #pragma omp single
	  {
		for(int m=1;m<size/chunksize; m++){
		  if(T[(m * chunksize)-1] > T[(m * chunksize)]){
			int s = T[(j * chunksize)-1];
			T[(m * chunksize)-1] = T[(m * chunksize)];
			T[(m * chunksize)] = s;
			//check2 = false;
		  }
		  
		} 
	  }
	  #pragma omp barrier
	}
	//print_array(T);
  return ;
}


int main (int argc, char **argv)
{
  unsigned long long int start, end, residu ;
  unsigned long long int av ;
  unsigned int exp ;

  /* the program takes one parameter N which is the size of the array to
	 be sorted. The array will have size 2^N */
  if (argc != 2)
	{
	  fprintf (stderr, "bubble N \n") ;
	  exit (-1) ;
	}

  N = 1 << (atoi(argv[1])) ;
  X = (int *) malloc (N * sizeof(int)) ;

  printf("--> Sorting an array of size %u\n",N);
  
  start = _rdtsc () ;
  end   = _rdtsc () ;
  residu = end - start ;
  

  for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
	{
	  init_array (X) ;
	  
	  start = _rdtsc () ;

		 sequential_bubble_sort (X, N) ;
	 
	  end = _rdtsc () ;
	  experiments [exp] = end - start ;

	  /* verifying that X is properly sorted */
	  if (! is_sorted (X))
	{
	  fprintf(stderr, "ERROR: the array is not properly sorted\n") ;
	  exit (-1) ;
	}
	}

  av = average (experiments) ;  

  printf ("\n bubble serial \t\t\t %Ld cycles\n\n", av-residu) ;

  
  for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
	{
	  init_array (X) ;
	  start = _rdtsc () ;

		  parallel_bubble_sort (X, N) ;
	 
	  end = _rdtsc () ;
	  experiments [exp] = end - start ;

	  /* verifying that X is properly sorted */
	  if (! is_sorted (X))
	{
			fprintf(stderr, "ERROR: the array is not properly sorted\n") ;
			exit (-1) ;
	}
	}

  av = average (experiments) ;  
  printf ("\n bubble parallel \t %Ld cycles\n\n", av-residu) ;

  
  // print_array (X) ;

}
