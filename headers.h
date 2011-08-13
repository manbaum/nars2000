//***************************************************************************
//  NARS2000 -- All Headers
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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

#ifdef _WIN64
  #pragma pack(push,8)
#else
  #pragma pack(push,4)
#endif

// Disable stack checking
#pragma check_stack(off)

#include "mpfr.h"
#include "macros.h"
#include "enums.h"
#include "defines.h"
#include "types.h"
#include "perfmon.h"
#include "uniscribe.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "resource.h"
#include "datatype.h"
#include "tokens.h"
#include "primfns.h"
#include "sysvars.h"
#include "symtab.h"
#include "pl_parse.h"
#include "Unicode.h"
#include "colornames.h"
#include "syntaxcolors.h"
#include "primspec.h"
#include "unitranshdr.h"
#include "externs.h"
#include "fh_parse.h"
#include "tokenize.h"
#include "execmfn.h"
#include "dictionary.h"
#include "eventtypes.h"
#include "pertab.h"
#include "threads.h"
#include "savefcn.h"
#include "pn_parse.h"
#include "cs_parse.h"
#include "display.h"
#include "grade.h"
#include "sis.h"
#include "editctrl.h"
#include "termcode.h"
#include "qf.h"
#include "fmtspec.h"
#include "primes.h"
#include "mpifns.h"

#pragma pack(pop)

#ifndef __ATTR_SAL
 /* used for msvc code analysis */
 /* http://msdn2.microsoft.com/en-us/library/ms235402.aspx */
#define _In_
#define _In_z_
#define _In_opt_z_
#define _In_opt_
#define _Printf_format_string_
#define _Ret_opt_z_
#define _Ret_z_
#define _Out_opt_
#define _Out_
#define _Check_return_
#define _Inout_
#define _Inout_opt_
#define _In_bytecount_(x)
#define _Out_opt_bytecapcount_(x)
#define _Out_bytecapcount_(x)
#define _Ret_
#define _Post_z_
#define _Out_cap_(x)
#define _Out_z_cap_(x)
#define _Out_ptrdiff_cap_(x)
#define _Out_opt_ptrdiff_cap_(x)
#endif


// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  End of File: headers.h
//***************************************************************************
