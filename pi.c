#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

//REDUCE
int MPI_FlattreeColectiva(void * buff, void *recvbuff, int count,
                          MPI_Datatype datatype, int root, MPI_Comm comm){
    int numprocs,rank;
    double n,totalCount;
    MPI_Status status;
    MPI_Comm_size(comm,&numprocs);
    MPI_Comm_rank(comm,&rank);

    if(rank==root){
        totalCount = *(double*) buff;
        for(int i=0;i<numprocs;i++){
            if(i!=root){
                MPI_Recv(&n,count,datatype,MPI_ANY_SOURCE,0,comm,&status); 
                totalCount+= n;   
            }
        }
        *(double*) recvbuff = totalCount; 
    }
    else
        MPI_Send(buff,1,datatype,root,0,comm);
    return MPI_SUCCESS;
}

//BROADCAST
int MPI_BinomialBCast(void *buff, int count, MPI_Datatype datatype,
                      int root, MPI_Comm comm){
    int mask = 0x1;
    int rank,numprocs;

    MPI_Status status;
    MPI_Comm_size(comm,&numprocs);
    MPI_Comm_rank(comm,&rank);

    while (mask < numprocs) {

        if(rank < mask && (rank + mask < numprocs)){
            MPI_Send(buff, count, datatype, (rank + mask), 0, comm);
        } else if(rank >= mask && rank < mask << 1){
            MPI_Recv(buff, count, datatype, (rank - mask), 0, comm, &status);
        }

        mask <<= 1;
    }
    return MPI_SUCCESS;
}

int main(int argc, char *argv[]) {
    int i, done = 0, n, count, localCount;
    int numprocs,rank;
    double PI25DT = 3.141592653589793238462643;
    double pi, x, y, z;
    
    MPI_Status status;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    while (!done) {
        if(rank==0){
            printf("Enter the number of points: (0 quits) \n");
            scanf("%d",&n);
        }
        MPI_BinomialBCast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
        if (n == 0)
            break;
                
        count = 0;  

        for (i = rank+1; i <= n; i+=numprocs) {
            // Get the random numbers between 0 and 1
            x = ((double) rand()) / ((double) RAND_MAX);
            y = ((double) rand()) / ((double) RAND_MAX);

            // Calculate the square root of the squares
            z = sqrt((x*x)+(y*y));

            // Check whether z is within the circle
            if(z <= 1.0)
                count++;
        }
        localCount=count;
        MPI_FlattreeColectiva(&localCount,&count,1,MPI_INT,0,MPI_COMM_WORLD);
        if(rank==0){
            pi = ((double) count/(double) n)*4.0;
            printf("pi is approx. %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));   
        }
    }
     MPI_Finalize();
}
