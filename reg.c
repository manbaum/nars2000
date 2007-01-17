//***************************************************************************
//  NARS2000 -- Registry Routines
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>
#include "resdebug.h"

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
     lpszRegStrSizeStateName[]      = "SizeState";

extern SIZE  MFSize;
extern int   MFSizeState;
extern POINT MFPosCtr;
extern char  szInitDir[_MAX_PATH];


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

    RegCloseKey (hKeyRoot); hKeyRoot = NULL;
} // End SaveEnvironment


//***************************************************************************
//  End of File: reg.c
//***************************************************************************
