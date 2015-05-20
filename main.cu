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
bool record_allbank = false;
int ndecks, ntrials, houserules, strategy, nbets;
float penet, bank, startbank, minbet, betspread;
char mystring[64];
int shoe[52*8], dealer[21], player[8][21];
float bets[8];
int ncards, maxcardpos, cardpos = 0;
int handno, cardno, nhands, ptotal[8], paces[8], dtotal, daces, dcardno;
float *allbank;

int main(int argc, char* argv[]) {

  int i;
  int seed = time(NULL);  
  srand(seed);

  read_params(argv[1]);
  initialize_shoe(shoe, ndecks);
  shuffle(shoe, ndecks);

  ncards = 52*8;
  maxcardpos = floor(ncards*penet);


  if(record_allbank) {
    allbank = (float*)malloc(sizeof(float)*ntrials*nbets);
  }
  
  startbank = bank;
  if(debug_trace) { printf("Playing trials...."); }
  for(i = 0; i < ntrials; i++) {
    play(i);
  }
  if(debug_trace) { printf(" done.\n"); }  
 
  if(record_allbank) {

    sprintf(mystring,argv[1]);
    strcat(mystring,".allbank.dat");
    if(debug_trace) {
      printf("Writing allbank to %s ....", mystring);
    }    
    write_allbank(mystring, allbank, ntrials, nbets);
    if(debug_trace) { printf(" done.\n"); }    
    free(allbank);

  }
 
}

void play(int trialnum) {
  int curbets = 1, playeraction, flag = 0;
  bank = startbank;
  while ( !trialover(curbets) ) {
    //    curbets++;
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
    if(record_allbank) {
      allbank[nbets*trialnum+curbets-1] = bank;
    }
    curbets++;
    //    printhands();
  }
}

void printhands() {
  int ll, temp;

  printf("Dealer: ");
  for(ll = 0; ll < dcardno; ll++) {
    printf("%d ", dealer[ll]);
  }
  printf("\ndtotal = %d ", dtotal);
  if (dtotal > 21) { printf("(BUST!)\n"); }
  else { printf("\n"); }

  for(temp = 0; temp < nhands; temp++) {
    printf("Player hand %d: ", temp);
    ll = 0;
    while(player[temp][ll] != 0) {
      printf("%d ", player[temp][ll++]);
    } printf("\nptotal = %d ",ptotal[temp]);
    if(ptotal[temp] > 21){
      printf("(BUST!)\n");
    } else if (ptotal[temp] == dtotal) {
      printf("(PUSH!)\n");      
    } else if((ptotal[temp] > dtotal) || (dtotal > 21)) {
      printf("(WIN!)\n");            
    } else {
      printf("(LOSS!)\n");                  
    }
  }
}

void resolvedeal() {

  // Surrender not being accounted for right now
  bool allbusted = true;
  dcardno = 2;
  // Check player busts and finish dealer card draws
  for(handno = 0; handno < nhands; handno++) {
    recomputeptotal();
    if(ptotal[handno] <= 21) { allbusted = false; }
  }

  if(!allbusted) {
    if(dealer[0] == 1) {
      dtotal += 11; dsoft = true; daces++;
    } else if (dealer[0] > 10) {
      dtotal += 10;
    } else { dtotal += dealer[0]; }

    if(dtotal > 21) {
      while(daces > 0) {
	dtotal -= 10;
	if(dtotal <= 21) { daces--; break; }
      }
    }

    // Modify soft-17 rule based on 'houserules' parameter
    while((dtotal < 17) || ((dtotal == 17) && (daces > 0))) {
      if( cardpos >= maxcardpos ) {
	shuffle(shoe, ndecks);
	cardpos = 0;
      }
      dealer[dcardno++] = shoe[cardpos++];
      updatedtotal();
    }

    for(handno = 0; handno < nhands; handno++) {
      if((ptotal[handno] <= 21) && ((ptotal[handno] > dtotal) || (dtotal > 21))) {
	// Check for Blackjack
	// Modify this according to 'houserules' parameter
	if((ptotal[handno] == 21) && (player[handno][2] == 0)) {
	  bank += 2.5*bets[handno];
	} else {
	  bank += 2*bets[handno];
	}
      }

    }
  }
  
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

  cardno = 0; ptotal[handno] = 0; paces[handno] = 0;
  while( player[handno][cardno] != 0 ) {
    if (player[handno][cardno] == 1) {
      ptotal[handno] += 11; psoft = true;
      paces[handno]++;
    } else if (player[handno][cardno] > 10) {
      ptotal[handno] += 10;
    } else { ptotal[handno] += player[handno][cardno]; }
    cardno++;
  }
  //  cardno--;
  if (ptotal[handno] > 21) {
    while(paces[handno] > 0) {
      ptotal[handno] -= 10; paces[handno]--;
      if (ptotal[handno] <= 21) { break; }
    }
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
    /*    for( int ll = 0; ll < cardno; ll++) {
	  if (player[handno][ll] == 1) { paces[handno]++; }
	  }
    */
    while(paces[handno] > 0) {
      ptotal[handno] -= 10; paces[handno]--;
      if (ptotal[handno] < 21) { break; }
    }
    if (paces[handno] == 0) { psoft = false; }
  }
  
}

void updatedtotal() {

  if (dealer[dcardno-1] == 1) {
    dtotal += 11; dsoft = true; daces++;
  } else if (dealer[dcardno-1] > 10) {
    dtotal += 10;
  } else { dtotal += dealer[dcardno-1]; }

  if (dtotal > 21 ) {
    /*    for( int ll = 0; ll < dcardno; ll++) {
	  if (dealer[ll] == 1) { daces++; }
	  }
    */
    while(daces > 0) {
      dtotal -= 10; daces--;
      if (dtotal< 21) { break; }
    }
    if (daces == 0) { dsoft = false; }
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
  cardno = 2;

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
  if (ptotal[handno] > 21 && paces[handno] == 0) {
    return true;
  }
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
    if (ptotal[handno] < 17) {
      return 2;
    } else {
      return 1;
    }
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
  sprintf(mystring,"record_allbank");
  get_bool_param(fname, mystring, &record_allbank, debug_trace);
  
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
