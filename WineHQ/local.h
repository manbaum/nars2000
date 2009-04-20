
#define GCL_HBRBACKGROUND       (-10)
#define NM_LAST                 (0U- 99U)

#define strlenW         (UINT_PTR) lstrlenW
#define strcpyW         lstrcpyW

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

#define CP_ACP          0
#define USE_IME         0

INT         WINAPI MultiByteToWideChar(UINT,DWORD,LPCSTR,INT,LPWSTR,INT);
INT         WINAPI WideCharToMultiByte(UINT,DWORD,LPCWSTR,INT,LPSTR,INT,LPCSTR,LPBOOL);

void DbgBrk (void);
BOOL IzitFE (HWND);
extern int __cdecl dprintfW(unsigned short *lpwszFmt,...);
#ifdef DEBUG
extern int gDbgLvl;
#endif

#include "main.h"
#include "editctrl.h"
#include "resource.h"

#ifdef DEBUG
  #define   LCLODSAPI   ODSAPI
extern void __cdecl ODSAPI(char *lpStr,struct HWND__ *hWnd,unsigned int message,unsigned int wParam,long lParam);
#else
  #define   LCLODSAPI
#endif


