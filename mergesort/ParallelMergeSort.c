#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SIZE 100

int array[SIZE];
int alternate[SIZE-1];

void fillArrayWithRandomNumbers(int arr[SIZE]) {
		int i;
    for(i = 0; i<SIZE; i++) array[i] = rand()%100;
}

void printArray(int arr[SIZE]) {
		int i;
    for(i = 0; i<SIZE; i++) printf("%5d", arr[i]);
    printf("\n");
}

typedef struct StartEndIndexes {
    int start;
    int end;
} StartEndIndexes;

void merge(int low, int mid, int high) {
   int l1, l2, i;
   for(l1 = low, l2 = mid + 1, i = low; l1 <= mid && l2 <= high; i++) {
      if(array[l1] <= array[l2])
         alternate[i] = array[l1++];
      else
         alternate[i] = array[l2++];
   }
   while(l1 <= mid)    
      alternate[i++] = array[l1++];
   while(l2 <= high)  
      alternate[i++] = array[l2++];
   for(i = low; i <= high; i++)
      array[i] = alternate[i];
}

// Runs mergesort on the array segment described in the
// argument. Spawns two threads to mergesort each half
// of the array segment, and then merges the results.
void* mergeSort(void* args) {
     int mid;
     StartEndIndexes args_temp = *(StartEndIndexes*) args;
     if (args_temp.start < args_temp.end) {
        mid = (args_temp.start + args_temp.end) / 2;
        StartEndIndexes sei_temp;
        sei_temp.start = args_temp.start;
        sei_temp.end = mid;
        mergeSort(&sei_temp);
        sei_temp.start = mid+1;
        sei_temp.end = args_temp.end;
        mergeSort(&sei_temp);
        merge(args_temp.start, mid, args_temp.end);
     } else {
        return 0;
     }  
}

int main() {
    srand(time(0));
    StartEndIndexes sei;
    sei.start = 0;
    sei.end = SIZE - 1;
    int mid = (sei.start + sei.end)/2;
 
    StartEndIndexes sei_1;
    sei_1.start = 0;
    sei_1.end = mid;
 
    StartEndIndexes sei_2;
    sei_2.start = mid+1;
    sei_2.end = SIZE - 1;
   
    // 1. Fill array with random numbers.
    fillArrayWithRandomNumbers(array);
   
    // 2. Print the array.
    printf("Unsorted array: ");
    printArray(array);
    printf("\n\n");
 
    // 3. Create 2 threads for merge sort.
    pthread_t thread_id1;
    pthread_t thread_id2; 
    pthread_create(&thread_id1, NULL, mergeSort, &sei_1);
    pthread_create(&thread_id2, NULL, mergeSort, &sei_2);
 
    int* ptr_1;
    int* ptr_2;
    // 4. Wait for mergesort to finish.
    pthread_join(thread_id1, (void**)&ptr_1);
    pthread_join(thread_id2, (void**)&ptr_2);

    // 5. Print the sorted array.
		merge(sei.start, mid , sei.end);
    printf("Sorted array:   ");
    printArray(array);
}
