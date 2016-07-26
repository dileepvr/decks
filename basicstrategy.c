/*
This project has been released under the MIT License, and it's text is
contained within the accompanying file LICENSE.md.
All future versions of the MIT License automatically apply.
*/
#include <stdio.h>
#include <math.h>
#include "basicstrategy.h"

int main( int argc, char *argv[])
{
  
  int i,j;
  
  // OKAY, this next part is just for testing. It shows the proper usage:

  int hand[5] = {7, 10, 0, 0, 0};
  int ncards = 2;
  int dealercard = 1;
  srand(10);
  

  for( i=0; i<10; i++ )
    {
      //      ncards = rand()%2+2; // 2-3 cards in a hand

      printf( "Player hand: " );

      //      for( j=0; j<5; j++ ){
      //	hand[j] = 0;
      //      }
      for( j=0; j<ncards; j++ ){
	//	hand[j] = rand()%13+1; // 1-13 card value per card
	printf( "%d ", hand[j] );
      }

      //      dealercard = rand()%13+1; // 2-11 card value per card
      printf( "\t  Dealer hand: %d", dealercard );      

      printf( "\t  Basic strategy move: %d \n", handaction_simple( hand, dealercard, DOUBLE_Y, SPLIT_Y, SURRENDER_N ));
 
    }

}
