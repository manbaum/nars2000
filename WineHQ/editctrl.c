/*
 *  Edit control
 *
 *  Copyright  David W. Metcalfe, 1994
 *  Copyright  William Magro, 1995, 1996
 *  Copyright  Frans van Dorsselaer, 1996, 1997
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 *
 * NOTES
 *
 * This code was audited for completeness against the documented features
 * of Comctl32.dll version 6.0 on Oct. 8, 2004, by Dimitrie O. Paun.
 *
 * Unless otherwise noted, we believe this code to be complete, as per
 * the specification mentioned above.
 * If you discover missing features, or bugs, please note them below.
 *
 * TODO:
 *   - EDITBALLOONTIP structure
 *   - EM_GETCUEBANNER/Edit_GetCueBannerText
 *   - EM_HIDEBALLOONTIP/Edit_HideBalloonTip
 *   - EM_SETCUEBANNER/Edit_SetCueBannerText
 *   - EM_SHOWBALLOONTIP/Edit_ShowBalloonTip
 *   - EM_GETIMESTATUS, EM_SETIMESTATUS
 *   - EN_ALIGN_LTR_EC
 *   - EN_ALIGN_RTL_EC
 *   - ES_OEMCONVERT
 *
 */

#include "config.h"

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "windef.h"
#include "winbase.h"
#include "winnt.h"
#include "wownt32.h"
#include "win.h"
#include "imm.h"
#include "wine/winbase16.h"
#include "wine/winuser16.h"
#include "wine/unicode.h"
#include "controls.h"
#include "user_private.h"
#include "wine/debug.h"

#include <windowsx.h>
#include "local.h"

WINE_DEFAULT_DEBUG_CHANNEL(edit);
WINE_DECLARE_DEBUG_CHANNEL(combo);
WINE_DECLARE_DEBUG_CHANNEL(relay);

#define BUFLIMIT_INITIAL    30000   /* initial buffer size */
#define GROWLENGTH      32  /* buffers granularity in bytes: must be power of 2 */
#define ROUND_TO_GROW(size) (((size) + (GROWLENGTH - 1)) & ~(GROWLENGTH - 1))
#define HSCROLL_FRACTION    3   /* scroll window by 1/3 width */
#define CARETWIDTH      2       // Default caret width

/*
 *  extra flags for EDITSTATE.flags field
 */
#define EF_MODIFIED     0x0001  /* text has been modified */
#define EF_FOCUSED      0x0002  /* we have input focus */
#define EF_UPDATE       0x0004  /* notify parent of changed state */
#define EF_VSCROLL_TRACK    0x0008  /* don't SetScrollPos() since we are tracking the thumb */
#define EF_HSCROLL_TRACK    0x0010  /* don't SetScrollPos() since we are tracking the thumb */
#define EF_AFTER_WRAP       0x0080  /* the caret is displayed after the last character of a
                       wrapped line, instead of in front of the next character */
#define EF_USE_SOFTBRK      0x0100  /* Enable soft breaks in text. */
#define EF_APP_HAS_HANDLE       0x0200  /* Set when an app sends EM_[G|S]ETHANDLE.  We are in sole control of
                                           the text buffer if this is clear. */
typedef enum
{
    END_0 = 0,          /* line ends with terminating '\0' character */
    END_WRAP,           /* line is wrapped */
    END_HARD,           /* line ends with a hard return '\r\n' */
    END_SOFT,           /* line ends with a soft return '\r\r\n' */
    END_RICH            /* line ends with a single '\n' */
} LINE_END;

typedef struct tagLINEDEF {
    INT length;         /* bruto length of a line in bytes */
    INT net_length;         /* netto length of a line in visible characters */
    LINE_END ending;
    INT width;          /* width of the line in pixels */
    INT index;          /* line index into the buffer */
    struct tagLINEDEF *next;
} LINEDEF;

typedef struct tagEDITSTATE
{
    BOOL is_unicode;        /* how the control was created */
    LPWSTR text;            /* the actual contents of the control */
    UINT text_length;       /* cached length of text buffer (in WCHARs) - use get_text_length() to retrieve */
    UINT buffer_size;       /* the size of the buffer in characters */
    UINT buffer_limit;      /* the maximum size to which the buffer may grow in characters */
    HFONT font;         /* NULL means standard system font */
    INT x_offset;           /* scroll offset    for multi lines this is in pixels
                                for single lines it's in characters */
    INT line_height;        /* height of a screen line in pixels */
    INT char_width;         /* average character width in pixels */
    DWORD style;            /* sane version of wnd->dwStyle */
    WORD flags;         /* flags that are not in es->style or wnd->flags (EF_XXX) */
    INT undo_insert_count;      /* number of characters inserted in sequence */
    UINT undo_position;     /* character index of the insertion and deletion */
    LPWSTR undo_text;       /* deleted text */
    UINT undo_buffer_size;      /* size of the deleted text buffer */
    INT selection_start;        /* == selection_end if no selection */
    INT selection_end;      /* == current caret position */
    WCHAR password_char;        /* == 0 if no password char, and for multi line controls */
    INT left_margin;        /* in pixels */
    INT right_margin;       /* in pixels */
    RECT format_rect;
    INT text_width;         /* width of the widest line in pixels for multi line controls
                       and just line width for single line controls */
    INT region_posx;        /* Position of cursor relative to region: */
    INT region_posy;        /* -1: to left, 0: within, 1: to right */
#ifdef _WIN16
    EDITWORDBREAKPROC16 word_break_proc16;
#endif
    void *word_break_proc;      /* 32-bit word break proc: ANSI or Unicode */
    INT line_count;         /* number of lines */
    INT y_offset;           /* scroll offset in number of lines */
    BOOL bCaptureState;         /* flag indicating whether mouse was captured */
    BOOL bEnableState;      /* flag keeping the enable state */
    HWND hwndSelf;          /* the our window handle */
    HWND hwndParent;        /* Handle of parent for sending EN_* messages.
                           Even if parent will change, EN_* messages
                       should be sent to the first parent. */
    HWND hwndListBox;       /* handle of ComboBox's listbox or NULL */
    /*
     *  only for multi line controls
     */
    INT lock_count;         /* amount of re-entries in the EditWndProc */
    INT tabs_count;
    LPINT tabs;
    LINEDEF *first_line_def;    /* linked list of (soft) linebreaks */
    HLOCAL hloc32W;         /* our unicode local memory block */
#ifdef _WIN16
    HLOCAL16 hloc16;        /* alias for 16-bit control receiving EM_GETHANDLE16
                       or EM_SETHANDLE16 */
#endif
    HLOCAL hloc32A;         /* alias for ANSI control receiving EM_GETHANDLE
                       or EM_SETHANDLE */
#if USE_IME
    /*
     * IME Data
     */
    UINT composition_len;   /* length of composition, 0 == no composition */
    int composition_start;  /* the character position for the composition */
#endif
} EDITSTATE;


#define SWAP_UINT32(x,y) do { UINT temp = (UINT)(x); (x) = (UINT)(y); (y) = temp; } while(0)
#define ORDER_UINT(x,y) do { if ((UINT)(y) < (UINT)(x)) SWAP_UINT32((x),(y)); } while(0)

/* used for disabled or read-only edit control */
#define EDIT_NOTIFY_PARENT(es, wNotifyCode) \
    do \
    { /* Notify parent which has created this edit control */ \
     /* TRACE("notification " #wNotifyCode " sent to hwnd=%p\n", es->hwndParent); */ \
        SendMessageW(es->hwndParent, WM_COMMAND, \
             MAKEWPARAM(GetWindowLongW((es->hwndSelf),GWLP_ID), wNotifyCode), \
             *(LPARAM *)(&es->hwndSelf)); \
    } while(0)

/*********************************************************************
 *
 *  Declarations
 *
 */

/*
 *  These functions have trivial implementations
 *  We still like to call them internally
 *  "static inline" makes them more like macro's
 */
static inline BOOL  EDIT_EM_CanUndo(EDITSTATE *es);
static inline void  EDIT_EM_EmptyUndoBuffer(EDITSTATE *es);
static inline void  EDIT_WM_Clear(EDITSTATE *es);
static inline void  EDIT_WM_Cut(EDITSTATE *es);

/*
 *  Helper functions only valid for one type of control
 */
static void EDIT_BuildLineDefs_ML(EDITSTATE *es, INT iStart, INT iEnd, INT delta, HRGN hrgn);
static void EDIT_CalcLineWidth_SL(EDITSTATE *es);
static LPWSTR   EDIT_GetPasswordPointer_SL(EDITSTATE *es);
static void EDIT_MoveDown_ML(EDITSTATE *es, BOOL extend);
static void EDIT_MovePageDown_ML(EDITSTATE *es, BOOL extend);
static void EDIT_MovePageUp_ML(EDITSTATE *es, BOOL extend);
static void EDIT_MoveUp_ML(EDITSTATE *es, BOOL extend);
/*
 *  Helper functions valid for both single line _and_ multi line controls
 */
static BOOL MyCreateCaret (HWND hWnd, HBITMAP hBitMap, int nWidth, int nHeight);
static INT  EDIT_CallWordBreakProc(EDITSTATE *es, INT start, INT index, INT count, INT action);
static INT  EDIT_CharFromPos(EDITSTATE *es, INT x, INT y, LPBOOL after_wrap);
static void EDIT_ConfinePoint(EDITSTATE *es, LPINT x, LPINT y);
static void EDIT_GetLineRect(EDITSTATE *es, INT line, INT scol, INT ecol, LPRECT rc);
static void EDIT_InvalidateText(EDITSTATE *es, INT start, INT end);
static void EDIT_LockBuffer(EDITSTATE *es);
static BOOL EDIT_MakeFit(EDITSTATE *es, UINT size);
static BOOL EDIT_MakeUndoFit(EDITSTATE *es, UINT size);
static void EDIT_MoveBackward(EDITSTATE *es, BOOL extend);
static void EDIT_MoveEnd(EDITSTATE *es, BOOL extend, BOOL ctrl);
static void EDIT_MoveForward(EDITSTATE *es, BOOL extend);
static void EDIT_MoveHome(EDITSTATE *es, BOOL extend, BOOL ctrl);
static void EDIT_MoveWordBackward(EDITSTATE *es, BOOL extend);
static void EDIT_MoveWordForward(EDITSTATE *es, BOOL extend);
static void EDIT_PaintLine(EDITSTATE *es, HDC hdc, INT line, BOOL rev, long lFlags, INT yLine);
static INT  EDIT_PaintText(EDITSTATE *es, HDC hdc, INT x, INT y, INT line, INT col, INT count, BOOL rev, long lFlags);
static void EDIT_SetCaretPos(EDITSTATE *es, INT pos, BOOL after_wrap);
static void EDIT_AdjustFormatRect(EDITSTATE *es);
static void EDIT_SetRectNP(EDITSTATE *es, LPRECT lprc);
static void EDIT_UnlockBuffer(EDITSTATE *es, BOOL force);
static void EDIT_UpdateScrollInfo(EDITSTATE *es);
static INT CALLBACK EDIT_WordBreakProc(LPWSTR s, INT index, INT count, INT action);
/*
 *  EM_XXX message handlers
 */
static LRESULT  EDIT_EM_CharFromPos(EDITSTATE *es, INT x, INT y);
static BOOL EDIT_EM_FmtLines(EDITSTATE *es, BOOL add_eol);
static HLOCAL   EDIT_EM_GetHandle(EDITSTATE *es);
#ifdef _WIN16
static HLOCAL16 EDIT_EM_GetHandle16(EDITSTATE *es);
#endif
static INT  EDIT_EM_GetLine(EDITSTATE *es, INT line, LPWSTR dst, BOOL unicode);
static LRESULT  EDIT_EM_GetSel(EDITSTATE *es, PUINT start, PUINT end);
static LRESULT  EDIT_EM_GetThumb(EDITSTATE *es);
static INT  EDIT_EM_LineFromChar(EDITSTATE *es, INT index);
static INT  EDIT_EM_LineIndex(EDITSTATE *es, INT line);
static INT  EDIT_EM_LineLength(EDITSTATE *es, INT index);
static BOOL EDIT_EM_LineScroll(EDITSTATE *es, INT dx, INT dy);
static BOOL EDIT_EM_LineScroll_internal(EDITSTATE *es, INT dx, INT dy);
static LRESULT  EDIT_EM_PosFromChar(EDITSTATE *es, INT index, BOOL after_wrap);
static void EDIT_EM_ReplaceSel(EDITSTATE *es, BOOL can_undo, LPCWSTR lpsz_replace, BOOL send_update, BOOL honor_limit);
static LRESULT  EDIT_EM_Scroll(EDITSTATE *es, INT action);
static void EDIT_EM_ScrollCaret(EDITSTATE *es);
static void EDIT_EM_SetHandle(EDITSTATE *es, HLOCAL hloc);
#ifdef _WIN16
static void EDIT_EM_SetHandle16(EDITSTATE *es, HLOCAL16 hloc);
#endif
static void EDIT_EM_SetLimitText(EDITSTATE *es, INT limit);
static void EDIT_EM_SetMargins(EDITSTATE *es, INT action, WORD left, WORD right, BOOL repaint);
static void EDIT_EM_SetPasswordChar(EDITSTATE *es, WCHAR c);
static void EDIT_EM_SetSel(EDITSTATE *es, UINT start, UINT end, BOOL after_wrap);
static BOOL EDIT_EM_SetTabStops(EDITSTATE *es, INT count, LPINT tabs);
#ifdef _WIN16
static BOOL EDIT_EM_SetTabStops16(EDITSTATE *es, INT count, LPINT16 tabs);
#endif
static void EDIT_EM_SetWordBreakProc(EDITSTATE *es, void *wbp);
#ifdef _WIN16
static void EDIT_EM_SetWordBreakProc16(EDITSTATE *es, EDITWORDBREAKPROC16 wbp);
#endif
static BOOL EDIT_EM_Undo(EDITSTATE *es);
/*
 *  WM_XXX message handlers
 */
static LRESULT  EDIT_WM_Char(EDITSTATE *es, WCHAR c);
static void EDIT_WM_Command(EDITSTATE *es, INT code, INT id, HWND conrtol);
static void EDIT_WM_ContextMenu(EDITSTATE *es, INT x, INT y);
static void EDIT_WM_Copy(EDITSTATE *es);
static LRESULT  EDIT_WM_Create(EDITSTATE *es, LPCWSTR name);
static LRESULT  EDIT_WM_Destroy(EDITSTATE *es);
static size_t  EDIT_WM_GetText(EDITSTATE *es, INT count, LPWSTR dst, BOOL unicode);
static LRESULT  EDIT_WM_HScroll(EDITSTATE *es, INT action, INT pos);
static LRESULT  EDIT_WM_KeyDown(EDITSTATE *es, INT key);
static LRESULT  EDIT_WM_KillFocus(EDITSTATE *es);
static LRESULT  EDIT_WM_LButtonDblClk(EDITSTATE *es);
static LRESULT  EDIT_WM_LButtonDown(EDITSTATE *es, DWORD keys, INT x, INT y);
static LRESULT  EDIT_WM_LButtonUp(EDITSTATE *es);
static LRESULT  EDIT_WM_MButtonDown(EDITSTATE *es);
static LRESULT  EDIT_WM_MouseMove(EDITSTATE *es, INT x, INT y);
static LRESULT  EDIT_WM_NCCreate(HWND hwnd, LPCREATESTRUCTW lpcs, BOOL unicode);
static void EDIT_WM_Paint(EDITSTATE *es, HDC hdc, long lFlags);
static void EDIT_WM_Paint2(EDITSTATE *es, HDC hdc, HDC hdcbg, long lFlags);
static BOOL LineHasSelection(EDITSTATE *es, INT line);
static BOOL LineOnCurrentPage(EDITSTATE *es, INT line, INT nLOP);
static void EDIT_WM_Paste(EDITSTATE *es);
static void EDIT_WM_SetFocus(EDITSTATE *es);
static void EDIT_WM_SetFont(EDITSTATE *es, HFONT font, BOOL redraw);
static void EDIT_WM_SetText(EDITSTATE *es, LPCWSTR text, BOOL unicode);
static void EDIT_WM_Size(EDITSTATE *es, UINT action, INT width, INT height);
static LRESULT  EDIT_WM_StyleChanged(EDITSTATE *es, WPARAM which, const STYLESTRUCT *style);
static LRESULT  EDIT_WM_SysKeyDown(EDITSTATE *es, INT key, DWORD key_data);
static void EDIT_WM_Timer(EDITSTATE *es);
static LRESULT  EDIT_WM_VScroll(EDITSTATE *es, INT action, INT pos);
static void EDIT_UpdateText(EDITSTATE *es, LPRECT rc, BOOL bErase);
static void EDIT_UpdateTextRegion(EDITSTATE *es, HRGN hrgn, BOOL bErase);
#if USE_IME
static void EDIT_ImeComposition(HWND hwnd, LPARAM CompFlag, EDITSTATE *es);
#endif

LRESULT WINAPI EditWndProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI EditWndProcW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/*********************************************************************
 * edit class descriptor
 */
 #if 0
static const WCHAR editW[] = {'E','d','i','t',0};
const struct builtin_class_descr EDIT_builtin_class =
{
    editW,                /* name */
    CS_DBLCLKS | CS_PARENTDC,   /* style */
    EditWndProcA,         /* procA */
    EditWndProcW,         /* procW */
    sizeof(EDITSTATE *),  /* extra */
    IDC_IBEAM,            /* cursor */
    0                     /* brush */
};
#endif


/*********************************************************************
 *
 *  EM_CANUNDO
 *
 */
static inline BOOL EDIT_EM_CanUndo(EDITSTATE *es)
{
    return (es->undo_insert_count || strlenW(es->undo_text));
} // End EDIT_EM_CanUndo


/*********************************************************************
 *
 *  EM_EMPTYUNDOBUFFER
 *
 */
static inline void EDIT_EM_EmptyUndoBuffer(EDITSTATE *es)
{
    es->undo_insert_count = 0;
    *es->undo_text = '\0';
} // End EDIT_EM_EmptyUndoBuffer


/*********************************************************************
 *
 *  WM_CLEAR
 *
 */
static inline void EDIT_WM_Clear(EDITSTATE *es)
{
    static const WCHAR empty_stringW[] = {0};

    /* Protect read-only edit control from modification */
    if(es->style & ES_READONLY)
        return;

    EDIT_EM_ReplaceSel(es, TRUE, empty_stringW, TRUE, TRUE);
} // End EDIT_WM_Clear


/*********************************************************************
 *
 *  WM_CUT
 *
 */
static inline void EDIT_WM_Cut(EDITSTATE *es)
{
    EDIT_WM_Copy(es);
    EDIT_WM_Clear(es);
} // End EDIT_WM_Cut


/**********************************************************************
 *         get_app_version
 *
 * Returns the window version in case Wine emulates a later version
 * of windows than the application expects.
 *
 * In a number of cases when windows runs an application that was
 * designed for an earlier windows version, windows reverts
 * to "old" behaviour of that earlier version.
 *
 * An example is a disabled  edit control that needs to be painted.
 * Old style behaviour is to send a WM_CTLCOLOREDIT message. This was
 * changed in Win95, NT4.0 by a WM_CTLCOLORSTATIC message _only_ for
 * applications with an expected version 0f 4.0 or higher.
 *
 */
static DWORD get_app_version(void)
{
    static DWORD version;
    if (!version)
    {
        DWORD dwEmulatedVersion;
        OSVERSIONINFOW info;
        DWORD dwProcVersion = GetProcessVersion(0);

    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
        GetVersionExW( &info );
        dwEmulatedVersion = MAKELONG( info.dwMinorVersion, info.dwMajorVersion );
        /* FIXME: this may not be 100% correct; see discussion on the
         * wine developer list in Nov 1999 */
        version = dwProcVersion < dwEmulatedVersion ? dwProcVersion : dwEmulatedVersion;
    }
    return version;
} // End get_app_version


static inline UINT get_text_length(EDITSTATE *es)
{
    if(es->text_length == (UINT)-1)
        es->text_length = strlenW(es->text);
    return es->text_length;
} // End get_text_length


static inline void text_buffer_changed(EDITSTATE *es)
{
    es->text_length = (UINT)-1;
} // End text_buffer_changed


static HBRUSH EDIT_NotifyCtlColor(EDITSTATE *es, HDC hdc)
{
        HBRUSH hbrush;
    UINT msg;

        if ( get_app_version() >= 0x40000 && (!es->bEnableState || (es->style & ES_READONLY)))
        msg = WM_CTLCOLORSTATIC;
        else
        msg = WM_CTLCOLOREDIT;

    /* why do we notify to es->hwndParent, and we send this one to GetParent()? */
        hbrush = (HBRUSH)(HANDLE_PTR)SendMessageW(GetParent(es->hwndSelf), msg, *(WPARAM *)&hdc, *(LPARAM *)&es->hwndSelf);
        if (!hbrush)
            hbrush = (HBRUSH)(HANDLE_PTR)DefWindowProcW(GetParent(es->hwndSelf), msg, *(WPARAM *)&hdc, *(LPARAM *)&es->hwndSelf);
        return hbrush;
} // End EDIT_NotifyCtlColor


static inline LRESULT DefWindowProcT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL unicode)
{
    if(unicode)
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    else
        return DefWindowProcA(hwnd, msg, wParam, lParam);
} // End DefWindowProcT


/*********************************************************************
 *
 *  EditWndProc_common
 *
 *  The messages are in the order of the actual integer values
 *  (which can be found in include/windows.h)
 *  Wherever possible the 16 bit versions are converted to
 *  the 32 bit ones, so that we can 'fall through' to the
 *  helper functions.  These are mostly 32 bit (with a few
 *  exceptions, clearly indicated by a '16' extension to their
 *  names).
 *
 */
static LRESULT WINAPI EditWndProc_common( HWND hwnd, UINT msg,
                                          WPARAM wParam, LPARAM lParam, BOOL unicode )
{
    EDITSTATE *es = (EDITSTATE *)(HANDLE_PTR)GetWindowLongPtrW( hwnd, GWLEC_ES );
    LRESULT result = 0;

//      TRACE("hwnd=%p msg=%x (%s) wparam=%x lparam=%lx\n", hwnd, msg, SPY_GetMsgName(msg, hwnd), wParam, lParam);

    if (!es && msg != WM_NCCREATE)
        return DefWindowProcT(hwnd, msg, wParam, lParam, unicode);

    if (es && (msg != WM_DESTROY)) EDIT_LockBuffer(es);

    switch (msg) {
#ifdef _WIN16
    case EM_GETSEL16:
        wParam = 0;
        lParam = 0;
        /* fall through */
#endif
    case EM_GETSEL:
        result = EDIT_EM_GetSel(es, *(PUINT *)&wParam, *(PUINT *)&lParam);
        break;

#ifdef _WIN16
    case EM_SETSEL16:
        if ((short)LOWORD(lParam) == -1)
            EDIT_EM_SetSel(es, (UINT)-1, 0, FALSE);
        else
            EDIT_EM_SetSel(es, LOWORD(lParam), HIWORD(lParam), FALSE);
        if (!wParam)
            EDIT_EM_ScrollCaret(es);
        result = 1;
        break;
#endif
    case MYWM_SELECTALL:
    case EM_SETSEL:
        EDIT_EM_SetSel(es, wParam, lParam, FALSE);
        EDIT_EM_ScrollCaret(es);
        result = 1;
        break;

#ifdef _WIN16
    case EM_GETRECT16:
        if (lParam)
                {
                    RECT16 *r16 = MapSL(lParam);
                    r16->left   = es->format_rect.left;
                    r16->top    = es->format_rect.top;
                    r16->right  = es->format_rect.right;
                    r16->bottom = es->format_rect.bottom;
                }
        break;
#endif
    case EM_GETRECT:
        if (lParam)
            CopyRect(*(LPRECT *)&lParam, &es->format_rect);
        break;

#ifdef _WIN16
    case EM_SETRECT16:
        if ((es->style & ES_MULTILINE) && lParam) {
            RECT rc;
            RECT16 *r16 = MapSL(lParam);
            rc.left   = r16->left;
            rc.top    = r16->top;
            rc.right  = r16->right;
            rc.bottom = r16->bottom;
            EDIT_SetRectNP(es, &rc);
            EDIT_UpdateText(es, NULL, TRUE);
        }
        break;
#endif
    case EM_SETRECT:
        if ((es->style & ES_MULTILINE) && lParam) {
            EDIT_SetRectNP(es, *(LPRECT *)&lParam);
            EDIT_UpdateText(es, NULL, TRUE);
        }
        break;

#ifdef _WIN16
    case EM_SETRECTNP16:
        if ((es->style & ES_MULTILINE) && lParam) {
            RECT rc;
            RECT16 *r16 = MapSL(lParam);
            rc.left   = r16->left;
            rc.top    = r16->top;
            rc.right  = r16->right;
            rc.bottom = r16->bottom;
            EDIT_SetRectNP(es, &rc);
        }
        break;
#endif
    case EM_SETRECTNP:
        if ((es->style & ES_MULTILINE) && lParam)
            EDIT_SetRectNP(es, *(LPRECT *)&lParam);
        break;

#ifdef _WIN16
    case EM_SCROLL16:
#endif
    case EM_SCROLL:
        result = EDIT_EM_Scroll(es, (INT)wParam);
                break;

#ifdef _WIN16
    case EM_LINESCROLL16:
        wParam = (WPARAM)(INT)(SHORT)HIWORD(lParam);
        lParam = (LPARAM)(INT)(SHORT)LOWORD(lParam);
        /* fall through */
#endif
    case EM_LINESCROLL:
        result = (LRESULT)EDIT_EM_LineScroll(es, (INT)wParam, (INT)lParam);
        break;

#ifdef _WIN16
    case EM_SCROLLCARET16:
#endif
    case EM_SCROLLCARET:
        EDIT_EM_ScrollCaret(es);
        result = 1;
        break;

#ifdef _WIN16
    case EM_GETMODIFY16:
#endif
    case EM_GETMODIFY:
        result = ((es->flags & EF_MODIFIED) != 0);
        break;

#ifdef _WIN16
    case EM_SETMODIFY16:
#endif
    case EM_SETMODIFY:
        if (wParam)
            es->flags |= EF_MODIFIED;
        else
                        es->flags &= ~(EF_MODIFIED | EF_UPDATE);  /* reset pending updates */
        break;

#ifdef _WIN16
    case EM_GETLINECOUNT16:
#endif
    case EM_GETLINECOUNT:
        result = (es->style & ES_MULTILINE) ? es->line_count : 1;
        break;

#ifdef _WIN16
    case EM_LINEINDEX16:
        if ((INT16)wParam == -1)
            wParam = (WPARAM)-1;
        /* fall through */
#endif
    case EM_LINEINDEX:
        result = (LRESULT)EDIT_EM_LineIndex(es, (INT)wParam);
        break;

#ifdef _WIN16
    case EM_SETHANDLE16:
        EDIT_EM_SetHandle16(es, (HLOCAL16)wParam);
        break;
#endif
    case EM_SETHANDLE:
        EDIT_EM_SetHandle(es, *(HLOCAL *)&wParam);
        break;

#ifdef _WIN16
    case EM_GETHANDLE16:
        result = (LRESULT)EDIT_EM_GetHandle16(es);
        break;
#endif
    case EM_GETHANDLE:
        result = (LRESULT)(HANDLE_PTR)EDIT_EM_GetHandle(es);
        break;

#ifdef _WIN16
    case EM_GETTHUMB16:
#endif
    case EM_GETTHUMB:
        result = EDIT_EM_GetThumb(es);
        break;

    case EM_SETPAINTHOOK:
        result = GetWindowLongPtrW (hwnd, GWLEC_PAINTHOOK);
        SetWindowLongPtrW (hwnd, GWLEC_PAINTHOOK, (long) lParam);
        break;

    /* these messages missing from specs */
    case WM_USER+15:
////case 0x00bf:
    case WM_USER+16:
    case 0x00c0:
    case WM_USER+19:
    case 0x00c3:
    case WM_USER+26:
    case 0x00ca:
//      FIXME("undocumented message 0x%x, please report\n", msg);
        result = DefWindowProcW(hwnd, msg, wParam, lParam);
        break;

#ifdef _WIN16
    case EM_LINELENGTH16:
#endif
    case EM_LINELENGTH:
        result = (LRESULT)EDIT_EM_LineLength(es, (INT)wParam);
        break;

#ifdef _WIN16
    case EM_REPLACESEL16:
        lParam = (LPARAM)MapSL(lParam);
        unicode = FALSE;  /* 16-bit message is always ascii */
        /* fall through */
#endif
    case EM_REPLACESEL:
    {
        LPWSTR textW;

        if(unicode)
            textW = *(LPWSTR *)&lParam;
        else
        {
            LPSTR textA = *(LPSTR *)&lParam;
            INT countW = MultiByteToWideChar(CP_ACP, 0, textA, -1, NULL, 0);
            if((textW = HeapAlloc(GetProcessHeap(), 0, countW * sizeof(WCHAR))))
            MultiByteToWideChar(CP_ACP, 0, textA, -1, textW, countW);
        }

        EDIT_EM_ReplaceSel(es, (BOOL)wParam, textW, TRUE, TRUE);
        result = 1;

        if(!unicode)
            HeapFree(GetProcessHeap(), 0, textW);
        break;
    }

#ifdef _WIN16
    case EM_GETLINE16:
        lParam = (LPARAM)MapSL(lParam);
        unicode = FALSE;  /* 16-bit message is always ascii */
        /* fall through */
#endif
    case EM_GETLINE:
        result = (LRESULT)EDIT_EM_GetLine(es, (INT)wParam, *(LPWSTR *)&lParam, unicode);
        break;

#ifdef _WIN16
    case EM_LIMITTEXT16:
#endif
    case EM_SETLIMITTEXT:
        EDIT_EM_SetLimitText(es, (INT)wParam);
        break;

#ifdef _WIN16
    case EM_CANUNDO16:
#endif
    case EM_CANUNDO:
        result = (LRESULT)EDIT_EM_CanUndo(es);
        break;

#ifdef _WIN16
    case EM_UNDO16:
#endif
    case EM_UNDO:
    case WM_UNDO:
        result = (LRESULT)EDIT_EM_Undo(es);
        break;

    case MYWM_REDO:
        result = 0;

        break;

#ifdef _WIN16
    case EM_FMTLINES16:
#endif
    case EM_FMTLINES:
        result = (LRESULT)EDIT_EM_FmtLines(es, (BOOL)wParam);
        break;

#ifdef _WIN16
    case EM_LINEFROMCHAR16:
#endif
    case EM_LINEFROMCHAR:
        result = (LRESULT)EDIT_EM_LineFromChar(es, (INT)wParam);
        break;

#ifdef _WIN16
    case EM_SETTABSTOPS16:
        result = (LRESULT)EDIT_EM_SetTabStops16(es, (INT)wParam, MapSL(lParam));
        break;
#endif
    case EM_SETTABSTOPS:
        result = (LRESULT)EDIT_EM_SetTabStops(es, (INT)wParam, *(LPINT *)&lParam);
        break;

#ifdef _WIN16
    case EM_SETPASSWORDCHAR16:
        unicode = FALSE;  /* 16-bit message is always ascii */
        /* fall through */
#endif
    case EM_SETPASSWORDCHAR:
    {
        WCHAR charW = 0;

        if(unicode)
            charW = (WCHAR)wParam;
        else
        {
            CHAR charA = wParam;
            MultiByteToWideChar(CP_ACP, 0, &charA, 1, &charW, 1);
        }

        EDIT_EM_SetPasswordChar(es, charW);
        break;
    }

#ifdef _WIN16
    case EM_EMPTYUNDOBUFFER16:
#endif
    case EM_EMPTYUNDOBUFFER:
        EDIT_EM_EmptyUndoBuffer(es);
        break;

#ifdef _WIN16
    case EM_GETFIRSTVISIBLELINE16:
        result = es->y_offset;
        break;
#endif
    case EM_GETFIRSTVISIBLELINE:
        result = (es->style & ES_MULTILINE) ? es->y_offset : es->x_offset;
        break;

#ifdef _WIN16
    case EM_SETREADONLY16:
#endif
    case EM_SETREADONLY:
        if (wParam) {
                    SetWindowLongW( hwnd, GWL_STYLE,
                                    GetWindowLongW( hwnd, GWL_STYLE ) | ES_READONLY );
                    es->style |= ES_READONLY;
        } else {
                    SetWindowLongW( hwnd, GWL_STYLE,
                                    GetWindowLongW( hwnd, GWL_STYLE ) & ~ES_READONLY );
                    es->style &= ~ES_READONLY;
        }
                result = 1;
        break;

#ifdef _WIN16
    case EM_SETWORDBREAKPROC16:
        EDIT_EM_SetWordBreakProc16(es, (EDITWORDBREAKPROC16)lParam);
        break;
#endif
    case EM_SETWORDBREAKPROC:
        EDIT_EM_SetWordBreakProc(es, *(void **)&lParam);
        break;

#ifdef _WIN16
    case EM_GETWORDBREAKPROC16:
        result = (LRESULT)es->word_break_proc16;
        break;
#endif
    case EM_GETWORDBREAKPROC:
        result = *(LRESULT *)&es->word_break_proc;
        break;

#ifdef _WIN16
    case EM_GETPASSWORDCHAR16:
        unicode = FALSE;  /* 16-bit message is always ascii */
        /* fall through */
#endif
    case EM_GETPASSWORDCHAR:
    {
        if(unicode)
            result = es->password_char;
        else
        {
            WCHAR charW = es->password_char;
            CHAR charA = 0;
            WideCharToMultiByte(CP_ACP, 0, &charW, 1, &charA, 1, NULL, NULL);
            result = charA;
        }
        break;
    }

    /* The following EM_xxx are new to win95 and don't exist for 16 bit */

    case EM_SETMARGINS:
        EDIT_EM_SetMargins(es, (INT)wParam, LOWORD(lParam), HIWORD(lParam), TRUE);
        break;

    case EM_GETMARGINS:
        result = MAKELONG(es->left_margin, es->right_margin);
        break;

    case EM_GETLIMITTEXT:
        result = es->buffer_limit;
        break;

    case EM_POSFROMCHAR:
        if ((INT)wParam >= get_text_length(es)) result = -1;
        else result = EDIT_EM_PosFromChar(es, (INT)wParam, FALSE);
        break;

    case EM_CHARFROMPOS:
        result = EDIT_EM_CharFromPos(es, (short)LOWORD(lParam), (short)HIWORD(lParam));
        break;

        /* End of the EM_ messages which were in numerical order; what order
         * are these in?  vaguely alphabetical?
         */

    case WM_NCCREATE:
        result = EDIT_WM_NCCreate(hwnd, *(LPCREATESTRUCTW *)&lParam, unicode);
        break;

    case WM_DESTROY:
        result = EDIT_WM_Destroy(es);
        es = NULL;
        break;

    case WM_GETDLGCODE:
        result = DLGC_HASSETSEL | DLGC_WANTCHARS | DLGC_WANTARROWS;

        if (es->style & ES_MULTILINE)
        {
           result |= DLGC_WANTALLKEYS;
           break;
        }

        if (lParam && ((*(LPMSG *)&lParam)->message == WM_KEYDOWN))
        {
           int vk = (int)(*(LPMSG *)&lParam)->wParam;

                   if (es->hwndListBox)
           {
                       if (vk == VK_RETURN || vk == VK_ESCAPE)
              if (SendMessageW(GetParent(hwnd), CB_GETDROPPEDSTATE, 0, 0))
                 result |= DLGC_WANTMESSAGE;
           }
                   else
                   {
                       switch (vk)
                       {
                           case VK_ESCAPE:
                               SendMessageW(GetParent(hwnd), WM_CLOSE, 0, 0);
                               break;
                           default:
                               break;
                       }
                   }
        }
        break;

#if USE_IME
        case WM_IME_CHAR:
            if (!unicode)
            {
                WCHAR charW;
                CHAR  strng[2];

                strng[0] = wParam >> 8;
                strng[1] = wParam & 0xff;
                if (strng[0]) MultiByteToWideChar(CP_ACP, 0, strng, 2, &charW, 1);
                else MultiByteToWideChar(CP_ACP, 0, &strng[1], 1, &charW, 1);
        result = EDIT_WM_Char(es, charW);
        break;
            }
            /* fall through */
#endif
    case WM_CHAR:
    {
        WCHAR charW;

        if(unicode)
            charW = wParam;
        else
        {
            CHAR charA = wParam;
            MultiByteToWideChar(CP_ACP, 0, &charA, 1, &charW, 1);
        }

                if (es->hwndListBox)
                {
                    if (charW == VK_RETURN || charW == VK_ESCAPE)
                    {
                        if (SendMessageW(GetParent(hwnd), CB_GETDROPPEDSTATE, 0, 0))
                            SendMessageW(GetParent(hwnd), WM_KEYDOWN, charW, 0);
                        break;
                    }
                }
        result = EDIT_WM_Char(es, charW);
        break;
    }

    case WM_CLEAR:
        EDIT_WM_Clear(es);
        break;

    case WM_COMMAND:
        EDIT_WM_Command(es, HIWORD(wParam), LOWORD(wParam), *(HWND *)&lParam);
        break;

    case WM_CONTEXTMENU:
        EDIT_WM_ContextMenu(es, (short)LOWORD(lParam), (short)HIWORD(lParam));
        break;

    case WM_COPY:
        EDIT_WM_Copy(es);
        break;

    case WM_CREATE:
        if(unicode)
            result = EDIT_WM_Create(es, (*(LPCREATESTRUCTW *)&lParam)->lpszName);
        else
        {
            LPCSTR nameA = (*(LPCREATESTRUCTA *)&lParam)->lpszName;
            LPWSTR nameW = NULL;
            if(nameA)
            {
            INT countW = MultiByteToWideChar(CP_ACP, 0, nameA, -1, NULL, 0);
            if((nameW = HeapAlloc(GetProcessHeap(), 0, countW * sizeof(WCHAR))))
                MultiByteToWideChar(CP_ACP, 0, nameA, -1, nameW, countW);
            }
            result = EDIT_WM_Create(es, nameW);
                    HeapFree(GetProcessHeap(), 0, nameW);
        }
        break;

    case WM_CUT:
        EDIT_WM_Cut(es);
        break;

    case WM_ENABLE:
        es->bEnableState = (BOOL) wParam;
        EDIT_UpdateText(es, NULL, TRUE);
        break;

    case WM_ERASEBKGND:
        /* we do the proper erase in EDIT_WM_Paint */
        result = 1;
        break;

    case WM_GETFONT:
        result = *(LRESULT *)&es->font;
        break;

    case WM_GETTEXT:
        result = (LRESULT)EDIT_WM_GetText(es, (INT)wParam, *(LPWSTR *)&lParam, unicode);
        break;

    case WM_GETTEXTLENGTH:
                if (unicode) result = get_text_length(es);
                else result = WideCharToMultiByte( CP_ACP, 0, es->text, get_text_length(es),
                                                   NULL, 0, NULL, NULL );
        break;

    case WM_HSCROLL:
        result = EDIT_WM_HScroll(es, LOWORD(wParam), (short)HIWORD(wParam));
        break;

    case WM_KEYDOWN:
        result = EDIT_WM_KeyDown(es, (INT)wParam);
        break;

    case WM_KILLFOCUS:
        result = EDIT_WM_KillFocus(es);
        break;

    case WM_LBUTTONDBLCLK:
        result = EDIT_WM_LButtonDblClk(es);
        break;

    case WM_LBUTTONDOWN:
        result = EDIT_WM_LButtonDown(es, wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
        break;

    case WM_LBUTTONUP:
        result = EDIT_WM_LButtonUp(es);
        break;

    case WM_MBUTTONDOWN:
        result = EDIT_WM_MButtonDown(es);
        break;

    case WM_MOUSEMOVE:
        result = EDIT_WM_MouseMove(es, (short)LOWORD(lParam), (short)HIWORD(lParam));
        break;

    case WM_PAINT:
        lParam = 0;
    case WM_PRINTCLIENT:
            EDIT_WM_Paint(es, *(HDC *)&wParam, (long) lParam);
        break;

    case WM_PASTE:
        EDIT_WM_Paste(es);
        break;

    case WM_SETFOCUS:
        EDIT_WM_SetFocus(es);
        break;

    case WM_SETFONT:
        EDIT_WM_SetFont(es, *(HFONT *)&wParam, LOWORD(lParam) != 0);
        break;

    case WM_SETREDRAW:
        /* FIXME: actually set an internal flag and behave accordingly */
        break;

    case WM_SETTEXT:
        EDIT_WM_SetText(es, *(LPCWSTR *)&lParam, unicode);
        result = TRUE;
        break;

    case WM_SIZE:
        EDIT_WM_Size(es, (UINT)wParam, LOWORD(lParam), HIWORD(lParam));
        break;

        case WM_STYLECHANGED:
                result = EDIT_WM_StyleChanged(es, wParam, *(const STYLESTRUCT **)&lParam);
                break;

        case WM_STYLECHANGING:
                result = 0; /* See EDIT_WM_StyleChanged */
                break;

    case WM_SYSKEYDOWN:
        result = EDIT_WM_SysKeyDown(es, (INT)wParam, (DWORD)lParam);
        break;

    case WM_TIMER:
        EDIT_WM_Timer(es);
        break;

    case WM_VSCROLL:
        result = EDIT_WM_VScroll(es, LOWORD(wParam), (short)HIWORD(wParam));
        break;

        case WM_MOUSEWHEEL:
                {
                    int gcWheelDelta = 0;
                    UINT pulScrollLines = 3;
                    SystemParametersInfoW(SPI_GETWHEELSCROLLLINES,0, &pulScrollLines, 0);

                    if (wParam & (MK_SHIFT | MK_CONTROL)) {
                        result = DefWindowProcW(hwnd, msg, wParam, lParam);
                        break;
                    }
                    gcWheelDelta -= GET_WHEEL_DELTA_WPARAM(wParam);
                    if (abs(gcWheelDelta) >= WHEEL_DELTA && pulScrollLines)
                    {
                        int cLineScroll= (int) min((UINT) es->line_count, pulScrollLines);
                        cLineScroll *= (gcWheelDelta / WHEEL_DELTA);
            result = EDIT_EM_LineScroll(es, 0, cLineScroll);
                    }
                }
                break;


#if USE_IME
    /* IME messages to make the edit control IME aware */
    case WM_IME_SETCONTEXT:
        break;

    case WM_IME_STARTCOMPOSITION:
        es->composition_start = es->selection_end;
        es->composition_len = 0;
        break;

    case WM_IME_COMPOSITION:
                EDIT_ImeComposition(hwnd, lParam, es);
        break;

    case WM_IME_ENDCOMPOSITION:
                if (es->composition_len > 0)
            {
                static const WCHAR empty_stringW[] = {0};
                EDIT_EM_ReplaceSel(es, TRUE, empty_stringW, TRUE, TRUE);
                        es->selection_end = es->selection_start;
                        es->composition_len= 0;
            }
        break;

    case WM_IME_COMPOSITIONFULL:
        break;

    case WM_IME_SELECT:
        break;

    case WM_IME_CONTROL:
        break;
#endif

    default:
        result = DefWindowProcT(hwnd, msg, wParam, lParam, unicode);
        break;
    }

    if (es) EDIT_UnlockBuffer(es, FALSE);

//      TRACE("hwnd=%p msg=%x (%s) -- 0x%08lx\n", hwnd, msg, SPY_GetMsgName(msg, hwnd), result);

    return result;
} // End EditWndProc_common


/*********************************************************************
 *
 *  EditWndProcW   (USER32.@)
 */
LRESULT WINAPI EditWndProcW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
////LCLODSAPI ("EC: ", hWnd, uMsg, wParam, lParam);
    return EditWndProc_common(hWnd, uMsg, wParam, lParam, TRUE);
} // End EditWndProcW


/*********************************************************************
 *
 *  EditWndProc   (USER32.@)
 */
LRESULT WINAPI EditWndProcA(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return EditWndProc_common(hWnd, uMsg, wParam, lParam, FALSE);
} // End EditWndProcA


/*********************************************************************
 *
 *  EDIT_BuildLineDefs_ML
 *
 *  Build linked list of text lines.
 *  Lines can end with '\0' (last line), a character (if it is wrapped),
 *  a soft return '\r\r\n' or a hard return '\r\n'
 *
 */
static void EDIT_BuildLineDefs_ML(EDITSTATE *es, INT istart, INT iend, INT delta, HRGN hrgn)
{
    HDC dc;
    HFONT old_font = 0;
    LPWSTR current_position, cp;
    INT fw;
    LINEDEF *current_line;
    LINEDEF *previous_line;
    LINEDEF *start_line;
    INT line_index = 0, nstart_line = 0, nstart_index = 0;
    INT line_count = es->line_count;
    INT orig_net_length;
    RECT rc;

    if (istart == iend && delta == 0)
        return;

    dc = GetDC(es->hwndSelf);
    if (es->font)
        old_font = SelectObject(dc, es->font);

    previous_line = NULL;
    current_line = es->first_line_def;

    /* Find starting line. istart must lie inside an existing line or
     * at the end of buffer */
    do {
        if (istart < current_line->index + current_line->length ||
                current_line->ending == END_0)
            break;

        previous_line = current_line;
        current_line = current_line->next;
        line_index++;
    } while (current_line);

    if (!current_line) /* Error occurred start is not inside previous buffer */
    {
//      FIXME(" modification occurred outside buffer\n");
        ReleaseDC(es->hwndSelf, dc); dc = NULL;
        return;
    }

    /* Remember start of modifications in order to calculate update region */
    nstart_line = line_index;
    nstart_index = current_line->index;

    /* We must start to reformat from the previous line since the modifications
     * may have caused the line to wrap upwards. */
    if (!(es->style & ES_AUTOHSCROLL) && line_index > 0)
    {
        line_index--;
        current_line = previous_line;
    }
    start_line = current_line;

    fw = es->format_rect.right - es->format_rect.left;
    current_position = es->text + current_line->index;
    do {
        if (current_line != start_line)
        {
            if (!current_line || current_line->index + delta > current_position - es->text)
            {
                /* The buffer has been expanded, create a new line and
                   insert it into the link list */
                LINEDEF *new_line = HeapAlloc(GetProcessHeap(), 0, sizeof(LINEDEF));
                new_line->next = previous_line->next;
                previous_line->next = new_line;
                current_line = new_line;
                es->line_count++;
            }
            else if (current_line->index + delta < current_position - es->text)
            {
                /* The previous line merged with this line so we delete this extra entry */
                previous_line->next = current_line->next;
                HeapFree(GetProcessHeap(), 0, current_line);
                current_line = previous_line->next;
                es->line_count--;
                continue;
            }
            else /* current_line->index + delta == current_position */
            {
                if (current_position - es->text > iend)
                    break; /* We reached end of line modifications */
                /* else recalulate this line */
            }
        }

        current_line->index = (UINT) (current_position - es->text);
        orig_net_length = current_line->net_length;

        /* Find end of line */
        cp = current_position;
        while (*cp) {
                    if (*cp == '\n') break;
            if ((*cp == '\r') && (*(cp + 1) == '\n'))
                break;
            cp++;
        }

        /* Mark type of line termination */
        if (!(*cp)) {
            current_line->ending = END_0;
            current_line->net_length = strlenW(current_position);
        } else if ((cp > current_position) && (*(cp - 1) == '\r')) {
            current_line->ending = END_SOFT;
            current_line->net_length = (UINT) (cp - current_position - 1);
        } else if (*cp == '\n') {
            current_line->ending = END_RICH;
            current_line->net_length = (UINT) (cp - current_position);
        } else {
            current_line->ending = END_HARD;
            current_line->net_length = (UINT) (cp - current_position);
        }

        /* Calculate line width */
        current_line->width = (INT)LOWORD(GetTabbedTextExtentW(dc,
                    current_position, current_line->net_length,
                    es->tabs_count, es->tabs));

        /* FIXME: check here for lines that are too wide even in AUTOHSCROLL (> 32767 ???) */
        if (!(es->style & ES_AUTOHSCROLL)) {
           if (current_line->width > fw) {
            INT next = 0;
            INT prev;
            do {
                prev = next;
                next = EDIT_CallWordBreakProc(es, (UINT) (current_position - es->text),
                        prev + 1, current_line->net_length, WB_RIGHT);
                current_line->width = (INT)LOWORD(GetTabbedTextExtentW(dc,
                            current_position, next, es->tabs_count, es->tabs));
            } while (current_line->width <= fw);
            if (!prev) { /* Didn't find a line break so force a break */
                next = 0;
                do {
                    prev = next;
                    next++;
                    current_line->width = (INT)LOWORD(GetTabbedTextExtentW(dc,
                                current_position, next, es->tabs_count, es->tabs));
                } while (current_line->width <= fw);
                if (!prev)
                    prev = 1;
            }

            /* If the first line we are calculating, wrapped before istart, we must
             * adjust istart in order for this to be reflected in the update region. */
            if (current_line->index == nstart_index && istart > current_line->index + prev)
                istart = current_line->index + prev;
            /* else if we are updating the previous line before the first line we
             * are re-calculating and it expanded */
            else if (current_line == start_line &&
                    current_line->index != nstart_index && orig_net_length < prev)
            {
              /* Line expanded due to an upwards line wrap so we must partially include
               * previous line in update region */
                nstart_line = line_index;
                nstart_index = current_line->index;
                istart = current_line->index + orig_net_length;
            }

            current_line->net_length = prev;
            current_line->ending = END_WRAP;
            current_line->width = (INT)LOWORD(GetTabbedTextExtentW(dc, current_position,
                    current_line->net_length, es->tabs_count, es->tabs));
            }
            else if (current_line == start_line &&
                             current_line->index != nstart_index &&
                             orig_net_length < current_line->net_length) {
            /* The previous line expanded but it's still not as wide as the client rect */
            /* The expansion is due to an upwards line wrap so we must partially include
               it in the update region */
            nstart_line = line_index;
            nstart_index = current_line->index;
            istart = current_line->index + orig_net_length;
            }
        }


        /* Adjust length to include line termination */
        switch (current_line->ending) {
        case END_SOFT:
            current_line->length = current_line->net_length + 3;
            break;
        case END_RICH:
            current_line->length = current_line->net_length + 1;
            break;
        case END_HARD:
            current_line->length = current_line->net_length + 2;
            break;
        case END_WRAP:
        case END_0:
            current_line->length = current_line->net_length;
            break;
        }
        es->text_width = max(es->text_width, current_line->width);
        current_position += current_line->length;
        previous_line = current_line;
        current_line = current_line->next;
        line_index++;
    } while (previous_line->ending != END_0);

    /* Finish adjusting line indexes by delta or remove hanging lines */
    if (previous_line->ending == END_0)
    {
        LINEDEF *pnext = NULL;

        previous_line->next = NULL;
        while (current_line)
        {
            pnext = current_line->next;
            HeapFree(GetProcessHeap(), 0, current_line);
            current_line = pnext;
            es->line_count--;
        }
    }
    else if (delta != 0)
    {
        while (current_line)
        {
            current_line->index += delta;
            current_line = current_line->next;
        }
    }

    /* Calculate rest of modification rectangle */
    if (hrgn)
    {
        HRGN tmphrgn;
       /*
        * We calculate two rectangles. One for the first line which may have
        * an indent with respect to the format rect. The other is a format-width
        * rectangle that spans the rest of the lines that changed or moved.
        */
        rc.top = es->format_rect.top + nstart_line * es->line_height -
            (es->y_offset * es->line_height); /* Adjust for vertical scrollbar */
        rc.bottom = rc.top + es->line_height;
        if ((es->style & ES_CENTER) || (es->style & ES_RIGHT))
            rc.left = es->format_rect.left;
        else
            rc.left = es->format_rect.left + (INT)LOWORD(GetTabbedTextExtentW(dc,
                    es->text + nstart_index, istart - nstart_index,
                    es->tabs_count, es->tabs)) - es->x_offset; /* Adjust for horz scroll */
        rc.right = es->format_rect.right;
        SetRectRgn(hrgn, rc.left, rc.top, rc.right, rc.bottom);

        rc.top = rc.bottom;
        rc.left = es->format_rect.left;
        rc.right = es->format_rect.right;
       /*
        * If lines were added or removed we must re-paint the remainder of the
        * lines since the remaining lines were either shifted up or down.
        */
        if (line_count < es->line_count) /* We added lines */
            rc.bottom = es->line_count * es->line_height;
        else if (line_count > es->line_count) /* We removed lines */
            rc.bottom = line_count * es->line_height;
        else
            rc.bottom = line_index * es->line_height;
        rc.bottom += es->format_rect.top;
        rc.bottom -= (es->y_offset * es->line_height); /* Adjust for vertical scrollbar */
        tmphrgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
        CombineRgn(hrgn, hrgn, tmphrgn, RGN_OR);
        DeleteObject(tmphrgn);
    }

    if (es->font)
        SelectObject(dc, old_font);

    ReleaseDC(es->hwndSelf, dc); dc = NULL;
} // End EDIT_BuildLineDefs_ML


/*********************************************************************
 *
 *  EDIT_CalcLineWidth_SL
 *
 */
static void EDIT_CalcLineWidth_SL(EDITSTATE *es)
{
    SIZE size;
    LPWSTR text;
    HDC dc;
    HFONT old_font = 0;

    text = EDIT_GetPasswordPointer_SL(es);

    dc = GetDC(es->hwndSelf);
    if (es->font)
        old_font = SelectObject(dc, es->font);

    GetTextExtentPoint32W(dc, text, strlenW(text), &size);

    if (es->font)
        SelectObject(dc, old_font);
    ReleaseDC(es->hwndSelf, dc); dc = NULL;

    if (es->style & ES_PASSWORD)
        HeapFree(GetProcessHeap(), 0, text);

    es->text_width = size.cx;
} // End EDIT_CalcLineWidth_SL


/*********************************************************************
 *
 *  EDIT_CallWordBreakProc
 *
 *  Call appropriate WordBreakProc (internal or external).
 *
 *  Note: The "start" argument should always be an index referring
 *      to es->text.  The actual wordbreak proc might be
 *      16 bit, so we can't always pass any 32 bit LPSTR.
 *      Hence we assume that es->text is the buffer that holds
 *      the string under examination (we can decide this for ourselves).
 *
 */
static INT EDIT_CallWordBreakProc(EDITSTATE *es, INT start, INT index, INT count, INT action)
{
    INT ret;

#ifdef _WIN16
    if (es->word_break_proc16) {
        HGLOBAL16 hglob16;
        SEGPTR segptr;
        INT countA;
            WORD args[5];
            DWORD result;

        countA = WideCharToMultiByte(CP_ACP, 0, es->text + start, count, NULL, 0, NULL, NULL);
        hglob16 = GlobalAlloc16(GMEM_MOVEABLE | GMEM_ZEROINIT, countA);
        segptr = WOWGlobalLock16(hglob16);
        WideCharToMultiByte(CP_ACP, 0, es->text + start, count, MapSL(segptr), countA, NULL, NULL);
            args[4] = SELECTOROF(segptr);
            args[3] = OFFSETOF(segptr);
            args[2] = index;
            args[1] = countA;
            args[0] = action;
            WOWCallback16Ex((DWORD)es->word_break_proc16, WCB16_PASCAL, sizeof(args), args, &result);
            ret = LOWORD(result);
        GlobalUnlock16(hglob16);
        GlobalFree16(hglob16);
    } else
#endif
     if (es->word_break_proc)
        {
        if(es->is_unicode)
        {
        EDITWORDBREAKPROCW wbpW = (EDITWORDBREAKPROCW)es->word_break_proc;

//      TRACE_(relay)("(UNICODE wordbrk=%p,str=%s,idx=%d,cnt=%d,act=%d)\n",
//          es->word_break_proc, debugstr_wn(es->text + start, count), index, count, action);
        ret = wbpW(es->text + start, index, count, action);
        }
        else
        {
        EDITWORDBREAKPROCA wbpA = (EDITWORDBREAKPROCA)es->word_break_proc;
        INT countA;
        CHAR *textA;

        countA = WideCharToMultiByte(CP_ACP, 0, es->text + start, count, NULL, 0, NULL, NULL);
        textA = HeapAlloc(GetProcessHeap(), 0, countA);
        WideCharToMultiByte(CP_ACP, 0, es->text + start, count, textA, countA, NULL, NULL);
//      TRACE_(relay)("(ANSI wordbrk=%p,str=%s,idx=%d,cnt=%d,act=%d)\n",
//          es->word_break_proc, debugstr_an(textA, countA), index, countA, action);
        ret = wbpA(textA, index, countA, action);
        HeapFree(GetProcessHeap(), 0, textA);
        }
        }
    else
            ret = EDIT_WordBreakProc(es->text + start, index, count, action);

    return ret;
} // End EDIT_CallWordBreakProc


/*********************************************************************
 *
 *  EDIT_CharFromPos
 *
 *  Beware: This is not the function called on EM_CHARFROMPOS
 *      The position _can_ be outside the formatting / client
 *      rectangle
 *      The return value is only the character index
 *
 */
static INT EDIT_CharFromPos(EDITSTATE *es, INT x, INT y, LPBOOL after_wrap)
{
    INT index;
    HDC dc;
    HFONT old_font = 0;
    INT x_high = 0, x_low = 0;

    if (es->style & ES_MULTILINE) {
        INT line = (y - es->format_rect.top) / es->line_height + es->y_offset;
        INT line_index = 0;
        LINEDEF *line_def = es->first_line_def;
        INT low, high;
        while ((line > 0) && line_def->next) {
            line_index += line_def->length;
            line_def = line_def->next;
            line--;
        }
        x += es->x_offset - es->format_rect.left;
        if (es->style & ES_RIGHT)
            x -= (es->format_rect.right - es->format_rect.left) - line_def->width;
        else if (es->style & ES_CENTER)
            x -= ((es->format_rect.right - es->format_rect.left) - line_def->width) / 2;
        if (x >= line_def->width) {
            if (after_wrap)
                *after_wrap = (line_def->ending == END_WRAP);
            return line_index + line_def->net_length;
        }
        if (x <= 0) {
            if (after_wrap)
                *after_wrap = FALSE;
            return line_index;
        }
        dc = GetDC(es->hwndSelf);
        if (es->font)
            old_font = SelectObject(dc, es->font);
                    low = line_index;
                    high = line_index + line_def->net_length + 1;
                    while (low < high - 1)
                    {
                        INT mid = (low + high) / 2;
                        INT x_now = LOWORD(GetTabbedTextExtentW(dc, es->text + line_index, mid - line_index, es->tabs_count, es->tabs));
                        if (x_now > x) {
                            high = mid;
                            x_high = x_now;
                        } else {
                            low = mid;
                            x_low = x_now;
                        }
                    }
                    if (abs(x_high - x) + 1 <= abs(x_low - x))
                        index = high;
                    else
                        index = low;

        if (after_wrap)
            *after_wrap = ((index == line_index + line_def->net_length) &&
                            (line_def->ending == END_WRAP));
    } else {
        LPWSTR text;
        SIZE size;
        if (after_wrap)
            *after_wrap = FALSE;
        x -= es->format_rect.left;
        if (!x)
            return es->x_offset;

        if (!es->x_offset)
        {
            INT indent = (es->format_rect.right - es->format_rect.left) - es->text_width;
            if (es->style & ES_RIGHT)
                x -= indent;
            else if (es->style & ES_CENTER)
                x -= indent / 2;
        }

        text = EDIT_GetPasswordPointer_SL(es);
        dc = GetDC(es->hwndSelf);
        if (es->font)
            old_font = SelectObject(dc, es->font);
        if (x < 0)
                {
                    INT low = 0;
                    INT high = es->x_offset;
                    while (low < high - 1)
                    {
                        INT mid = (low + high) / 2;
                        GetTextExtentPoint32W( dc, text + mid,
                                               es->x_offset - mid, &size );
                        if (size.cx > -x) {
                            low = mid;
                            x_low = size.cx;
                        } else {
                            high = mid;
                            x_high = size.cx;
                        }
                    }
                    if (abs(x_high + x) <= abs(x_low + x) + 1)
                        index = high;
                    else
                        index = low;
        }
                else
                {
                    INT low = es->x_offset;
                    INT high = get_text_length(es) + 1;
                    while (low < high - 1)
                    {
                        INT mid = (low + high) / 2;
                        GetTextExtentPoint32W( dc, text + es->x_offset,
                                               mid - es->x_offset, &size );
                        if (size.cx > x) {
                               high = mid;
                               x_high = size.cx;
                        } else {
                               low = mid;
                               x_low = size.cx;
                       }
                    }
                   if (abs(x_high - x) <= abs(x_low - x) + 1)
                       index = high;
                   else
                       index = low;
        }
        if (es->style & ES_PASSWORD)
            HeapFree(GetProcessHeap(), 0, text);
    }
    if (es->font)
        SelectObject(dc, old_font);
    ReleaseDC(es->hwndSelf, dc); dc = NULL;
    return index;
} // End EDIT_CharFromPos


/*********************************************************************
 *
 *  EDIT_ConfinePoint
 *
 *  adjusts the point to be within the formatting rectangle
 *  (so CharFromPos returns the nearest _visible_ character)
 *
 */
static void EDIT_ConfinePoint(EDITSTATE *es, LPINT x, LPINT y)
{
    *x = min(max(*x, es->format_rect.left), es->format_rect.right - 1);
    *y = min(max(*y, es->format_rect.top), es->format_rect.bottom - 1);
} // End EDIT_ConfinePoint


/*********************************************************************
 *
 *  EDIT_GetLineRect
 *
 *  Calculates the bounding rectangle for a line from a starting
 *  column to an ending column.
 *
 */
static void EDIT_GetLineRect(EDITSTATE *es, INT line, INT scol, INT ecol, LPRECT rc)
{
    INT line_index =  EDIT_EM_LineIndex(es, line);

    if (es->style & ES_MULTILINE)
        rc->top = es->format_rect.top + (line - es->y_offset) * es->line_height;
    else
        rc->top = es->format_rect.top;
    rc->bottom = rc->top + es->line_height;
    rc->left = (scol == 0) ? es->format_rect.left : (short)LOWORD(EDIT_EM_PosFromChar(es, line_index + scol, TRUE));
    rc->right = (ecol == -1) ? es->format_rect.right : (short)LOWORD(EDIT_EM_PosFromChar(es, line_index + ecol, TRUE));
} // End EDIT_GetLineRect


/*********************************************************************
 *
 *  EDIT_GetPasswordPointer_SL
 *
 *  note: caller should free the (optionally) allocated buffer
 *
 */
static LPWSTR EDIT_GetPasswordPointer_SL(EDITSTATE *es)
{
    if (es->style & ES_PASSWORD) {
        INT len = get_text_length(es);
        LPWSTR text = HeapAlloc(GetProcessHeap(), 0, (len + 1) * sizeof(WCHAR));
        text[len] = '\0';
        while(len) text[--len] = es->password_char;
        return text;
    } else
        return es->text;
} // End EDIT_GetPasswordPointer_SL


/*********************************************************************
 *
 *  EDIT_LockBuffer
 *
 *  This acts as a LocalLock16(), but it locks only once.  This way
 *  you can call it whenever you like, without unlocking.
 *
 *  Initially the edit control allocates a HLOCAL32 buffer
 *  (32 bit linear memory handler).  However, 16 bit application
 *  might send an EM_GETHANDLE message and expect a HLOCAL16 (16 bit SEG:OFF
 *  handler).  From that moment on we have to keep using this 16 bit memory
 *  handler, because it is supposed to be valid at all times after EM_GETHANDLE.
 *  What we do is create a HLOCAL16 buffer, copy the text, and do pointer
 *  conversion.
 *
 */
static void EDIT_LockBuffer(EDITSTATE *es)
{
#ifdef _WIN16
    STACK16FRAME* stack16 = MapSL(PtrToUlong(NtCurrentTeb()->WOW32Reserved));
    HINSTANCE16 hInstance = GetWindowLongPtrW( es->hwndSelf, GWLP_HINSTANCE );
#endif

    if (!es->text) {
        CHAR *textA = NULL;
        UINT countA = 0;
#ifdef _WIN16
        BOOL _16bit = FALSE;
#endif

        if(es->hloc32W)
        {
        if(es->hloc32A)
        {
//          TRACE("Synchronizing with 32-bit ANSI buffer\n");
            textA = LocalLock(es->hloc32A);
            countA = (UINT) strlen(textA) + 1;
        }
#ifdef _WIN16
        else if(es->hloc16)
        {
            HANDLE16 oldDS = stack16->ds;
//          TRACE("Synchronizing with 16-bit ANSI buffer\n");
            stack16->ds = hInstance;
            textA = MapSL(LocalLock16(es->hloc16));
            stack16->ds = oldDS;
            countA = strlen(textA) + 1;
            _16bit = TRUE;
        }
#endif
        }
        else {
//      ERR("no buffer ... please report\n");
        return;
        }

        if(textA)
        {
        HLOCAL hloc32W_new;
        UINT countW_new = MultiByteToWideChar(CP_ACP, 0, textA, countA, NULL, 0);
//      TRACE("%d bytes translated to %d WCHARs\n", countA, countW_new);
        if(countW_new > es->buffer_size + 1)
        {
            UINT alloc_size = ROUND_TO_GROW(countW_new * sizeof(WCHAR));
//          TRACE("Resizing 32-bit UNICODE buffer from %d+1 to %d WCHARs\n", es->buffer_size, countW_new);
            hloc32W_new = LocalReAlloc(es->hloc32W, alloc_size, LMEM_MOVEABLE | LMEM_ZEROINIT);
            if(hloc32W_new)
            {
            es->hloc32W = hloc32W_new;
            es->buffer_size = LocalSize(hloc32W_new)/sizeof(WCHAR) - 1;
//          TRACE("Real new size %d+1 WCHARs\n", es->buffer_size);
            }
//          else
//          WARN("FAILED! Will synchronize partially\n");
        }
        }

//      /*TRACE("Locking 32-bit UNICODE buffer\n");*/
        es->text = LocalLock(es->hloc32W);

        if(textA)
        {
        MultiByteToWideChar(CP_ACP, 0, textA, countA, es->text, es->buffer_size + 1);
#ifdef _WIN16
        if(_16bit)
        {
            HANDLE16 oldDS = stack16->ds;
            stack16->ds = hInstance;
            LocalUnlock16(es->hloc16);
            stack16->ds = oldDS;
        }
        else
#endif
            LocalUnlock(es->hloc32A);
        }
    }
        if(es->flags & EF_APP_HAS_HANDLE) text_buffer_changed(es);
    es->lock_count++;
} // End EDIT_LockBuffer


/*********************************************************************
 *
 *  EDIT_SL_InvalidateText
 *
 *  Called from EDIT_InvalidateText().
 *  Does the job for single-line controls only.
 *
 */
static void EDIT_SL_InvalidateText(EDITSTATE *es, INT start, INT end)
{
    RECT line_rect;
    RECT rc;

    EDIT_GetLineRect(es, 0, start, end, &line_rect);
    if (IntersectRect(&rc, &line_rect, &es->format_rect))
        EDIT_UpdateText(es, &rc, TRUE);
} // End EDIT_SL_InvalidateText


/*********************************************************************
 *
 *  EDIT_ML_InvalidateText
 *
 *  Called from EDIT_InvalidateText().
 *  Does the job for multi-line controls only.
 *
 */
static void EDIT_ML_InvalidateText(EDITSTATE *es, INT start, INT end)
{
    INT vlc = (es->format_rect.bottom - es->format_rect.top) / es->line_height;
    INT sl = EDIT_EM_LineFromChar(es, start);
    INT el = EDIT_EM_LineFromChar(es, end);
    INT sc;
    INT ec;
    RECT rc1;
    RECT rcWnd;
    RECT rcLine;
    RECT rcUpdate;
    INT l;

    if ((el < es->y_offset) || (sl > es->y_offset + vlc))
        return;

    sc = start - EDIT_EM_LineIndex(es, sl);
    ec = end - EDIT_EM_LineIndex(es, el);
    if (sl < es->y_offset) {
        sl = es->y_offset;
        sc = 0;
    }
    if (el > es->y_offset + vlc) {
        el = es->y_offset + vlc;
        ec = EDIT_EM_LineLength(es, EDIT_EM_LineIndex(es, el));
    }
    GetClientRect(es->hwndSelf, &rc1);
    IntersectRect(&rcWnd, &rc1, &es->format_rect);
    if (sl == el) {
        EDIT_GetLineRect(es, sl, sc, ec, &rcLine);
        if (IntersectRect(&rcUpdate, &rcWnd, &rcLine))
            EDIT_UpdateText(es, &rcUpdate, TRUE);
    } else {
        EDIT_GetLineRect(es, sl, sc,
                EDIT_EM_LineLength(es,
                    EDIT_EM_LineIndex(es, sl)),
                &rcLine);
        if (IntersectRect(&rcUpdate, &rcWnd, &rcLine))
            EDIT_UpdateText(es, &rcUpdate, TRUE);
        for (l = sl + 1 ; l < el ; l++) {
            EDIT_GetLineRect(es, l, 0,
                EDIT_EM_LineLength(es,
                    EDIT_EM_LineIndex(es, l)),
                &rcLine);
            if (IntersectRect(&rcUpdate, &rcWnd, &rcLine))
                EDIT_UpdateText(es, &rcUpdate, TRUE);
        }
        EDIT_GetLineRect(es, el, 0, ec, &rcLine);
        if (IntersectRect(&rcUpdate, &rcWnd, &rcLine))
            EDIT_UpdateText(es, &rcUpdate, TRUE);
    }
} // End EDIT_ML_InvalidateText


/*********************************************************************
 *
 *  EDIT_InvalidateText
 *
 *  Invalidate the text from offset start up to, but not including,
 *  offset end.  Useful for (re)painting the selection.
 *  Regions outside the linewidth are not invalidated.
 *  end == -1 means end == TextLength.
 *  start and end need not be ordered.
 *
 */
static void EDIT_InvalidateText(EDITSTATE *es, INT start, INT end)
{
    if (end == start)
        return;

    if (end == -1)
        end = get_text_length(es);

    if (end < start) {
        INT tmp = start;
        start = end;
        end = tmp;
    }

    if (es->style & ES_MULTILINE)
        EDIT_ML_InvalidateText(es, start, end);
    else
        EDIT_SL_InvalidateText(es, start, end);
} // End EDIT_InvalidateText


/*********************************************************************
 *
 *  EDIT_MakeFit
 *
 * Try to fit size + 1 characters in the buffer.
 */
static BOOL EDIT_MakeFit(EDITSTATE *es, UINT size)
{
    HLOCAL hNew32W;

    if (size <= es->buffer_size)
        return TRUE;

//  TRACE("trying to ReAlloc to %d+1 characters\n", size);

    /* Force edit to unlock it's buffer. es->text now NULL */
    EDIT_UnlockBuffer(es, TRUE);

    if (es->hloc32W) {
        UINT alloc_size = ROUND_TO_GROW((size + 1) * sizeof(WCHAR));
        if ((hNew32W = LocalReAlloc(es->hloc32W, alloc_size, LMEM_MOVEABLE | LMEM_ZEROINIT))) {
//      TRACE("Old 32 bit handle %p, new handle %p\n", es->hloc32W, hNew32W);
        es->hloc32W = hNew32W;
        es->buffer_size = LocalSize(hNew32W)/sizeof(WCHAR) - 1;
        }
    }

    EDIT_LockBuffer(es);

    if (es->buffer_size < size) {
//      WARN("FAILED !  We now have %d+1\n", es->buffer_size);
        EDIT_NOTIFY_PARENT(es, EN_ERRSPACE);
        return FALSE;
    } else {
//      TRACE("We now have %d+1\n", es->buffer_size);
        return TRUE;
    }
} // End EDIT_MakeFit


/*********************************************************************
 *
 *  EDIT_MakeUndoFit
 *
 *  Try to fit size + 1 bytes in the undo buffer.
 *
 */
static BOOL EDIT_MakeUndoFit(EDITSTATE *es, UINT size)
{
    UINT alloc_size;

    if (size <= es->undo_buffer_size)
        return TRUE;

//  TRACE("trying to ReAlloc to %d+1\n", size);

    alloc_size = ROUND_TO_GROW((size + 1) * sizeof(WCHAR));
    if ((es->undo_text = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, es->undo_text, alloc_size))) {
        es->undo_buffer_size = alloc_size/sizeof(WCHAR) - 1;
        return TRUE;
    }
    else
    {
//      WARN("FAILED !  We now have %d+1\n", es->undo_buffer_size);
        return FALSE;
    }
} // End EDIT_MakeUndoFit


/*********************************************************************
 *
 *  EDIT_MoveBackward
 *
 */
static void EDIT_MoveBackward(EDITSTATE *es, BOOL extend)
{
    INT e = es->selection_end;

    if (e) {
        e--;
        if ((es->style & ES_MULTILINE) && e &&
                (es->text[e - 1] == '\r') && (es->text[e] == '\n')) {
            e--;
            if (e && (es->text[e - 1] == '\r'))
                e--;
        }
    }
    EDIT_EM_SetSel(es, extend ? es->selection_start : e, e, FALSE);
    EDIT_EM_ScrollCaret(es);
} // End EDIT_MoveBackward


/*********************************************************************
 *
 *  EDIT_MoveDown_ML
 *
 *  Only for multi line controls
 *  Move the caret one line down, on a column with the nearest
 *  x coordinate on the screen (might be a different column).
 *
 */
static void EDIT_MoveDown_ML(EDITSTATE *es, BOOL extend)
{
    INT s = es->selection_start;
    INT e = es->selection_end;
    BOOL after_wrap = (es->flags & EF_AFTER_WRAP);
    LRESULT pos = EDIT_EM_PosFromChar(es, e, after_wrap);
    INT x = (short)LOWORD(pos);
    INT y = (short)HIWORD(pos);

    e = EDIT_CharFromPos(es, x, y + es->line_height, &after_wrap);
    if (!extend)
        s = e;
    EDIT_EM_SetSel(es, s, e, after_wrap);
    EDIT_EM_ScrollCaret(es);
} // End EDIT_MoveDown_ML


/*********************************************************************
 *
 *  EDIT_MoveEnd
 *
 */
static void EDIT_MoveEnd(EDITSTATE *es, BOOL extend, BOOL ctrl)
{
    BOOL after_wrap = FALSE;
    INT e;

    /* Pass a high value in x to make sure of receiving the end of the line */
    if (!ctrl && (es->style & ES_MULTILINE))
        e = EDIT_CharFromPos(es, 0x3fffffff,
            HIWORD(EDIT_EM_PosFromChar(es, es->selection_end, es->flags & EF_AFTER_WRAP)), &after_wrap);
    else
        e = get_text_length(es);
    EDIT_EM_SetSel(es, extend ? es->selection_start : e, e, after_wrap);
    EDIT_EM_ScrollCaret(es);
} // End EDIT_MoveEnd


/*********************************************************************
 *
 *  EDIT_MoveForward
 *
 */
static void EDIT_MoveForward(EDITSTATE *es, BOOL extend)
{
    INT e = es->selection_end;

    if (es->text[e]) {
        e++;
        if ((es->style & ES_MULTILINE) && (es->text[e - 1] == '\r')) {
            if (es->text[e] == '\n')
                e++;
            else if ((es->text[e] == '\r') && (es->text[e + 1] == '\n'))
                e += 2;
        }
    }
    EDIT_EM_SetSel(es, extend ? es->selection_start : e, e, FALSE);
    EDIT_EM_ScrollCaret(es);
} // End EDIT_MoveForward


/*********************************************************************
 *
 *  EDIT_MoveHome
 *
 *  Home key: move to beginning of line.
 *
 */
static void EDIT_MoveHome(EDITSTATE *es, BOOL extend, BOOL ctrl)
{
    INT e;

    /* Pass the x_offset in x to make sure of receiving the first position of the line */
    if (!ctrl && (es->style & ES_MULTILINE))
        e = EDIT_CharFromPos(es, -es->x_offset,
            HIWORD(EDIT_EM_PosFromChar(es, es->selection_end, es->flags & EF_AFTER_WRAP)), NULL);
    else
        e = 0;
    EDIT_EM_SetSel(es, extend ? es->selection_start : e, e, FALSE);
    EDIT_EM_ScrollCaret(es);
} // End EDIT_MoveHome


/*********************************************************************
 *
 *  EDIT_MovePageDown_ML
 *
 *  Only for multi line controls
 *  Move the caret one page down, on a column with the nearest
 *  x coordinate on the screen (might be a different column).
 *
 */
static void EDIT_MovePageDown_ML(EDITSTATE *es, BOOL extend)
{
    INT s = es->selection_start;
    INT e = es->selection_end;
    BOOL after_wrap = (es->flags & EF_AFTER_WRAP);
    LRESULT pos = EDIT_EM_PosFromChar(es, e, after_wrap);
    INT x = (short)LOWORD(pos);
    INT y = (short)HIWORD(pos);

    e = EDIT_CharFromPos(es, x,
        y + (es->format_rect.bottom - es->format_rect.top),
        &after_wrap);
    if (!extend)
        s = e;
    EDIT_EM_SetSel(es, s, e, after_wrap);
    EDIT_EM_ScrollCaret(es);
} // End EDIT_MovePageDown_ML


/*********************************************************************
 *
 *  EDIT_MovePageUp_ML
 *
 *  Only for multi line controls
 *  Move the caret one page up, on a column with the nearest
 *  x coordinate on the screen (might be a different column).
 *
 */
static void EDIT_MovePageUp_ML(EDITSTATE *es, BOOL extend)
{
    INT s = es->selection_start;
    INT e = es->selection_end;
    BOOL after_wrap = (es->flags & EF_AFTER_WRAP);
    LRESULT pos = EDIT_EM_PosFromChar(es, e, after_wrap);
    INT x = (short)LOWORD(pos);
    INT y = (short)HIWORD(pos);

    e = EDIT_CharFromPos(es, x,
        y - (es->format_rect.bottom - es->format_rect.top),
        &after_wrap);
    if (!extend)
        s = e;
    EDIT_EM_SetSel(es, s, e, after_wrap);
    EDIT_EM_ScrollCaret(es);
} // End EDIT_MovePageUp_ML


/*********************************************************************
 *
 *  EDIT_MoveUp_ML
 *
 *  Only for multi line controls
 *  Move the caret one line up, on a column with the nearest
 *  x coordinate on the screen (might be a different column).
 *
 */
static void EDIT_MoveUp_ML(EDITSTATE *es, BOOL extend)
{
    INT s = es->selection_start;
    INT e = es->selection_end;
    BOOL after_wrap = (es->flags & EF_AFTER_WRAP);
    LRESULT pos = EDIT_EM_PosFromChar(es, e, after_wrap);
    INT x = (short)LOWORD(pos);
    INT y = (short)HIWORD(pos);

    e = EDIT_CharFromPos(es, x, y - es->line_height, &after_wrap);
    if (!extend)
        s = e;
    EDIT_EM_SetSel(es, s, e, after_wrap);
    EDIT_EM_ScrollCaret(es);
} // End EDIT_MoveUp_ML


/*********************************************************************
 *
 *  EDIT_MoveWordBackward
 *
 */
static void EDIT_MoveWordBackward(EDITSTATE *es, BOOL extend)
{
    INT s = es->selection_start;
    INT e = es->selection_end;
    INT l;
    INT ll;
    INT li;

    l = EDIT_EM_LineFromChar(es, e);
    ll = EDIT_EM_LineLength(es, e);
    li = EDIT_EM_LineIndex(es, l);
    if (e - li == 0) {
        if (l) {
            li = EDIT_EM_LineIndex(es, l - 1);
            e = li + EDIT_EM_LineLength(es, li);
        }
    } else {
        e = li + (INT)EDIT_CallWordBreakProc(es,
                li, e - li, ll, WB_LEFT);
    }
    if (!extend)
        s = e;
    EDIT_EM_SetSel(es, s, e, FALSE);
    EDIT_EM_ScrollCaret(es);
} // End EDIT_MoveWordBackward


/*********************************************************************
 *
 *  EDIT_MoveWordForward
 *
 */
static void EDIT_MoveWordForward(EDITSTATE *es, BOOL extend)
{
    INT s = es->selection_start;
    INT e = es->selection_end;
    INT l;
    INT ll;
    INT li;

    l = EDIT_EM_LineFromChar(es, e);
    ll = EDIT_EM_LineLength(es, e);
    li = EDIT_EM_LineIndex(es, l);
    if (e - li == ll) {
        if ((es->style & ES_MULTILINE) && (l != es->line_count - 1))
            e = EDIT_EM_LineIndex(es, l + 1);
    } else {
        e = li + EDIT_CallWordBreakProc(es,
                li, e - li + 1, ll, WB_RIGHT);
    }
    if (!extend)
        s = e;
    EDIT_EM_SetSel(es, s, e, FALSE);
    EDIT_EM_ScrollCaret(es);
} // End EDIT_MoveWordForward


/*********************************************************************
 *
 *  EDIT_PaintLine
 *
 */
static void EDIT_PaintLine(EDITSTATE *es, HDC dc, INT line, BOOL rev, long lFlags, INT yLine)
{
    INT s = es->selection_start;
    INT e = es->selection_end;
    INT li;
    INT ll;
    INT x;
    INT y;
    LRESULT pos;
    RECT rc1, rc2;
    HBRUSH hBrush;

    if (es->style & ES_MULTILINE) {
        INT vlc = (es->format_rect.bottom - es->format_rect.top) / es->line_height;
        if ((line < es->y_offset) || (line > es->y_offset + vlc) || (line >= es->line_count))
            return;
    } else if (line)
        return;

//  TRACE("line=%d\n", line);

    pos = EDIT_EM_PosFromChar(es, EDIT_EM_LineIndex(es, line), FALSE);
    x = (short)LOWORD(pos);
    y = (yLine EQ -1) ? (short)HIWORD(pos) : yLine * es->line_height;
    li = EDIT_EM_LineIndex(es, line);
    ll = EDIT_EM_LineLength(es, li);
    s = min(es->selection_start, es->selection_end);
    e = max(es->selection_start, es->selection_end);
    s = min(li + ll, max(li, s));
    e = min(li + ll, max(li, e));
    if (rev && (s != e) &&
            ((es->flags & EF_FOCUSED) || (es->style & ES_NOHIDESEL))) {
        if (!(lFlags & PRF_SELECTION))
            x += EDIT_PaintText(es, dc, x, y, line, 0, s - li, FALSE, lFlags);
        else
            x += (s - li) * es->char_width;
        x += EDIT_PaintText(es, dc, x, y, line, s - li, e - s, !(lFlags & PRF_SELECTION), lFlags);
        if (!(lFlags & PRF_SELECTION))
            x += EDIT_PaintText(es, dc, x, y, line, e - li, li + ll - e, FALSE, lFlags);
    } else
    if (!(lFlags & PRF_SELECTION))
        x += EDIT_PaintText(es, dc, x, y, line, 0, ll, FALSE, lFlags);

    // If we're not printing, ...
    if (!(lFlags & PRF_PRINTCLIENT))
    {
        // Get the background brush
        hBrush = EDIT_NotifyCtlColor(es, dc);

        // Fill out the rest of the line
        rc2.left   = x;
        rc2.top    = y;
        rc2.right  = 0x7FFFFFFF;    // Largest positive #
        rc2.bottom = y + es->line_height;
        IntersectRect (&rc1, &rc2, &es->format_rect);

        // If rc1.bottom is at the bottom of the formatting rectangle,
        //   add in a char line height to clear the bottom half line
        if ((rc1.bottom + es->line_height) >= es->format_rect.bottom)
            rc1.bottom += es->line_height;

        FillRect (dc, &rc1, hBrush);
    } // End IF
} // End EDIT_PaintLine


/*********************************************************************
 *
 *  EDIT_PaintText
 *
 */
static INT EDIT_PaintText(EDITSTATE *es, HDC dc, INT x, INT y, INT line, INT col, INT count, BOOL rev, long lFlags)
{
    COLORREF BkColor;
    COLORREF TextColor;
#if USE_IME
    LOGFONTW underline_font;
    HFONT hUnderline = 0;
#endif
    HFONT old_font = 0;
    INT ret;
    INT li;
    INT BkMode;
    SIZE size;

    if (!count)
        return 0;
    BkMode = GetBkMode(dc);
    BkColor = GetBkColor(dc);
    TextColor = GetTextColor(dc);
    if (rev) {
#if USE_IME
            if (es->composition_len == 0)
            {
#endif
            SetBkColor(dc, GetSysColor(COLOR_HIGHLIGHT));
            SetTextColor(dc, GetSysColor(COLOR_HIGHLIGHTTEXT));
            SetBkMode( dc, OPAQUE);
#if USE_IME
            }
        else
        {
            HFONT current = GetCurrentObject(dc,OBJ_FONT);
            GetObjectW(current,sizeof(LOGFONTW),&underline_font);
            underline_font.lfUnderline = TRUE;
                    hUnderline = CreateFontIndirectW(&underline_font);
            old_font = SelectObject(dc,hUnderline);
            }
#endif
    }
    li = EDIT_EM_LineIndex(es, line);
    if (es->style & ES_MULTILINE) {
        LPPAINTHOOK lpPaintHook;

        (HANDLE_PTR) lpPaintHook = GetWindowLongPtrW (es->hwndSelf, GWLEC_PAINTHOOK);
        if (lpPaintHook)
            ret = (INT)LOWORD((*lpPaintHook) (es->hwndSelf, dc, x, y, es->text + li, col, count, lFlags, es->line_height, es->char_width, rev));
        else
            ret = (INT)LOWORD(TabbedTextOutW(dc, x, y, es->text + li + col, count,
                        es->tabs_count, es->tabs, es->format_rect.left - es->x_offset));
    } else {
        LPWSTR text = EDIT_GetPasswordPointer_SL(es);
        LPPAINTHOOK lpPaintHook;

        (HANDLE_PTR) lpPaintHook = GetWindowLongPtrW (es->hwndSelf, GWLEC_PAINTHOOK);
        if (lpPaintHook)
            ret = (INT)LOWORD((*lpPaintHook) (es->hwndSelf, dc, x, y, text + li, col, count, lFlags, es->line_height, es->char_width, rev));
        else
            TextOutW(dc, x, y, text + li + col, count);
        GetTextExtentPoint32W(dc, text + li + col, count, &size);
        ret = size.cx;
        if (es->style & ES_PASSWORD)
            HeapFree(GetProcessHeap(), 0, text);
    }
    if (rev) {
#if USE_IME
        if (es->composition_len == 0)
        {
#endif
            SetBkColor(dc, BkColor);
            SetTextColor(dc, TextColor);
            SetBkMode( dc, BkMode);
#if USE_IME
        }
        else
        {
            if (old_font)
                SelectObject(dc,old_font);
            if (hUnderline)
                DeleteObject(hUnderline);
            }
#endif
    }
    return ret;
} // End EDIT_PaintText


/*********************************************************************
 *
 *  EDIT_SetCaretPos
 *
 */
static void EDIT_SetCaretPos(EDITSTATE *es, INT pos,
                 BOOL after_wrap)
{
    LRESULT res = EDIT_EM_PosFromChar(es, pos, after_wrap);
//  TRACE("%d - %dx%d\n", pos, (short)LOWORD(res), (short)HIWORD(res));
    SetCaretPos((short)LOWORD(res), (short)HIWORD(res));
} // End EDIT_SetCaretPos


/*********************************************************************
 *
 *  EDIT_AdjustFormatRect
 *
 *  Adjusts the format rectangle for the current font and the
 *  current client rectangle.
 *
 */
static void EDIT_AdjustFormatRect(EDITSTATE *es)
{
    RECT ClientRect;

    es->format_rect.right = max(es->format_rect.right, es->format_rect.left + es->char_width);
    if (es->style & ES_MULTILINE)
    {
        INT fw, vlc, max_x_offset, max_y_offset;

        vlc = (es->format_rect.bottom - es->format_rect.top) / es->line_height;
        es->format_rect.bottom = es->format_rect.top + max(1, vlc) * es->line_height;

        /* correct es->x_offset */
        fw = es->format_rect.right - es->format_rect.left;
        max_x_offset = es->text_width - fw;
        if(max_x_offset < 0) max_x_offset = 0;
        if(es->x_offset > max_x_offset)
        es->x_offset = max_x_offset;

        /* correct es->y_offset */
        max_y_offset = es->line_count - vlc;
        if(max_y_offset < 0) max_y_offset = 0;
        if(es->y_offset > max_y_offset)
        es->y_offset = max_y_offset;

        /* force scroll info update */
        EDIT_UpdateScrollInfo(es);
    }
    else
    /* Windows doesn't care to fix text placement for SL controls */
        es->format_rect.bottom = es->format_rect.top + es->line_height;

    /* Always stay within the client area */
    GetClientRect(es->hwndSelf, &ClientRect);
    es->format_rect.bottom = min(es->format_rect.bottom, ClientRect.bottom);

    if ((es->style & ES_MULTILINE) && !(es->style & ES_AUTOHSCROLL))
        EDIT_BuildLineDefs_ML(es, 0, get_text_length(es), 0, NULL);

    EDIT_SetCaretPos(es, es->selection_end, es->flags & EF_AFTER_WRAP);
} // End EDIT_AdjustFormatRect


/*********************************************************************
 *
 *  EDIT_SetRectNP
 *
 *  note:   this is not (exactly) the handler called on EM_SETRECTNP
 *      it is also used to set the rect of a single line control
 *
 */
static void EDIT_SetRectNP(EDITSTATE *es, LPRECT rc)
{
    LONG_PTR ExStyle;
    INT bw, bh;
    ExStyle = GetWindowLongW(es->hwndSelf, GWL_EXSTYLE);

    CopyRect(&es->format_rect, rc);

    if (ExStyle & WS_EX_CLIENTEDGE) {
        es->format_rect.left++;
        es->format_rect.right--;

        if (es->format_rect.bottom - es->format_rect.top
            >= es->line_height + 2)
        {
            es->format_rect.top++;
            es->format_rect.bottom--;
        }
    }
    else if (es->style & WS_BORDER) {
        bw = GetSystemMetrics(SM_CXBORDER) + 1;
        bh = GetSystemMetrics(SM_CYBORDER) + 1;
        es->format_rect.left += bw;
        es->format_rect.right -= bw;
        if (es->format_rect.bottom - es->format_rect.top
          >= es->line_height + 2 * bh)
        {
            es->format_rect.top += bh;
            es->format_rect.bottom -= bh;
        }
    }

    es->format_rect.left += es->left_margin;
    es->format_rect.right -= es->right_margin;
    EDIT_AdjustFormatRect(es);
} // End EDIT_SetRectNP


/*********************************************************************
 *
 *  EDIT_UnlockBuffer
 *
 */
static void EDIT_UnlockBuffer(EDITSTATE *es, BOOL force)
{

    /* Edit window might be already destroyed */
    if(!IsWindow(es->hwndSelf))
    {
//      WARN("edit hwnd %p already destroyed\n", es->hwndSelf);
        return;
        }

    if (!es->lock_count) {
//      ERR("lock_count == 0 ... please report\n");
        return;
    }
    if (!es->text) {
//      ERR("es->text == 0 ... please report\n");
        return;
    }

    if (force || (es->lock_count == 1)) {
        if (es->hloc32W) {
        CHAR *textA = NULL;
        UINT countA = 0;
        UINT countW = get_text_length(es) + 1;
#ifdef _WIN16
        STACK16FRAME* stack16 = NULL;
            HANDLE16 oldDS = 0;
#endif

        if(es->hloc32A)
        {
            UINT countA_new = WideCharToMultiByte(CP_ACP, 0, es->text, countW, NULL, 0, NULL, NULL);
//          TRACE("Synchronizing with 32-bit ANSI buffer\n");
//          TRACE("%d WCHARs translated to %d bytes\n", countW, countA_new);
            countA = LocalSize(es->hloc32A);
            if(countA_new > countA)
            {
            HLOCAL hloc32A_new;
            UINT alloc_size = ROUND_TO_GROW(countA_new);
//          TRACE("Resizing 32-bit ANSI buffer from %d to %d bytes\n", countA, alloc_size);
            hloc32A_new = LocalReAlloc(es->hloc32A, alloc_size, LMEM_MOVEABLE | LMEM_ZEROINIT);
            if(hloc32A_new)
            {
                es->hloc32A = hloc32A_new;
                countA = LocalSize(hloc32A_new);
//              TRACE("Real new size %d bytes\n", countA);
            }
//          else
//              WARN("FAILED! Will synchronize partially\n");
            }
            textA = LocalLock(es->hloc32A);
        }
#ifdef _WIN16
        else if(es->hloc16)
        {
            UINT countA_new = WideCharToMultiByte(CP_ACP, 0, es->text, countW, NULL, 0, NULL, NULL);

//          TRACE("Synchronizing with 16-bit ANSI buffer\n");
//          TRACE("%d WCHARs translated to %d bytes\n", countW, countA_new);

            stack16 = MapSL(PtrToUlong(NtCurrentTeb()->WOW32Reserved));
            oldDS = stack16->ds;
            stack16->ds = GetWindowLongPtrW( es->hwndSelf, GWLP_HINSTANCE );

            countA = LocalSize16(es->hloc16);
            if(countA_new > countA)
            {
            HLOCAL16 hloc16_new;
            UINT alloc_size = ROUND_TO_GROW(countA_new);
//          TRACE("Resizing 16-bit ANSI buffer from %d to %d bytes\n", countA, alloc_size);
            hloc16_new = LocalReAlloc16(es->hloc16, (WORD) alloc_size, LMEM_MOVEABLE | LMEM_ZEROINIT);
            if(hloc16_new)
            {
                es->hloc16 = hloc16_new;
                countA = LocalSize16(hloc16_new);
//              TRACE("Real new size %d bytes\n", countA);
            }
            else
//              WARN("FAILED! Will synchronize partially\n");
            }
            textA = MapSL(LocalLock16(es->hloc16));
        }
#endif
        if(textA)
        {
            WideCharToMultiByte(CP_ACP, 0, es->text, countW, textA, countA, NULL, NULL);
#ifdef _WIN16
            if(stack16)
            LocalUnlock16(es->hloc16);
            else
#endif
            LocalUnlock(es->hloc32A);
        }

#ifdef _WIN16
        if (stack16) stack16->ds = oldDS;
#endif
        LocalUnlock(es->hloc32W);
        es->text = NULL;
        }
        else {
//      ERR("no buffer ... please report\n");
        return;
        }
    }
    es->lock_count--;
} // End EDIT_UnlockBuffer


/*********************************************************************
 *
 *  EDIT_UpdateScrollInfo
 *
 */
static void EDIT_UpdateScrollInfo(EDITSTATE *es)
{
    if ((es->style & WS_VSCROLL) && !(es->flags & EF_VSCROLL_TRACK))
    {
    SCROLLINFO si;
    si.cbSize   = sizeof(SCROLLINFO);
    si.fMask    = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;
    si.nMin     = 0;
    si.nMax     = es->line_count - 1;
    si.nPage    = (es->format_rect.bottom - es->format_rect.top) / es->line_height;
    si.nPos     = es->y_offset;
//  TRACE("SB_VERT, nMin=%d, nMax=%d, nPage=%d, nPos=%d\n",
//      si.nMin, si.nMax, si.nPage, si.nPos);
    SetScrollInfo(es->hwndSelf, SB_VERT, &si, TRUE);
    }

    if ((es->style & WS_HSCROLL) && !(es->flags & EF_HSCROLL_TRACK))
    {
    SCROLLINFO si;
    si.cbSize   = sizeof(SCROLLINFO);
    si.fMask    = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;
    si.nMin     = 0;
    si.nMax     = es->text_width - 1;
    si.nPage    = es->format_rect.right - es->format_rect.left;
    si.nPos     = es->x_offset;
//  TRACE("SB_HORZ, nMin=%d, nMax=%d, nPage=%d, nPos=%d\n",
//      si.nMin, si.nMax, si.nPage, si.nPos);
    SetScrollInfo(es->hwndSelf, SB_HORZ, &si, TRUE);
    }
} // End EDIT_UpdateScrollInfo


/*********************************************************************
 *
 *  EDIT_WordBreakProc
 *
 *  Find the beginning of words.
 *  Note:   unlike the specs for a WordBreakProc, this function only
 *      allows to be called without linebreaks between s[0] up to
 *      s[count - 1].  Remember it is only called
 *      internally, so we can decide this for ourselves.
 *
 */
static INT CALLBACK EDIT_WordBreakProc(LPWSTR s, INT index, INT count, INT action)
{
    INT ret = 0;

//  TRACE("s=%p, index=%d, count=%d, action=%d\n", s, index, count, action);

    if(!s) return 0;

    switch (action) {
    case WB_LEFT:
        if (!count)
            break;
        if (index)
            index--;
        if (s[index] == ' ') {
            while (index && (s[index] == ' '))
                index--;
            if (index) {
                while (index && (s[index] != ' '))
                    index--;
                if (s[index] == ' ')
                    index++;
            }
        } else {
            while (index && (s[index] != ' '))
                index--;
            if (s[index] == ' ')
                index++;
        }
        ret = index;
        break;
    case WB_RIGHT:
        if (!count)
            break;
        if (index)
            index--;
        if (s[index] == ' ')
            while ((index < count) && (s[index] == ' ')) index++;
        else {
            while (s[index] && (s[index] != ' ') && (index < count))
                index++;
            while ((s[index] == ' ') && (index < count)) index++;
        }
        ret = index;
        break;
    case WB_ISDELIMITER:
        ret = (s[index] == ' ');
        break;
    default:
//      ERR("unknown action code, please report !\n");
        break;
    }
    return ret;
} // End EDIT_WordBreakProc


/*********************************************************************
 *
 *  EM_CHARFROMPOS
 *
 *      returns line number (not index) in high-order word of result.
 *      NB : Q137805 is unclear about this. POINT * pointer in lParam apply
 *      to Richedit, not to the edit control. Original documentation is valid.
 *  FIXME: do the specs mean to return -1 if outside client area or
 *      if outside formatting rectangle ???
 *
 */
static LRESULT EDIT_EM_CharFromPos(EDITSTATE *es, INT x, INT y)
{
    POINT pt;
    RECT rc;
    INT index;

    pt.x = x;
    pt.y = y;
    GetClientRect(es->hwndSelf, &rc);
    if (!PtInRect(&rc, pt))
        return -1;

    index = EDIT_CharFromPos(es, x, y, NULL);
    return MAKELONG(index, EDIT_EM_LineFromChar(es, index));
} // End EDIT_EM_CharFromPos


/*********************************************************************
 *
 *  EM_FMTLINES
 *
 * Enable or disable soft breaks.
 *
 * This means: insert or remove the soft linebreak character (\r\r\n).
 * Take care to check if the text still fits the buffer after insertion.
 * If not, notify with EN_ERRSPACE.
 *
 */
static BOOL EDIT_EM_FmtLines(EDITSTATE *es, BOOL add_eol)
{
    es->flags &= ~EF_USE_SOFTBRK;
    if (add_eol) {
        es->flags |= EF_USE_SOFTBRK;
//      FIXME("soft break enabled, not implemented\n");
    }
    return add_eol;
} // End EDIT_EM_FmtLines


/*********************************************************************
 *
 *  EM_GETHANDLE
 *
 *  Hopefully this won't fire back at us.
 *  We always start with a fixed buffer in the local heap.
 *  Despite of the documentation says that the local heap is used
 *  only if DS_LOCALEDIT flag is set, NT and 2000 always allocate
 *  buffer on the local heap.
 *
 */
static HLOCAL EDIT_EM_GetHandle(EDITSTATE *es)
{
    HLOCAL hLocal;

    if (!(es->style & ES_MULTILINE))
        return 0;

    if(es->is_unicode)
        hLocal = es->hloc32W;
    else
    {
        if(!es->hloc32A)
        {
        CHAR *textA;
        UINT countA, alloc_size;
//      TRACE("Allocating 32-bit ANSI alias buffer\n");
        countA = WideCharToMultiByte(CP_ACP, 0, es->text, -1, NULL, 0, NULL, NULL);
        alloc_size = ROUND_TO_GROW(countA);
        if(!(es->hloc32A = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, alloc_size)))
        {
//          ERR("Could not allocate %d bytes for 32-bit ANSI alias buffer\n", alloc_size);
            return 0;
        }
        textA = LocalLock(es->hloc32A);
        WideCharToMultiByte(CP_ACP, 0, es->text, -1, textA, countA, NULL, NULL);
        LocalUnlock(es->hloc32A);
        }
        hLocal = es->hloc32A;
    }

        es->flags |= EF_APP_HAS_HANDLE;
//  TRACE("Returning %p, LocalSize() = %ld\n", hLocal, LocalSize(hLocal));
    return hLocal;
} // End EDIT_EM_GetHandle


#ifdef _WIN16
/*********************************************************************
 *
 *  EM_GETHANDLE16
 *
 *  Hopefully this won't fire back at us.
 *  We always start with a buffer in 32 bit linear memory.
 *  However, with this message a 16 bit application requests
 *  a handle of 16 bit local heap memory, where it expects to find
 *  the text.
 *  It's a pitty that from this moment on we have to use this
 *  local heap, because applications may rely on the handle
 *  in the future.
 *
 *  In this function we'll try to switch to local heap.
 */
static HLOCAL16 EDIT_EM_GetHandle16(EDITSTATE *es)
{
    CHAR *textA;
    UINT countA, alloc_size;
    STACK16FRAME* stack16;
    HANDLE16 oldDS;

    if (!(es->style & ES_MULTILINE))
        return 0;

    if (es->hloc16)
        return es->hloc16;

    stack16 = MapSL(PtrToUlong(NtCurrentTeb()->WOW32Reserved));
    oldDS = stack16->ds;
    stack16->ds = GetWindowLongPtrW( es->hwndSelf, GWLP_HINSTANCE );

    if (!LocalHeapSize16()) {

        if (!LocalInit16(stack16->ds, 0, GlobalSize16(stack16->ds))) {
//          ERR("could not initialize local heap\n");
            goto done;
        }
//      TRACE("local heap initialized\n");
    }

    countA = WideCharToMultiByte(CP_ACP, 0, es->text, -1, NULL, 0, NULL, NULL);
    alloc_size = ROUND_TO_GROW(countA);

//  TRACE("Allocating 16-bit ANSI alias buffer\n");
    if (!(es->hloc16 = LocalAlloc16(LMEM_MOVEABLE | LMEM_ZEROINIT, alloc_size))) {
//      ERR("could not allocate new 16 bit buffer\n");
        goto done;
    }

    if (!(textA = MapSL(LocalLock16( es->hloc16)))) {
//      ERR("could not lock new 16 bit buffer\n");
        LocalFree16(es->hloc16);
        es->hloc16 = 0;
        goto done;
    }

    WideCharToMultiByte(CP_ACP, 0, es->text, -1, textA, countA, NULL, NULL);
    LocalUnlock16(es->hloc16);
        es->flags |= EF_APP_HAS_HANDLE;

//  TRACE("Returning %04X, LocalSize() = %d\n", es->hloc16, LocalSize16(es->hloc16));

done:
    stack16->ds = oldDS;
    return es->hloc16;
} // End EDIT_EM_GetHandle16
#endif


/*********************************************************************
 *
 *  EM_GETLINE
 *
 */
static INT EDIT_EM_GetLine(EDITSTATE *es, INT line, LPWSTR dst, BOOL unicode)
{
    LPWSTR src;
    INT line_len, dst_len;
    INT i;

    if (es->style & ES_MULTILINE) {
        if (line >= es->line_count)
            return 0;
    } else
        line = 0;
    i = EDIT_EM_LineIndex(es, line);
    src = es->text + i;
    line_len = EDIT_EM_LineLength(es, i);
    dst_len = *(WORD *)dst;
    if(unicode)
    {
        if(dst_len <= line_len)
        {
        memcpy(dst, src, dst_len * sizeof(WCHAR));
        return dst_len;
        }
        else /* Append 0 if enough space */
        {
        memcpy(dst, src, line_len * sizeof(WCHAR));
        dst[line_len] = 0;
        return line_len;
        }
    }
    else
    {
        INT ret = WideCharToMultiByte(CP_ACP, 0, src, line_len, (LPSTR)dst, dst_len, NULL, NULL);
        if(!ret && line_len) /* Insufficient buffer size */
        return dst_len;
        if(ret < dst_len) /* Append 0 if enough space */
        ((LPSTR)dst)[ret] = 0;
        return ret;
    }
} // End EDIT_EM_GetLine


/*********************************************************************
 *
 *  EM_GETSEL
 *
 */
static LRESULT EDIT_EM_GetSel(EDITSTATE *es, PUINT start, PUINT end)
{
    UINT s = es->selection_start;
    UINT e = es->selection_end;

    ORDER_UINT(s, e);
    if (start)
        *start = s;
    if (end)
        *end = e;
    return MAKELONG(s, e);
} // End EDIT_EM_GetSel


/*********************************************************************
 *
 *  EM_GETTHUMB
 *
 *  FIXME: is this right ?  (or should it be only VSCROLL)
 *  (and maybe only for edit controls that really have their
 *  own scrollbars) (and maybe only for multiline controls ?)
 *  All in all: very poorly documented
 *
 */
static LRESULT EDIT_EM_GetThumb(EDITSTATE *es)
{
    return MAKELONG(EDIT_WM_VScroll(es, EM_GETTHUMB16, 0),
        EDIT_WM_HScroll(es, EM_GETTHUMB16, 0));
} // End EDIT_EM_GetThumb


/*********************************************************************
 *
 *  EM_LINEFROMCHAR
 *
 */
static INT EDIT_EM_LineFromChar(EDITSTATE *es, INT index)
{
    INT line;
    LINEDEF *line_def;

    if (!(es->style & ES_MULTILINE))
        return 0;
    if (index > (INT)get_text_length(es))
        return es->line_count - 1;
    if (index == -1)
        index = min(es->selection_start, es->selection_end);

    line = 0;
    line_def = es->first_line_def;
    index -= line_def->length;
    while ((index >= 0) && line_def->next) {
        line++;
        line_def = line_def->next;
        index -= line_def->length;
    }
    return line;
} // End EDIT_EM_LineFromChar


/*********************************************************************
 *
 *  EM_LINEINDEX
 *
 */
static INT EDIT_EM_LineIndex(EDITSTATE *es, INT line)
{
    INT line_index;
    LINEDEF *line_def;

    if (!(es->style & ES_MULTILINE))
        return 0;
    if (line >= es->line_count)
        return -1;

    line_index = 0;
    line_def = es->first_line_def;
    if (line == -1) {
        INT index = es->selection_end - line_def->length;
        while ((index >= 0) && line_def->next) {
            line_index += line_def->length;
            line_def = line_def->next;
            index -= line_def->length;
        }
    } else {
        while (line > 0) {
            line_index += line_def->length;
            line_def = line_def->next;
            line--;
        }
    }
    return line_index;
} // End EDIT_EM_LineIndex


/*********************************************************************
 *
 *  EM_LINELENGTH
 *
 */
static INT EDIT_EM_LineLength(EDITSTATE *es, INT index)
{
    LINEDEF *line_def;

    if (!(es->style & ES_MULTILINE))
        return get_text_length(es);

    if (index == -1) {
        /* get the number of remaining non-selected chars of selected lines */
        INT32 l; /* line number */
        INT32 li; /* index of first char in line */
        INT32 count;
        l = EDIT_EM_LineFromChar(es, es->selection_start);
        /* # chars before start of selection area */
        count = es->selection_start - EDIT_EM_LineIndex(es, l);
        l = EDIT_EM_LineFromChar(es, es->selection_end);
        /* # chars after end of selection */
        li = EDIT_EM_LineIndex(es, l);
        count += li + EDIT_EM_LineLength(es, li) - es->selection_end;
        return count;
    }
    line_def = es->first_line_def;
    index -= line_def->length;
    while ((index >= 0) && line_def->next) {
        line_def = line_def->next;
        index -= line_def->length;
    }
    return line_def->net_length;
} // End EDIT_EM_LineLength


/*********************************************************************
 *
 *  EM_LINESCROLL
 *
 *  NOTE: dx is in average character widths, dy - in lines;
 *
 */
static BOOL EDIT_EM_LineScroll(EDITSTATE *es, INT dx, INT dy)
{
    if (!(es->style & ES_MULTILINE))
        return FALSE;

    dx *= es->char_width;
    return EDIT_EM_LineScroll_internal(es, dx, dy);
} // End EDIT_EM_LineScroll


/*********************************************************************
 *
 *  EDIT_EM_LineScroll_internal
 *
 *  Version of EDIT_EM_LineScroll for internal use.
 *  It doesn't refuse if ES_MULTILINE is set and assumes that
 *  dx is in pixels, dy - in lines.
 *
 */
static BOOL EDIT_EM_LineScroll_internal(EDITSTATE *es, INT dx, INT dy)
{
    INT nyoff;
    INT x_offset_in_pixels;
    INT lines_per_page = (es->format_rect.bottom - es->format_rect.top) /
                  es->line_height;

    if (es->style & ES_MULTILINE)
    {
        x_offset_in_pixels = es->x_offset;
    }
    else
    {
        dy = 0;
        x_offset_in_pixels = (short)LOWORD(EDIT_EM_PosFromChar(es, es->x_offset, FALSE));
    }

    if (-dx > x_offset_in_pixels)
        dx = -x_offset_in_pixels;
    if (dx > es->text_width - x_offset_in_pixels)
        dx = es->text_width - x_offset_in_pixels;
    nyoff = max(0, es->y_offset + dy);
    if (nyoff >= es->line_count - lines_per_page)
        nyoff = max(0, es->line_count - lines_per_page);
    dy = (es->y_offset - nyoff) * es->line_height;
    if (dx || dy) {
        RECT rc1;
        RECT rc;

        es->y_offset = nyoff;
        if(es->style & ES_MULTILINE)
            es->x_offset += dx;
        else
            es->x_offset += dx / es->char_width;

        GetClientRect(es->hwndSelf, &rc1);
        IntersectRect(&rc, &rc1, &es->format_rect);
        ScrollWindowEx(es->hwndSelf, -dx, dy,
                NULL, &rc, NULL, NULL, SW_INVALIDATE);
        /* force scroll info update */
        EDIT_UpdateScrollInfo(es);
    }
    if (dx && !(es->flags & EF_HSCROLL_TRACK))
        EDIT_NOTIFY_PARENT(es, EN_HSCROLL);
    if (dy && !(es->flags & EF_VSCROLL_TRACK))
        EDIT_NOTIFY_PARENT(es, EN_VSCROLL);
    return TRUE;
} // End EDIT_EM_LineScroll_internal


/*********************************************************************
 *
 *  EM_POSFROMCHAR
 *
 */
static LRESULT EDIT_EM_PosFromChar(EDITSTATE *es, INT index, BOOL after_wrap)
{
    INT len = get_text_length(es);
    INT l;
    INT li;
    INT x;
    INT y = 0;
    INT w;
    INT lw = 0;
    INT ll = 0;
    HDC dc;
    HFONT old_font = 0;
    SIZE size;
    LINEDEF *line_def;

    index = min(index, len);
    dc = GetDC(es->hwndSelf);
    if (es->font)
        old_font = SelectObject(dc, es->font);
    if (es->style & ES_MULTILINE) {
        l = EDIT_EM_LineFromChar(es, index);
        y = (l - es->y_offset) * es->line_height;
        li = EDIT_EM_LineIndex(es, l);
        if (after_wrap && (li == index) && l) {
            INT l2 = l - 1;
            line_def = es->first_line_def;
            while (l2) {
                line_def = line_def->next;
                l2--;
            }
            if (line_def->ending == END_WRAP) {
                l--;
                y -= es->line_height;
                li = EDIT_EM_LineIndex(es, l);
            }
        }

        line_def = es->first_line_def;
        while (line_def->index != li)
            line_def = line_def->next;

        ll = line_def->net_length;
        lw = line_def->width;

        w = es->format_rect.right - es->format_rect.left;
        if (es->style & ES_RIGHT)
        {
            x = LOWORD(GetTabbedTextExtentW(dc, es->text + li + (index - li), ll - (index - li),
                es->tabs_count, es->tabs)) - es->x_offset;
            x = w - x;
        }
        else if (es->style & ES_CENTER)
        {
            x = LOWORD(GetTabbedTextExtentW(dc, es->text + li, index - li,
                es->tabs_count, es->tabs)) - es->x_offset;
            x += (w - lw) / 2;
        }
        else /* ES_LEFT */
        {
            x = LOWORD(GetTabbedTextExtentW(dc, es->text + li, index - li,
                es->tabs_count, es->tabs)) - es->x_offset;
        }
    } else {
        LPWSTR text = EDIT_GetPasswordPointer_SL(es);
        if (index < es->x_offset) {
            GetTextExtentPoint32W(dc, text + index,
                    es->x_offset - index, &size);
            x = -size.cx;
        } else {
            GetTextExtentPoint32W(dc, text + es->x_offset,
                    index - es->x_offset, &size);
             x = size.cx;

            if (!es->x_offset && (es->style & (ES_RIGHT | ES_CENTER)))
            {
                w = es->format_rect.right - es->format_rect.left;
                if (w > es->text_width)
                {
                    if (es->style & ES_RIGHT)
                        x += w - es->text_width;
                    else if (es->style & ES_CENTER)
                        x += (w - es->text_width) / 2;
                }
            }
        }
        y = 0;
        if (es->style & ES_PASSWORD)
            HeapFree(GetProcessHeap(), 0, text);
    }
    x += es->format_rect.left;
    y += es->format_rect.top;
    if (es->font)
        SelectObject(dc, old_font);
    ReleaseDC(es->hwndSelf, dc); dc = NULL;
    return MAKELONG((INT16)x, (INT16)y);
} // End EDIT_EM_PosFromChar


/*********************************************************************
 *
 *  EM_REPLACESEL
 *
 *  FIXME: handle ES_NUMBER and ES_OEMCONVERT here
 *
 */
static void EDIT_EM_ReplaceSel(EDITSTATE *es, BOOL can_undo, LPCWSTR lpsz_replace, BOOL send_update, BOOL honor_limit)
{
    UINT strl = strlenW(lpsz_replace);
    UINT tl = get_text_length(es);
    UINT utl;
    UINT s;
    UINT e;
    UINT i;
    UINT size;
    LPWSTR p;
    HRGN hrgn = 0;
    LPWSTR buf = NULL;
    UINT bufl = 0;

//  TRACE("%s, can_undo %d, send_update %d\n",
//      debugstr_w(lpsz_replace), can_undo, send_update);

    s = es->selection_start;
    e = es->selection_end;

    if ((s == e) && !strl)
        return;

    ORDER_UINT(s, e);

    size = tl - (e - s) + strl;
    if (!size)
        es->text_width = 0;

    /* Issue the EN_MAXTEXT notification and continue with replacing text
     * such that buffer limit is honored. */
    if ((honor_limit) && (size > es->buffer_limit)) {
        while (size > es->buffer_limit)
        {
            UINT limit = es->buffer_limit;
            EDIT_NOTIFY_PARENT(es, EN_MAXTEXT);
            // If the new limit is no bigger than the old one, don't
            //   try again
            if (limit >= es->buffer_limit)
                break;
        } // End WHILE

        /* Buffer limit can be smaller than the actual length of text in combobox */
        if (es->buffer_limit < (tl - (e-s)))
            strl = 0;
        else
        {
            UINT strl2 = es->buffer_limit - (tl - (e-s));
            strl = min (strl, strl2);
        }
    }

    if (!EDIT_MakeFit(es, tl - (e - s) + strl))
        return;

    if (e != s) {
        /* there is something to be deleted */
//      TRACE("deleting stuff.\n");
        bufl = e - s;
        buf = HeapAlloc(GetProcessHeap(), 0, (bufl + 1) * sizeof(WCHAR));
        if (!buf) return;
        memcpy(buf, es->text + s, bufl * sizeof(WCHAR));
        buf[bufl] = 0; /* ensure 0 termination */
        /* now delete */
        strcpyW(es->text + s, es->text + e);
                text_buffer_changed(es);
    }
    if (strl) {
        /* there is an insertion */
        tl = get_text_length(es);
//      TRACE("inserting stuff (tl %d, strl %d, selstart %d (%s), text %s)\n", tl, strl, s, debugstr_w(es->text + s), debugstr_w(es->text));
        for (p = es->text + tl ; p >= es->text + s ; p--)
            p[strl] = p[0];
        for (i = 0 , p = es->text + s ; i < strl ; i++)
            p[i] = lpsz_replace[i];
        if(es->style & ES_UPPERCASE)
            CharUpperBuffW(p, strl);
        else if(es->style & ES_LOWERCASE)
            CharLowerBuffW(p, strl);
                text_buffer_changed(es);
    }
    if (es->style & ES_MULTILINE)
    {
        INT st = min(es->selection_start, es->selection_end);
        INT vlc = (es->format_rect.bottom - es->format_rect.top) / es->line_height;

        hrgn = CreateRectRgn(0, 0, 0, 0);
        EDIT_BuildLineDefs_ML(es, st, st + strl,
                strl - abs(es->selection_end - es->selection_start), hrgn);
        /* if text is too long undo all changes */
        if (honor_limit && !(es->style & ES_AUTOVSCROLL) && (es->line_count > vlc)) {
            if (strl)
                strcpyW(es->text + e, es->text + e + strl);
            if (e != s)
                for (i = 0 , p = es->text ; i < e - s ; i++)
                    p[i + s] = buf[i];
                        text_buffer_changed(es);
            EDIT_BuildLineDefs_ML(es, s, e,
                abs(es->selection_end - es->selection_start) - strl, hrgn);
            strl = 0;
            e = s;
            hrgn = CreateRectRgn(0, 0, 0, 0);
            EDIT_NOTIFY_PARENT(es, EN_MAXTEXT);
        }
    }
    else {
        INT fw = es->format_rect.right - es->format_rect.left;
        EDIT_CalcLineWidth_SL(es);
        /* remove chars that don't fit */
        if (honor_limit && !(es->style & ES_AUTOHSCROLL) && (es->text_width > fw)) {
            while ((es->text_width > fw) && s + strl >= s) {
                strcpyW(es->text + s + strl - 1, es->text + s + strl);
                strl--;
                EDIT_CalcLineWidth_SL(es);
            }
                        text_buffer_changed(es);
            EDIT_NOTIFY_PARENT(es, EN_MAXTEXT);
        }
    }

    if (e != s) {
        if (can_undo) {
            utl = strlenW(es->undo_text);
            if (!es->undo_insert_count && (*es->undo_text && (s == es->undo_position))) {
                /* undo-buffer is extended to the right */
                EDIT_MakeUndoFit(es, utl + e - s);
                memcpy(es->undo_text + utl, buf, (e - s)*sizeof(WCHAR));
                (es->undo_text + utl)[e - s] = 0; /* ensure 0 termination */
            } else if (!es->undo_insert_count && (*es->undo_text && (e == es->undo_position))) {
                /* undo-buffer is extended to the left */
                EDIT_MakeUndoFit(es, utl + e - s);
                for (p = es->undo_text + utl ; p >= es->undo_text ; p--)
                    p[e - s] = p[0];
                for (i = 0 , p = es->undo_text ; i < e - s ; i++)
                    p[i] = buf[i];
                es->undo_position = s;
            } else {
                /* new undo-buffer */
                EDIT_MakeUndoFit(es, e - s);
                memcpy(es->undo_text, buf, (e - s)*sizeof(WCHAR));
                es->undo_text[e - s] = 0; /* ensure 0 termination */
                es->undo_position = s;
            }
            /* any deletion makes the old insertion-undo invalid */
            es->undo_insert_count = 0;
        } else
            EDIT_EM_EmptyUndoBuffer(es);
    }
    if (strl) {
        if (can_undo) {
            if ((s == es->undo_position) ||
                ((es->undo_insert_count) &&
                (s == es->undo_position + es->undo_insert_count)))
                /*
                 * insertion is new and at delete position or
                 * an extension to either left or right
                 */
                es->undo_insert_count += strl;
            else {
                /* new insertion undo */
                es->undo_position = s;
                es->undo_insert_count = strl;
                /* new insertion makes old delete-buffer invalid */
                *es->undo_text = '\0';
            }
        } else
            EDIT_EM_EmptyUndoBuffer(es);
    }

    if (bufl)
        HeapFree(GetProcessHeap(), 0, buf);

    s += strl;

    /* If text has been deleted and we're right or center aligned then scroll rightward */
    if (es->style & (ES_RIGHT | ES_CENTER))
    {
        INT delta = strl - abs(es->selection_end - es->selection_start);

        if (delta < 0 && es->x_offset)
        {
            if (abs(delta) > es->x_offset)
                es->x_offset = 0;
            else
                es->x_offset += delta;
        }
    }

    EDIT_EM_SetSel(es, s, s, FALSE);
    es->flags |= EF_MODIFIED;
    if (send_update) es->flags |= EF_UPDATE;
    if (hrgn)
    {
        EDIT_UpdateTextRegion(es, hrgn, TRUE);
        DeleteObject(hrgn);
    }
    else
            EDIT_UpdateText(es, NULL, TRUE);

    EDIT_EM_ScrollCaret(es);

    /* force scroll info update */
    EDIT_UpdateScrollInfo(es);


        if(send_update || (es->flags & EF_UPDATE))
    {
        es->flags &= ~EF_UPDATE;
        EDIT_NOTIFY_PARENT(es, EN_CHANGE);
    }
} // End EDIT_EM_ReplaceSel


/*********************************************************************
 *
 *  EM_SCROLL
 *
 */
static LRESULT EDIT_EM_Scroll(EDITSTATE *es, INT action)
{
    INT dy;

    if (!(es->style & ES_MULTILINE))
        return (LRESULT)FALSE;

    dy = 0;

    switch (action) {
    case SB_LINEUP:
        if (es->y_offset)
            dy = -1;
        break;
    case SB_LINEDOWN:
        if (es->y_offset < es->line_count - 1)
            dy = 1;
        break;
    case SB_PAGEUP:
        if (es->y_offset)
            dy = -(es->format_rect.bottom - es->format_rect.top) / es->line_height;
        break;
    case SB_PAGEDOWN:
        if (es->y_offset < es->line_count - 1)
            dy = (es->format_rect.bottom - es->format_rect.top) / es->line_height;
        break;
    default:
        return (LRESULT)FALSE;
    }
    if (dy) {
        INT vlc = (es->format_rect.bottom - es->format_rect.top) / es->line_height;
        /* check if we are going to move too far */
        if(es->y_offset + dy > es->line_count - vlc)
        dy = es->line_count - vlc - es->y_offset;

        /* Notification is done in EDIT_EM_LineScroll */
        if(dy)
        EDIT_EM_LineScroll(es, 0, dy);
    }
    return MAKELONG((INT16)dy, (BOOL16)TRUE);
} // End EDIT_EM_Scroll


/*********************************************************************
 *
 *  EM_SCROLLCARET
 *
 */
static void EDIT_EM_ScrollCaret(EDITSTATE *es)
{
    if (es->style & ES_MULTILINE) {
        INT l;
        INT vlc;
        INT ww;
        INT cw = es->char_width;
        INT x;
        INT dy = 0;
        INT dx = 0;

        l = EDIT_EM_LineFromChar(es, es->selection_end);
        x = (short)LOWORD(EDIT_EM_PosFromChar(es, es->selection_end, es->flags & EF_AFTER_WRAP));
        vlc = (es->format_rect.bottom - es->format_rect.top) / es->line_height;
        if (l >= es->y_offset + vlc)
            dy = l - vlc + 1 - es->y_offset;
        if (l < es->y_offset)
            dy = l - es->y_offset;
        ww = es->format_rect.right - es->format_rect.left;
        if (x < es->format_rect.left)
            dx = x - es->format_rect.left - ww / HSCROLL_FRACTION / cw * cw;
        if (x > es->format_rect.right)
            dx = x - es->format_rect.left - (HSCROLL_FRACTION - 1) * ww / HSCROLL_FRACTION / cw * cw;
        if (dy || dx || (es->y_offset && (es->line_count - es->y_offset < vlc)))
        {
            /* check if we are going to move too far */
            if(es->x_offset + dx + ww > es->text_width)
            dx = es->text_width - ww - es->x_offset;
            if(dx || dy || (es->y_offset && (es->line_count - es->y_offset < vlc)))
            EDIT_EM_LineScroll_internal(es, dx, dy);
        }
    } else {
        INT x;
        INT goal;
        INT format_width;

        x = (short)LOWORD(EDIT_EM_PosFromChar(es, es->selection_end, FALSE));
        format_width = es->format_rect.right - es->format_rect.left;
        if (x < es->format_rect.left) {
            goal = es->format_rect.left + format_width / HSCROLL_FRACTION;
            do {
                es->x_offset--;
                x = (short)LOWORD(EDIT_EM_PosFromChar(es, es->selection_end, FALSE));
            } while ((x < goal) && es->x_offset);
            /* FIXME: use ScrollWindow() somehow to improve performance */
            EDIT_UpdateText(es, NULL, TRUE);
        } else if (x > es->format_rect.right) {
            INT x_last;
            INT len = get_text_length(es);
            goal = es->format_rect.right - format_width / HSCROLL_FRACTION;
            do {
                es->x_offset++;
                x = (short)LOWORD(EDIT_EM_PosFromChar(es, es->selection_end, FALSE));
                x_last = (short)LOWORD(EDIT_EM_PosFromChar(es, len, FALSE));
            } while ((x > goal) && (x_last > es->format_rect.right));
            /* FIXME: use ScrollWindow() somehow to improve performance */
            EDIT_UpdateText(es, NULL, TRUE);
        }
    }

    if (es->flags & EF_FOCUSED)
        EDIT_SetCaretPos(es, es->selection_end, es->flags & EF_AFTER_WRAP);
} // End EDIT_EM_ScrollCaret


/*********************************************************************
 *
 *  EM_SETHANDLE
 *
 *  FIXME:  ES_LOWERCASE, ES_UPPERCASE, ES_OEMCONVERT, ES_NUMBER ???
 *
 */
static void EDIT_EM_SetHandle(EDITSTATE *es, HLOCAL hloc)
{
    if (!(es->style & ES_MULTILINE))
        return;

    if (!hloc) {
//      WARN("called with NULL handle\n");
        return;
    }

    EDIT_UnlockBuffer(es, TRUE);

#ifdef _WIN16
    if(es->hloc16)
    {
        STACK16FRAME* stack16 = MapSL(PtrToUlong(NtCurrentTeb()->WOW32Reserved));
        HANDLE16 oldDS = stack16->ds;

        stack16->ds = GetWindowLongPtrW( es->hwndSelf, GWLP_HINSTANCE );
        LocalFree16(es->hloc16);
        stack16->ds = oldDS;
        es->hloc16 = 0;
    }
#endif

    if(es->is_unicode)
    {
        if(es->hloc32A)
        {
        LocalFree(es->hloc32A);
        es->hloc32A = NULL;
        }
        es->hloc32W = hloc;
    }
    else
    {
        INT countW, countA;
        HLOCAL hloc32W_new;
        WCHAR *textW;
        CHAR *textA;

        countA = LocalSize(hloc);
        textA = LocalLock(hloc);
        countW = MultiByteToWideChar(CP_ACP, 0, textA, countA, NULL, 0);
        if(!(hloc32W_new = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, countW * sizeof(WCHAR))))
        {
//      ERR("Could not allocate new unicode buffer\n");
        return;
        }
        textW = LocalLock(hloc32W_new);
        MultiByteToWideChar(CP_ACP, 0, textA, countA, textW, countW);
        LocalUnlock(hloc32W_new);
        LocalUnlock(hloc);

        if(es->hloc32W)
        LocalFree(es->hloc32W);

        es->hloc32W = hloc32W_new;
        es->hloc32A = hloc;
    }

    es->buffer_size = LocalSize(es->hloc32W)/sizeof(WCHAR) - 1;

        es->flags |= EF_APP_HAS_HANDLE;
    EDIT_LockBuffer(es);

    es->x_offset = es->y_offset = 0;
    es->selection_start = es->selection_end = 0;
    EDIT_EM_EmptyUndoBuffer(es);
    es->flags &= ~EF_MODIFIED;
    es->flags &= ~EF_UPDATE;
    EDIT_BuildLineDefs_ML(es, 0, get_text_length(es), 0, NULL);
    EDIT_UpdateText(es, NULL, TRUE);
    EDIT_EM_ScrollCaret(es);
    /* force scroll info update */
    EDIT_UpdateScrollInfo(es);
} // End EDIT_EM_SetHandle


#ifdef _WIN16
/*********************************************************************
 *
 *  EM_SETHANDLE16
 *
 *  FIXME:  ES_LOWERCASE, ES_UPPERCASE, ES_OEMCONVERT, ES_NUMBER ???
 *
 */
static void EDIT_EM_SetHandle16(EDITSTATE *es, HLOCAL16 hloc)
{
    STACK16FRAME* stack16 = MapSL(PtrToUlong(NtCurrentTeb()->WOW32Reserved));
    HINSTANCE16 hInstance = GetWindowLongPtrW( es->hwndSelf, GWLP_HINSTANCE );
    HANDLE16 oldDS = stack16->ds;
    INT countW, countA;
    HLOCAL hloc32W_new;
    WCHAR *textW;
    CHAR *textA;

    if (!(es->style & ES_MULTILINE))
        return;

    if (!hloc) {
//      WARN("called with NULL handle\n");
        return;
    }

    EDIT_UnlockBuffer(es, TRUE);

    if(es->hloc32A)
    {
        LocalFree(es->hloc32A);
        es->hloc32A = NULL;
    }

    stack16->ds = hInstance;
    countA = LocalSize16(hloc);
    textA = MapSL(LocalLock16(hloc));
    countW = MultiByteToWideChar(CP_ACP, 0, textA, countA, NULL, 0);
    if(!(hloc32W_new = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, countW * sizeof(WCHAR))))
    {
//      ERR("Could not allocate new unicode buffer\n");
        return;
    }
    textW = LocalLock(hloc32W_new);
    MultiByteToWideChar(CP_ACP, 0, textA, countA, textW, countW);
    LocalUnlock(hloc32W_new);
    LocalUnlock16(hloc);
    stack16->ds = oldDS;

    if(es->hloc32W)
        LocalFree(es->hloc32W);

    es->hloc32W = hloc32W_new;
    es->hloc16 = hloc;

    es->buffer_size = LocalSize(es->hloc32W)/sizeof(WCHAR) - 1;

        es->flags |= EF_APP_HAS_HANDLE;
    EDIT_LockBuffer(es);

    es->x_offset = es->y_offset = 0;
    es->selection_start = es->selection_end = 0;
    EDIT_EM_EmptyUndoBuffer(es);
    es->flags &= ~EF_MODIFIED;
    es->flags &= ~EF_UPDATE;
    EDIT_BuildLineDefs_ML(es, 0, get_text_length(es), 0, NULL);
    EDIT_UpdateText(es, NULL, TRUE);
    EDIT_EM_ScrollCaret(es);
    /* force scroll info update */
    EDIT_UpdateScrollInfo(es);
} // End EDIT_EM_SetHandle16
#endif


/*********************************************************************
 *
 *  EM_SETLIMITTEXT
 *
 *  NOTE: this version currently implements WinNT limits
 *
 */
static void EDIT_EM_SetLimitText(EDITSTATE *es, INT limit)
{
    if (!limit) limit = ~0u;
    if (!(es->style & ES_MULTILINE)) limit = min(limit, 0x7ffffffe);
    es->buffer_limit = limit;
} // End EDIT_EM_SetLimitText


/*********************************************************************
 *
 *  EM_SETMARGINS
 *
 * EC_USEFONTINFO is used as a left or right value i.e. lParam and not as an
 * action wParam despite what the docs say. EC_USEFONTINFO calculates the
 * margin according to the textmetrics of the current font.
 *
 * FIXME - With TrueType or vector fonts EC_USEFONTINFO currently sets one third
 * of the char's width as the margin, but this is not how Windows handles this.
 * For all other fonts Windows sets the margins to zero.
 *
 * FIXME - When EC_USEFONTINFO is used the margins only change if the
 * edit control is equal to or larger than a certain size.
 * Interestingly if one subtracts both the left and right margins from
 * this size one always seems to get an even number.  The extents of
 * the (four character) string "'**'" match this quite closely, so
 * we'll use this until we come up with a better idea.
 */
static int calc_min_set_margin_size(HDC dc, INT left, INT right)
{
    WCHAR magic_string[] = {'\'','*','*','\'', 0};
    SIZE sz;

    GetTextExtentPointW(dc, magic_string, sizeof(magic_string)/sizeof(WCHAR) - 1, &sz);
    return sz.cx + left + right;
} // End calc_min_set_margin_size


static void EDIT_EM_SetMargins(EDITSTATE *es, INT action,
                   WORD left, WORD right, BOOL repaint)
{
    TEXTMETRICW tm;
    INT default_left_margin  = 0; /* in pixels */
    INT default_right_margin = 0; /* in pixels */

        /* Set the default margins depending on the font */
        if (es->font && (left == EC_USEFONTINFO || right == EC_USEFONTINFO)) {
            HDC dc = GetDC(es->hwndSelf);
            HFONT old_font = SelectObject(dc, es->font);
            GetTextMetricsW(dc, &tm);
            /* The default margins are only non zero for TrueType or Vector fonts */
            if (tm.tmPitchAndFamily & ( TMPF_VECTOR | TMPF_TRUETYPE )) {
                int min_size;
                RECT rc;
                /* This must be calculated more exactly! But how? */
                default_left_margin = tm.tmAveCharWidth / 2;
                default_right_margin = tm.tmAveCharWidth / 2;
                min_size = calc_min_set_margin_size(dc, default_left_margin, default_right_margin);
                GetClientRect(es->hwndSelf, &rc);
                if(rc.right - rc.left < min_size) {
                    default_left_margin = es->left_margin;
                    default_right_margin = es->right_margin;
                }
            }
            SelectObject(dc, old_font);
            ReleaseDC(es->hwndSelf, dc); dc = NULL;
        }

    if (action & EC_LEFTMARGIN) {
        es->format_rect.left -= es->left_margin;
        if (left != EC_USEFONTINFO)
            es->left_margin = left;
        else
            es->left_margin = default_left_margin;
        es->format_rect.left += es->left_margin;
    }

    if (action & EC_RIGHTMARGIN) {
        es->format_rect.right += es->right_margin;
        if (right != EC_USEFONTINFO)
            es->right_margin = right;
        else
            es->right_margin = default_right_margin;
        es->format_rect.right -= es->right_margin;
    }

    if (action & (EC_LEFTMARGIN | EC_RIGHTMARGIN)) {
        EDIT_AdjustFormatRect(es);
        if (repaint) EDIT_UpdateText(es, NULL, TRUE);
    }

//  TRACE("left=%d, right=%d\n", es->left_margin, es->right_margin);
} // End EDIT_EM_SetMargins


/*********************************************************************
 *
 *  EM_SETPASSWORDCHAR
 *
 */
static void EDIT_EM_SetPasswordChar(EDITSTATE *es, WCHAR c)
{
    LONG style;

    if (es->style & ES_MULTILINE)
        return;

    if (es->password_char == c)
        return;

        style = GetWindowLongW( es->hwndSelf, GWL_STYLE );
    es->password_char = c;
    if (c) {
            SetWindowLongW( es->hwndSelf, GWL_STYLE, style | ES_PASSWORD );
            es->style |= ES_PASSWORD;
    } else {
            SetWindowLongW( es->hwndSelf, GWL_STYLE, style & ~ES_PASSWORD );
            es->style &= ~ES_PASSWORD;
    }
    EDIT_UpdateText(es, NULL, TRUE);
} // End EDIT_EM_SetPasswordChar


/*********************************************************************
 *
 *  EDIT_EM_SetSel
 *
 *  note:   unlike the specs say: the order of start and end
 *      _is_ preserved in Windows.  (i.e. start can be > end)
 *      In other words: this handler is OK
 *
 */
static void EDIT_EM_SetSel(EDITSTATE *es, UINT start, UINT end, BOOL after_wrap)
{
    UINT old_start = es->selection_start;
    UINT old_end = es->selection_end;
    UINT len = get_text_length(es);

    if (start == (UINT)-1) {
        start = es->selection_end;
        end = es->selection_end;
    } else {
        start = min(start, len);
        end = min(end, len);
    }
    es->selection_start = start;
    es->selection_end = end;
    if (after_wrap)
        es->flags |= EF_AFTER_WRAP;
    else
        es->flags &= ~EF_AFTER_WRAP;
    /* Compute the necessary invalidation region. */
    /* Note that we don't need to invalidate regions which have
     * "never" been selected, or those which are "still" selected.
     * In fact, every time we hit a selection boundary, we can
     * *toggle* whether we need to invalidate.  Thus we can optimize by
     * *sorting* the interval endpoints.  Let's assume that we sort them
     * in this order:
     *        start <= end <= old_start <= old_end
     * Knuth 5.3.1 (p 183) assures us that this can be done optimally
     * in 5 comparisons; i.e. it is impossible to do better than the
     * following: */
        ORDER_UINT(end, old_end);
        ORDER_UINT(start, old_start);
        ORDER_UINT(old_start, old_end);
        ORDER_UINT(start, end);
    /* Note that at this point 'end' and 'old_start' are not in order, but
     * start is definitely the min. and old_end is definitely the max. */
    if (end != old_start)
        {
/*
 * One can also do
 *          ORDER_UINT32(end, old_start);
 *          EDIT_InvalidateText(es, start, end);
 *          EDIT_InvalidateText(es, old_start, old_end);
 * in place of the following if statement.
 * (That would complete the optimal five-comparison four-element sort.)
 */
            if (old_start > end )
            {
                EDIT_InvalidateText(es, start, end);
                EDIT_InvalidateText(es, old_start, old_end);
            }
            else
            {
                EDIT_InvalidateText(es, start, old_start);
                EDIT_InvalidateText(es, end, old_end);
            }
    }
        else EDIT_InvalidateText(es, start, old_end);
} // End EDIT_EM_SetSel


/*********************************************************************
 *
 *  EM_SETTABSTOPS
 *
 */
static BOOL EDIT_EM_SetTabStops(EDITSTATE *es, INT count, LPINT tabs)
{
    if (!(es->style & ES_MULTILINE))
        return FALSE;
        HeapFree(GetProcessHeap(), 0, es->tabs);
    es->tabs_count = count;
    if (!count)
        es->tabs = NULL;
    else {
        es->tabs = HeapAlloc(GetProcessHeap(), 0, count * sizeof(INT));
        memcpy(es->tabs, tabs, count * sizeof(INT));
    }
    return TRUE;
} // End EDIT_EM_SetTabStops


#ifdef _WIN16
/*********************************************************************
 *
 *  EM_SETTABSTOPS16
 *
 */
static BOOL EDIT_EM_SetTabStops16(EDITSTATE *es, INT count, LPINT16 tabs)
{
    if (!(es->style & ES_MULTILINE))
        return FALSE;
        HeapFree(GetProcessHeap(), 0, es->tabs);
    es->tabs_count = count;
    if (!count)
        es->tabs = NULL;
    else {
        INT i;
        es->tabs = HeapAlloc(GetProcessHeap(), 0, count * sizeof(INT));
        for (i = 0 ; i < count ; i++)
            es->tabs[i] = *tabs++;
    }
    return TRUE;
} // End EDIT_EM_SetTabStops16
#endif


/*********************************************************************
 *
 *  EM_SETWORDBREAKPROC
 *
 */
static void EDIT_EM_SetWordBreakProc(EDITSTATE *es, void *wbp)
{
    if (es->word_break_proc == wbp)
        return;

    es->word_break_proc = wbp;
#ifdef _WIN16
    es->word_break_proc16 = NULL;
#endif

    if ((es->style & ES_MULTILINE) && !(es->style & ES_AUTOHSCROLL)) {
        EDIT_BuildLineDefs_ML(es, 0, get_text_length(es), 0, NULL);
        EDIT_UpdateText(es, NULL, TRUE);
    }
} // End EDIT_EM_SetWordBreakProc


#ifdef _WIN16
/*********************************************************************
 *
 *  EM_SETWORDBREAKPROC16
 *
 */
static void EDIT_EM_SetWordBreakProc16(EDITSTATE *es, EDITWORDBREAKPROC16 wbp)
{
    if (es->word_break_proc16 == wbp)
        return;

    es->word_break_proc = NULL;
    es->word_break_proc16 = wbp;
    if ((es->style & ES_MULTILINE) && !(es->style & ES_AUTOHSCROLL)) {
        EDIT_BuildLineDefs_ML(es, 0, get_text_length(es), 0, NULL);
        EDIT_UpdateText(es, NULL, TRUE);
    }
} // End EDIT_EM_SetWordBreakProc16
#endif


/*********************************************************************
 *
 *  EM_UNDO / WM_UNDO
 *
 */
static BOOL EDIT_EM_Undo(EDITSTATE *es)
{
    INT ulength;
    LPWSTR utext;

    /* As per MSDN spec, for a single-line edit control,
       the return value is always TRUE */
    if( es->style & ES_READONLY )
            return !(es->style & ES_MULTILINE);

    ulength = strlenW(es->undo_text);

    utext = HeapAlloc(GetProcessHeap(), 0, (ulength + 1) * sizeof(WCHAR));

    strcpyW(utext, es->undo_text);

//  TRACE("before UNDO:insertion length = %d, deletion buffer = %s\n",
//           es->undo_insert_count, debugstr_w(utext));

    EDIT_EM_SetSel(es, es->undo_position, es->undo_position + es->undo_insert_count, FALSE);
    EDIT_EM_EmptyUndoBuffer(es);
    EDIT_EM_ReplaceSel(es, TRUE, utext, TRUE, TRUE);
    EDIT_EM_SetSel(es, es->undo_position, es->undo_position + es->undo_insert_count, FALSE);
        /* send the notification after the selection start and end are set */
        EDIT_NOTIFY_PARENT(es, EN_CHANGE);
    EDIT_EM_ScrollCaret(es);
    HeapFree(GetProcessHeap(), 0, utext);

//  TRACE("after UNDO:insertion length = %d, deletion buffer = %s\n",
//          es->undo_insert_count, debugstr_w(es->undo_text));
    return TRUE;
}


/* Helper function for WM_CHAR
 *
 * According to an MSDN blog article titled "Just because you're a control
 * doesn't mean that you're necessarily inside a dialog box," multiline edit
 * controls without ES_WANTRETURN would attempt to detect whether it is inside
 * a dialog box or not.
 */
static BOOL EDIT_IsInsideDialog(EDITSTATE *es)
{
#if FALSE       // Because we're running under Windows, we don't maintain the list of handles
    WND *pParent;
    BOOL r = FALSE;
    if (es->hwndParent)
    {
        pParent = WIN_GetPtr(es->hwndParent);
        if (pParent && pParent != WND_OTHER_PROCESS && pParent != WND_DESKTOP)
        {
            if (pParent->flags & WIN_ISDIALOG)
                r = TRUE;
            WIN_ReleasePtr(pParent);
        }
    }
    return r;
#else
    return FALSE;
#endif
} // End EDIT_IsInsideDialog


/*********************************************************************
 *
 *  WM_CHAR
 *
 */
static LRESULT EDIT_WM_Char(EDITSTATE *es, WCHAR c)
{
        BOOL control;

    control = GetKeyState(VK_CONTROL) & 0x8000;

    switch (c) {
    case '\r':
            /* If it's not a multiline edit box, it would be ignored below.
             * For multiline edit without ES_WANTRETURN, we have to make a
             * special case.
             */
            if ((es->style & ES_MULTILINE) && !(es->style & ES_WANTRETURN))
                if (EDIT_IsInsideDialog(es))
        break;
    case '\n':
        if (es->style & ES_MULTILINE) {
            if (es->style & ES_READONLY) {
                EDIT_MoveHome(es, FALSE, FALSE);
                EDIT_MoveDown_ML(es, FALSE);
            } else {
                static const WCHAR cr_lfW[] = {'\r','\n',0};
                EDIT_EM_ReplaceSel(es, TRUE, cr_lfW, TRUE, TRUE);
            }
        }
        break;
    case '\t':
        if ((es->style & ES_MULTILINE) && !(es->style & ES_READONLY))
        {
            static const WCHAR tabW[] = {'\t',0};
                        if (EDIT_IsInsideDialog(es))
                            break;
            EDIT_EM_ReplaceSel(es, TRUE, tabW, TRUE, TRUE);
        }
        break;
    case VK_BACK:
        if (!(es->style & ES_READONLY) && !control) {
            if (es->selection_start != es->selection_end)
                SendMessageW (es->hwndSelf, WM_CLEAR, 0, 0);
////////////////EDIT_WM_Clear(es);
            else {
                /* delete character left of caret */
                EDIT_EM_SetSel(es, (UINT)-1, 0, FALSE);
                EDIT_MoveBackward(es, TRUE);
                SendMessageW (es->hwndSelf, WM_CLEAR, 0, 0);
////////////////EDIT_WM_Clear(es);
            }
        }
        break;
    case 0x03: /* ^C */
        if (!(es->style & ES_PASSWORD))
            SendMessageW(es->hwndSelf, WM_COPY, 0, 0);
        break;
    case 0x16: /* ^V */
        if (!(es->style & ES_READONLY))
            SendMessageW(es->hwndSelf, WM_PASTE, 0, 0);
        break;
    case 0x18: /* ^X */
        if (!((es->style & ES_READONLY) || (es->style & ES_PASSWORD)))
            SendMessageW(es->hwndSelf, WM_CUT, 0, 0);
        break;
    case 0x1A: /* ^Z */
            if (!(es->style & ES_READONLY))
            SendMessageW(es->hwndSelf, WM_UNDO, 0, 0);
        break;

    default:
        /*If Edit control style is ES_NUMBER allow users to key in only numeric values*/
        if( (es->style & ES_NUMBER) && !( c >= '0' && c <= '9') )
            break;

        if (!(es->style & ES_READONLY) && (c >= ' ') && (c != 127)) {
            WCHAR str[2];
            str[0] = c;
            str[1] = '\0';
            EDIT_EM_ReplaceSel(es, TRUE, str, TRUE, TRUE);
        }
        break;
    }

    return 0;
} // End EDIT_WM_Char


/*********************************************************************
 *
 *  WM_COMMAND
 *
 */
static void EDIT_WM_Command(EDITSTATE *es, INT code, INT id, HWND control)
{
    if (code || control)
        return;

    switch (id) {
////    case EM_UNDO:
        case IDM_UNDO:      // Send as WM_xxx message so it can be caught
                            //   in the subclass
            PostMessageW (es->hwndSelf, WM_UNDO, 0, 0);
////////////EDIT_EM_Undo(es);
            break;
        case IDM_REDO:      // Send as WM_xxx message so it can be caught
                            //   in the subclass
            PostMessageW (es->hwndSelf, MYWM_REDO, 0, 0);
            break;
////    case WM_CUT:
        case IDM_CUT:       // Send as WM_xxx message so it can be caught
                            //   in the subclass
            PostMessageW (es->hwndSelf, WM_CUT, 0, 0);
////////////EDIT_WM_Cut(es);
            break;
////    case WM_COPY:
        case IDM_COPY:      // Send as WM_xxx message so it can be caught
                            //   in the subclass
            PostMessageW (es->hwndSelf, WM_COPY, 0, 0);
////////////EDIT_WM_Copy(es);
            break;
////    case WM_PASTE:
        case IDM_PASTE:     // Send as WM_xxx message so it can be caught
                            //   in the subclass
            PostMessageW (es->hwndSelf, WM_PASTE, 0, 0);
////////////EDIT_WM_Paste(es);
            break;
////    case WM_CLEAR:
        case IDM_DELETE:    // Send as WM_xxx message so it can be caught
                            //   in the subclass
            PostMessageW (es->hwndSelf, WM_CLEAR, 0, 0);
////////////EDIT_WM_Clear(es);
            break;
////    case EM_SETSEL:
        case IDM_SELECTALL:
            EDIT_EM_SetSel(es, 0, (UINT)-1, FALSE);
            EDIT_EM_ScrollCaret(es);
            break;
        default:
//          ERR("unknown menu item, please report\n");
            break;
    }
} // End EDIT_WM_Command


/*********************************************************************
 *
 *  WM_CONTEXTMENU
 *
 *  Note: the resource files resource/sysres_??.rc cannot define a
 *      single popup menu.  Hence we use a (dummy) menubar
 *      containing the single popup menu as its first item.
 *
 *  FIXME: the message identifiers have been chosen arbitrarily,
 *      hence we use MF_BYPOSITION.
 *      We might as well use the "real" values (anybody knows ?)
 *      The menu definition is in resources/sysres_??.rc.
 *      Once these are OK, we better use MF_BYCOMMAND here
 *      (as we do in EDIT_WM_Command()).
 *
 */
static void EDIT_WM_ContextMenu (EDITSTATE *es, INT x, INT y)
{
////HMENU menu = LoadMenuA(user32_module, "EDITMENU");
////HMENU popup = GetSubMenu(menu, 0);
    HMENU menu = LoadMenuA(user32_module, (LPSTR) MAKEINTRESOURCE (IDR_SMMENU));
    HMENU popup = GetSubMenu(menu, IDMPOS_SM_EDIT);
    UINT start = es->selection_start;
    UINT end = es->selection_end;

    ORDER_UINT(start, end);

    /* undo */
    EnableMenuItem(popup, IDM_UNDO        , (SendMessageA(es->hwndSelf, EM_CANUNDO, 0, 0) && !(es->style & ES_READONLY) ? MF_ENABLED : MF_GRAYED));
    /* redo */
    EnableMenuItem(popup, IDM_REDO        , ((end - start) && !(es->style & ES_PASSWORD) && !(es->style & ES_READONLY) ? MF_ENABLED : MF_GRAYED));
    /* cut */
    EnableMenuItem(popup, IDM_CUT         , ((end - start) && !(es->style & ES_PASSWORD) && !(es->style & ES_READONLY) ? MF_ENABLED : MF_GRAYED));
    /* copy */
    EnableMenuItem(popup, IDM_COPY        , ((end - start) && !(es->style & ES_PASSWORD) ? MF_ENABLED : MF_GRAYED));
    /* copy APL+WIN */
    EnableMenuItem(popup, IDM_COPY_APLWIN , ((end - start) && !(es->style & ES_PASSWORD) ? MF_ENABLED : MF_GRAYED));
    /* copy APL2    */
    EnableMenuItem(popup, IDM_COPY_APL2   , ((end - start) && !(es->style & ES_PASSWORD) ? MF_ENABLED : MF_GRAYED));
    /* copy ISO     */
    EnableMenuItem(popup, IDM_COPY_ISO    , ((end - start) && !(es->style & ES_PASSWORD) ? MF_ENABLED : MF_GRAYED));
    /* copy PC/3270 */
    EnableMenuItem(popup, IDM_COPY_PC3270 , ((end - start) && !(es->style & ES_PASSWORD) ? MF_ENABLED : MF_GRAYED));
    /* copy Braces */
    EnableMenuItem(popup, IDM_COPY_BRACES , ((end - start) && !(es->style & ES_PASSWORD) ? MF_ENABLED : MF_GRAYED));
    /* paste */
    EnableMenuItem(popup, IDM_PASTE       , ((IsClipboardFormatAvailable (CF_TEXT) || IsClipboardFormatAvailable (CF_PRIVATEFIRST)) && !(es->style & ES_READONLY) ? MF_ENABLED : MF_GRAYED));
    /* paste APL+WIN */
    EnableMenuItem(popup, IDM_PASTE_APLWIN, ((IsClipboardFormatAvailable (CF_TEXT) || IsClipboardFormatAvailable (CF_PRIVATEFIRST)) && !(es->style & ES_READONLY) ? MF_ENABLED : MF_GRAYED));
    /* paste APL2 */
    EnableMenuItem(popup, IDM_PASTE_APL2  , ((IsClipboardFormatAvailable (CF_TEXT) || IsClipboardFormatAvailable (CF_PRIVATEFIRST)) && !(es->style & ES_READONLY) ? MF_ENABLED : MF_GRAYED));
    /* paste ISO */
    EnableMenuItem(popup, IDM_PASTE_ISO   , ((IsClipboardFormatAvailable (CF_TEXT) || IsClipboardFormatAvailable (CF_PRIVATEFIRST)) && !(es->style & ES_READONLY) ? MF_ENABLED : MF_GRAYED));
    /* paste PC/3270 */
    EnableMenuItem(popup, IDM_PASTE_PC3270, ((IsClipboardFormatAvailable (CF_TEXT) || IsClipboardFormatAvailable (CF_PRIVATEFIRST)) && !(es->style & ES_READONLY) ? MF_ENABLED : MF_GRAYED));
    /* paste Braces */
    EnableMenuItem(popup, IDM_PASTE_BRACES, ((IsClipboardFormatAvailable (CF_TEXT) || IsClipboardFormatAvailable (CF_PRIVATEFIRST)) && !(es->style & ES_READONLY) ? MF_ENABLED : MF_GRAYED));
    /* delete */
    EnableMenuItem(popup, IDM_DELETE      , ((end - start) && !(es->style & ES_READONLY) ? MF_ENABLED : MF_GRAYED));
    /* select all */
    EnableMenuItem(popup, IDM_SELECTALL   , (start || (end != strlenW(es->text)) ? MF_ENABLED : MF_GRAYED));

    // If we're editing a function, ...
    if (IzitFE (es->hwndParent))
    {
        BOOL bItsaName;

        bItsaName = SendMessageA (es->hwndSelf, MYWM_IZITNAME, x, y);

        AppendMenuA (popup, MF_SEPARATOR                           , 0             , NULL);
        AppendMenuA (popup, MF_STRING | (bItsaName ? 0 : MF_GRAYED), IDM_LOCALIZE  , "&Localize");
        AppendMenuA (popup, MF_STRING | (bItsaName ? 0 : MF_GRAYED), IDM_UNLOCALIZE, "Unl&ocalize");
    } // End IF

        if (x == -1 && y == -1) /* passed via VK_APPS press/release */
        {
            RECT rc;
            /* Windows places the menu at the edit's center in this case */
            GetClientRect(es->hwndSelf, &rc);
            MapWindowPoints(es->hwndSelf, 0, (POINT *)&rc, 2);
            x = rc.left + (rc.right - rc.left) / 2;
            y = rc.top + (rc.bottom - rc.top) / 2;
        }

    TrackPopupMenu (popup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, 0, es->hwndSelf, NULL);
    DestroyMenu (menu);
    PostMessageW (GetParent (es->hwndSelf), MYWM_SETFOCUS, 0, 0);
} // End EDIT_WM_ContextMenu


/*********************************************************************
 *
 *  WM_COPY
 *
 */
static void EDIT_WM_Copy(EDITSTATE *es)
{
    INT s = min(es->selection_start, es->selection_end);
    INT e = max(es->selection_start, es->selection_end);
    HGLOBAL hdst;
    LPWSTR dst;
    DWORD len;

    if (e == s) return;

    len = e - s;
    hdst = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (len + 1) * sizeof(WCHAR));
    dst = GlobalLock(hdst);
    memcpy(dst, es->text + s, len * sizeof(WCHAR));
    dst[len] = 0; /* ensure 0 termination */
//  TRACE("%s\n", debugstr_w(dst));
    GlobalUnlock(hdst);
    OpenClipboard(es->hwndSelf);
    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, hdst);
    CloseClipboard();
} // End EDIT_WM_Copy


/*********************************************************************
 *
 *  WM_CREATE
 *
 */
static LRESULT EDIT_WM_Create(EDITSTATE *es, LPCWSTR name)
{
        RECT clientRect;

//  TRACE("%s\n", debugstr_w(name));
       /*
        *   To initialize some final structure members, we call some helper
        *   functions.  However, since the EDITSTATE is not consistent (i.e.
        *   not fully initialized), we should be very careful which
        *   functions can be called, and in what order.
        */
        EDIT_WM_SetFont(es, 0, FALSE);
        EDIT_EM_EmptyUndoBuffer(es);

        /* We need to calculate the format rect
           (applications may send EM_SETMARGINS before the control gets visible) */
        GetClientRect(es->hwndSelf, &clientRect);
        EDIT_SetRectNP(es, &clientRect);

       if (name && *name) {
       EDIT_EM_ReplaceSel(es, FALSE, name, FALSE, FALSE);
       /* if we insert text to the editline, the text scrolls out
            * of the window, as the caret is placed after the insert
            * pos normally; thus we reset es->selection... to 0 and
            * update caret
            */
       es->selection_start = es->selection_end = 0;
           /* Adobe Photoshop does NOT like this. and MSDN says that EN_CHANGE
            * Messages are only to be sent when the USER does something to
            * change the contents. So I am removing this EN_CHANGE
            *
            * EDIT_NOTIFY_PARENT(es, EN_CHANGE);
            */
       EDIT_EM_ScrollCaret(es);
       }
       /* force scroll info update */
       EDIT_UpdateScrollInfo(es);
       /* The rule seems to return 1 here for success */
       /* Power Builder masked edit controls will crash  */
       /* if not. */
       /* FIXME: is that in all cases so ? */
       return 1;
} // End EDIT_WM_Create


/*********************************************************************
 *
 *  WM_DESTROY
 *
 */
static LRESULT EDIT_WM_Destroy(EDITSTATE *es)
{
    LINEDEF *pc, *pp;

    if (es->hloc32W) {
        LocalFree(es->hloc32W);
    }
    if (es->hloc32A) {
        LocalFree(es->hloc32A);
    }
#ifdef _WIN16
    if (es->hloc16) {
        STACK16FRAME* stack16 = MapSL(PtrToUlong(NtCurrentTeb()->WOW32Reserved));
        HANDLE16 oldDS = stack16->ds;

        stack16->ds = GetWindowLongPtrW( es->hwndSelf, GWLP_HINSTANCE );
        while (LocalUnlock16(es->hloc16)) ;
        LocalFree16(es->hloc16);
        stack16->ds = oldDS;
    }
#endif

    pc = es->first_line_def;
    while (pc)
    {
        pp = pc->next;
        HeapFree(GetProcessHeap(), 0, pc);
        pc = pp;
    }

        SetWindowLongPtrW( es->hwndSelf, GWLEC_ES, 0 );
    HeapFree(GetProcessHeap(), 0, es);

    return 0;
} // End EDIT_WM_Destroy


/*********************************************************************
 *
 *  WM_GETTEXT
 *
 */
static size_t EDIT_WM_GetText(EDITSTATE *es, INT count, LPWSTR dst, BOOL unicode)
{
    if(!count) return 0;

    if(unicode)
    {
    lstrcpynW(dst, es->text, count);
    return strlenW(dst);
    }
    else
    {
    LPSTR textA = (LPSTR)dst;
    if (!WideCharToMultiByte(CP_ACP, 0, es->text, -1, textA, count, NULL, NULL))
            textA[count - 1] = 0; /* ensure 0 termination */
    return strlen(textA);
    }
} // End EDIT_WM_GetText


/*********************************************************************
 *
 *  WM_HSCROLL
 *
 */
static LRESULT EDIT_WM_HScroll(EDITSTATE *es, INT action, INT pos)
{
    INT dx;
    INT fw;

    if (!(es->style & ES_MULTILINE))
        return 0;

    if (!(es->style & ES_AUTOHSCROLL))
        return 0;

    dx = 0;
    fw = es->format_rect.right - es->format_rect.left;
    switch (action) {
    case SB_LINELEFT:
//      TRACE("SB_LINELEFT\n");
        if (es->x_offset)
            dx = -es->char_width;
        break;
    case SB_LINERIGHT:
//      TRACE("SB_LINERIGHT\n");
        if (es->x_offset < es->text_width)
            dx = es->char_width;
        break;
    case SB_PAGELEFT:
//      TRACE("SB_PAGELEFT\n");
        if (es->x_offset)
            dx = -fw / HSCROLL_FRACTION / es->char_width * es->char_width;
        break;
    case SB_PAGERIGHT:
//      TRACE("SB_PAGERIGHT\n");
        if (es->x_offset < es->text_width)
            dx = fw / HSCROLL_FRACTION / es->char_width * es->char_width;
        break;
    case SB_LEFT:
//      TRACE("SB_LEFT\n");
        if (es->x_offset)
            dx = -es->x_offset;
        break;
    case SB_RIGHT:
//      TRACE("SB_RIGHT\n");
        if (es->x_offset < es->text_width)
            dx = es->text_width - es->x_offset;
        break;
    case SB_THUMBTRACK:
//      TRACE("SB_THUMBTRACK %d\n", pos);
        es->flags |= EF_HSCROLL_TRACK;
        if(es->style & WS_HSCROLL)
            dx = pos - es->x_offset;
        else
        {
            INT fw, new_x;
            /* Sanity check */
            if(pos < 0 || pos > 100) return 0;
            /* Assume default scroll range 0-100 */
            fw = es->format_rect.right - es->format_rect.left;
            new_x = pos * (es->text_width - fw) / 100;
            dx = es->text_width ? (new_x - es->x_offset) : 0;
        }
        break;
    case SB_THUMBPOSITION:
//      TRACE("SB_THUMBPOSITION %d\n", pos);
        es->flags &= ~EF_HSCROLL_TRACK;
        if(GetWindowLongW( es->hwndSelf, GWL_STYLE ) & WS_HSCROLL)
            dx = pos - es->x_offset;
        else
        {
            INT fw, new_x;
            /* Sanity check */
            if(pos < 0 || pos > 100) return 0;
            /* Assume default scroll range 0-100 */
            fw = es->format_rect.right - es->format_rect.left;
            new_x = pos * (es->text_width - fw) / 100;
            dx = es->text_width ? (new_x - es->x_offset) : 0;
        }
        if (!dx) {
            /* force scroll info update */
            EDIT_UpdateScrollInfo(es);
            EDIT_NOTIFY_PARENT(es, EN_HSCROLL);
        }
        break;
    case SB_ENDSCROLL:
//      TRACE("SB_ENDSCROLL\n");
        break;
    /*
     *  FIXME : the next two are undocumented !
     *  Are we doing the right thing ?
     *  At least Win 3.1 Notepad makes use of EM_GETTHUMB this way,
     *  although it's also a regular control message.
     */
#ifdef _WIN16
    case EM_GETTHUMB16:
#endif
    case EM_GETTHUMB: /* this one is used by NT notepad */
    {
        LRESULT ret;
        if(GetWindowLongW( es->hwndSelf, GWL_STYLE ) & WS_HSCROLL)
            ret = GetScrollPos(es->hwndSelf, SB_HORZ);
        else
        {
            /* Assume default scroll range 0-100 */
            INT fw = es->format_rect.right - es->format_rect.left;
            ret = es->text_width ? es->x_offset * 100 / (es->text_width - fw) : 0;
        }
//      TRACE("EM_GETTHUMB: returning %ld\n", ret);
        return ret;
    }
#ifdef _WIN16
    case EM_LINESCROLL16:
//      TRACE("EM_LINESCROLL16\n");
        dx = pos;
        break;
#endif

    default:
//      ERR("undocumented WM_HSCROLL action %d (0x%04x), please report\n",
//                  action, action);
        return 0;
    }
    if (dx)
    {
        INT fw = es->format_rect.right - es->format_rect.left;
        /* check if we are going to move too far */
        if(es->x_offset + dx + fw > es->text_width)
        dx = es->text_width - fw - es->x_offset;
        if(dx)
        EDIT_EM_LineScroll_internal(es, dx, 0);
    }
    return 0;
} // End EDIT_WM_HScroll


/*********************************************************************
 *
 *  EDIT_CheckCombo
 *
 */
static BOOL EDIT_CheckCombo(EDITSTATE *es, UINT msg, INT key)
{
   HWND hLBox = es->hwndListBox;
   HWND hCombo;
   BOOL bDropped;
   int  nEUI;

   if (!hLBox)
      return FALSE;

   hCombo   = GetParent(es->hwndSelf);
   bDropped = TRUE;
   nEUI     = 0;

// TRACE_(combo)("[%p]: handling msg %x (%x)\n", es->hwndSelf, msg, key);

   if (key == VK_UP || key == VK_DOWN)
   {
      if (SendMessageW(hCombo, CB_GETEXTENDEDUI, 0, 0))
         nEUI = 1;

      if (msg == WM_KEYDOWN || nEUI)
          bDropped = (BOOL)SendMessageW(hCombo, CB_GETDROPPEDSTATE, 0, 0);
   }

   switch (msg)
   {
      case WM_KEYDOWN:
         if (!bDropped && nEUI && (key == VK_UP || key == VK_DOWN))
         {
            /* make sure ComboLBox pops up */
            SendMessageW(hCombo, CB_SETEXTENDEDUI, FALSE, 0);
            key = VK_F4;
            nEUI = 2;
         }

         SendMessageW(hLBox, WM_KEYDOWN, (WPARAM)key, 0);
         break;

      case WM_SYSKEYDOWN: /* Handle Alt+up/down arrows */
         if (nEUI)
            SendMessageW(hCombo, CB_SHOWDROPDOWN, bDropped ? FALSE : TRUE, 0);
         else
            SendMessageW(hLBox, WM_KEYDOWN, (WPARAM)VK_F4, 0);
         break;
   }

   if(nEUI == 2)
      SendMessageW(hCombo, CB_SETEXTENDEDUI, TRUE, 0);

   return TRUE;
} // End EDIT_CheckCombo


/*********************************************************************
 *
 *  WM_KEYDOWN
 *
 *  Handling of special keys that don't produce a WM_CHAR
 *  (i.e. non-printable keys) & Backspace & Delete
 *
 */
static LRESULT EDIT_WM_KeyDown(EDITSTATE *es, INT key)
{
    BOOL shift;
    BOOL control;

    if (GetKeyState(VK_MENU) & 0x8000)
        return 0;

    shift = GetKeyState(VK_SHIFT) & 0x8000;
    control = GetKeyState(VK_CONTROL) & 0x8000;

    switch (key) {
    case VK_F4:
    case VK_UP:
        if (EDIT_CheckCombo(es, WM_KEYDOWN, key) || key == VK_F4)
            break;

        /* fall through */
    case VK_LEFT:
        if ((es->style & ES_MULTILINE) && (key == VK_UP))
            EDIT_MoveUp_ML(es, shift);
        else
            if (control)
                EDIT_MoveWordBackward(es, shift);
            else
                EDIT_MoveBackward(es, shift);
        break;
    case VK_DOWN:
        if (EDIT_CheckCombo(es, WM_KEYDOWN, key))
            break;
        /* fall through */
    case VK_RIGHT:
        if ((es->style & ES_MULTILINE) && (key == VK_DOWN))
            EDIT_MoveDown_ML(es, shift);
        else if (control)
            EDIT_MoveWordForward(es, shift);
        else
            EDIT_MoveForward(es, shift);
        break;
    case VK_HOME:
        EDIT_MoveHome(es, shift, control);
        break;
    case VK_END:
        EDIT_MoveEnd(es, shift, control);
        break;
    case VK_PRIOR:
        if (es->style & ES_MULTILINE)
            EDIT_MovePageUp_ML(es, shift);
        else
            EDIT_CheckCombo(es, WM_KEYDOWN, key);
        break;
    case VK_NEXT:
        if (es->style & ES_MULTILINE)
            EDIT_MovePageDown_ML(es, shift);
        else
            EDIT_CheckCombo(es, WM_KEYDOWN, key);
        break;
    case VK_DELETE:
        if (!(es->style & ES_READONLY) && !(shift && control)) {
            if (es->selection_start != es->selection_end) {
                if (shift)
                    SendMessageW(es->hwndSelf, WM_CUT, 0, 0);
////////////////////EDIT_WM_Cut(es);
                else
                    SendMessageW (es->hwndSelf, WM_CLEAR, 0, 0);
////////////////////EDIT_WM_Clear(es);
            } else {
                if (shift) {
                    /* delete character left of caret */
                    EDIT_EM_SetSel(es, (UINT)-1, 0, FALSE);
                    EDIT_MoveBackward(es, TRUE);
                    SendMessageW (es->hwndSelf, WM_CLEAR, 0, 0);
////////////////////EDIT_WM_Clear(es);
                } else if (control) {
                    /* delete to end of line */
                    EDIT_EM_SetSel(es, (UINT)-1, 0, FALSE);
                    EDIT_MoveEnd(es, TRUE, FALSE);
                    SendMessageW (es->hwndSelf, WM_CLEAR, 0, 0);
////////////////////EDIT_WM_Clear(es);
                } else {
                    /* delete character right of caret */
                    EDIT_EM_SetSel(es, (UINT)-1, 0, FALSE);
                    EDIT_MoveForward(es, TRUE);
                    SendMessageW (es->hwndSelf, WM_CLEAR, 0, 0);
////////////////////EDIT_WM_Clear(es);
                }
            }
        }
        break;
    case VK_INSERT:
        if (shift) {
            if (!(es->style & ES_READONLY))
                SendMessageW (es->hwndSelf, WM_PASTE, 0, 0);
////////////////EDIT_WM_Paste(es);
        } else if (control)
            SendMessageW (es->hwndSelf, WM_COPY, 0, 0);
////////////EDIT_WM_Copy(es);
        else
        {
            MyCreateCaret (es->hwndSelf, 0, CARETWIDTH, es->line_height);
            ShowCaret(es->hwndSelf);
        }
        break;
    case VK_RETURN:
        /* If the edit doesn't want the return send a message to the default object */
        if(!(es->style & ES_MULTILINE) || !(es->style & ES_WANTRETURN))
        {
        HWND hwndParent;
        DWORD dw;

                if (!EDIT_IsInsideDialog(es)) return 1;
                if (control) break;
                hwndParent = GetParent(es->hwndSelf);
                dw = SendMessageW( hwndParent, DM_GETDEFID, 0, 0 );
        if (HIWORD(dw) == DC_HASDEFID)
        {
            SendMessageW( hwndParent, WM_COMMAND,
                  MAKEWPARAM( LOWORD(dw), BN_CLICKED ),
                  (LPARAM)(INT_PTR)(HANDLE_PTR)GetDlgItem( hwndParent, LOWORD(dw) ) );
        }
                else
                    SendMessageW (hwndParent, WM_COMMAND, GET_WM_COMMAND_MPS (IDOK, (HANDLE_PTR) GetDlgItem (hwndParent, IDOK), 0));
        }
        break;
        case VK_ESCAPE:
        if (!(es->style & ES_MULTILINE))
                SendMessageW(GetParent(es->hwndSelf), WM_COMMAND, GET_WM_COMMAND_MPS (IDCANCEL, (HANDLE_PTR) GetDlgItem (GetParent (es->hwndSelf), IDCANCEL), 0));
            break;
        case VK_TAB:
            SendMessageW(es->hwndParent, WM_NEXTDLGCTL, shift, 0);
            break;
    }
    return 0;
} // End EDIT_WM_KeyDown


/*********************************************************************
 *
 *  WM_KILLFOCUS
 *
 */
static LRESULT EDIT_WM_KillFocus(EDITSTATE *es)
{
    es->flags &= ~EF_FOCUSED;
    DestroyCaret();
    if(!(es->style & ES_NOHIDESEL))
        EDIT_InvalidateText(es, es->selection_start, es->selection_end);
    EDIT_NOTIFY_PARENT(es, EN_KILLFOCUS);
    return 0;
} // End EDIT_WM_KillFocus


/*********************************************************************
 *
 *  WM_LBUTTONDBLCLK
 *
 *  The caret position has been set on the WM_LBUTTONDOWN message
 *
 */
static LRESULT EDIT_WM_LButtonDblClk(EDITSTATE *es)
{
    INT s;
    INT e = es->selection_end;
    INT l;
    INT li;
    INT ll;

    es->bCaptureState = TRUE;
    SetCapture(es->hwndSelf);

    l = EDIT_EM_LineFromChar(es, e);
    li = EDIT_EM_LineIndex(es, l);
    ll = EDIT_EM_LineLength(es, e);
    s = li + EDIT_CallWordBreakProc(es, li, e - li, ll, WB_LEFT);
    e = li + EDIT_CallWordBreakProc(es, li, e - li, ll, WB_RIGHT);
    EDIT_EM_SetSel(es, s, e, FALSE);
    EDIT_EM_ScrollCaret(es);
    es->region_posx = es->region_posy = 0;
    SetTimer(es->hwndSelf, 0, 100, NULL);
    return 0;
} // End EDIT_WM_LButtonDblClk


/*********************************************************************
 *
 *  WM_LBUTTONDOWN
 *
 */
static LRESULT EDIT_WM_LButtonDown(EDITSTATE *es, DWORD keys, INT x, INT y)
{
    INT e;
    BOOL after_wrap;

    es->bCaptureState = TRUE;
    SetCapture(es->hwndSelf);
    EDIT_ConfinePoint(es, &x, &y);
    e = EDIT_CharFromPos(es, x, y, &after_wrap);
    EDIT_EM_SetSel(es, (keys & MK_SHIFT) ? es->selection_start : e, e, after_wrap);
    EDIT_EM_ScrollCaret(es);
    es->region_posx = es->region_posy = 0;
    SetTimer(es->hwndSelf, 0, 100, NULL);

    if (!(es->flags & EF_FOCUSED))
            SetFocus(es->hwndSelf);

    return 0;
} // End EDIT_WM_LButtonDown


/*********************************************************************
 *
 *  WM_LBUTTONUP
 *
 */
static LRESULT EDIT_WM_LButtonUp(EDITSTATE *es)
{
    if (es->bCaptureState) {
        KillTimer(es->hwndSelf, 0);
        if (GetCapture() == es->hwndSelf) ReleaseCapture();
    }
    es->bCaptureState = FALSE;
    return 0;
} // End EDIT_WM_LButtonUp


/*********************************************************************
 *
 *  WM_MBUTTONDOWN
 *
 */
static LRESULT EDIT_WM_MButtonDown(EDITSTATE *es)
{
    SendMessageW(es->hwndSelf, WM_PASTE, 0, 0);
    return 0;
} // End EDIT_WM_MButtonDown


/*********************************************************************
 *
 *  WM_MOUSEMOVE
 *
 */
static LRESULT EDIT_WM_MouseMove(EDITSTATE *es, INT x, INT y)
{
    INT e;
    BOOL after_wrap;
    INT prex, prey;

        /* If the mouse has been captured by process other than the edit control itself,
         * the windows edit controls will not select the strings with mouse move.
         */
        if (!es->bCaptureState || GetCapture() != es->hwndSelf)
        return 0;

    /*
     *  FIXME: gotta do some scrolling if outside client
     *      area.  Maybe reset the timer ?
     */
    prex = x; prey = y;
    EDIT_ConfinePoint(es, &x, &y);
    es->region_posx = (prex < x) ? -1 : ((prex > x) ? 1 : 0);
    es->region_posy = (prey < y) ? -1 : ((prey > y) ? 1 : 0);
    e = EDIT_CharFromPos(es, x, y, &after_wrap);
    EDIT_EM_SetSel(es, es->selection_start, e, after_wrap);
    EDIT_SetCaretPos(es,es->selection_end,es->flags & EF_AFTER_WRAP);
    return 0;
} // End EDIT_WM_MouseMove


/*********************************************************************
 *
 *  WM_NCCREATE
 *
 * See also EDIT_WM_StyleChanged
 */
static LRESULT EDIT_WM_NCCreate(HWND hwnd, LPCREATESTRUCTW lpcs, BOOL unicode)
{
    EDITSTATE *es;
    UINT alloc_size;

//  TRACE("Creating %s edit control, style = %08x\n",
//      unicode ? "Unicode" : "ANSI", lpcs->style);

    if (!(es = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*es))))
        return FALSE;
        SetWindowLongPtrW( hwnd, GWLEC_ES, (INT_PTR) (HANDLE_PTR) es );

       /*
        *      Note: since the EDITSTATE has not been fully initialized yet,
        *            we can't use any API calls that may send
        *            WM_XXX messages before WM_NCCREATE is completed.
        */

    es->is_unicode = unicode;
    es->style = lpcs->style;

        es->bEnableState = !(es->style & WS_DISABLED);

    es->hwndSelf = hwnd;
    /* Save parent, which will be notified by EN_* messages */
    es->hwndParent = lpcs->hwndParent;

    if (es->style & ES_COMBO)
       es->hwndListBox = GetDlgItem(es->hwndParent, ID_CB_LISTBOX);

        /* FIXME: should we handle changes to WS_EX_RIGHT style after creation? */
        if (lpcs->dwExStyle & WS_EX_RIGHT) es->style |= ES_RIGHT;

        /* Number overrides lowercase overrides uppercase (at least it
         * does in Win95).  However I'll bet that ES_NUMBER would be
         * invalid under Win 3.1.
         */
        if (es->style & ES_NUMBER) {
                ; /* do not override the ES_NUMBER */
        }  else if (es->style & ES_LOWERCASE) {
                es->style &= ~ES_UPPERCASE;
        }
    if (es->style & ES_MULTILINE) {
        es->buffer_limit = BUFLIMIT_INITIAL;
        if (es->style & WS_VSCROLL)
            es->style |= ES_AUTOVSCROLL;
        if (es->style & WS_HSCROLL)
            es->style |= ES_AUTOHSCROLL;
        es->style &= ~ES_PASSWORD;
        if ((es->style & ES_CENTER) || (es->style & ES_RIGHT)) {
                        /* Confirmed - RIGHT overrides CENTER */
            if (es->style & ES_RIGHT)
                es->style &= ~ES_CENTER;
            es->style &= ~WS_HSCROLL;
            es->style &= ~ES_AUTOHSCROLL;
        }
    } else {
        es->buffer_limit = BUFLIMIT_INITIAL;
        if ((es->style & ES_RIGHT) && (es->style & ES_CENTER))
            es->style &= ~ES_CENTER;
        es->style &= ~WS_HSCROLL;
        es->style &= ~WS_VSCROLL;
        if (es->style & ES_PASSWORD)
            es->password_char = '*';
    }

    alloc_size = ROUND_TO_GROW((es->buffer_size + 1) * sizeof(WCHAR));
    if(!(es->hloc32W = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, alloc_size)))
        return FALSE;
    es->buffer_size = LocalSize(es->hloc32W)/sizeof(WCHAR) - 1;

    if (!(es->undo_text = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (es->buffer_size + 1) * sizeof(WCHAR))))
        return FALSE;
    es->undo_buffer_size = es->buffer_size;

    if (es->style & ES_MULTILINE)
        if (!(es->first_line_def = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LINEDEF))))
            return FALSE;
    es->line_count = 1;

    /*
     * In Win95 look and feel, the WS_BORDER style is replaced by the
     * WS_EX_CLIENTEDGE style for the edit control. This gives the edit
     * control a nonclient area so we don't need to draw the border.
         * If WS_BORDER without WS_EX_CLIENTEDGE is specified we shouldn't have
         * a nonclient area and we should handle painting the border ourselves.
         *
         * When making modifications please ensure that the code still works
         * for edit controls created directly with style 0x50800000, exStyle 0
         * (which should have a single pixel border)
     */
    if (lpcs->dwExStyle & WS_EX_CLIENTEDGE)
        es->style &= ~WS_BORDER;
        else if (es->style & WS_BORDER)
        SetWindowLongW(hwnd, GWL_STYLE, es->style & ~WS_BORDER);

    return TRUE;
} // End EDIT_WM_NCCreate


/*********************************************************************
 *
 *  WM_PAINT
 *
 */
static void EDIT_WM_Paint(EDITSTATE *es, HDC hdc, long lFlags)
{
    HDC         hDCInc;
    HDC         hDCMem;
    RECT        rcClient;
    HBRUSH      hBrush;
    HBITMAP     hBitmap,
                hBitmapOld;
    PAINTSTRUCT ps;

    // Get the incoming DC
    hDCInc = hdc ? hdc : BeginPaint(es->hwndSelf, &ps);

    // Get the client rectangle
    GetClientRect (es->hwndSelf, &rcClient);

    // Get the background brush
    hBrush = (HBRUSH) (HANDLE_PTR) GetClassLongPtrW (es->hwndSelf, GCL_HBRBACKGROUND);

    // Create a compatible DC and bitmap
    hDCMem = CreateCompatibleDC (hDCInc);
    hBitmap = CreateCompatibleBitmap (hDCInc,
                                      rcClient.right,
                                      rcClient.bottom);
    hBitmapOld = SelectObject (hDCMem, hBitmap);

#define hDCSub  hDCInc      // or hDCMem

    // Handle WM_ERASEBKGND here by filling in the client area
    //   with the class background brush
    FillRect (hDCSub, &rcClient, hBrush);

    // Copy various attributes from the screen DC to the memory DC
    SetBkMode    (hDCMem, GetBkMode    (hDCInc));
    SetBkColor   (hDCMem, GetBkColor   (hDCInc));
    SetTextColor (hDCMem, GetTextColor (hDCInc));

    // Call the original handler
    EDIT_WM_Paint2 (es, hDCSub, hDCInc, lFlags);

#undef  hDCSub

    // Copy the memory DC to the screen DC
////BitBlt (hDCInc,
////        0,
////        0,
////        rcClient.right,
////        rcClient.bottom,
////        hDCMem,
////        0,
////        0,
////        SRCCOPY);
    // Restore the old resources
    SelectObject (hDCMem, hBitmapOld);

    // We no longer need these resources
    DeleteObject (hBitmap); hBitmap = NULL;
    DeleteDC (hDCMem); hDCMem = NULL;

    // If we called BeginPaint at the start, ...
    if (!hdc)
        EndPaint(es->hwndSelf, &ps);
} // End EDIT_WM_Paint


static void EDIT_WM_Paint2(EDITSTATE *es, HDC dc, HDC dcbg, long lFlags)
{
    INT i;
    HFONT old_font = 0;
    RECT rc;
    RECT rcClient;
    RECT rcLine;
    RECT rcRgn;
////HBRUSH brush;
    HBRUSH old_brush;
    INT bw, bh;
    INT yLine = 0;
    BOOL rev = es->bEnableState &&
                ((es->flags & EF_FOCUSED) ||
                    (es->style & ES_NOHIDESEL));

    GetClientRect(es->hwndSelf, &rcClient);

/////* get the background brush */
////brush = EDIT_NotifyCtlColor(es, dcbg);

    // If we're not printing, ...
    if (!(lFlags & PRF_PRINTCLIENT))
    {
        /* paint the border and the background */
        IntersectClipRect(dcbg, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

        if(es->style & WS_BORDER) {
            bw = GetSystemMetrics(SM_CXBORDER);
            bh = GetSystemMetrics(SM_CYBORDER);
            rc = rcClient;
            if(es->style & ES_MULTILINE) {
                if(es->style & WS_HSCROLL) rc.bottom+=bh;
                if(es->style & WS_VSCROLL) rc.right+=bw;
            }

            /* Draw the frame. Same code as in nonclient.c */
            old_brush = SelectObject(dcbg, GetSysColorBrush(COLOR_WINDOWFRAME));
            PatBlt(dcbg, rc.left, rc.top, rc.right - rc.left, bh, PATCOPY);
            PatBlt(dcbg, rc.left, rc.top, bw, rc.bottom - rc.top, PATCOPY);
            PatBlt(dcbg, rc.left, rc.bottom - 1, rc.right - rc.left, -bw, PATCOPY);
            PatBlt(dcbg, rc.right - 1, rc.top, -bw, rc.bottom - rc.top, PATCOPY);
            SelectObject(dcbg, old_brush);

            /* Keep the border clean */
            IntersectClipRect(dcbg, rc.left+bw, rc.top+bh,
                max(rc.right-bw, rc.left+bw), max(rc.bottom-bh, rc.top+bh));
        }
    } // End IF

    // The following two lines were deleted to reduce screen flicker
////GetClipBox(dc, &rc);
////FillRect(dc, &rc, brush);

    // If we're not printing, ...
    if (!(lFlags & PRF_PRINTCLIENT))
    {
        IntersectClipRect(dc, es->format_rect.left,
                    es->format_rect.top,
                    es->format_rect.right,
                    es->format_rect.bottom);
        if (es->style & ES_MULTILINE)
            IntersectClipRect(dc, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
    } // End IF

    if (es->font)
        old_font = SelectObject(dc, es->font);

    if (!es->bEnableState)
        SetTextColor(dc, GetSysColor(COLOR_GRAYTEXT));
    GetClipBox(dc, &rcRgn);
    if (es->style & ES_MULTILINE) {
        INT vlc = (es->format_rect.bottom - es->format_rect.top) / es->line_height;
        INT nLOP = (rcClient.bottom - rcClient.top) / es->line_height;
        for (i = es->y_offset ; i <= min(es->y_offset + vlc, es->y_offset + es->line_count - 1) ; i++) {
            EDIT_GetLineRect(es, i, 0, -1, &rcLine);
            if (lFlags & PRF_SELECTION  )
            {
                if (LineHasSelection (es, i))
                    EDIT_PaintLine(es, dc, i, rev, lFlags, yLine++);
            } else
            if (lFlags & PRF_CURRENTPAGE)
            {
                if (LineOnCurrentPage (es, i, nLOP))
                    EDIT_PaintLine(es, dc, i, rev, lFlags, yLine++);
            } else
            if (IntersectRect(&rc, &rcRgn, &rcLine))
                EDIT_PaintLine(es, dc, i, rev, lFlags, -1);
        }
    } else {
        EDIT_GetLineRect(es, 0, 0, -1, &rcLine);
        if (IntersectRect(&rc, &rcRgn, &rcLine))
            EDIT_PaintLine(es, dc, 0, rev, lFlags, -1);
    }
    if (es->font)
        SelectObject(dc, old_font);
} // End EDIT_WM_Paint2


/*********************************************************************
 *
 * LineHasSelection
 *
 */

static BOOL LineHasSelection (EDITSTATE *es, INT line)
{
    INT s = es->selection_start;
    INT e = es->selection_end;
    INT li;
    INT ll;
    INT x;
    INT y;
    LRESULT pos;

    pos = EDIT_EM_PosFromChar(es, EDIT_EM_LineIndex(es, line), FALSE);
    x = (short)LOWORD(pos);
    y = (short)HIWORD(pos);
    li = EDIT_EM_LineIndex(es, line);
    ll = EDIT_EM_LineLength(es, li);
    s = min(es->selection_start, es->selection_end);
    e = max(es->selection_start, es->selection_end);
    s = min(li + ll, max(li, s));
    e = min(li + ll, max(li, e));

    return (s != e);
} // End LineHasSelection


/*********************************************************************
 *
 * LineOnCurrentPage
 *
 */

static BOOL LineOnCurrentPage (EDITSTATE *es, INT line, INT nLOP)
{
    return (es->y_offset <= line
         &&                 line < (es->y_offset + nLOP));
} // End LineOnCurrentPage


/*********************************************************************
 *
 *  WM_PASTE
 *
 */
static void EDIT_WM_Paste(EDITSTATE *es)
{
    HGLOBAL hsrc;
    LPWSTR src;

    /* Protect read-only edit control from modification */
    if(es->style & ES_READONLY)
        return;

    OpenClipboard(es->hwndSelf);

    // First try our private format
    if ((hsrc = GetClipboardData(CF_PRIVATEFIRST))) {
        src = (LPWSTR)GlobalLock(hsrc);
        EDIT_EM_ReplaceSel(es, TRUE, src, TRUE, TRUE);
        GlobalUnlock(hsrc);
    } else
    if ((hsrc = GetClipboardData(CF_UNICODETEXT))) {
        src = (LPWSTR)GlobalLock(hsrc);
        EDIT_EM_ReplaceSel(es, TRUE, src, TRUE, TRUE);
        GlobalUnlock(hsrc);
    }
        else if (es->style & ES_PASSWORD) {
            /* clear selected text in password edit box even with empty clipboard */
            const WCHAR empty_strW[] = { 0 };
            EDIT_EM_ReplaceSel(es, TRUE, empty_strW, TRUE, TRUE);
        }
    CloseClipboard();
} // End EDIT_WM_Paste


/*********************************************************************
 *
 *  WM_SETFOCUS
 *
 */
static void EDIT_WM_SetFocus(EDITSTATE *es)
{
    es->flags |= EF_FOCUSED;

        if (!(es->style & ES_NOHIDESEL))
            EDIT_InvalidateText(es, es->selection_start, es->selection_end);

        /* single line edit updates itself */
        if (!(es->style & ES_MULTILINE))
        {
            HDC hdc = GetDC(es->hwndSelf);
            EDIT_WM_Paint(es, hdc, 0);
            ReleaseDC(es->hwndSelf, hdc); hdc = NULL;
        }

    MyCreateCaret(es->hwndSelf, 0, CARETWIDTH, es->line_height);
    EDIT_SetCaretPos(es, es->selection_end,
             es->flags & EF_AFTER_WRAP);
    ShowCaret(es->hwndSelf);
    EDIT_NOTIFY_PARENT(es, EN_SETFOCUS);
} // End EDIT_WM_SetFocus


/*********************************************************************
 *
 *  WM_SETFONT
 *
 * With Win95 look the margins are set to default font value unless
 * the system font (font == 0) is being set, in which case they are left
 * unchanged.
 *
 */
static void EDIT_WM_SetFont(EDITSTATE *es, HFONT font, BOOL redraw)
{
    TEXTMETRICW tm;
    HDC dc;
    HFONT old_font = 0;
    RECT clientRect;

    es->font = font;
    dc = GetDC(es->hwndSelf);
    if (font)
        old_font = SelectObject(dc, font);
    GetTextMetricsW(dc, &tm);
    es->line_height = tm.tmHeight;
    es->char_width = tm.tmAveCharWidth;
    if (font)
        SelectObject(dc, old_font);
    ReleaseDC(es->hwndSelf, dc); dc = NULL;

    /* Reset the format rect and the margins */
    GetClientRect(es->hwndSelf, &clientRect);
    EDIT_SetRectNP(es, &clientRect);
    // The following line was deleted to reduce caret flicker
////EDIT_EM_SetMargins(es, EC_LEFTMARGIN | EC_RIGHTMARGIN,
////           EC_USEFONTINFO, EC_USEFONTINFO, FALSE);

    if (es->style & ES_MULTILINE)
        EDIT_BuildLineDefs_ML(es, 0, get_text_length(es), 0, NULL);
    else
        EDIT_CalcLineWidth_SL(es);

    if (redraw)
        EDIT_UpdateText(es, NULL, TRUE);
    if (es->flags & EF_FOCUSED) {
        MyCreateCaret(es->hwndSelf, 0, CARETWIDTH, es->line_height);
        EDIT_SetCaretPos(es, es->selection_end,
                 es->flags & EF_AFTER_WRAP);
        ShowCaret(es->hwndSelf);
    }
} // End EDIT_WM_SetFont


/*********************************************************************
 *  MyCreateCaret
 *
 *  Create a caret using either the default or parent-specified width
 */

static BOOL MyCreateCaret (HWND hWnd, HBITMAP hBitMap, int nWidth, int nHeight)
{
    NMEDITCTRL nmEC = {0};

    nmEC.nmHdr.hwndFrom = hWnd;
    nmEC.cbSize         = sizeof (nmEC);
    nmEC.nmHdr.idFrom   = GetWindowLongW (hWnd, GWL_ID);
    nmEC.nmHdr.code     = 0;                    // ***FIXME*** -- replace with our own code
    nmEC.lpCaretWidth   = &nWidth;

    // Ask the parent how wide the caret should be
    SendMessageW (GetParent (hWnd), WM_NOTIFY, nmEC.nmHdr.idFrom, (LPARAM) (HANDLE_PTR) &nmEC);

    return CreateCaret (hWnd, hBitMap, nWidth, nHeight);
} // End MyCreateCaret


/*********************************************************************
 *
 *  WM_SETTEXT
 *
 * NOTES
 *  For multiline controls (ES_MULTILINE), reception of WM_SETTEXT triggers:
 *  The modified flag is reset. No notifications are sent.
 *
 *  For single-line controls, reception of WM_SETTEXT triggers:
 *  The modified flag is reset. EN_UPDATE and EN_CHANGE notifications are sent.
 *
 */
static void EDIT_WM_SetText(EDITSTATE *es, LPCWSTR text, BOOL unicode)
{
    LPWSTR textW = NULL;
    if (!unicode && text)
    {
    LPCSTR textA = (LPCSTR)text;
    INT countW = MultiByteToWideChar(CP_ACP, 0, textA, -1, NULL, 0);
        textW = HeapAlloc(GetProcessHeap(), 0, countW * sizeof(WCHAR));
    if (textW)
        MultiByteToWideChar(CP_ACP, 0, textA, -1, textW, countW);
    text = textW;
    }

//  if (es->flags & EF_UPDATE)
    /* fixed this bug once; complain if we see it about to happen again. */
//  ERR("SetSel may generate UPDATE message whose handler may reset "
//      "selection.\n");

    EDIT_EM_SetSel(es, 0, (UINT)-1, FALSE);
    if (text)
    {
//  TRACE("%s\n", debugstr_w(text));
    EDIT_EM_ReplaceSel(es, FALSE, text, FALSE, FALSE);
    if(!unicode)
        HeapFree(GetProcessHeap(), 0, textW);
    }
    else
    {
    static const WCHAR empty_stringW[] = {0};
//  TRACE("<NULL>\n");
    EDIT_EM_ReplaceSel(es, FALSE, empty_stringW, FALSE, FALSE);
    }
    es->x_offset = 0;
    es->flags &= ~EF_MODIFIED;
    EDIT_EM_SetSel(es, 0, 0, FALSE);

    /* Send the notification after the selection start and end have been set
     * edit control doesn't send notification on WM_SETTEXT
     * if it is multiline, or it is part of combobox
     */
    if( !((es->style & ES_MULTILINE) || es->hwndListBox))
    {
        EDIT_NOTIFY_PARENT(es, EN_UPDATE);
        EDIT_NOTIFY_PARENT(es, EN_CHANGE);
    }
    EDIT_EM_ScrollCaret(es);
    EDIT_UpdateScrollInfo(es);
} // End EDIT_WM_SetText


/*********************************************************************
 *
 *  WM_SIZE
 *
 */
static void EDIT_WM_Size(EDITSTATE *es, UINT action, INT width, INT height)
{
    if ((action == SIZE_MAXIMIZED) || (action == SIZE_RESTORED)) {
        RECT    rc;
        RECT    rcClient;
        HDC     hDC, hDCMem;
        HBITMAP hBitmap, hBitmapOld;
        HFONT   hFontOld;
        HBRUSH  hBrush;

        // Get the client rectangle
        GetClientRect (es->hwndSelf, &rcClient);

        // Get a device context
        hDC = GetDC (es->hwndSelf);

        // Get the background brush
        hBrush = EDIT_NotifyCtlColor(es, hDC);

        // Create a compatible DC and bitmap
        hDCMem = CreateCompatibleDC (hDC);
        hBitmap = CreateCompatibleBitmap (hDC,
                                          rcClient.right,
                                          rcClient.bottom);
        hBitmapOld = SelectObject (hDCMem, hBitmap);

        // Handle WM_ERASEBKGND here by filling in the client area
        FillRect (hDCMem, &rcClient, hBrush);

        // Select our font into the memory DC
        hFontOld = SelectObject (hDCMem, GetCurrentObject (hDC, OBJ_FONT));

        // Copy various attributes from the screen DC to the memory DC
        SetBkMode    (hDCMem, GetBkMode    (hDC));
        SetBkColor   (hDCMem, GetBkColor   (hDC));
        SetTextColor (hDCMem, GetTextColor (hDC));

//      TRACE("width = %d, height = %d\n", width, height);
        SetRect(&rc, 0, 0, width, height);
        EDIT_SetRectNP(es, &rc);
        // The following line is replaced by a call to WM_PAINT
        //   in order to reduce screen flicker
////    EDIT_UpdateText(es, NULL, TRUE);
        EDIT_WM_Paint2 (es, hDCMem, hDC, 0);

        // Restore the old resources
        SelectObject (hDCMem, hFontOld);
        SelectObject (hDCMem, hBitmapOld);

        // We no longer need these resources
        DeleteObject (hBitmap); hBitmap = NULL;
        DeleteDC (hDCMem); hDCMem = NULL;
        ReleaseDC (es->hwndSelf, hDC);
    }
} // End EDIT_WM_Size


/*********************************************************************
 *
 *  WM_STYLECHANGED
 *
 * This message is sent by SetWindowLong on having changed either the Style
 * or the extended style.
 *
 * We ensure that the window's version of the styles and the EDITSTATE's agree.
 *
 * See also EDIT_WM_NCCreate
 *
 * It appears that the Windows version of the edit control allows the style
 * (as retrieved by GetWindowLong) to be any value and maintains an internal
 * style variable which will generally be different.  In this function we
 * update the internal style based on what changed in the externally visible
 * style.
 *
 * Much of this content as based upon the MSDN, especially:
 *  Platform SDK Documentation -> User Interface Services ->
 *      Windows User Interface -> Edit Controls -> Edit Control Reference ->
 *      Edit Control Styles
 */
static LRESULT  EDIT_WM_StyleChanged ( EDITSTATE *es, WPARAM which, const STYLESTRUCT *style)
{
        if (GWL_STYLE == which) {
                DWORD style_change_mask;
                DWORD new_style;
                /* Only a subset of changes can be applied after the control
                 * has been created.
                 */
                style_change_mask = ES_UPPERCASE | ES_LOWERCASE |
                                    ES_NUMBER;
                if (es->style & ES_MULTILINE)
                        style_change_mask |= ES_WANTRETURN;

                new_style = style->styleNew & style_change_mask;

                /* Number overrides lowercase overrides uppercase (at least it
                 * does in Win95).  However I'll bet that ES_NUMBER would be
                 * invalid under Win 3.1.
                 */
                if (new_style & ES_NUMBER) {
                        ; /* do not override the ES_NUMBER */
                }  else if (new_style & ES_LOWERCASE) {
                        new_style &= ~ES_UPPERCASE;
                }

                es->style = (es->style & ~style_change_mask) | new_style;
        } else if (GWL_EXSTYLE == which) {
                ; /* FIXME - what is needed here */
        } else {
//              WARN ("Invalid style change %d\n",which);
        }

        return 0;
} // End EDIT_WM_StyleChanged


/*********************************************************************
 *
 *  WM_SYSKEYDOWN
 *
 */
static LRESULT EDIT_WM_SysKeyDown(EDITSTATE *es, INT key, DWORD key_data)
{
    if ((key == VK_BACK) && (key_data & 0x2000)) {
////////if (EDIT_EM_CanUndo(es))
////////    EDIT_EM_Undo(es);
        if (SendMessageA (es->hwndSelf, EM_CANUNDO, 0, 0))
            SendMessageA (es->hwndSelf, WM_UNDO   , 0, 0);
        return 0;
    } else if (key == VK_UP || key == VK_DOWN) {
        if (EDIT_CheckCombo(es, WM_SYSKEYDOWN, key))
            return 0;
    }
    return DefWindowProcW(es->hwndSelf, WM_SYSKEYDOWN, (WPARAM)key, (LPARAM)key_data);
} // End EDIT_WM_SysKeyDown


/*********************************************************************
 *
 *  WM_TIMER
 *
 */
static void EDIT_WM_Timer(EDITSTATE *es)
{
    if (es->region_posx < 0) {
        EDIT_MoveBackward(es, TRUE);
    } else if (es->region_posx > 0) {
        EDIT_MoveForward(es, TRUE);
    }
/*
 *  FIXME: gotta do some vertical scrolling here, like
 *      EDIT_EM_LineScroll(hwnd, 0, 1);
 */
} // End EDIT_WM_Timer


/*********************************************************************
 *
 *  WM_VSCROLL
 *
 */
static LRESULT EDIT_WM_VScroll(EDITSTATE *es, INT action, INT pos)
{
    INT dy;

    if (!(es->style & ES_MULTILINE))
        return 0;

    if (!(es->style & ES_AUTOVSCROLL))
        return 0;

    dy = 0;
    switch (action) {
    case SB_LINEUP:
    case SB_LINEDOWN:
    case SB_PAGEUP:
    case SB_PAGEDOWN:
//      TRACE("action %d (%s)\n", action, (action == SB_LINEUP ? "SB_LINEUP" :
//                         (action == SB_LINEDOWN ? "SB_LINEDOWN" :
//                          (action == SB_PAGEUP ? "SB_PAGEUP" :
//                           "SB_PAGEDOWN"))));
        EDIT_EM_Scroll(es, action);
        return 0;
    case SB_TOP:
//      TRACE("SB_TOP\n");
        dy = -es->y_offset;
        break;
    case SB_BOTTOM:
//      TRACE("SB_BOTTOM\n");
        dy = es->line_count - 1 - es->y_offset;
        break;
    case SB_THUMBTRACK:
//      TRACE("SB_THUMBTRACK %d\n", pos);
        es->flags |= EF_VSCROLL_TRACK;
        if(es->style & WS_VSCROLL)
            dy = pos - es->y_offset;
        else
        {
            /* Assume default scroll range 0-100 */
            INT vlc, new_y;
            /* Sanity check */
            if(pos < 0 || pos > 100) return 0;
            vlc = (es->format_rect.bottom - es->format_rect.top) / es->line_height;
            new_y = pos * (es->line_count - vlc) / 100;
            dy = es->line_count ? (new_y - es->y_offset) : 0;
//          TRACE("line_count=%d, y_offset=%d, pos=%d, dy = %d\n",
//              es->line_count, es->y_offset, pos, dy);
        }
        break;
    case SB_THUMBPOSITION:
//      TRACE("SB_THUMBPOSITION %d\n", pos);
        es->flags &= ~EF_VSCROLL_TRACK;
        if(es->style & WS_VSCROLL)
            dy = pos - es->y_offset;
        else
        {
            /* Assume default scroll range 0-100 */
            INT vlc, new_y;
            /* Sanity check */
            if(pos < 0 || pos > 100) return 0;
            vlc = (es->format_rect.bottom - es->format_rect.top) / es->line_height;
            new_y = pos * (es->line_count - vlc) / 100;
            dy = es->line_count ? (new_y - es->y_offset) : 0;
//          TRACE("line_count=%d, y_offset=%d, pos=%d, dy = %d\n",
//              es->line_count, es->y_offset, pos, dy);
        }
        if (!dy)
        {
            /* force scroll info update */
            EDIT_UpdateScrollInfo(es);
            EDIT_NOTIFY_PARENT(es, EN_VSCROLL);
        }
        break;
    case SB_ENDSCROLL:
//      TRACE("SB_ENDSCROLL\n");
        break;
    /*
     *  FIXME : the next two are undocumented !
     *  Are we doing the right thing ?
     *  At least Win 3.1 Notepad makes use of EM_GETTHUMB this way,
     *  although it's also a regular control message.
     */
#ifdef _WIN16
    case EM_GETTHUMB16:
#endif
    case EM_GETTHUMB: /* this one is used by NT notepad */
    {
        LRESULT ret;
        if(GetWindowLongW( es->hwndSelf, GWL_STYLE ) & WS_VSCROLL)
            ret = GetScrollPos(es->hwndSelf, SB_VERT);
        else
        {
            /* Assume default scroll range 0-100 */
            INT vlc = (es->format_rect.bottom - es->format_rect.top) / es->line_height;
            ret = es->line_count ? es->y_offset * 100 / (es->line_count - vlc) : 0;
        }
//      TRACE("EM_GETTHUMB: returning %ld\n", ret);
        return ret;
    }
#ifdef _WIN16
    case EM_LINESCROLL16:
//      TRACE("EM_LINESCROLL16 %d\n", pos);
        dy = pos;
        break;
#endif

    default:
//      ERR("undocumented WM_VSCROLL action %d (0x%04x), please report\n",
//                  action, action);
        return 0;
    }
    if (dy)
        EDIT_EM_LineScroll(es, 0, dy);
    return 0;
} // End EDIT_WM_VScroll


/*********************************************************************
 *
 *  EDIT_UpdateTextRegion
 *
 */
static void EDIT_UpdateTextRegion(EDITSTATE *es, HRGN hrgn, BOOL bErase)
{
    if (es->flags & EF_UPDATE) {
        es->flags &= ~EF_UPDATE;
        EDIT_NOTIFY_PARENT(es, EN_UPDATE);
    }
    // The following line was changed to reduce screen flicker
////InvalidateRgn(es->hwndSelf, hrgn, bErase);
    InvalidateRgn(es->hwndSelf, hrgn, FALSE);
} // End EDIT_UpdateTextRegion


/*********************************************************************
 *
 *  EDIT_UpdateText
 *
 */
static void EDIT_UpdateText(EDITSTATE *es, LPRECT rc, BOOL bErase)
{
    if (es->flags & EF_UPDATE) {
        es->flags &= ~EF_UPDATE;
        EDIT_NOTIFY_PARENT(es, EN_UPDATE);
    }
    // The following line was changed to reduce screen flicker
////InvalidateRect(es->hwndSelf, rc, bErase);
    InvalidateRect(es->hwndSelf, rc, FALSE);
} // End EDIT_UpdateText


#if USE_IME
/********************************************************************
 *
 * The Following code is to handle inline editing from IMEs
 */

static void EDIT_GetCompositionStr(HIMC hIMC, LPARAM CompFlag, EDITSTATE *es)
{
    LONG buflen;
    LPWSTR lpCompStr = NULL;
    LPSTR lpCompStrAttr = NULL;
    DWORD dwBufLenAttr;

    buflen = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, NULL, 0);

    if (buflen < 0)
    {
        return;
    }

    lpCompStr = HeapAlloc(GetProcessHeap(),0,buflen + sizeof(WCHAR));
    if (!lpCompStr)
    {
//      ERR("Unable to allocate IME CompositionString\n");
        return;
    }

    if (buflen)
        ImmGetCompositionStringW(hIMC, GCS_COMPSTR, lpCompStr, buflen);
    lpCompStr[buflen/sizeof(WCHAR)] = 0;

    if (CompFlag & GCS_COMPATTR)
    {
        /*
         * We do not use the attributes yet. it would tell us what characters
         * are in transition and which are converted or decided upon
         */
        dwBufLenAttr = ImmGetCompositionStringW(hIMC, GCS_COMPATTR, NULL, 0);
        if (dwBufLenAttr)
        {
            dwBufLenAttr ++;
            lpCompStrAttr = HeapAlloc(GetProcessHeap(),0,dwBufLenAttr+1);
            if (!lpCompStrAttr)
            {
//              ERR("Unable to allocate IME Attribute String\n");
                HeapFree(GetProcessHeap(),0,lpCompStr);
                return;
            }
            ImmGetCompositionStringW(hIMC,GCS_COMPATTR, lpCompStrAttr,
                    dwBufLenAttr);
            lpCompStrAttr[dwBufLenAttr] = 0;
        }
        else
            lpCompStrAttr = NULL;
    }

    /* check for change in composition start */
    if (es->selection_end < es->composition_start)
        es->composition_start = es->selection_end;

    /* replace existing selection string */
    es->selection_start = es->composition_start;

    if (es->composition_len > 0)
        es->selection_end = es->composition_start + es->composition_len;
    else
        es->selection_end = es->selection_start;

    EDIT_EM_ReplaceSel(es, FALSE, lpCompStr, TRUE, TRUE);
    es->composition_len = abs(es->composition_start - es->selection_end);

    es->selection_start = es->composition_start;
    es->selection_end = es->selection_start + es->composition_len;

    HeapFree(GetProcessHeap(),0,lpCompStrAttr);
    HeapFree(GetProcessHeap(),0,lpCompStr);
} // End EDIT_GetCompositionStr
#endif

#if USE_IME
static void EDIT_GetResultStr(HIMC hIMC, EDITSTATE *es)
{
    LONG buflen;
    LPWSTR lpResultStr;

    buflen = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, NULL, 0);
    if (buflen <= 0)
    {
        return;
    }

    lpResultStr = HeapAlloc(GetProcessHeap(),0, buflen+sizeof(WCHAR));
    if (!lpResultStr)
    {
//      ERR("Unable to alloc buffer for IME string\n");
        return;
    }

    ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, lpResultStr, buflen);
    lpResultStr[buflen/sizeof(WCHAR)] = 0;

    /* check for change in composition start */
    if (es->selection_end < es->composition_start)
        es->composition_start = es->selection_end;

    es->selection_start = es->composition_start;
    es->selection_end = es->composition_start + es->composition_len;
    EDIT_EM_ReplaceSel(es, TRUE, lpResultStr, TRUE, TRUE);
    es->composition_start = es->selection_end;
    es->composition_len = 0;

    HeapFree(GetProcessHeap(),0,lpResultStr);
} // End EDIT_GetResultStr
#endif

#if USE_IME
static void EDIT_ImeComposition(HWND hwnd, LPARAM CompFlag, EDITSTATE *es)
{
    HIMC hIMC;
    int cursor;

    if (es->composition_len == 0 && es->selection_start != es->selection_end)
    {
        static const WCHAR empty_stringW[] = {0};
        EDIT_EM_ReplaceSel(es, TRUE, empty_stringW, TRUE, TRUE);
        es->composition_start = es->selection_end;
    }

    hIMC = ImmGetContext(hwnd);
    if (!hIMC)
        return;

    if (CompFlag & GCS_RESULTSTR)
        EDIT_GetResultStr(hIMC, es);
    if (CompFlag & GCS_COMPSTR)
        EDIT_GetCompositionStr(hIMC, CompFlag, es);
    cursor = ImmGetCompositionStringW(hIMC, GCS_CURSORPOS, 0, 0);
    ImmReleaseContext(hwnd, hIMC);
    EDIT_SetCaretPos(es, es->selection_start + cursor, es->flags & EF_AFTER_WRAP);
} // End EDIT_ImeComposition
#endif
