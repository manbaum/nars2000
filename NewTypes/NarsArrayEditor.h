//***************************************************************************
//  NARS2000 -- Structs, Macros, and Defines for David Liebtag's Array Editor
//***************************************************************************


typedef struct tagARRAY_EDITOR
{
    int bQuadIO,                                        // Value of []IO
        iQuadPP;                                        // ...      []PP
    HGLOBAL (__stdcall *NarsAlloc) (UINT, SIZE_T);      // Same as Windows GlobalAlloc
    int (__cdecl *NarsChangeRefCntDir) (HGLOBAL, int);  // Change RefCnt of the ptr by ±1
    int (__cdecl *NarsFree) (HGLOBAL);                  // Free an array (changing the RefCnt to 0 does not free the array)
    int (__cdecl *MyCheckCtrlBreak) (void);             // Returns TRUE/FALSE:  TRUE iff Ctrl-Break is signalled
//  A way to signal an error
//  Format a nested array

} ARRAY_EDITOR, *LPARRAY_EDITOR;

// In your file, do not #define DEFINE_VARS.  This is here only so I can use the same file in my code.

#ifdef DEFINE_VARS
ARRAY_EDITOR arrayEditorStruct
= {0,
   0,
   GlobalAlloc,
   ChangeRefCntDirGlb,
   FreeResultGlobalVar,
   MyCheckCtrlBreak,
//  A way to signal an error
//  Format a nested array
  };

LPARRAY_EDITOR lpArrayEditor = &arrayEditorStruct;
#endif

extern LPARRAY_EDITOR lpArrayEditor;
