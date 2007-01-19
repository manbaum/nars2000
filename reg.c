//***************************************************************************
//  NARS2000 -- Registry Routines
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>
#include "main.h"
#include "datatype.h"
#include "resdebug.h"
#include "sysvars.h"
#include "Unicode.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


char lpszVersion[]                  = "NARS2000\nVersion %s";
char lpszRegKeyRoot[]               = "Software\\NARS2000",
     lpszRegStrXPosName[]           = "xPos",
     lpszRegStrYPosName[]           = "yPos",
     lpszRegStrXSizeName[]          = "xSize",
     lpszRegStrYSizeName[]          = "ySize",
     lpszRegStrInitDirName[]        = "InitDir",
     lpszRegStrSizeStateName[]      = "SizeState",
     lpszRegStrGlbALXName[]         = "GlbALX",
     lpszRegStrGlbCTName[]          = "GlbCT",
     lpszRegStrGlbDFName[]          = "GlbDF",
     lpszRegStrGlbELXName[]         = "GlbELX",
     lpszRegStrGlbIFName[]          = "GlbIF",
     lpszRegStrGlbIOName[]          = "GlbIO",
     lpszRegStrGlbLXName[]          = "GlbLX",
     lpszRegStrGlbPPName[]          = "GlbPP",
     lpszRegStrGlbPRName[]          = "GlbPR",
     lpszRegStrGlbPWName[]          = "GlbPW",
     lpszRegStrGlbRLName[]          = "GlbRL",
     lpszRegStrGlbSAName[]          = "GlbSA";

extern SIZE  MFSize;
extern int   MFSizeState;
extern POINT MFPosCtr;
extern char  szInitDir[_MAX_PATH];

extern APLFLOAT fQuadCT_CWS;
extern APLBOOL  bQuadDF_CWS,
                bQuadIF_CWS,
                bQuadIO_CWS,
                bQuadxSA_CWS;
extern APLINT   uQuadPP_CWS,
                uQuadPW_CWS,
                uQuadRL_CWS;
extern APLCHAR  cQuadPR_CWS;
extern HGLOBAL hGlbMTChar,
               hGlbSAClear,
               hGlbSAError,
               hGlbSAExit,
               hGlbSAOff,
               hGlbQuadPR_CWS,
               hGlbQuadSA_CWS,
               hGlbQuadLX_CWS,
               hGlbQuadALX_CWS,
               hGlbQuadELX_CWS;


//***************************************************************************
//  ReadReg
//
//  Read in registry entries
//***************************************************************************

void ReadReg (HWND hWnd)

{
    RECT  rcDtop;           // Rectangle for desktop
    POINT PosCtr;           // x- and y- positions

    // Save the window size for use in WM_MOVE messages
    // Note that this is the window size, not the client area size.
    GetWindowRect (hWnd, &rcDtop);
    MFSize.cx = rcDtop.right  - rcDtop.left;
    MFSize.cy = rcDtop.bottom - rcDtop.top;

    MFSize.cx = GetRegDword (HKEY_CURRENT_USER,
                             lpszRegKeyRoot,
                             lpszRegStrXSizeName,
                             MFSize.cx);
    MFSize.cy = GetRegDword (HKEY_CURRENT_USER,
                             lpszRegKeyRoot,
                             lpszRegStrYSizeName,
                             MFSize.cy);
    // Setup the default values in case the .INI file is not present
    GetWindowRect (GetDesktopWindow(), &rcDtop);
    PosCtr.x = rcDtop.left + MFSize.cx / 2; // Center horizontally
    PosCtr.y = rcDtop.top  + MFSize.cy / 2; // Center vertically

    MFPosCtr.x = GetRegDword (HKEY_CURRENT_USER,
                              lpszRegKeyRoot,
                              lpszRegStrXPosName,
                              PosCtr.x);
    MFPosCtr.y = GetRegDword (HKEY_CURRENT_USER,
                              lpszRegKeyRoot,
                              lpszRegStrYPosName,
                              PosCtr.y);
    MFSizeState = GetRegDword (HKEY_CURRENT_USER,
                               lpszRegKeyRoot,
                               lpszRegStrSizeStateName,
                               MFSizeState);
    // Read in File Open/Save Initial Dir
    GetRegStr (HKEY_CURRENT_USER,
               lpszRegKeyRoot,
               lpszRegStrInitDirName,
               szInitDir,
               sizeof (szInitDir),
               szInitDir);

    // Read in default values for system variables in a CLEAR WS

    // Read in []CT
    ((QWORD) fQuadCT_CWS) =
    GetRegQword (HKEY_CURRENT_USER,
                 lpszRegKeyRoot,
                 lpszRegStrGlbCTName,
                 DEF_QUADxCT_CWS);

    // Read in []DF
    bQuadDF_CWS = (APLBOOL)
    GetRegDword (HKEY_CURRENT_USER,
                 lpszRegKeyRoot,
                 lpszRegStrGlbDFName,
                 DEF_QUADDF_CWS);

    // Read in []IF
    bQuadIF_CWS = (APLBOOL)
    GetRegDword (HKEY_CURRENT_USER,
                 lpszRegKeyRoot,
                 lpszRegStrGlbIFName,
                 DEF_QUADIF_CWS);

    // Read in []IO
    bQuadIO_CWS = (APLBOOL)
    GetRegDword (HKEY_CURRENT_USER,
                 lpszRegKeyRoot,
                 lpszRegStrGlbIOName,
                 DEF_QUADIO_CWS);

    // Read in []PP
    uQuadPP_CWS =
    GetRegQword (HKEY_CURRENT_USER,
                 lpszRegKeyRoot,
                 lpszRegStrGlbPPName,
                 DEF_QUADPP_CWS);

    // Read in []PR
    cQuadPR_CWS =
    GetRegWchar (HKEY_CURRENT_USER,
                 lpszRegKeyRoot,
                 lpszRegStrGlbPRName,
                 DEF_QUADPR_CWS);
    // If the WCHAR is 0, we interpret that
    //   as the empty vector case
    hGlbQuadPR_CWS = ((cQuadPR_CWS EQ L'\0') ? hGlbMTChar : NULL);

    // Read in []PW
    uQuadPW_CWS =
    GetRegQword (HKEY_CURRENT_USER,
                 lpszRegKeyRoot,
                 lpszRegStrGlbPWName,
                 DEF_QUADPW_CWS);

    // Read in []RL
    uQuadRL_CWS =
    GetRegQword (HKEY_CURRENT_USER,
                 lpszRegKeyRoot,
                 lpszRegStrGlbRLName,
                 DEF_QUADRL_CWS);

    // Read in []SA
    bQuadxSA_CWS = (APLBOOL)
    GetRegDword (HKEY_CURRENT_USER,
                 lpszRegKeyRoot,
                 lpszRegStrGlbSAName,
                 DEF_QUADxSA_CWS);
    // Set the correct value for []SA
    switch (bQuadxSA_CWS)
    {
        case 0:         // ""
            hGlbQuadSA_CWS = hGlbMTChar;
            break;

        case 1:         // "CLEAR"
            hGlbQuadSA_CWS = hGlbSAClear;
            break;

        case 2:         // "ERROR"
            hGlbQuadSA_CWS = hGlbSAError;
            break;

        case 3:         // "EXIT"
            hGlbQuadSA_CWS = hGlbSAExit;
            break;

        case 4:         // "OFF"
            hGlbQuadSA_CWS = hGlbSAOff;
            break;

        defstop
            break;
    } // End SWITCH

    // Read in []LX
    hGlbQuadLX_CWS =
    GetRegGlb (HKEY_CURRENT_USER,
               lpszRegKeyRoot,
               lpszRegStrGlbLXName,
               DEF_QUADLX_CWS);

    // Read in []ALX
    hGlbQuadALX_CWS =
    GetRegGlb (HKEY_CURRENT_USER,
               lpszRegKeyRoot,
               lpszRegStrGlbALXName,
               DEF_QUADALX_CWS);

    // Read in []ELX
    hGlbQuadELX_CWS =
    GetRegGlb (HKEY_CURRENT_USER,
               lpszRegKeyRoot,
               lpszRegStrGlbELXName,
               DEF_QUADELX_CWS);
} // End ReadReg


//***************************************************************************
//  SaveEnvironment
//
//  Save the environment to the registry
//***************************************************************************

void SaveEnvironment (void)

{
    HKEY hKeyRoot;

    // Write out root settings to the registry
    RegCreateKey (HKEY_CURRENT_USER,    // Handle of an open key
                  lpszRegKeyRoot,       // Name of subkey to open
                  &hKeyRoot);           // Buffer for opened handle

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrXPosName,  // Name of value to set
                   0,                   // Reserved
                   REG_DWORD,           // Flag for type
                   (LPBYTE) &MFPosCtr.x,// Ptr to value
                   sizeof (MFPosCtr.x));// Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrYPosName,  // Name of value to set
                   0,                   // Reserved
                   REG_DWORD,           // Flag for type
                   (LPBYTE) &MFPosCtr.y,// Ptr to value
                   sizeof (MFPosCtr.y));// Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrXSizeName, // Name of value to set
                   0,                   // Reserved
                   REG_DWORD,           // Flag for type
                   (LPBYTE) &MFSize.cx, // Ptr to value
                   sizeof (MFSize.cx)); // Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrYSizeName, // Name of value to set
                   0,                   // Reserved
                   REG_DWORD,           // Flag for type
                   (LPBYTE) &MFSize.cy, // Ptr to value
                   sizeof (MFSize.cy)); // Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrSizeStateName, // Name of value to set
                   0,                   // Reserved
                   REG_DWORD,           // Flag for type
                   (LPBYTE) &MFSizeState, // Ptr to value
                   sizeof (MFSizeState)); // Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrInitDirName,// Name of value to set
                   0,                   // Reserved
                   REG_SZ,              // Flag for type
                   szInitDir,           // Ptr to value
                   lstrlen (szInitDir) + 1);// Size of value (including terminating zero)

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbCTName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (char *) &fQuadCT_CWS,// Ptr to value
                   sizeof (fQuadCT_CWS));// Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbDFName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (char *) &bQuadDF_CWS,// Ptr to value
                   sizeof (bQuadDF_CWS));// Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbIFName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (char *) &bQuadIF_CWS,// Ptr to value
                   sizeof (bQuadIF_CWS));// Size of value


    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbIOName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (char *) &bQuadIO_CWS,// Ptr to value
                   sizeof (bQuadIO_CWS));// Size of value


    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbPPName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (char *) &uQuadPP_CWS,// Ptr to value
                   sizeof (uQuadPP_CWS));// Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbPWName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (char *) &uQuadPW_CWS,// Ptr to value
                   sizeof (uQuadPW_CWS));// Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbRLName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (char *) &uQuadRL_CWS,// Ptr to value
                   sizeof (uQuadRL_CWS));// Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbPRName, // Name of value to set
                   0,                   // Reserved
                   REG_DWORD,           // Flag for type
                   (char *) &cQuadPR_CWS,// Ptr to value
                   sizeof (cQuadPR_CWS));// Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbPRName, // Name of value to set
                   0,                   // Reserved
                   REG_DWORD,           // Flag for type
                   (char *) &cQuadPR_CWS,// Ptr to value
                   sizeof (cQuadPR_CWS));// Size of value






    RegCloseKey (hKeyRoot); hKeyRoot = NULL;
} // End SaveEnvironment


//***************************************************************************
//  End of File: reg.c
//***************************************************************************
