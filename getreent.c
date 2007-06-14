/* default reentrant pointer when multithread enabled */

////#include <_ansi.h>
////#include <reent.h>
////
////struct _reent *
////_DEFUN_VOID(__getreent)
////{
////  return _impure_ptr;
////}

extern void DbgBrk(void);

void *__getreent (void)
{
    DbgBrk ();
    return (void *)0;
}
