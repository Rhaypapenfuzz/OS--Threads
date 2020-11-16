#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
  //add what is needed
  int x;
  int y;
} compute_stuct;

int MAX;

int ** matA; 
int ** matB; 

int ** matSumResult;
int ** matDiffResult; 
int ** matProductResult; 

void fillMatrix(int** matrix) {
  // Fill matrix with random values.
  for(int i = 0; i<MAX; i++) {
    for(int j = 0; j<MAX; j++) {
        matrix[i][j] = rand()%10+1;
    }
  }
}

void printMatrix(int** matrix) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the sum of the
// values at the coordinates of matA and matB.
void* computeSum(void* args) { // pass in the number of the ith thread
  compute_stuct * aArgs = args;
  int x = aArgs->x;
  int y = aArgs->y;
  matSumResult[x][y] = matA[x][y] + matB[x][y];
  free(aArgs);
  return NULL;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the difference of the
// values at the coordinates of matA and matB.
void* computeDiff(void* args) { // pass in the number of the ith thread
  compute_stuct * aArgs = args;
  int x = aArgs->x;
  int y = aArgs->y;
  matDiffResult[x][y] = matA[x][y] - matB[x][y];
  free(aArgs);
  return NULL;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the inner product
// of matA and matB.
void* computeProduct(void* args) { // pass in the number of the ith thread
  compute_stuct * aArgs = args;
  int x = aArgs->x;
  int y = aArgs->y;
  matProductResult[x][y] = matA[x][y] * matB[x][y];
  free(aArgs);
  return NULL;
}

// Spawn a thread to fill each cell in each result matrix.
// How many threads will you spawn?
int main(int argc, char *argv[]) {
  int i = 0;
  int j = 0;
  srand(time(0));  // Do Not Remove. Just ignore and continue below.

  // 0. Get the matrix size from the command line and assign it to MAX
  MAX = 4;
  if(argc > 1){
    MAX = atoi(argv[1]);
    if(MAX <= 0){
      MAX = 4;
    }
  }

  // 1. Fill the matrices (matA and matB) with random values.
  matA = malloc(MAX * sizeof(int *));
  matB = malloc(MAX * sizeof(int *));
  matSumResult = malloc(MAX * sizeof(int *));
  matDiffResult = malloc(MAX * sizeof(int *));
  matProductResult = malloc(MAX * sizeof(int *));
  for (i=0; i<MAX; i++){
    matA[i] = malloc(MAX * sizeof(int)); 
    matB[i] = malloc(MAX * sizeof(int));
    matSumResult[i] = malloc(MAX * sizeof(int));
    matDiffResult[i] = malloc(MAX * sizeof(int));
    matProductResult[i] = malloc(MAX * sizeof(int));
  }
  fillMatrix(matA);
  fillMatrix(matB);

  // 2. Print the initial matrices.
  printf("Matrix A:\n");
  printMatrix(matA);
  printf("Matrix B:\n");
  printMatrix(matB);

  // 3. Create pthread_t objects for our threads.
  pthread_t add_tid[MAX];
  pthread_t subtract_tid[MAX];
  pthread_t multiply_tid[MAX];
  
  // 4. Create a thread for each cell of each matrix operation.
    // 
    // You'll need to pass in the coordinates of the cell you want the thread
    // to compute.
    // 
    // One way to do this is to malloc memory for the thread number i, populate the coordinates
    // into that space, and pass that address to the thread. The thread will use that number to calcuate 
    // its portion of the matrix. The thread will then have to free that space when it's done with what's in that memory.
  int x = 0;
  int y = 0;
  // Adding.
  for(x = 0; x < MAX; x++){
    for(y = 0; y < MAX; y++){
      // Thread that is assigned is picked.
      int threadNum = ((x*MAX)+y) % MAX;
      
      // Pass needed arguments.
      compute_stuct *addArgs = malloc(sizeof *addArgs);
      addArgs->x = x;
      addArgs->y = y;

      // Add thread.
      if(pthread_create(&add_tid[threadNum], NULL, computeSum, addArgs)){
        free(addArgs);
      }
      
      // Pass needed arguments.
      compute_stuct *subArgs = malloc(sizeof *subArgs);
      subArgs->x = x;
      subArgs->y = y;

      // Subtract thread.
      if(pthread_create(&subtract_tid[threadNum], NULL, computeDiff, subArgs)){
        free(subArgs);
      }
      
      // Pass needed arguments.
      compute_stuct *mltArgs = malloc(sizeof *subArgs);
      mltArgs->x = x;
      mltArgs->y = y;
      
      // Multiply thread.
      if(pthread_create(&multiply_tid[threadNum], NULL, computeProduct, mltArgs)){
        free(subArgs);
      }
    }
  }
  
  // 5. Wait for all threads to finish.
  for(i = 0; i < MAX; i++){
    pthread_join(add_tid[i], NULL);
    pthread_join(subtract_tid[i], NULL);
    pthread_join(multiply_tid[i], NULL);
  }
  
  // 6. Print the results.
  printf("Results:\n");
  printf("Sum:\n");
  printMatrix(matSumResult);
  printf("Difference:\n");
  printMatrix(matDiffResult);
  printf("Product:\n");
  printMatrix(matProductResult);
  
  // 7. Free memory.
  for(i = 0; i < MAX; i++){
    free(matA[i]);
    free(matB[i]);
  }
  free(matA);
  free(matB);
  return 0;
}