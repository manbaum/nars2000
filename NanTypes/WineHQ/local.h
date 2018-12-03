#include "WineCom.h"

#define GCL_HBRBACKGROUND       (-10)
#define NM_LAST                 (0U- 99U)

#undef  WM_KEYLAST

#ifndef WM_UNICHAR
#define WM_UNICHAR                      0x0109
#define WM_KEYLAST                      0x0109
#define UNICODE_NOCHAR                  0xFFFF
#else
#define WM_KEYLAST                      0x0108
#endif

#ifdef _WIN64
  typedef          __int64 APLI3264;    // Widest native signed value
  typedef unsigned __int64 APLU3264;    // ...           unsigned ...
#elif defined (_WIN32)
  typedef          __int32 APLI3264;    // Widest native signed value
  typedef unsigned __int32 APLU3264;    // ...           unsigned ...
#else
  #error Need code for this architecture.
#endif


#define WINAPI          __stdcall
typedef int             BOOL,       *PBOOL,    *LPBOOL;
typedef int             INT,        *PINT,     *LPINT;
#define CHAR            char
#define UINT            unsigned int
typedef unsigned long   DWORD,      *PDWORD,   *LPDWORD;
typedef unsigned short  WCHAR,      *PWCHAR;
typedef CHAR           *PSTR,       *LPSTR,     *NPSTR;
typedef const CHAR     *PCSTR,      *LPCSTR;
typedef WCHAR          *PWSTR,      *LPWSTR,    *NWPSTR;
typedef const WCHAR    *PCWSTR,     *LPCWSTR;

LPWSTR strcpyW  (LPWSTR, LPWSTR);
LPWSTR strcpynW (LPWSTR, LPWSTR, int);

#define CP_ACP          0
#define USE_IME         0

INT         WINAPI MultiByteToWideChar(UINT,DWORD,LPCSTR,INT,LPWSTR,INT);
INT         WINAPI WideCharToMultiByte(UINT,DWORD,LPCWSTR,INT,LPSTR,INT,LPCSTR,LPBOOL);

void DbgBrk (void);
BOOL IzitFE (HWND);
extern int __cdecl dprintfWL0 (unsigned short *lpwszFmt,...);
extern int __cdecl dprintfWL9 (unsigned short *lpwszFmt,...);
extern int __cdecl oprintfW   (unsigned short *lpwszFmt,...);
extern void nop (void);
extern HCURSOR hCursorNo;
extern HCURSOR hCursorDragMove;
extern HCURSOR hCursorDragCopy;
#ifdef DEBUG
extern int gDbgLvl;
extern char * FileNameOnly(char * lpFileName);
#endif

extern HFONT hFontPR;
void DrawLineContSub (HWND, HDC, int, int, int);

#define MP_RAT          int // For APLRAT in "types.h"
#define __mpfr_struct   int // For APLVFP in "types.h"
#include "enums.h"
#include "defines.h"
#define MP_INT          int
#define mp_bitcnt_t     int
#define mp_prec_t       int
#define NO_CHO_TYPES
#include "types.h"
#include "editctrl.h"
#include "resource.h"
#include "unicode.h"
#include "macros.h"
#define EXTERN extern
#include "keyboards.h"
#undef  EXTERN

extern HGLOBAL __cdecl SplitLines (HGLOBAL, UINT, LPUBOOL);
#ifdef DEBUG
  extern HWND _MySetFocus (HWND, LPCHAR, UINT);
#endif

#ifdef DEBUG
  #define   LCLODSAPI   ODSAPI
extern void __cdecl ODSAPI(char *lpStr,struct HWND__ *hWnd,unsigned int message,unsigned int wParam,long lParam);
extern char * __cdecl APIMsg(struct HWND__ *hWnd,unsigned int message,unsigned int wParam,long lParam,char *lpStr, BOOL bFlag);
#else
  #define   LCLODSAPI
#endif

