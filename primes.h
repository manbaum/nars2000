//***************************************************************************
//  NARS2000 -- Prime Factoring Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/


//***************************************************************************
//  Prime factoring and number theoretic functions
//***************************************************************************

typedef struct tagPROCESSPRIME
{
    APLUINT uFcnIndex,              // 00:  Function index (see enum NUMTHEORY)
            uTotient,               // 08:  Result of totient function
            uDivisor;               // 10:  Result of divisor count/sum
    UBOOL   bSquareFree;            // 18:  TRUE iff the number is square-free
    UINT    seed1,                  // 1C:  Random seeds
            seed2;                  // 20:  ...
    LPUBOOL lpbCtrlBreak;           // 24:  Ptr to Ctrl-Break flag
} PROCESSPRIME, *LPPROCESSPRIME;


#define PRECOMPUTED_PRIME_NEXT          100003                  // Next prime after PRECOMPUTED_PRIME_MAX
                                                                //   = min # not factorable by trial division
#define PRECOMPUTED_PRIME_NEXT2         10000600009L            // Square of PRECOMPUTED_PRIME_NEXT
#define PRECOMPUTED_PRIME_NEXT2_MP      {2, {0x54150BC9, 2}}    // PRECOMPUTED_PRIME_NEXT2 as mp_t
#define PRECOMPUTED_PRIME_NEXT2_LOG2    33                      // Floor Log2 of PRECOMPUTED_PRIME_NEXT2


//***************************************************************************
//  End of File: primes.h
//***************************************************************************
