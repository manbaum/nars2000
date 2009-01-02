/* default reentrant pointer when multithread enabled */

////#include <_ansi.h>
////#include <reent.h>
////
////struct _reent *
////_DEFUN_VOID(__getreent)
////{
////  return _impure_ptr;
////}

void *__getreent (void)
{
    _asm int 1;
    return (void *)0;
}
