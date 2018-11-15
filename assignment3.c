#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define a 100

int main(int argc, char* argv[])
{
	int rawNum[a];
  	int sortNum[a];
  	int* smallBucket;
  	int rank,size;
  	int* pCount;
  	int* disp;
  	MPI_Status status;
  	int i,j,n,counter;
  	int local_min,local_max;
  	int tmp;

  	MPI_Init(&argc,&argv);

  	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  	MPI_Comm_size(MPI_COMM_WORLD,&size);

 	if (rank == 0)
	{
    		for (i = 0; i < a; i++)
		{
      			rawNum[i] = rand() % a;
    		}

    		for (i = 1; i < size; i++)
		{
      			MPI_Send(rawNum,S,MPI_INT,i,1,MPI_COMM_WORLD);
    		}
  	}
  
	else 
	{
    		MPI_Recv(rawNum,a,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
  	}



  	counter = 0;
  	local_min = rank * (a/size);
  	local_max = (rank + 1) * (a/size);

  	for (i = 0; i < a; i++)
	{
    		if ((rawNum[i] >= local_min) && (rawNum[i] < local_max))
		{
      			counter += 1;
    		}
  	}

  	smallBucket = malloc(counter * sizeof(int));

  	counter = 0;
  	for (i = 0; i < a; i++)
	{
    		if ((rawNum[i] >= local_min) && (rawNum[i] < local_max))
		{
      			smallBucket[counter] = rawNum[i];
      			counter += 1;
    		}
  	}

	
  	for (i = 0; i < counter; i++)
	{
    		for (j = i+1; j < counter; j++)
		{
      			if (smallBucket[i] > smallBucket[j])
			{
        			tmp = smallBucket[i];
        			smallBucket[i] = smallBucket[j];
        			smallBucket[j] = tmp;
      			}
    		}
  	}

  	
  	if (rank == 0)
	{
      		pCount = malloc(size * sizeof(int));
    		disp = malloc(size * sizeof(int));
  	}

  	MPI_Gather(&counter,1,MPI_INT,pCount,1,MPI_INT,0,MPI_COMM_WORLD);


  	if (rank == 0)
	{
    		disp[0] = 0;
    		for (i = 0; i < size-1; i++)
		{
      			disp[i+1] = disp[i] + pCount[i];
    		}
  	}

  	MPI_Gatherv(smallBucket,counter,MPI_INT,sortNum,pCount,disp,MPI_INT,0,MPI_COMM_WORLD);

  	if (rank == 0)
	{
    		printf("Before sort: \n");
    		for (i = 0; i < a; i++) 
			printf("%d ",rawNum[i]);
    		printf("\nAfter sort: \n");
    		for (i = 0; i < S; i++) 
			printf("%d ",sortNum[i]);
  	}

  	MPI_Finalize();
  	return 0;
}
