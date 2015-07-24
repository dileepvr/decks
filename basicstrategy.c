#include <stdio.h>
#include <math.h>
#include "basicstrategy.h"

int main( int argc, char *argv[])
{
  
  int i,j;
  
  // OKAY, this next part is just for testing. It shows the proper usage:

  int hand[5];
  int ncards;
  int dealercard = 11;
  srand(10);

  for( i=0; i<2000; i++ )
    {
      ncards = rand()%2+2; // 2-3 cards in a hand

      printf( "Player hand: " );

      for( j=0; j<5; j++ ){
	hand[j] = 0;
      }
      for( j=0; j<ncards; j++ ){
	hand[j] = rand()%13+1; // 1-13 card value per card
	printf( "%d ", hand[j] );
      }

      dealercard = rand()%13+1; // 2-11 card value per card
      printf( "\t  Dealer hand: %d", dealercard );      

      printf( "\t  Basic strategy move: %d \n", handaction( hand, dealercard ) );
 
    }

}
