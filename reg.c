//***************************************************************************
//  NARS2000 -- Registry Routines
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "sysvars.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


char lpszRegKeyRoot[]               = "Software\\NARS2000",
     lpszRegStrXPosName[]           = "xPos",
     lpszRegStrYPosName[]           = "yPos",
     lpszRegStrXSizeName[]          = "xSize",
     lpszRegStrYSizeName[]          = "ySize",
     lpszRegStrSizeStateName[]      = "SizeState",
/////lpszRegStrInitDirName[]        = "InitDir",
     lpszRegStrLoadDirName[]        = "LoadDir",
     lpszRegStrSaveDirName[]        = "SaveDir",
     lpszRegStrLFTCName[]           = "LogfontTC",
     lpszRegStrLFSMName[]           = "LogfontSM",
     lpszRegStrLFFEName[]           = "LogfontFE",
     lpszRegStrLFMEName[]           = "LogfontME",
     lpszRegStrLFVEName[]           = "LogfontVE",
     lpszRegStrGlbALXName[]         = "GlbALX",
     lpszRegStrGlbCTName[]          = "GlbCT",
     lpszRegStrGlbELXName[]         = "GlbELX",
     lpszRegStrGlbFCName[]          = "GlbFC",
     lpszRegStrGlbIOName[]          = "GlbIO",
     lpszRegStrGlbLXName[]          = "GlbLX",
     lpszRegStrGlbPPName[]          = "GlbPP",
     lpszRegStrGlbPRName[]          = "GlbPR",
     lpszRegStrGlbPWName[]          = "GlbPW",
     lpszRegStrGlbRLName[]          = "GlbRL",
     lpszRegStrGlbSAName[]          = "GlbSA";


//***************************************************************************
//  $ReadRegGlb
//
//  Read in registry entries having to do with global values
//***************************************************************************

void ReadRegGlb
    (void)

{
////// Read in File Open/Save Initial Dir
////GetRegStr (HKEY_CURRENT_USER,
////           lpszRegKeyRoot,
////           lpszRegStrInitDirName,
////           szInitDir,
////           sizeof (szInitDir),
////           szInitDir);

    // Read in Load Workspaces Dir
    GetRegBinary (HKEY_CURRENT_USER,
                  lpszRegKeyRoot,
                  lpszRegStrLoadDirName,
                  wszLoadDir,
                  sizeof (wszLoadDir),
                  wszLoadDir);

    // Read in Save Workspaces Dir
    GetRegBinary (HKEY_CURRENT_USER,
                  lpszRegKeyRoot,
                  lpszRegStrSaveDirName,
                  wszSaveDir,
                  sizeof (wszSaveDir),
                  wszSaveDir);

    // Read in LOGFONT struc for TC
    GetRegBinary (HKEY_CURRENT_USER,
                  lpszRegKeyRoot,
                  lpszRegStrLFTCName,
                  &lfTC,
                  sizeof (lfTC),
                  &lfTC);

    // Read in LOGFONT struc for SM
    GetRegBinary (HKEY_CURRENT_USER,
                  lpszRegKeyRoot,
                  lpszRegStrLFSMName,
                  &lfSM,
                  sizeof (lfSM),
                  &lfSM);

    // Read in LOGFONT struc for FE
    GetRegBinary (HKEY_CURRENT_USER,
                  lpszRegKeyRoot,
                  lpszRegStrLFFEName,
                  &lfFE,
                  sizeof (lfFE),
                  &lfFE);

    // Read in LOGFONT struc for ME
    GetRegBinary (HKEY_CURRENT_USER,
                  lpszRegKeyRoot,
                  lpszRegStrLFMEName,
                  &lfME,
                  sizeof (lfME),
                  &lfME);

    // Read in LOGFONT struc for VE
    GetRegBinary (HKEY_CURRENT_USER,
                  lpszRegKeyRoot,
                  lpszRegStrLFVEName,
                  &lfVE,
                  sizeof (lfVE),
                  &lfVE);

    // Read in default values for system variables in a CLEAR WS

    // Read in []CT
    ((QWORD) fQuadCT_CWS) =
    GetRegQword (HKEY_CURRENT_USER,
                 lpszRegKeyRoot,
                 lpszRegStrGlbCTName,
                 DEF_QUADxCT_CWS);

    // Read in []FC
    hGlbQuadFC_CWS =
    GetRegGlbChar (HKEY_CURRENT_USER,
                   lpszRegKeyRoot,
                   lpszRegStrGlbFCName,
                   DEF_QUADFC_GLB,
                   DEF_QUADFC_CWS);

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
    //   as the empty vector case.
    // Because we only reference this var when
    //   it's an empty vector, it always has that value.
    hGlbQuadPR_CWS = hGlbV0Char;

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
            hGlbQuadSA_CWS = hGlbV0Char;
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
    GetRegGlbChar (HKEY_CURRENT_USER,
                   lpszRegKeyRoot,
                   lpszRegStrGlbLXName,
                   DEF_QUADLX_GLB,
                   DEF_QUADLX_CWS);

    // Read in []ALX
    hGlbQuadALX_CWS =
    GetRegGlbChar (HKEY_CURRENT_USER,
                   lpszRegKeyRoot,
                   lpszRegStrGlbALXName,
                   DEF_QUADALX_GLB,
                   DEF_QUADALX_CWS);

    // Read in []ELX
    hGlbQuadELX_CWS =
    GetRegGlbChar (HKEY_CURRENT_USER,
                   lpszRegKeyRoot,
                   lpszRegStrGlbELXName,
                   DEF_QUADELX_GLB,
                   DEF_QUADELX_CWS);
} // End ReadRegGlb


//***************************************************************************
//  $ReadRegWnd
//
//  Read in window-specific registry entries
//***************************************************************************

void ReadRegWnd
    (HWND hWnd)

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
} // End ReadRegWnd


//***************************************************************************
//  $SaveEnvironment
//
//  Save the environment to the registry
//***************************************************************************

void SaveEnvironment
    (void)

{
    HKEY    hKeyRoot;
    LPVOID  lpMem;
    UINT    nBytes;

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

////RegSetValueEx (hKeyRoot,            // Handle of key to set
////               lpszRegStrInitDirName,// Name of value to set
////               0,                   // Reserved
////               REG_SZ,              // Flag for type
////               szInitDir,           // Ptr to value
////               lstrlen (szInitDir) + 1);// Size of value (including terminating zero)

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrLoadDirName,// Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) wszLoadDir, // Ptr to value
                   lstrlenW (wszLoadDir) + 1);// Size of value (including terminating zero)

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrSaveDirName,// Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) wszSaveDir, // Ptr to value
                   lstrlenW (wszSaveDir) + 1);// Size of value (including terminating zero)

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrLFTCName,  // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) &lfTC,      // Ptr to value
                   sizeof (lfTC));      // Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrLFSMName,  // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) &lfSM,      // Ptr to value
                   sizeof (lfSM));      // Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrLFFEName,  // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) &lfFE,      // Ptr to value
                   sizeof (lfFE));      // Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrLFMEName,  // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) &lfME,      // Ptr to value
                   sizeof (lfME));      // Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrLFVEName,  // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) &lfVE,      // Ptr to value
                   sizeof (lfVE));      // Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbCTName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) &fQuadCT_CWS,// Ptr to value
                   sizeof (fQuadCT_CWS));// Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbIOName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) &bQuadIO_CWS,// Ptr to value
                   sizeof (bQuadIO_CWS));// Size of value


    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbPPName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) &uQuadPP_CWS,// Ptr to value
                   sizeof (uQuadPP_CWS));// Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbPRName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) &cQuadPR_CWS,// Ptr to value
                   sizeof (cQuadPR_CWS));// Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbPWName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) &uQuadPW_CWS,// Ptr to value
                   sizeof (uQuadPW_CWS));// Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbRLName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) &uQuadRL_CWS,// Ptr to value
                   sizeof (uQuadRL_CWS));// Size of value

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbSAName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) &bQuadxSA_CWS,// Ptr to value
                   sizeof (bQuadxSA_CWS));// Size of value

    //************************ []FC ***************************
    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbQuadFC_CWS);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
    // Get the # bytes
    nBytes = ((UINT) lpHeader->NELM) * sizeof (APLCHAR);
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, 1);

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbFCName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) lpMem,      // Ptr to value
                   nBytes);             // Size of value

    // We no longer need this ptr
    MyGlobalUnlock (hGlbQuadFC_CWS); lpMem = NULL;

    //************************ []LX ***************************
    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbQuadLX_CWS);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
    // Get the # bytes
    nBytes = ((UINT) lpHeader->NELM) * sizeof (APLCHAR);
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, 1);

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbLXName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) lpMem,      // Ptr to value
                   nBytes);             // Size of value

    // We no longer need this ptr
    MyGlobalUnlock (hGlbQuadLX_CWS); lpMem = NULL;

    //************************ []ALX **************************
    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbQuadALX_CWS);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
    // Get the # bytes
    nBytes = ((UINT) lpHeader->NELM) * sizeof (APLCHAR);
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, 1);

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbALXName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) lpMem,      // Ptr to value
                   nBytes);             // Size of value

    // We no longer need this ptr
    MyGlobalUnlock (hGlbQuadALX_CWS); lpMem = NULL;

    //************************ []ELX **************************
    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbQuadELX_CWS);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
    // Get the # bytes
    nBytes = ((UINT) lpHeader->NELM) * sizeof (APLCHAR);
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, 1);

    RegSetValueEx (hKeyRoot,            // Handle of key to set
                   lpszRegStrGlbELXName, // Name of value to set
                   0,                   // Reserved
                   REG_BINARY,          // Flag for type
                   (LPCHAR) lpMem,      // Ptr to value
                   nBytes);             // Size of value

    // We no longer need this ptr
    MyGlobalUnlock (hGlbQuadELX_CWS); lpMem = NULL;

    RegCloseKey (hKeyRoot); hKeyRoot = NULL;
} // End SaveEnvironment


//***************************************************************************
//  End of File: reg.c
//***************************************************************************
