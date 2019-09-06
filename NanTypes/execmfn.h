//***************************************************************************
//  NARS2000 -- Magic Function/Operator Execution Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2019 Sudley Place Software

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

// Magic Function/Operator Enum
typedef enum tagMFO
{
    MFOE_MonIota         ,      // 00:  Extended Monadic Iota
    MFOE_DydIota         ,      // 01:  Extended Dyadic Iota
    MFOE_MonRank         ,      // 02:  Monadic Derived Function from Rank Operator
    MFOE_DydRank         ,      // 03:  Dyadic  ...
    MFOE_DetPerm         ,      // 04:  Permanent of a matrix
    MFOE_MonFMT          ,      // 05:  Monadic []FMT
    MFOE_BoxFMT          ,      // 06:  Box (for monadic []FMT)
    MFOE_MonVR           ,      // 07:  Monadic []VR
    MFOE_IdnDot          ,      // 08:  Identity function for Inner Product
    MFOE_IdnJotDot       ,      // 09:  Identity function for Outer Product
    MFOE_MAD             ,      // 0A:  Multiset Asymmetric Difference
    MFOE_MSD             ,      // 0B:  Multiset Symmetric Difference
    MFOE_MU              ,      // 0C:  Multiset Union
    MFOE_MI              ,      // 0D:  Multiset Intersection
    MFOE_MIO             ,      // 0E:  Multiset Index Of
    MFOE_MEO             ,      // 0F:  Multiset Element Of
    MFOE_MM              ,      // 10:  Multiset Match
    MFOE_MLRS            ,      // 11:  Multiset LeftShoe
    MFOE_MLRSU           ,      // 12:  Multiset LeftShoeUnderbar
    MFOE_MMUL            ,      // 13:  Multiset Multiplicities
    MFOE_DydDnShoe       ,      // 14:  Dyadic Down Shoe
    MFOE_DydUpShoe       ,      // 15:  Dyadic Up Shoe
    MFOE_DydLRShoeUnd    ,      // 16:  Dyadic Left/Right Shoe Underbar
    MFOE_SD              ,      // 17:  Symmetric Difference
    MFOE_MonDomino       ,      // 18:  Monadic Domino for global numerics
    MFOE_DydDomino       ,      // 19:  Dyadic  ...
    MFOE_DydDotDot       ,      // 1A:  Dyadic DotDot
    MFOE_DydIotaUnderbar ,      // 1B:  Dyadic Iota Underbar
    MFOE_MonDot          ,      // 1C:  Monadic derived function from f.g for determinant
    MFOE_MonDotInit      ,      // 1D:  Initialization for #MonDot
    MFOE_MonDotAll       ,      // 1E:  Subroutine to ...
    MFOE_MonDotPerm      ,      // 1F:  ...
    MFOE_MonDotCr        ,      // 20:  ...
    MFOE_DydEpsUnderbar  ,      // 21:  Dyadic Epsilon Underbar on empty args
    MFOE_DydConv         ,      // 22:  Dyadic derived function from Convolution
    MFOE_IdnConv         ,      // 23:  Identity function for Convolution
    MFOE_DydScan         ,      // 24:  Dyadic Scan
    MFOE_DydScan1        ,      // 25:  Dyadic Scan, first dimension
    MFOE_RoS1L           ,      // 26:  Reduction of Singletons, type #1, left identity element
    MFOE_RoS1R           ,      // 27:  ...                           #1, right ...
    MFOE_RoS2            ,      // 28:  ...                           #2
    MFOE_RoS3            ,      // 29:  ...                           #3
    MFOE_MDIU            ,      // 2A:  Multiset dyadic iota underbar
    MFOE_DetSing         ,      // 2B:  Determinant of a singleton
    MFOE_DydVOFact       ,      // 2C:  Pochhammer symbol:  rising/falling factorial
    MFOE_MonExecute      ,      // 2D:  Monadic Execute with []FPC localized
    MFOE_DydSquad        ,      // 2E:  Dyadic Squad
    MFOE_DydSlope        ,      // 2F:  Dyadic Slope
    MFOE_MonShriek       ,      // 30:  Monadic Shriek
    MFOE_MatOpr          ,      // 31:  Matrix Operator
    MFOE_DydDot2         ,      // 32:  APL2's defn of Inner Product
    MFOE_DydMask         ,      // 33:  Dyadic function derived from Monadic Mask Operator
    MFOE_DydMesh         ,      // 34:  ...                                  Mesh ...
    MFOE_LENGTH                 // 35:  # entries in this enum
} MFO, *LPMFO;

// Magic Function/Operator Names
#define MFON_MonIota            L"#MonIota"
#define MFON_DydIota            L"#DydIota"
#define MFON_MonRank            L"#MonRank"
#define MFON_DydRank            L"#DydRank"
#define MFON_MonFMT             L"#MonFMT"
#define MFON_BoxFMT             L"#BoxFMT"
#define MFON_MonVR              L"#MonVR"
#define MFON_IdnDot             L"#IdnDot"
#define MFON_IdnJotDot          L"#IdnJotDot"
#define MFON_MAD                L"#MAD"
#define MFON_MSD                L"#MSD"
#define MFON_MU                 L"#MU"
#define MFON_MI                 L"#MI"
#define MFON_MIO                L"#MIO"
#define MFON_MEO                L"#MEO"
#define MFON_MM                 L"#MM"
#define MFON_MLRS               L"#MLRS"
#define MFON_MLRSU              L"#MLRSU"
#define MFON_MMUL               L"#MMUL"
#define MFON_DydDnShoe          L"#DydDnShoe"
#define MFON_DydUpShoe          L"#DydUpShoe"
#define MFON_DydLRShoeUnd       L"#DydLRShoeUnd"
#define MFON_SD                 L"#SD"
#define MFON_MonDomino          L"#MonDomino"
#define MFON_DydDomino          L"#DydDomino"
#define MFON_DydDotDot          L"#DydDotDot"
#define MFON_DydIotaUnderbar    L"#DydIotaUnderbar"
#define MFON_MonDot             L"#MonDot"
#define MFON_MonDotInit         L"#MonDotInit"
#define MFON_MonDotAll          L"#MonDotAll"
#define MFON_MonDotPerm         L"#MonDotPerm"
#define MFON_MonDotCr           L"#MonDotCr"
#define MFON_DydEpsUnderbar     L"#DydEpsUnderbar"
#define MFON_DydConv            L"#DydConv"
#define MFON_IdnConv            L"#IdnConv"
#define MFON_DydScan            L"#DydScan"
#define MFON_DydScan1           L"#DydScan1"
#define MFON_RoS1L              L"#RoS1L"
#define MFON_RoS1R              L"#RoS1R"
#define MFON_RoS2               L"#RoS2"
#define MFON_RoS3               L"#RoS3"
#define MFON_MDIU               L"#MDIU"
#define MFON_DetSing            L"#DetSing"
#define MFON_DydVOFact          L"#DydVOFact"
#define MFON_MonExecute         L"#MonExecute"
#define MFON_DydSquad           L"#DydSquad"
#define MFON_DydSlope           L"#DydSlope"
#define MFON_MonShriek          L"#MonShriek"
#define MFON_MatOpr             L"#MatOpr"
#define MFON_DetPerm            L"#DetPerm"
#define MFON_DydDot2            L"#DydDot2"
#define MFON_DydMask            L"#DydMask"
#define MFON_DydMesh            L"#DydMesh"

// Magic Function/Operator Variables
#define $ALPHA          WS_UTF16_ALPHA
#define $AFORESULT      WS_UTF16_QUAD L"Z"
#define $AND            WS_UTF16_UPCARET
#define $ARROWS         WS_UTF16_LEFTARROW WS_UTF16_RIGHTARROW
#define $CEILING        WS_UTF16_UPSTILE
#define $COMMABAR       WS_UTF16_COMMABAR
#define $COMMENT        WS_UTF16_LAMP
#define $COMMUTE        WS_UTF16_DIERESISTILDE
#define $DECODE         WS_UTF16_UPTACK
#define $DIAMOND        WS_UTF16_DIAMOND
#define $DROP           WS_UTF16_DOWNARROW
#define $DEL            WS_UTF16_DEL
#define $DELDEL         WS_UTF16_DELDEL
#define $DERIV          WS_UTF16_DERIV
#define $DISCLOSE       WS_UTF16_RIGHTSHOE
#define $DIVIDE         WS_UTF16_COLONBAR
#define $DNSH           WS_UTF16_DOWNSHOE
#define $DOMINO         WS_UTF16_DOMINO
#define $DS             WS_UTF16_DOUBLESHRIEK
#define $DT             WS_UTF16_DOUBLETILDE
#define $DUPLICATE      WS_UTF16_DIERESISTILDE
#define $EACH           WS_UTF16_DIERESIS
#define $ENCLOSE        WS_UTF16_LEFTSHOE
#define $ENCODE         WS_UTF16_DOWNTACK
#define $EPS            WS_UTF16_EPSILON
#define $EPSILON        WS_UTF16_EPSILON
#define $EXECUTE        WS_UTF16_UPTACKJOT
#define $FIRST          WS_UTF16_UPARROW
#define $FLOOR          WS_UTF16_DOWNSTILE
#define $FORMAT         WS_UTF16_DOWNTACKJOT
#define $GE             WS_UTF16_RIGHTCARETUNDERBAR
#define $GOTO           WS_UTF16_RIGHTARROW
#define $GRADEDN        WS_UTF16_DELSTILE
#define $GRADEUP        WS_UTF16_DELTASTILE
#define $GDN            WS_UTF16_DELSTILE
#define $GUP            WS_UTF16_DELTASTILE
#define $HASH           WS_UTF16_NUMBER
#define $IOTA           WS_UTF16_IOTA
#define $IOTAUND        WS_UTF16_IOTAUNDERBAR
#define $IS             WS_UTF16_LEFTARROW
#define $JOT            WS_UTF16_JOT
#define $LDC_LT_HORZ    WS_UTF16_LDC_LT_HORZ
#define $LDC_LT_VERT    WS_UTF16_LDC_LT_VERT
#define $LDC_LT_UL      WS_UTF16_LDC_LT_UL
#define $LDC_LT_UR      WS_UTF16_LDC_LT_UR
#define $LDC_LT_LL      WS_UTF16_LDC_LT_LL
#define $LDC_LT_LR      WS_UTF16_LDC_LT_LR
#define $LDC_LT_VERT_R  WS_UTF16_LDC_LT_VERT_R
#define $LDC_LT_VERT_L  WS_UTF16_LDC_LT_VERT_L
#define $LDC_LT_HORZ_D  WS_UTF16_LDC_LT_HORZ_D
#define $LDC_LT_HORZ_U  WS_UTF16_LDC_LT_HORZ_U
#define $LE             WS_UTF16_LEFTCARETUNDERBAR
#define $LOPER          WS_UTF16_LFTOPER
#define $MATCH          WS_UTF16_EQUALUNDERBAR
#define $MAX            WS_UTF16_UPSTILE
#define $MIN            WS_UTF16_DOWNSTILE
#define $MISMATCH       WS_UTF16_NOTEQUALUNDERBAR
#define $MULTISET       WS_UTF16_DOWNSHOESTILE
#define $NE             WS_UTF16_NOTEQUAL
#define $NEG            WS_UTF16_OVERBAR
#define $NOT            WS_UTF16_TILDE
#define $NOTEQUAL       WS_UTF16_NOTEQUAL
#define $NUMBER         WS_UTF16_NUMBER
#define $OMEGA          WS_UTF16_OMEGA
#define $OR             WS_UTF16_DOWNCARET
#define $PARENT         WS_UTF16_NUMBER WS_UTF16_NUMBER
#define $PICK           WS_UTF16_RIGHTSHOE
#define $PSUBSET        WS_UTF16_LEFTSHOE
#define $PSUPERSET      WS_UTF16_RIGHTSHOE
#define $QJ             WS_UTF16_QUADJOT
#define $QUAD           WS_UTF16_QUAD
#define $QUADJOT        WS_UTF16_QUADJOT
#define $QUAD_ALX       WS_UTF16_QUAD L"ALX"
#define $QUAD_AT        WS_UTF16_QUAD L"AT"
#define $QUAD_CT        WS_UTF16_QUAD L"CT"
#define $QUAD_CR        WS_UTF16_QUAD L"CR"
#define $QUAD_DM        WS_UTF16_QUAD L"DM"
#define $QUAD_EA        WS_UTF16_QUAD L"EA"
#define $QUAD_EC        WS_UTF16_QUAD L"EC"
#define $QUAD_ELX       WS_UTF16_QUAD L"ELX"
#define $QUAD_EM        WS_UTF16_QUAD L"EM"
#define $QUAD_ERROR     WS_UTF16_QUAD L"ERROR"
#define $QUAD_FC        WS_UTF16_QUAD L"FC"
#define $QUAD_FEATURE   WS_UTF16_QUAD L"FEATURE"
#define $QUAD_FMT       WS_UTF16_QUAD L"FMT"
#define $QUAD_FPC       WS_UTF16_QUAD L"FPC"
#define $QUAD_FX        WS_UTF16_QUAD L"FX"
#define $QUAD_ID        WS_UTF16_QUAD SYSLBL_ID
#define $QUAD_INV       WS_UTF16_QUAD SYSLBL_INV
#define $QUAD_IO        WS_UTF16_QUAD L"IO"
#define $QUAD_LX        WS_UTF16_QUAD L"LX"
#define $QUAD_MS        WS_UTF16_QUAD SYSLBL_MS
#define $QUAD_NC        WS_UTF16_QUAD L"NC"
#define $QUAD_PRO       WS_UTF16_QUAD SYSLBL_PRO
#define $QUAD_RL        WS_UTF16_QUAD L"RL"
#define $QUAD_TCNL      WS_UTF16_QUAD L"TCNL"
#define $QUAD_WSID      WS_UTF16_QUAD L"WSID"
#define $RANK           WS_UTF16_DIERESISJOT
#define $REVERSE        WS_UTF16_CIRCLESTILE
#define $RHO            WS_UTF16_RHO
#define $ROPER          WS_UTF16_RHTOPER
#define $ROTATE         WS_UTF16_CIRCLESTILE
#define $SQUAD          WS_UTF16_SQUAD
#define $SUBSET         WS_UTF16_LEFTSHOEUNDERBAR
#define $SUPERSET       WS_UTF16_RIGHTSHOEUNDERBAR
#define $TAKE           WS_UTF16_UPARROW
#define $TILDE          WS_UTF16_TILDE
#define $TIMES          WS_UTF16_TIMES
#define $TRANSPOSE      WS_UTF16_CIRCLESLOPE
#define $TYPE           WS_UTF16_DOWNTACK
#define $UPSH           WS_UTF16_UPSHOE
#define $ZILDE          WS_UTF16_ZILDE


typedef struct tagMAGIC_FCNOPR
{
    LPAPLCHAR  Header;              // 00:  Ptr to function/operator header
    LPAPLCHAR *Body;                // 04:  Ptr to array of function lines
    UINT       numFcnLines;         // 08:  # function lines in the body
                                    // 0C:  Length
} MAGIC_FCNOPR, *LPMAGIC_FCNOPR;

typedef struct tagINIT_MFO
{
    struct tagHSHTABSTR
                *lphtsMFO;          // 00:  Ptr to HSHTABSTR struc
    struct tagMEMVIRTSTR
                *lpLclMemVirtStr;   // 04:  Ptr to local MemVirtStr
    UINT         uPtdMemVirtStart,  // 08:  Starting offset into lpLclMemVirtStr
                 uPtdMemVirtEnd,    // 0C:  Ending   ...
                 uCount;            // 10:  Count of MFOs with local SYMTAB
                                    // 14:  Length
} INIT_MFO, *LPINIT_MFO;


//***************************************************************************
//  End of File: execmfn.h
//***************************************************************************
