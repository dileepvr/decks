#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <time.h>
//#include <curand.h>
//#include <curand_kernel.h>


#include "main.h"
#include "basicstrategy.h"
//extern "C" {
#include "fileio.h"
//}

/*
__global__ void mykernel() {

}
*/

bool debug_trace = true, debug_printhands = false, psoft = false, dsoft = false;
bool record_allbank = false;
int ndecks, ntrials, strategy, nbets, hoptc = 0;
bool dhitssoft17 = false, dblaftsplit = true, resplitaces = false;
bool hitsplitaces = false, surrndr = false, bj3to2 = true;
bool hoptables = false;
int resplithandsmax = 4;
float penet, bank, startbank, minbet, betspread;
char mystring[64];
int shoe[416], player[8][21];
int dealer[21];
float bets[8];
int ncards, maxcardpos, cardpos = 0;
int handno, cardno, nhands, ptotal[8], paces[8], dtotal, daces, dcardno;
float *allbank;

// shoe_counts[:] are total counts of shoe so far
// true_counts[:] are true counts (divided by num of decks left)
// order: Hi-Lo, Hi-OptI, Hi-OptII, KO, OmegaII, Zen
int shoe_counts[6];
float true_counts[6];
int myflag = 0;
// Variables for statistics
float avegain = 0.0, siggain = 0.0, drawavegain = 0.0, drawsiggain = 0.0;
float maxgain = 0.0, mingain = 0.0, maxbank = 0.0, minbank = 0.0;
float maxmidbank = 0.0, minmidbank = 0.0;
double avebet = 0.0, sigbet = 0.0, maxbet = 0.0, minebet = 0.0;
int nwinhands = 0, ndbusts = 0, npbusts = 0;
float fwinhands = 0.0, fdbusts = 0.0, fpbusts = 0.0;
int nsur = 0, nstd = 0, nhits = 0, ndbls = 0, nsplts = 0, nbjs = 0, npush = 0;
long nbankrupt = 0, totalbets = 0;
float fsur = 0.0, fstd = 0.0, fhits = 0.0, fdbls = 0.0, fsplts = 0.0;
// double tcgainhist[321];
// long tcnumlist[321];

int *testarray;

int main(int argc, char* argv[]) {

  int i;
  int seed = time(NULL);  
  srand(seed);
  /*
  for(i = 0; i < 321; i++) {
    tcgainhist[i] = 0.0;
    tcnumlist[i] = 0;
  }
  */
  read_params(argv[1]);
  minebet = minbet;
  initialize_shoe(shoe, ndecks);
  shuffle(shoe, ndecks);


  ncards = 52*8;
  maxcardpos = floor(52*ndecks*penet);

  if(record_allbank) { 
    allbank = (float*)malloc(sizeof(float)*ntrials*nbets);
  }
  testarray = (int*)malloc(sizeof(int)*21);

  
  startbank = bank;
  minbank = bank;
  minebet = bank;

  
  if(debug_trace) { printf("Playing trials...."); }
  for(i = 0; i < ntrials; i++) {
    play(i);
  }
  if(debug_trace) { printf(" done.\n"); }

  if(debug_trace) { printf("Computing statistics...."); }
  compute_stats();
  if(debug_trace) { printf(" done.\n"); }

  sprintf(mystring,argv[1]);
  strcat(mystring,".stats.dat");
  if(debug_trace) { printf("Writing stats...."); }
  write_stats(mystring);
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
  free(testarray);    
  return 0;
}

void play(int trialnum) {
  int curbets = 1, playeraction, flag = 0, curtc;
  float stbank, stavebet;
  bank = startbank;
  nsur = 0; nstd = 0; nhits = 0; ndbls = 0; nsplts = 0;  
  while ( !trialover(curbets) ) {

    handno = 0, nhands = 1;
    stbank = bank;
    stavebet = avebet;


    // ADD IN SITTING OUT HANDS (really, running away)
    if (hoptables) {
      if( true_counts[0] < hoptc ) {
	while( true_counts[0] < 0 ) {
	  int jayjay;
	  for( jayjay=0; jayjay<10; jayjay++){
	    update_shoe_counts(shoe[cardpos++]);
	    if( cardpos >= maxcardpos ) {
	      shuffle(shoe, ndecks);
	      cardpos = 0;
	    }
	  }
	}
      }
    }

    curtc = true_counts[0];
    
    opendraw();
    flag = 0;
    while( flag == 0 ) {
      playeraction = verb(1);
      switch(playeraction) {
      case 0: // surrender
	nsur++;
	bank += bets[handno--]/2.0;
	if (handno < 0) { flag = 1; } // Last hand surrendered
	else { recomputeptotal(); }
	break; 
      case 1: // stand
	nstd++; 
	if (--handno < 0) { flag = 2; } // Last hand stood
	else { recomputeptotal(); }	
	break;
      case 2: // hit
	nhits++;
	hitdeal();
	if (handbust()) {
	  if (--handno < 0) { flag = 3; } // Last hand busted
	  else { recomputeptotal(); }	  
	}
	break;
      case 3: // double
	ndbls++;
	bank -= bets[handno];
	avebet = avebet + bets[handno];
	sigbet = sigbet + 3*bets[handno]*bets[handno];
	bets[handno] = 2*bets[handno];
	hitdeal();
	if (--handno < 0) { flag = 4; } // Last hand doubled
	else { recomputeptotal(); }	
	break;
      case 4: // split
	nsplts++;
	splitdeal(); // Not checking if split command is valid
	// Check if Aces were split, and change flag if so
	if((player[handno][0] == 1) && (player[handno-1][0] == 1) && (!hitsplitaces)) {
	  handno = handno - 2;
	  if (handno < 0) { flag = 5; }
	  else { recomputeptotal(); }
	}
	break;
      default: // do nothing
	break;
      }
    }
    resolvedeal();
    //	printf("t= %d, b = %d, avebet = %f, sigbet = %f\n", trialnum, curbets, avebet, sigbet);

    //    allbank[nbets*trialnum+curbets-1] = bank;
    if((bank - stbank) > maxgain) { maxgain = bank - stbank; }
    if((bank - stbank) < mingain) { mingain = bank - stbank; }
    if(bank > maxmidbank) { maxmidbank = bank; }
    if(bank < minmidbank) { minmidbank = bank; }
    if((avebet - stavebet) > maxbet) { maxbet = avebet - stavebet; }
    if((avebet - stavebet) < minebet) { minebet = avebet - stavebet; }

    if (record_allbank) { allbank[nbets*trialnum+curbets-1] = bank; }
    
    curbets++;

    if( debug_printhands ) { printhands(); }

    //    printf("trail no: %d. bet no. %d\n", trialnum, curbets);

  }

  if (bank > maxbank) { maxbank = bank; }
  if (bank < minbank) { minbank = bank; }
  if (bank <= 0.0) { nbankrupt++; }
  
  avegain = avegain + bank - startbank;
  siggain = siggain + (bank - startbank)*(bank - startbank);
  drawavegain = drawavegain + (bank - startbank)/(curbets-1);
  drawsiggain = drawsiggain + (bank - startbank)/(curbets-1)*(bank - startbank)/(curbets-1);
  fsur = fsur + 1.0*nsur;///(curbets-1);
  fstd = fstd + 1.0*nstd;///(curbets-1);
  fhits = fhits + 1.0*nhits;///(curbets-1);
  fdbls = fdbls + 1.0*ndbls;///(curbets-1);
  fsplts = fsplts + 1.0*nsplts;///(curbets-1);  
  totalbets = totalbets + curbets - 1;
  
}

void printhands() {
  int ll, temp;

  printf("Bank: %f \n", bank);

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
    } printf("\nptotal = %d ", ptotal[temp]);
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
  update_shoe_counts(dealer[0]);          
  // Check player busts and finish dealer card draws
  for(handno = 0; handno < nhands; handno++) {
    recomputeptotal();
    if(ptotal[handno] <= 21) { allbusted = false; }
    else { npbusts++; }
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
      if(dtotal > 21) { ndbusts++; } 
    }

    // Soft-17 rule based on 'dhitssoft17' parameter
    while((dtotal < 17) || ((dtotal == 17) && (daces > 0) && dhitssoft17)) {
      if( cardpos >= maxcardpos ) {
	shuffle(shoe, ndecks);
	cardpos = 0;
      }
      dealer[dcardno++] = shoe[cardpos++];
      update_shoe_counts(dealer[dcardno-1]);        
      updatedtotal();
    }

    for(handno = 0; handno < nhands; handno++) {

      // Check for Blackjack
      // Modify this according to house rules
      if((ptotal[handno] == 21) && (player[handno][2] == 0) && ((dtotal != 21) || (dealer[2] != 0) )) {
	nwinhands++;
	nbjs++;
	  
	if( handno == 0 && nhands == 1  ) {
	  if (bj3to2) {
	    bank += 2.5*bets[handno]; // 3 to 2 payout
	  } else {
	    bank +=2.2*bets[handno];  // 6 to 5 payout
	  }
	} else {
	  bank += 2*bets[handno];
	}
	
	  
      } else if (ptotal[handno] == dtotal) {
	  npush++;
	  bank += bets[handno];
      } else {

	if((ptotal[handno] <= 21) && ((ptotal[handno] > dtotal) || (dtotal > 21))) {
	  nwinhands++;
	  bank += 2*bets[handno];
	  if(dtotal > 21) { ndbusts++; } 
	}

      }

    }
  }
  
}

void splitdeal() {
  int tempno, isp;
  nhands++;
  if( cardpos >= maxcardpos - 2 ) {
    shuffle(shoe, ndecks);
    cardpos = 0;
  }

  // If previous split has occupied next array position
  // shift one over
  if( bets[handno+1] > 0 ) {
    for( tempno = 7; tempno > handno+1; tempno-- ){
      bets[tempno] = bets[tempno-1];
      for( isp = 0; isp < 21; isp++ ){
	player[tempno][isp] = player[tempno-1][isp];
      }
    }
    for( isp = 0; isp < 21; isp++ ){
      player[handno+1][isp] = 0;
    }
  }
  
  bank -= bets[handno++];
  bets[handno] = bets[handno-1];
  avebet = avebet + bets[handno];
  sigbet = sigbet + (2*nhands-1)*bets[handno]*bets[handno];
  //  printf("split! avebet = %f, sigbet = %f\n", avebet, sigbet);  
  player[handno][0] = player[handno-1][1];
  player[handno-1][1] = shoe[cardpos++];
  player[handno][1] = shoe[cardpos++];
  update_shoe_counts(player[handno-1][1]);
  update_shoe_counts(player[handno][1]);    
  cardno = 2;



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
  if( cardpos >= maxcardpos-1 ) {
    shuffle(shoe, ndecks);
    cardpos = 0;
  }
  player[handno][cardno++] = shoe[cardpos++];
  update_shoe_counts(player[handno][cardno-1]);  

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

  //  printf("ptotal %d, cardno = %d\n", ptotal[handno], cardno);
  
  if (player[handno][cardno-1] == 1) {
    ptotal[handno] += 11; psoft = true; paces[handno]++;
  } else if (player[handno][cardno-1] > 10) {
    ptotal[handno] += 10;
  } else {
    ptotal[handno] += player[handno][cardno-1];
  }

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
  avebet = avebet + bets[handno];
  sigbet = sigbet + bets[handno]*bets[handno];
  bank -= bets[handno];
  // dealer[0] card is hidden from player
  dealer[0] = shoe[cardpos++];
  dealer[1] = shoe[cardpos++];
  player[handno][0] = shoe[cardpos++];
  player[handno][1] = shoe[cardpos++];
  cardno = 2;
  update_shoe_counts(dealer[1]);
  update_shoe_counts(player[handno][0]);
  update_shoe_counts(player[handno][1]);

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

  int try;
  float k_p, k_b, k_q;

  switch(strategy) {

  case 0: // Test strategy
    return minbet;

  case 1:
    
    //    return minbet;
    /*
    k_b = 2;
    k_p = true_counts[0]/100.0+0.5;
    k_q = 1-k_p;

    try = (k_b*k_p - k_q)/k_b * 1000;
      if( try > minbet * betspread ){
	try = minbet * betspread;
      } else if ( try < minbet ){
	try = minbet;
      }
    return try;
    */

    /*
    if( true_counts[0] > 2 ) {

      //      try = (bank*0.005) * round( true_counts[0] );
      try = minbet * round( true_counts[0] );

      if ( try > minbet * betspread ){
	return  minbet * betspread;
      } else {
	return try;
      }

    } else {
      return minbet;
    }
    */
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
    int allow_doubles=0;
    int allow_splits=0;
    int allow_surrender=0;

    // Establish contextual house rules for use in strategies
    if(surrndr) { allow_surrender = SURRENDER_Y; }

    // allow double/splits if only 2 cards in hand
    if ( player[handno][2] == 0 ) {
      allow_doubles = DOUBLE_Y;
      if ((nhands > 1) && (!dblaftsplit)) {
	allow_doubles = DOUBLE_N;
      }
      allow_splits = SPLIT_Y;
      // Check if resplitting aces is allowed
      if ((player[handno][0] == 1) && (player[handno][1] == 1)) {
	if ((nhands > 1) && (!resplitaces)) {
	  allow_splits = SPLIT_N;
	}
      }
    }
    
    // but dissallow doubles if on a split hand with an ace
    if ( nhands > 1 ) {
      if ( player[handno][0] == 1 ){
	allow_doubles = DOUBLE_N;
      }
    }

    // and don't allow splits if already have resplithandsmax hands
    if ( nhands > resplithandsmax - 1 ) {
      allow_splits = SPLIT_N;
    }
    

  switch(strategy) {
  case 999: // Another test strategy
    if ((cardno == 2) && (player[handno][0] == player[handno][1]) && (allow_splits)) {
      return 4; // Split whenever possible
    } else { // Else double if ptotal < 17, else stand
      if (ptotal[handno] < 17) {
	return 3;
      } else {
	return 1;
      }
    }

  case 1:
    // Does basic action using house rules (allow_doubles/splits/surrenders

    return handaction_simple(player[handno],
			     dealer[1],
			     allow_doubles,
			     allow_splits,
			     allow_surrender);
    break;

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
  int i, j, k;
  for(i = 0; i < 21; i++) {
    dealer[i] = 0;
    for(j = 0; j < 8; j++) { player[j][i] = 0; }
  }
  for(k = 0; k < 8; k++) {
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

  sprintf(mystring,"debug_printhands");
  get_bool_param(fname, mystring, &debug_printhands, debug_trace);

  sprintf(mystring,"ndecks");
  get_int_param(fname, mystring, &ndecks, debug_trace);
  if (ndecks < 1) { ndecks = 1; }
  if (ndecks > 8) { ndecks = 8; }
  sprintf(mystring,"ntrials");  
  get_int_param(fname, mystring, &ntrials, debug_trace);
  sprintf(mystring,"dhitssoft17");
  get_bool_param(fname, mystring, &dhitssoft17, debug_trace);  
  sprintf(mystring,"dblaftsplit");
  get_bool_param(fname, mystring, &dblaftsplit, debug_trace);  
  sprintf(mystring,"resplitaces");
  get_bool_param(fname, mystring, &resplitaces, debug_trace);  
  sprintf(mystring,"hitsplitaces");
  get_bool_param(fname, mystring, &hitsplitaces, debug_trace);  
  sprintf(mystring,"surrndr");
  get_bool_param(fname, mystring, &surrndr, debug_trace);  
  sprintf(mystring,"bj3to2");
  get_bool_param(fname, mystring, &bj3to2, debug_trace);  
  sprintf(mystring,"resplithandsmax");
  get_int_param(fname, mystring, &resplithandsmax, debug_trace);
  if ( resplithandsmax < 2) { resplithandsmax = 2; }
  sprintf(mystring,"strategy");  
  get_int_param(fname, mystring, &strategy, debug_trace);
  sprintf(mystring,"hoptables");
  get_bool_param(fname, mystring, &hoptables, debug_trace);
  if (hoptables) {
    sprintf(mystring,"hoptc");
    get_int_param(fname, mystring, &hoptc, debug_trace);
  }
  sprintf(mystring,"nbets");  
  get_int_param(fname, mystring, &nbets, debug_trace);
  sprintf(mystring,"penetration");  
  get_real_param(fname, mystring, &penet, debug_trace);
  if (penet < 0.1) { penet = 0.1; }
  if (penet > 1.0) { penet = 1.0; }  
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

  
  //  print_shoe_counts();
  for (ii = 0; ii < 6; ii++) {
    shoe_counts[ii] = 0;
    true_counts[ii] = 0.0;
  }

  
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

  int ii, jj, kk;

  // 1s are aces, J = 11, Q = 12, K = 13
  for (ii = 0; ii < ndeck; ii++) {
    for (jj = 0; jj < 4; jj++) {    
      for (kk = 0; kk < 13; kk++) {
	arr[ii*52+jj*13+kk] = kk+1;
      }
    }
  }

}

void compute_stats() {

  float invntrials;

  invntrials = 1.0/ntrials;
  
  avegain = avegain*invntrials;
  siggain = sqrt(siggain*invntrials - avegain*avegain);
  drawavegain = drawavegain*invntrials;
  drawsiggain = sqrt(drawsiggain*invntrials - drawavegain*drawavegain);
  printf("avebet = %.1f, sigbet = %.1f, totalbets = %lu\n", avebet, sigbet, totalbets);    
  avebet = avebet/(totalbets);
  sigbet = sqrt((sigbet/totalbets) - (avebet*avebet));
  printf("avebet = %f, sigbet = %f\n", avebet, sigbet);  

  //  nbankrupt = nbankrupt*invntrials;
  fwinhands = nwinhands*invntrials;
  fpbusts = npbusts*invntrials;
  fdbusts = ndbusts*invntrials;

  fsur = fsur*invntrials;
  fstd = fstd*invntrials;
  fhits = fhits*invntrials;
  fdbls = fdbls*invntrials;
  fsplts = fsplts*invntrials;
  nbjs = nbjs*invntrials;
  npush = npush*invntrials;
  
}


void write_stats(char* p_file) {

  FILE *fp;

  if((fp = fopen(p_file, "w")) == NULL) {
    printf("Couldn't open file: %s\n", p_file);
    exit(1);
  }

  fprintf(fp, "Dealer hits soft 17: \t%s\n", dhitssoft17 ? "true" : "false");
  fprintf(fp, "Can double after split: %s\n", dblaftsplit ? "true" : "false");
  fprintf(fp, "Can resplit aces: \t%s\n", resplitaces ? "true" : "false");
  fprintf(fp, "Can hit split aces: \t%s\n", hitsplitaces ? "true" : "false");
  fprintf(fp, "Surrenders allowed: \t%s\n", surrndr ? "true" : "false");
  fprintf(fp, "Blackjack payout: \t%s\n", bj3to2 ? "3 to 2" : "6 to 5");
  fprintf(fp, "Maximum number of split hands  = %d\n\n", resplithandsmax);  
  
  fprintf(fp, "strategy = \t%d\n", strategy);
  fprintf(fp, "Hop tables: \t%s\n", hoptables ? "true" : "false");  
  fprintf(fp, "hoptc = \t%d\n", hoptc);  
  fprintf(fp, "ntrials = \t%d\n", ntrials);
  fprintf(fp, "nbets = \t%d\n\n", nbets);
  
  fprintf(fp, "ndecks = \t%d\n", ndecks);
  fprintf(fp, "penetration = \t%f\n\n", penet);

  fprintf(fp, "bank = \t\t%f\n", startbank);
  fprintf(fp, "minbet = \t%f\n", minbet);
  fprintf(fp, "betspread = \t%f\n\n", betspread);

  fprintf(fp, "Average gain per trial = \t%f\n", avegain);
  fprintf(fp, "Sigma gain over trials = \t%f\n", siggain);
  fprintf(fp, "Average gain per draw = \t%f\n", drawavegain);
  fprintf(fp, "Sigma gain over draws = \t%f\n\n", drawsiggain);

  fprintf(fp, "Biggest gain per draw = \t%f\n", maxgain);
  fprintf(fp, "Smallest gain per draw = \t%f\n", mingain);
  fprintf(fp, "Bank Peak end of trial = \t%f\n", maxbank);
  fprintf(fp, "Lowest bank end of trail = \t%f\n", minbank);
  fprintf(fp, "Biggest bank peak = \t\t%f\n", maxmidbank);
  fprintf(fp, "Lowest bank trough = \t\t%f\n\n", minmidbank);

  fprintf(fp, "Average bet size = \t\t%f\n", avebet);
  fprintf(fp, "Sigma of bet size = \t\t%f\n", sigbet);
  fprintf(fp, "Biggest bet made = \t\t%f\n", maxbet);
  fprintf(fp, "Smallest bet made = \t\t%f\n", minebet);
  fprintf(fp, "Number of bankruptcies = \t%d\n\n", nbankrupt);

  fprintf(fp, "Total number of bets made = \t\t%d\n", totalbets);
  fprintf(fp, "Average number of winning hands per trial = \t%f\n", fwinhands);
  fprintf(fp, "Average number of player busts per trial = \t%f\n", fpbusts);
  fprintf(fp, "Average number of dealer busts per trial = \t%f\n\n", fdbusts);

  fprintf(fp, "Average number of surrenders = \t%f\n", fsur);
  fprintf(fp, "Average number of stands = \t%f\n", fstd);
  fprintf(fp, "Average number of hits = \t%f\n", fhits);
  fprintf(fp, "Average number of doubles = \t%f\n", fdbls);
  fprintf(fp, "Average number of splits = \t%f\n", fsplts);
  fprintf(fp, "Average number of player blackjacks = \t%d\n", nbjs);
  fprintf(fp, "Average number of player pushes = \t%d\n", npush);
  
  fclose(fp);
}

void update_shoe_counts(int card) {

  int ii, ndecks_left;
  switch(card) {

  case 1:
    shoe_counts[0]--;
    shoe_counts[3]--;
    shoe_counts[5]--;
    break;
  case 2:
    shoe_counts[0]++;
    shoe_counts[2]++;
    shoe_counts[3]++;
    shoe_counts[4]++;
    shoe_counts[5]++;
    break;
  case 3:
    shoe_counts[0]++;
    shoe_counts[1]++;    
    shoe_counts[2]++;
    shoe_counts[3]++;
    shoe_counts[4]++;
    shoe_counts[5]++;
    break;
  case 4:
  case 5:
    shoe_counts[0]++;
    shoe_counts[1]++;    
    shoe_counts[2]+=2;
    shoe_counts[3]++;
    shoe_counts[4]+=2;
    shoe_counts[5]+=2;    
    break;
  case 6:
    shoe_counts[0]++;
    shoe_counts[1]++;    
    shoe_counts[2]++;
    shoe_counts[3]++;
    shoe_counts[4]+=2;
    shoe_counts[5]+=2;    
    break;
  case 7:
    shoe_counts[2]++;
    shoe_counts[3]++;
    shoe_counts[4]++;
    shoe_counts[5]++;    
    break;
  case 9:
    shoe_counts[4]--;
    break;
  case 10:
  case 11:
  case 12:
  case 13:
    shoe_counts[0]--;
    shoe_counts[1]--;    
    shoe_counts[2]-=2;
    shoe_counts[3]--;
    shoe_counts[4]-=2;
    shoe_counts[5]-=2;
    break;

  }
  ndecks_left = ceil(1.0*(maxcardpos - cardpos)/52.0);
  for (ii = 0; ii < 6; ii++) {
    true_counts[ii] = shoe_counts[ii]*1.0/ndecks_left;
  }

  
}

void print_shoe_counts() {

// order: Hi-Lo, Hi-OptI, Hi-OptII, KO, OmegaII, Zen
  printf("Count type\t Shoe count\t True count\n");
  printf("Hi-Lo\t %d\t %f\n", shoe_counts[0], true_counts[0]);
  printf("Hi-Opt I\t %d\t %f\n", shoe_counts[1], true_counts[1]);
  printf("Hi-Opt II\t %d\t %f\n", shoe_counts[2], true_counts[2]);
  printf("KO\t %d\t %f\n", shoe_counts[3], true_counts[3]);
  printf("Omega II\t %d\t %f\n", shoe_counts[4], true_counts[4]);
  printf("Zen\t %d\t %f\n", shoe_counts[5], true_counts[5]);  

}
