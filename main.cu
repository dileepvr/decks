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

bool debug_trace = true, psoft = false, dsoft = false;
int ndecks, ntrials, houserules, strategy, nbets;
float penet, bank, startbank, minbet, betspread;
char mystring[64];
int shoe[52*8], dealer[21], player[8][21];
float bets[8];
int ncards, maxcardpos, cardpos = 0;
int handno, cardno, nhands, ptotal[8], paces[8], dtotal, daces;

int main(int argc, char* argv[]) {

  int i;
  int seed = time(NULL);  
  srand(seed);

  read_params(argv[1]);
  initialize_shoe(shoe, ndecks);
  shuffle(shoe, ndecks);

  ncards = 52*8;
  maxcardpos = floor(ncards*penet);

  startbank = bank;
  for(i = 0; i < ntrials; i++) {
    play();
  }
}

void play() {
  int curbets = 0, playeraction, flag = 0;
  bank = startbank;
  while ( !trialover(curbets) ) {
    handno = 0, nhands = 1;    
    opendraw();
    while( flag == 0 ) {
      playeraction = verb(1);      
      switch(playeraction) {
      case 0: // surrender
	bank += bets[handno--]/2.0;
	if (handno < 0) { flag = 1; } // Last hand surrendered
	else { recomputeptotal(); }
	break; 
      case 1: // stand
	if (--handno < 0) { flag = 2; } // Last hand stood
	else { recomputeptotal(); }	
	break;
      case 2: // hit
	hitdeal();
	if (handbust()) {
	  if (--handno < 0) { flag = 3; } // Last hand busted
	  else { recomputeptotal(); }	  
	}
	break;
      case 3: // double
	bank -= bets[handno];
	bets[handno] = 2*bets[handno];
	hitdeal();
	if (--handno < 0) { flag = 4; } // Last hand doubled
	else { recomputeptotal(); }	
	break;
      case 4: // split
	splitdeal(); // Not checking if split command is valid
	break;
      default: // do nothing
	break;
      }
    }
    resolvedeal();
  }
}

void resolvedeal() {

  // Check player busts and finish dealer card draws
  handno = nhands-1;
  
}

void splitdeal() {
  nhands++;
  if( cardpos >= maxcardpos - 2 ) {
    shuffle(shoe, ndecks);
    cardpos = 0;
  }
  bank -= bets[handno++];
  bets[handno] = bets[handno-1];
  player[handno+1][0] = player[handno++][1];
  player[handno-1][1] = shoe[cardpos++];
  player[handno][1] = shoe[cardpos++];
  cardno = 1;

  if (player[handno][0] == 1) {
    ptotal[handno] = 11; psoft = true; paces[handno]++;
  } else if (player[handno][0] > 10) {
    ptotal[handno] = 10;
  } else { ptotal[handno] = player[handno][0]; }

  if (player[handno][1] == 1) {
    ptotal[handno] += 11; psoft = true; paces[handno]++;
  } else if (player[handno][1] > 10) {
    ptotal[handno] += 10;
  } else { ptotal[handno] += player[handno][1]; }
  
}

void hitdeal() {
  if( cardpos >= maxcardpos ) {
    shuffle(shoe, ndecks);
    cardpos = 0;
  }
  player[handno][cardno++] = shoe[cardpos++];
  updateptotal();
}

void recomputeptotal() {

  cardno = 0; ptotal[handno] = 0;
  while( player[handno][cardno] != 0 ) {
    if (player[handno][cardno] == 1) {
      ptotal[handno] += 11; psoft = true;
      paces[handno]++;
    } else if (player[handno][cardno] > 10) {
      ptotal[handno] += 10;
    } else { ptotal[handno] += player[handno][cardno]; }
    cardno++;
  }
  cardno--;
  while(paces[handno] > 0) {
    ptotal[handno] -= 10; paces[handno]--;
    if (ptotal[handno] < 21) { break; }
  }
  if (paces[handno] == 0) { psoft = false; }

}

void updateptotal() {

  if (player[handno][cardno-1] == 1) {
    ptotal[handno] += 11; psoft = true; paces[handno]++;
  } else if (player[handno][cardno-1] > 10) {
    ptotal[handno] += 10;
  } else { ptotal[handno] += player[handno][cardno-1]; }

  if (ptotal[handno] > 21 ) {
    for( int ll = 0; ll < cardno; ll++) {
      if (player[handno][ll] == 1) { paces[handno]++; }
    }
    while(paces[handno] > 0) {
      ptotal[handno] -= 10; paces[handno]--;
      if (ptotal[handno] < 21) { break; }
    }
    if (paces[handno] == 0) { psoft = false; }
  }
  
}

void opendraw() {
  cleartable();
  if( cardpos >= maxcardpos - 4 ) {
    shuffle(shoe, ndecks);
    cardpos = 0;
  }
  bets[handno] = verb(0);
  bank -= bets[handno];
  // dealer[0] card is hidden from player
  dealer[0] = shoe[cardpos++];
  dealer[1] = shoe[cardpos++];
  player[handno][0] = shoe[cardpos++];
  player[handno][1] = shoe[cardpos++];
  cardno = 1;

  if (dealer[1] == 1) {
    dtotal = 11; dsoft = true; daces++;
  } else if (dealer[1] > 10) {
    dtotal = 10;
  } else { dtotal = dealer[1]; }
  
  if (player[handno][0] == 1) {
    ptotal[handno] = 11; psoft = true; paces[handno]++;
  } else if (player[handno][0] > 10) {
    ptotal[handno] = 10;
  } else { ptotal[handno] = player[handno][0]; }

  if (player[handno][1] == 1) {
    ptotal[handno] += 11; psoft = true; paces[handno]++;
  } else if (player[handno][1] > 10) {
    ptotal[handno] += 10;
  } else { ptotal[handno] += player[handno][1]; }
  
}

bool handbust() {
  if (ptotal[handno] > 21 && paces[handno] == 0) { return true; }
  return false;
}

int verb(int state) {
  // This is the player action function
  switch(state) {
  case 0: // pre-deal betting cycle, return bet amount
    return openbet();
  case 1: // cards dealt, return action
    return pdecision();
  default: // Same as case 0
    return openbet();
  }
}

int openbet() {
  switch(strategy) {
  case 0: // Test strategy
    return minbet;
  default: // Same as case 0
    return minbet;
  }
}

int pdecision() {
  /* return meanings
     0 == surrender
     1 == stand
     2 == hit
     3 == double
     4 == split
  */

  switch(strategy) {
  case 0: // Test strategy, hit if ptotal < 17, else stand
  default: // Same as case 0
    if (ptotal[handno] < 17) { return 2; }
    else { return 1; }
  }
 
}

void cleartable() {
  // Clean up hands
  for(int i = 0; i < 21; i++) {
    dealer[i] = 0;
    for(int j = 0; j < 8; j++) { player[j][i] = 0; }
  }
  for(int k = 0; k < 8; k++) {
    bets[k] = 0.0; ptotal[k] = 0; paces[k] = 0;
  }
  dtotal = 0; psoft = false; dsoft = false; daces = 0;
}

bool trialover(int curbetno) {
  if (nbets > 0) {
    if (curbetno > nbets || bank <= 0) {
      return true; } else {return false;}
  } else if (nbets < 0) {
    if (curbetno > abs(nbets)) { return true; }
    else { return false; }
  } else {
    if (bank <= 0) { return true; } else { return false; }
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
  sprintf(mystring,"nbets");  
  get_int_param(fname, mystring, &nbets, debug_trace);
  sprintf(mystring,"penetration");  
  get_real_param(fname, mystring, &penet, debug_trace);
  if (penet < 2.0) { penet = 10.0; }
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
