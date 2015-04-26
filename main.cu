#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <curand.h>
#include <curand_kernel.h>


#include "main.h"
extern "C" {
#include "fileio.h"
}


__global__ void mykernel() {

}

bool debug_trace = true;
int ndecks, ntrials, houserules, strategy;
float penet, bank, minbet, betspread;
char mystring[64];
int shoe[52*8]; 

int main(int argc, char* argv[]) {

  int i;

  int seed = time(NULL);  
  srand(seed);

  read_params(argv[1]);
  initialize_shoe(shoe, ndecks);
  shuffle(shoe, ndecks);

  for(i = 0; i < ndecks*52; i++) {
    printf("shoe[%d] = %d\n", i, shoe[i]);
  }
  
}


void read_params(char* fname) {

  sprintf(mystring,"debug_trace");
  get_bool_param(fname, mystring, &debug_trace, true);
  sprintf(mystring,"ndecks");
  get_int_param(fname, mystring, &ndecks, debug_trace);
  if (ndecks < 1) { ndecks = 1; }
  if (ndecks > 8) { ndecks = 8; }
  sprintf(mystring,"ntrials");  
  get_int_param(fname, mystring, &ntrials, debug_trace);
  sprintf(mystring,"houserules");  
  get_int_param(fname, mystring, &houserules, debug_trace);
  sprintf(mystring,"strategy");  
  get_int_param(fname, mystring, &strategy, debug_trace);      
  sprintf(mystring,"penetration");  
  get_real_param(fname, mystring, &penet, debug_trace);
  if (penet < 0.0) { penet = 10.0; }
  if (penet > 100.0) { penet = 100.0; }  
  sprintf(mystring,"bank");  
  get_real_param(fname, mystring, &bank, debug_trace);
  sprintf(mystring,"minbet");  
  get_real_param(fname, mystring, &minbet, debug_trace);
  if (minbet > bank) { minbet = bank; }
  sprintf(mystring,"betspread");  
  get_real_param(fname, mystring, &betspread, debug_trace);  
  
}

void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}


// Fischer-Yates random permutation of arr[]
// Move this to GPU eventually
void shuffle( int* arr, int ndeck ) {

  int ii, jj, kk;
  //  srand ( time(NULL) );

  // Shuffle thrice, just 'cause
  for (kk = 0; kk < 3; kk++) {
    // Start from the last element and swap one by one. We don't
    // need to run for the first element that's why ii > 0
    for (ii = ndeck*52-1; ii > 0; ii--){

      // Pick a random index from 0 to ii
      jj = rand() % (ii+1);

      // Swap arr[i] with the element at random index
      swap(&arr[ii], &arr[jj]);
    }
  }
}

void initialize_shoe(int* arr, int ndeck) {

  // 1s are aces, J = 11, Q = 12, K = 13
  for (int ii = 0; ii < ndeck; ii++) {
    for (int jj = 0; jj < 4; jj++) {    
      for (int kk = 0; kk < 13; kk++) {
	arr[ii*52+jj*13+kk] = kk+1;
      }
    }
  }


}
