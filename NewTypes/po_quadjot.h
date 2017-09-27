//***************************************************************************
//  NARS2000 -- Primitive Operator -- QuadJot Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2017 Sudley Place Software

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


typedef APLINT   (*LPAPLINT2)[2][2];
typedef APLINT   (*LPAPLINT4)[4][4];
typedef APLINT   (*LPAPLINT8)[8][8];

typedef APLFLOAT (*LPAPLFLT2)[2][2];
typedef APLFLOAT (*LPAPLFLT4)[4][4];
typedef APLFLOAT (*LPAPLFLT8)[8][8];

typedef APLRAT   (*LPAPLRAT2)[2][2];
typedef APLRAT   (*LPAPLRAT4)[4][4];
typedef APLRAT   (*LPAPLRAT8)[8][8];

typedef APLVFP   (*LPAPLVFP2)[2][2];
typedef APLVFP   (*LPAPLVFP4)[4][4];
typedef APLVFP   (*LPAPLVFP8)[8][8];


#ifdef DEFINE_VALUES
int MatRep2[2][2] =
{
    {  0, -1},
    {  1,  0},
},
    MatRep4[4][4] =
{
    {  0, -1, -2, -3},
    {  1,  0, -3,  2},
    {  2,  3,  0, -1},
    {  3, -2,  1,  0},
},
    MatRep8L[8][8] =
{
    {  0, -1, -2, -3, -4, -5, -6, -7 },
    {  1,  0, -3,  2, -5,  4,  7, -6 },
    {  2,  3,  0, -1, -6, -7,  4,  5 },
    {  3, -2,  1,  0, -7,  6, -5,  4 },
    {  4,  5,  6,  7,  0, -1, -2, -3 },
    {  5, -4,  7, -6,  1,  0,  3, -2 },
    {  6, -7, -4,  5,  2, -3,  0,  1 },
    {  7,  6, -5, -4,  3,  2, -1,  0 },

},
    MatRep8R[8][8] =
{
    {  0, -1, -2, -3, -4, -5, -6, -7 },
    {  1,  0,  3, -2,  5, -4, -7,  6 },
    {  2, -3,  0,  1,  6,  7, -4, -5 },
    {  3,  2, -1,  0,  7, -6,  5, -4 },
    {  4, -5, -6, -7,  0,  1,  2,  3 },
    {  5,  4, -7,  6, -1,  0, -3,  2 },
    {  6,  7,  4, -5, -2,  3,  0, -1 },
    {  7, -6,  5,  4, -3, -2,  1,  0 },
};
#else
extern int MatRep2[2][2];
extern int MatRep4[4][4];
extern int MatRep8L[8][8];
extern int MatRep8R[8][8];
#endif

#define MATREP_INT(N,NLR)                                               \
                                                                        \
    /* Loop through the rows and cols */                                \
    for (i = 0; i < N; i++)                                             \
    for (j = 0; j < N; j++)                                             \
    {                                                                   \
        APLINT l;                                                       \
                                                                        \
        k = MatRep##NLR[i][j];                                          \
        l = ((LPAPLINT) lpMemRht)[abs (k)];                             \
                                                                        \
        (*(LPAPLINT##N) lpMemRes)[i][j] = (signum (k) >= 0) ? l : -l;   \
    } /* End FOR/FOR */


#define MATREP_FLT(N,NLR)                                               \
                                                                        \
    /* Loop through the rows and cols */                                \
    for (i = 0; i < N; i++)                                             \
    for (j = 0; j < N; j++)                                             \
    {                                                                   \
        APLFLOAT l;                                                     \
                                                                        \
        k = MatRep##NLR[i][j];                                          \
        l = ((LPAPLFLOAT) lpMemRht)[abs (k)];                           \
                                                                        \
        (*(LPAPLFLT##N) lpMemRes)[i][j] = (signum (k) >= 0) ? l : -l;   \
    } /* End FOR/FOR */


#define MATREP_RAT(N,NLR)                                               \
    {                                                                   \
        APLRAT lNeg;                                                    \
                                                                        \
        /* Initialize to 0/1 */                                         \
        mpq_init (&lNeg);                                               \
                                                                        \
        /* Loop through the rows and cols */                            \
        for (i = 0; i < N; i++)                                         \
        for (j = 0; j < N; j++)                                         \
        {                                                               \
            APLRAT l;                                                   \
                                                                        \
            k = MatRep##NLR[i][j];                                      \
            l = ((LPAPLRAT) lpMemRht)[abs (k)];                         \
                                                                        \
            /* If the sign is non-negative, ... */                      \
            if (signum (k) >= 0)                                        \
                mpq_init_set (&(*(LPAPLRAT##N) lpMemRes)[i][j], &l);    \
            else                                                        \
            {                                                           \
                mpq_neg (&lNeg, &l);                                    \
                mpq_init_set (&(*(LPAPLRAT##N) lpMemRes)[i][j], &lNeg); \
            } /* End IF/ELSE */                                         \
        } /* End FOR/FOR */                                             \
                                                                        \
        /* We no longer need this storage */                            \
        mpq_clear (&lNeg);                                              \
    } /* case ARRAY_RAT */


#define MATREP_VFP(N,NLR)                                                           \
    {                                                                               \
        APLVFP lNeg;                                                                \
                                                                                    \
        /* Initialize to 0 */                                                       \
        mpfr_init0 (&lNeg);                                                         \
                                                                                    \
        /* Loop through the rows and cols */                                        \
        for (i = 0; i < N; i++)                                                     \
        for (j = 0; j < N; j++)                                                     \
        {                                                                           \
            APLVFP l;                                                               \
                                                                                    \
            k = MatRep##NLR[i][j];                                                  \
            l = ((LPAPLVFP) lpMemRht)[abs (k)];                                     \
                                                                                    \
            /* If the sign is non-negative, ... */                                  \
            if (signum (k) >= 0)                                                    \
                mpfr_init_set (&(*(LPAPLVFP##N) lpMemRes)[i][j], &l, MPFR_RNDN);    \
            else                                                                    \
            {                                                                       \
                mpfr_neg (&lNeg, &l, MPFR_RNDN);                                    \
                mpfr_init_set (&(*(LPAPLVFP##N) lpMemRes)[i][j], &lNeg, MPFR_RNDN); \
            } /* End IF/ELSE */                                                     \
        } /* End FOR/FOR */                                                         \
                                                                                    \
        /* We no longer need this storage */                                        \
        mpfr_clear (&lNeg);                                                         \
    } /* case ARRAY_VFP */


//***************************************************************************
//  End of File: po_quadjot.h
//***************************************************************************
