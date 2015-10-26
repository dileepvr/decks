#ifndef BASICSTRATEGY_H
#define BASICSTRATEGY_H

//#include <stdio.h>
//#include <math.h>

// Tables copied from WizardOfOdds calculator until we make our own.

// Legend:

// PRIMARY ACITONS: (chosen action allowed by rules)
// adjusted constants to match dileeps code:
#define R  0   // Surrender
#define S  1   // Stand
#define H  2   // Hit
#define D  3   // Double
#define P  4   // Split

// SECONDARY ACITONS: (action depends on ruleset)
#define DH 5   // Double if allowed, otherwise hit
#define DS 6   // Double if allowed, otherwise stand
#define QH 7   // Split if allowed, otherwise hit
#define QS 8   // Split if allowed, otherwise stand
#define QD 9   // Split if allowed, otherwise double
#define RH 10  // Surrender if allowed, otherwise hit
#define RS 11  // Surrender if allowed, otherwise stand
#define RP 12  // Surrender if allowed, otherwise split

// Sizes of the lookup tables
#define N_DEALER_HANDS 10+2 // The +2 here is for the ET/EA (european T and A cases) which don't really matter for us
#define N_PLAYER_HANDS 36

// Hand can be Hard, Soft, or a Pair. Could put these in a macro, but for now I just add in to the index
#define OFFSET_DEALER (-2)
#define OFFSET_HARD (-5)
#define OFFSET_SOFT 4
#define OFFSET_PAIR 24

#define DOUBLE_Y 1
#define DOUBLE_N 0

#define SPLIT_Y 1
#define SPLIT_N 0

#define SURRENDER_Y 1
#define SURRENDER_N 0

// 1 DECK DEALER HITS ON SOFT 17
const int H17_0[N_PLAYER_HANDS][N_DEALER_HANDS] =
    {
        // 2     3     4     5     6     7     8     9     T     A    ET    EA

        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  5       0     offset -5
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  6
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  7
        { H,  H,  H, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Hard  8
        {DH, DH, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Hard  9
        {DH, DH, DH, DH, DH, DH, DH, DH,  H,  H,  H,  H},   // Hard 10       5
        {DH, DH, DH, DH, DH, DH, DH, DH, DH, DH,  H,  H},   // Hard 11
        { H,  H,  S,  S,  S,  H,  H,  H,  H,  H,  H, RH},   // Hard 12
        { S,  S,  S,  S,  S,  H,  H,  H,  H,  H,  H, RH},   // Hard 13
        { S,  S,  S,  S,  S,  H,  H,  H,  H,  H, RH, RH},   // Hard 14
        { S,  S,  S,  S,  S,  H,  H,  H,  H, RH, RH, RH},   // Hard 15       10
        { S,  S,  S,  S,  S,  H,  H,  H, RH, RH, RH, RH},   // Hard 16
        { S,  S,  S,  S,  S,  S,  S,  S,  S, RS,  S, RS},   // Hard 17
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 18
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 19
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 20       15
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 21
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 13             offset +4
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 14
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 15
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 16       20
        {DH, DH, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 17
        { S, DS, DS, DS, DS,  S,  S,  H,  H,  H,  H,  H},   // Soft 18
        { S,  S,  S,  S, DS,  S,  S,  S,  S,  S,  S,  S},   // Soft 19
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Soft 20
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Soft 21       25
        {QH,  P,  P,  P,  P,  P,  H,  H,  H,  H,  H, RH},   // 2,2                 offset +24
        {QH, QH,  P,  P,  P,  P, QH,  H,  H,  H,  H, RH},   // 3,3
        { H,  H, QH, QD, QD,  H,  H,  H,  H,  H,  H,  H},   // 4,4
        {DH, DH, DH, DH, DH, DH, DH, DH,  H,  H,  H,  H},   // 5,5
        { P,  P,  P,  P,  P, QH,  H,  H,  H,  H,  H, RH},   // 6,6           30
        { P,  P,  P,  P,  P,  P, QH,  H, RS, RH, RS, RH},   // 7,7
        { P,  P,  P,  P,  P,  P,  P,  P,  P,  P, RH, RH},   // 8,8
        { P,  P,  P,  P,  P,  S,  P,  P,  S, QS,  S,  S},   // 9,9
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // T,T
        { P,  P,  P,  P,  P,  P,  P,  P,  P,  P,  P,  H}    // A,A           35
    };

// 1 DECK, DEALER STANDS ON SOFT 17
const int S17_0[N_PLAYER_HANDS][N_DEALER_HANDS] =
    {
        // 2     3     4     5     6     7     8     9     T     A    ET    EA

        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  5
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  6
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  7
        { H,  H,  H, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Hard  8
        {DH, DH, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Hard  9
        {DH, DH, DH, DH, DH, DH, DH, DH,  H,  H,  H,  H},   // Hard 10
        {DH, DH, DH, DH, DH, DH, DH, DH, DH, DH,  H,  H},   // Hard 11
        { H,  H,  S,  S,  S,  H,  H,  H,  H,  H,  H, RH},   // Hard 12
        { S,  S,  S,  S,  S,  H,  H,  H,  H,  H,  H, RH},   // Hard 13
        { S,  S,  S,  S,  S,  H,  H,  H,  H,  H, RH, RH},   // Hard 14
        { S,  S,  S,  S,  S,  H,  H,  H,  H,  H, RH, RH},   // Hard 15
        { S,  S,  S,  S,  S,  H,  H,  H, RH, RH, RH, RH},   // Hard 16
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S, RS},   // Hard 17
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 18
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 19
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 20
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 21
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 13
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 14
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 15
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 16
        {DH, DH, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 17
        { S, DS, DS, DS, DS,  S,  S,  H,  H,  S,  H,  S},   // Soft 18
        { S,  S,  S,  S, DS,  S,  S,  S,  S,  S,  S,  S},   // Soft 19
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Soft 20
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Soft 21
        {QH,  P,  P,  P,  P,  P,  H,  H,  H,  H,  H,  H},   // 2,2
        {QH, QH,  P,  P,  P,  P, QH,  H,  H,  H,  H, RH},   // 3,3
        { H,  H, QH, QD, QD,  H,  H,  H,  H,  H,  H,  H},   // 4,4
        {DH, DH, DH, DH, DH, DH, DH, DH,  H,  H,  H,  H},   // 5,5
        { P,  P,  P,  P,  P, QH,  H,  H,  H,  H,  H, RH},   // 6,6
        { P,  P,  P,  P,  P,  P, QH,  H, RS,  H, RS, RH},   // 7,7
        { P,  P,  P,  P,  P,  P,  P,  P,  P,  P, RH, RH},   // 8,8
        { P,  P,  P,  P,  P,  S,  P,  P,  S,  S,  S,  S},   // 9,9
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // T,T
        { P,  P,  P,  P,  P,  P,  P,  P,  P,  P,  P,  H}    // A,A
    };

// 2 DECK, DEALER HITS SOFT 17
const int H17_1[N_PLAYER_HANDS][N_DEALER_HANDS] =
    {
        // 2     3     4     5     6     7     8     9     T     A    ET    EA

        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  5
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  6
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  7
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H},   // Hard  8
        {DH, DH, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Hard  9
        {DH, DH, DH, DH, DH, DH, DH, DH,  H,  H,  H,  H},   // Hard 10
        {DH, DH, DH, DH, DH, DH, DH, DH, DH, DH,  H,  H},   // Hard 11
        { H,  H,  S,  S,  S,  H,  H,  H,  H,  H,  H, RH},   // Hard 12
        { S,  S,  S,  S,  S,  H,  H,  H,  H,  H,  H, RH},   // Hard 13
        { S,  S,  S,  S,  S,  H,  H,  H,  H,  H, RH, RH},   // Hard 14
        { S,  S,  S,  S,  S,  H,  H,  H, RH, RH, RH, RH},   // Hard 15
        { S,  S,  S,  S,  S,  H,  H,  H, RH, RH, RH, RH},   // Hard 16
        { S,  S,  S,  S,  S,  S,  S,  S,  S, RS,  S, RS},   // Hard 17
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 18
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 19
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 20
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 21
        { H,  H,  H, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 13
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 14
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 15
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 16
        { H, DH, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 17
        {DS, DS, DS, DS, DS,  S,  S,  H,  H,  H,  H,  H},   // Soft 18
        { S,  S,  S,  S, DS,  S,  S,  S,  S,  S,  S,  S},   // Soft 19
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Soft 20
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Soft 21
        {QH, QH,  P,  P,  P,  P,  H,  H,  H,  H,  H, RH},   // 2,2
        {QH, QH,  P,  P,  P,  P,  H,  H,  H,  H,  H, RH},   // 3,3
        { H,  H,  H, QH, QH,  H,  H,  H,  H,  H,  H,  H},   // 4,4
        {DH, DH, DH, DH, DH, DH, DH, DH,  H,  H,  H,  H},   // 5,5
        { P,  P,  P,  P,  P, QH,  H,  H,  H,  H,  H, RH},   // 6,6
        { P,  P,  P,  P,  P,  P, QH,  H,  H,  H, RH, RH},   // 7,7
        { P,  P,  P,  P,  P,  P,  P,  P,  P, RP, RH, RH},   // 8,8
        { P,  P,  P,  P,  P,  S,  P,  P,  S,  S,  S,  S},   // 9,9
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // T,T
        { P,  P,  P,  P,  P,  P,  P,  P,  P,  P,  P,  H}    // A,A
    };

// 2 DECKS, DEALER STANDS ON SOFT 17
const int S17_1[N_PLAYER_HANDS][N_DEALER_HANDS] =
    {
        // 2     3     4     5     6     7     8     9     T     A    ET    EA

        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  5
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  6
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  7
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H},   // Hard  8
        {DH, DH, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Hard  9
        {DH, DH, DH, DH, DH, DH, DH, DH,  H,  H,  H,  H},   // Hard 10
        {DH, DH, DH, DH, DH, DH, DH, DH, DH, DH,  H,  H},   // Hard 11
        { H,  H,  S,  S,  S,  H,  H,  H,  H,  H,  H, RH},   // Hard 12
        { S,  S,  S,  S,  S,  H,  H,  H,  H,  H,  H, RH},   // Hard 13
        { S,  S,  S,  S,  S,  H,  H,  H,  H,  H, RH, RH},   // Hard 14
        { S,  S,  S,  S,  S,  H,  H,  H, RH,  H, RH, RH},   // Hard 15
        { S,  S,  S,  S,  S,  H,  H,  H, RH, RH, RH, RH},   // Hard 16
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S, RS},   // Hard 17
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 18
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 19
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 20
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 21
        { H,  H,  H, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 13
        { H,  H,  H, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 14
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 15
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 16
        { H, DH, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 17
        { S, DS, DS, DS, DS,  S,  S,  H,  H,  H,  H,  H},   // Soft 18
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Soft 19
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Soft 20
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Soft 21
        {QH, QH,  P,  P,  P,  P,  H,  H,  H,  H,  H,  H},   // 2,2
        {QH, QH,  P,  P,  P,  P,  H,  H,  H,  H,  H, RH},   // 3,3
        { H,  H,  H, QH, QH,  H,  H,  H,  H,  H,  H,  H},   // 4,4
        {DH, DH, DH, DH, DH, DH, DH, DH,  H,  H,  H,  H},   // 5,5
        { P,  P,  P,  P,  P, QH,  H,  H,  H,  H,  H, RH},   // 6,6
        { P,  P,  P,  P,  P,  P, QH,  H,  H,  H, RH, RH},   // 7,7
        { P,  P,  P,  P,  P,  P,  P,  P,  P,  P, RH, RH},   // 8,8
        { P,  P,  P,  P,  P,  S,  P,  P,  S,  S,  S,  S},   // 9,9
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // T,T
        { P,  P,  P,  P,  P,  P,  P,  P,  P,  P,  P,  H}    // A,A
    };

// 4 OR MORE DECKS, DEALER HITS SOFT 17
const int H17_2[N_PLAYER_HANDS][N_DEALER_HANDS] =
    {
        // 2     3     4     5     6     7     8     9     T     A    ET    EA

        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  5
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  6
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  7
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H},   // Hard  8
        { H, DH, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Hard  9
        {DH, DH, DH, DH, DH, DH, DH, DH,  H,  H,  H,  H},   // Hard 10
        {DH, DH, DH, DH, DH, DH, DH, DH, DH, DH,  H,  H},   // Hard 11
        { H,  H,  S,  S,  S,  H,  H,  H,  H,  H,  H, RH},   // Hard 12
        { S,  S,  S,  S,  S,  H,  H,  H,  H,  H,  H, RH},   // Hard 13
        { S,  S,  S,  S,  S,  H,  H,  H,  H,  H, RH, RH},   // Hard 14
        { S,  S,  S,  S,  S,  H,  H,  H, RH, RH, RH, RH},   // Hard 15
        { S,  S,  S,  S,  S,  H,  H, RH, RH, RH, RH, RH},   // Hard 16
        { S,  S,  S,  S,  S,  S,  S,  S,  S, RS,  S, RS},   // Hard 17
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 18
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 19
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 20
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 21
        { H,  H,  H, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 13
        { H,  H,  H, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 14
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 15
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 16
        { H, DH, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 17
        {DS, DS, DS, DS, DS,  S,  S,  H,  H,  H,  H,  H},   // Soft 18
        { S,  S,  S,  S, DS,  S,  S,  S,  S,  S,  S,  S},   // Soft 19
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Soft 20
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Soft 21
        {QH, QH,  P,  P,  P,  P,  H,  H,  H,  H,  H, RH},   // 2,2
        {QH, QH,  P,  P,  P,  P,  H,  H,  H,  H,  H, RH},   // 3,3
        { H,  H,  H, QH, QH,  H,  H,  H,  H,  H,  H,  H},   // 4,4
        {DH, DH, DH, DH, DH, DH, DH, DH,  H,  H,  H,  H},   // 5,5
        {QH,  P,  P,  P,  P,  H,  H,  H,  H,  H,  H, RH},   // 6,6
        { P,  P,  P,  P,  P,  P,  H,  H,  H,  H, RH, RH},   // 7,7
        { P,  P,  P,  P,  P,  P,  P,  P,  P, RP, RH, RH},   // 8,8
        { P,  P,  P,  P,  P,  S,  P,  P,  S,  S,  S,  S},   // 9,9
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // T,T
        { P,  P,  P,  P,  P,  P,  P,  P,  P,  P,  P,  H}    // A,A
    };

// 4 OR MORE DECKS, DEALER STANDS ON SOFT 17
const int S17_2[N_PLAYER_HANDS][N_DEALER_HANDS] =
    {
        // 2     3     4     5     6     7     8     9     T     A    ET    EA

        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  5
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  6
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H, RH},   // Hard  7
        { H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H,  H},   // Hard  8
        { H, DH, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Hard  9
        {DH, DH, DH, DH, DH, DH, DH, DH,  H,  H,  H,  H},   // Hard 10
        {DH, DH, DH, DH, DH, DH, DH, DH, DH,  H,  H,  H},   // Hard 11
        { H,  H,  S,  S,  S,  H,  H,  H,  H,  H,  H, RH},   // Hard 12
        { S,  S,  S,  S,  S,  H,  H,  H,  H,  H,  H, RH},   // Hard 13
        { S,  S,  S,  S,  S,  H,  H,  H,  H,  H, RH, RH},   // Hard 14
        { S,  S,  S,  S,  S,  H,  H,  H, RH,  H, RH, RH},   // Hard 15
        { S,  S,  S,  S,  S,  H,  H, RH, RH, RH, RH, RH},   // Hard 16
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S, RS},   // Hard 17
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 18
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 19
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 20
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Hard 21
        { H,  H,  H, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 13
        { H,  H,  H, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 14
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 15
        { H,  H, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 16
        { H, DH, DH, DH, DH,  H,  H,  H,  H,  H,  H,  H},   // Soft 17
        { S, DS, DS, DS, DS,  S,  S,  H,  H,  H,  H,  H},   // Soft 18
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Soft 19
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Soft 20
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // Soft 21
        {QH, QH,  P,  P,  P,  P,  H,  H,  H,  H,  H,  H},   // 2,2
        {QH, QH,  P,  P,  P,  P,  H,  H,  H,  H,  H, RH},   // 3,3
        { H,  H,  H, QH, QH,  H,  H,  H,  H,  H,  H,  H},   // 4,4
        {DH, DH, DH, DH, DH, DH, DH, DH,  H,  H,  H,  H},   // 5,5
        {QH,  P,  P,  P,  P,  H,  H,  H,  H,  H,  H, RH},   // 6,6
        { P,  P,  P,  P,  P,  P,  H,  H,  H,  H, RH, RH},   // 7,7
        { P,  P,  P,  P,  P,  P,  P,  P,  P,  P, RH, RH},   // 8,8
        { P,  P,  P,  P,  P,  S,  P,  P,  S,  S,  S,  S},   // 9,9
        { S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S},   // T,T
        { P,  P,  P,  P,  P,  P,  P,  P,  P,  P,  P,  H}    // A,A
    };


// Translates "if-allowed" actions to a final action. Might not be sufficient for
// the table used above, since they have P's everywhere which might not be allowed.

int simplify_action( int x, int allow_double, int allow_split, int allow_surrender )
{

  //  printf( "BOOM: %d", x );


  if( x < 5 ){
    return x; // Already been simplified, probably
  }

  switch(x){

  case DH:
    if( allow_double ){
      return D;
    } else {
      return H;
    }
    break;

  case DS:
    if( allow_double ){
      return D;
    } else {
      return S;
    }
    break;

  case QH:
    if( allow_split ){
      return P;
    } else {
      return H;
    }
    break;

  case QS:
    if( allow_split ){
      return P;
    } else {
      return S;
    }
    break;

  case QD:
    if( allow_split ){
      return P;
    } else {
      return D;
    }
    break;

  case RH:
    printf("RH\n");
    if( allow_surrender ){
      return R;
    } else {
      return H;
    }
    break;

  case RS:
    if( allow_surrender ){
      return R;
    } else {
      return S;
    }
    break;

  case RP:
    if( allow_surrender ){
      return R;
    } else {
      return P;
    }
    break;
     
  default:
    return 99; // error! table must be malformed
  }
}

  // Okay, define the basic strategy lookup function.
  // You give it a hand, number of cards, and the
  // dealercard, and it tells you the best move.
  int handaction(int * hand_in,
		 int dealercard_in
		 //		 int num_ace,
		 //		 int total,
		 //		 int hard,
		 )

  {
    int hand[21] = {0,0,0,0,0,0,0,0,0,0,0,0};
    int num_ace=0;
    int num_cards=0;
    int total=0;
    int dealercard=0;
    //    int hard=0;
    //    int action=0;
    int i;

    if( dealercard_in > 10 ){
      dealercard = 10;
    } else if( dealercard_in == 1 ) {
      dealercard = 11;
    } else {
      dealercard = dealercard_in;
    }



    // Count cards in hand, convert hand type
    i=0;
    while( hand_in[i] != 0 ){
      num_cards++;
      if( hand_in[i] > 10 ){
	hand[i] = 10;
      } else if( hand_in[i] == 1 ) {
	hand[i] = 11;
      } else {
	hand[i] = hand_in[i];
      }
      i++;    
    }

    //    printf( "FAKEPAIR|%d|%d|%d|-|%d|", hand[0], hand[1], hand[2], dealercard );


    // If only 2 cards and they match, return pair offset. Done.
        if( hand_in[2] == 0 && hand[0] == hand[1] ){
	  //	  printf( "FAKEPAIR|%d|%d|%d|", hand[0], hand[1], hand[2] );
          return H17_0[hand[0]+OFFSET_PAIR][dealercard+OFFSET_DEALER];
        }


    //    if( ( hand[2] == 0 ) && ( hand[0] == hand[1] ) ){
      //printf( "PAIR|%d|%d|", hand[0]+OFFSET_PAIR, dealercard+OFFSET_DEALER );
    //      return H17_0[hand[0]+OFFSET_PAIR][dealercard+OFFSET_DEALER];
    //    }

    // Total the hand, count the aces
    for( i=0; i<num_cards; i++){    
      if( hand[i] == 11 ) { num_ace++; }
      total += hand[i];
    }

    // If we're over 21 with an ace, count ace 1 point until under 21 (or until we run out of aces)
    while( num_ace > 0 ){
      if( total>21 ){
	total -= 10;
	num_ace--;
      } else {
	break;
      }
    }



    // If we're over 21 with an ace, count ace 1 point until under 21 (or until we run out of aces)
    //    while( num_ace > 0 ){
    //      if( total>21 ){
    // total -= 10;
    //	num_ace--;
    //      } else {
    //	break;
    //      }
    //    }

    // If index is out of bounds, fix it. We shouldn't be allowed to do anything on a bust anyway (right?!).
    if( total > 21 ){
      total=21;
    }

    // If no aces acting as 11 points, we have a hard hand, otherwise it's a soft hand
    if( num_ace == 0 ){
      //      printf( "HARD|%d|%d|", total, dealercard );
      //      printf( "HARD|%d|%d|", total+OFFSET_HARD, dealercard+OFFSET_DEALER );
      

      //      printf( "THIS|%d|", H17_0[total+OFFSET_HARD][dealercard+OFFSET_DEALER] );

      return H17_0[total+OFFSET_HARD][dealercard+OFFSET_DEALER];
    } else {
      //printf( "SOFT|%d|%d|", total+OFFSET_SOFT, dealercard+OFFSET_DEALER );
      return H17_0[total+OFFSET_SOFT][dealercard+OFFSET_DEALER];
    }


  };


  int complexhandaction(int * hand_in,
			int dealercard_in,
			int num_decks,
			int adv_strat,
			bool dhsoft17
			)

  {
    int hand[21] = {0,0,0,0,0,0,0,0,0,0,0,0};
    int num_ace=0;
    int num_cards=0;
    int total=0;
    int dealercard=0;
    //    int hard=0;
    //    int action=0;
    int i;

    if( dealercard_in > 10 ){
      dealercard = 10;
    } else if( dealercard_in == 1 ) {
      dealercard = 11;
    } else {
      dealercard = dealercard_in;
    }



    // Count cards in hand, convert hand type
    i=0;
    while( hand_in[i] != 0 ){
      num_cards++;
      if( hand_in[i] > 10 ){
	hand[i] = 10;
      } else if( hand_in[i] == 1 ) {
	hand[i] = 11;
      } else {
	hand[i] = hand_in[i];
      }
      i++;    
    }

    //    printf( "FAKEPAIR|%d|%d|%d|-|%d|", hand[0], hand[1], hand[2], dealercard );


    // If only 2 cards and they match, return pair offset. Done.
        if( hand_in[2] == 0 && hand[0] == hand[1] ){
	  //	  printf( "FAKEPAIR|%d|%d|%d|", hand[0], hand[1], hand[2] );
	  if(num_decks < 2) { // 1 deck
	    if(dhsoft17) {
	      return H17_0[hand[0]+OFFSET_PAIR][dealercard+OFFSET_DEALER];
	    } else {
	      return S17_0[hand[0]+OFFSET_PAIR][dealercard+OFFSET_DEALER];	      
	    }
	  } else if(num_decks < 4) { // 2 decks
	    if(dhsoft17) {
	      return H17_1[hand[0]+OFFSET_PAIR][dealercard+OFFSET_DEALER];
	    } else {
	      return S17_1[hand[0]+OFFSET_PAIR][dealercard+OFFSET_DEALER];	      
	    }	    
	  } else { // 4 or more decks
	    if(dhsoft17) {
	      return H17_2[hand[0]+OFFSET_PAIR][dealercard+OFFSET_DEALER];
	    } else {
	      return S17_2[hand[0]+OFFSET_PAIR][dealercard+OFFSET_DEALER];	      
	    }	    
	  }
        }


    //    if( ( hand[2] == 0 ) && ( hand[0] == hand[1] ) ){
      //printf( "PAIR|%d|%d|", hand[0]+OFFSET_PAIR, dealercard+OFFSET_DEALER );
    //      return H17_0[hand[0]+OFFSET_PAIR][dealercard+OFFSET_DEALER];
    //    }

    // Total the hand, count the aces
    for( i=0; i<num_cards; i++){    
      if( hand[i] == 11 ) { num_ace++; }
      total += hand[i];
    }

    // If we're over 21 with an ace, count ace 1 point until under 21 (or until we run out of aces)
    while( num_ace > 0 ){
      if( total>21 ){
	total -= 10;
	num_ace--;
      } else {
	break;
      }
    }



    // If we're over 21 with an ace, count ace 1 point until under 21 (or until we run out of aces)
    //    while( num_ace > 0 ){
    //      if( total>21 ){
    // total -= 10;
    //	num_ace--;
    //      } else {
    //	break;
    //      }
    //    }

    // If index is out of bounds, fix it. We shouldn't be allowed to do anything on a bust anyway (right?!).
    if( total > 21 ){
      total=21;
    }

    // If no aces acting as 11 points, we have a hard hand, otherwise it's a soft hand
    if( num_ace == 0 ){
      //      printf( "HARD|%d|%d|", total, dealercard );
      //      printf( "HARD|%d|%d|", total+OFFSET_HARD, dealercard+OFFSET_DEALER );
      

      //      printf( "THIS|%d|", H17_0[total+OFFSET_HARD][dealercard+OFFSET_DEALER] );

      if(num_decks < 2) { // 1 deck
	if(dhsoft17) {
	  return H17_0[total+OFFSET_HARD][dealercard+OFFSET_DEALER];
	} else {
	  return S17_0[total+OFFSET_HARD][dealercard+OFFSET_DEALER];
	}
      } else if(num_decks < 4) { // 2 decks
	if(dhsoft17) {
	  return H17_1[total+OFFSET_HARD][dealercard+OFFSET_DEALER];
	} else {
	  return S17_1[total+OFFSET_HARD][dealercard+OFFSET_DEALER];
	}	
      } else { // 4 or more decks
	if(dhsoft17) {
	  return H17_2[total+OFFSET_HARD][dealercard+OFFSET_DEALER];
	} else {
	  return S17_2[total+OFFSET_HARD][dealercard+OFFSET_DEALER];
	}	
      }
    } else {
      //printf( "SOFT|%d|%d|", total+OFFSET_SOFT, dealercard+OFFSET_DEALER );
      if(num_decks < 2) { // 1 deck
	if(dhsoft17) {
	  return H17_0[total+OFFSET_SOFT][dealercard+OFFSET_DEALER];
	} else {
	  return S17_0[total+OFFSET_SOFT][dealercard+OFFSET_DEALER];
	}
      } else if(num_decks < 4) { // 2 decks
	if(dhsoft17) {
	  return H17_1[total+OFFSET_SOFT][dealercard+OFFSET_DEALER];
	} else {
	  return S17_1[total+OFFSET_SOFT][dealercard+OFFSET_DEALER];
	}	
      } else { // 4 or more decks
	if(dhsoft17) {
	  return H17_2[total+OFFSET_SOFT][dealercard+OFFSET_DEALER];
	} else {
	  return S17_2[total+OFFSET_SOFT][dealercard+OFFSET_DEALER];
	}	
      }
      
    }


  };

// macro that filters handaction through simplify action
int handaction_simple(  int * hand,
			int dealercard,
			int allow_double,
			int allow_split,
			int allow_surrender)
{

  return simplify_action(handaction(hand, dealercard), allow_double, allow_split, allow_surrender);

}

int handaction_complex( int * hand,
			int dealercard,
			int allow_double,
			int allow_split,
			int allow_surrender,
			int numdecks,
			int advanced_stratnum,
			bool dhsoft17)
{

  return simplify_action(complexhandaction(hand, dealercard, numdecks, advanced_stratnum,dhsoft17), allow_double, allow_split, allow_surrender);

}



/* // Testing functionality:


int main( int argc, char *argv[])
{
  
  int i,j;
  

  // Print all the rules.... why not...
  for( i=0; i<N_PLAYER_HANDS; i++)
    {
      for( j=0; j<N_DEALER_HANDS; j++)
	{
	  printf("%d,%d: %d\t", i, j, H17_0[i][j]);
	  printf("%d,%d: %d\t", i, j, H17_1[i][j]);
	  printf("%d,%d: %d\t", i, j, H17_2[i][j]);
	  printf("%d,%d: %d\t", i, j, S17_0[i][j]);
	  printf("%d,%d: %d\t", i, j, S17_1[i][j]);
	  printf("%d,%d: %d\t", i, j, S17_2[i][j]);
	  printf("\n");
	}
    }





  // OKAY, this next part is just for testing. It shows the proper usage:

  int ncards;
  int hand[5];
  int dealercard = 11;
  srand(1);

  for( i=0; i<2000; i++ )
    {
      ncards = rand()%2+2; // 2-3 cards in a hand

      printf( "Player hand: " );

      for( j=0; j<5; j++ ){
	if( j < ncards ) {
	  hand[j] = rand()%10+2; // 2-11 card value per card
	  printf( "%d ", hand[j] );
	} else {
	  hand[j] = 0;
	}

      }

      dealercard = rand()%10+2; // 2-11 card value per card
      printf( "\t  Dealer hand: %d", dealercard );      

      printf( "\t  Basic strategy move: %d \n", handaction_simple( hand, dealercard, 1, 1, 1 ) );
 
    }

}

*/

#endif
