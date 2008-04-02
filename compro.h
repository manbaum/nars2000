//****************************************************************************
//  NARS2000 -- Common Prototypes
//****************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

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

#ifndef COMPRO_INCLUDED
#define COMPRO_INCLUDED

#include "assign.pro"
#include "axisfns.pro"
#include "bjhash.pro"
#include "common.pro"
#include "convert.pro"
#include "debug.pro"
#include "dfnhdr.pro"
#include "dispdbg.pro"
#include "display.pro"
#include "dtoa.pro"
////lude "editctrl.pro"
#include "editfcn.pro"
#include "editmat.pro"
#include "editvec.pro"
#include "errmsg.pro"
#include "except.pro"
#include "execdfn.pro"
#include "execfns.pro"
#include "execmfn.pro"
#include "fastbool.pro"
#include "free.pro"
#include "g_fmt.pro"
#include "getfns.pro"
#include "goto.pro"
#include "immexec.pro"
#include "initdata.pro"
#include "main.pro"
#include "parse.pro"
#include "pf_bar.pro"
#include "pf_circle.pro"
#include "pf_colonbar.pro"
#include "pf_comma.pro"
#include "pf_cslope.pro"
#include "pf_cstar.pro"
#include "pf_cstile.pro"
#include "pf_darrow.pro"
#include "pf_dcaret.pro"
#include "pf_deltastile.pro"
#include "pf_domino.pro"
#include "pf_dshoe.pro"
#include "pf_dstile.pro"
#include "pf_dtack.pro"
#include "pf_dtackjot.pro"
#include "pf_epsilon.pro"
#include "pf_equal.pro"
#include "pf_equalund.pro"
#include "pf_index.pro"
#include "pf_iota.pro"
#include "pf_lcaret.pro"
#include "pf_lcaretund.pro"
#include "pf_lshoe.pro"
#include "pf_ltack.pro"
#include "pf_ndcaret.pro"
#include "pf_nequal.pro"
#include "pf_nucaret.pro"
#include "pf_plus.pro"
#include "pf_qdot.pro"
#include "pf_query.pro"
#include "pf_rcaret.pro"
#include "pf_rcaretund.pro"
#include "pf_rho.pro"
#include "pf_rshoe.pro"
#include "pf_rtack.pro"
#include "pf_slash.pro"
#include "pf_slope.pro"
#include "pf_squad.pro"
#include "pf_star.pro"
#include "pf_stile.pro"
#include "pf_tilde.pro"
#include "pf_times.pro"
#include "pf_uarrow.pro"
#include "pf_ucaret.pro"
#include "pf_ustile.pro"
#include "pf_utack.pro"
#include "pf_utackjot.pro"
#include "po_cmiddot.pro"
#include "po_dicircle.pro"
#include "po_dieresis.pro"
#include "po_dijot.pro"
#include "po_ditilde.pro"
#include "po_dot.pro"
#include "po_jot.pro"
#include "po_jotdot.pro"
#include "po_slash.pro"
#include "po_slope.pro"
#include "primfns.pro"
#include "primspec.pro"
#include "qf_av.pro"
#include "qf_cr.pro"
#include "qf_dl.pro"
#include "qf_dm.pro"
#include "qf_dr.pro"
#include "qf_em.pro"
#include "qf_error.pro"
#include "qf_es.pro"
#include "qf_et.pro"
#include "qf_ex.pro"
#include "qf_fx.pro"
#include "qf_lc.pro"
#include "qf_nc.pro"
#include "qf_nl.pro"
#include "qf_size.pro"
#include "qf_sysid.pro"
#include "qf_sysver.pro"
#include "qf_tc.pro"
#include "qf_ts.pro"
#include "qf_type.pro"
#include "qf_ucs.pro"
#include "refcnt.pro"
#include "reg.pro"
//nclude "resdebug.pro"
#include "savefcn.pro"
#include "sc_copy.pro"
#include "sc_drop.pro"
#include "sc_erase.pro"
#include "sc_fnov.pro"
#include "sc_lib.pro"
#include "sc_load.pro"
#include "sc_reset.pro"
#include "sc_save.pro"
#include "sc_si.pro"
#include "sc_wsid.pro"
#include "sessman.pro"
#include "sort.pro"
#include "strand.pro"
#include "symtab.pro"
#include "symtrans.pro"
#include "syscmds.pro"
#include "sysvars.pro"
#include "tabctrl.pro"
#include "tokenize.pro"
#include "translate.pro"
#include "typemote.pro"
#include "ver.pro"
#include "yyfns.pro"
#if RESDEBUG
#include "resdebug.pro"
#endif
#include <QDebug.pro>

#endif

//***************************************************************************
//  End of File: compro.h
//***************************************************************************
