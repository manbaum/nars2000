//****************************************************************************
//  NARS2000 -- Common Prototypes
//****************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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

  #include "afofns.pro"
  #include "assign.pro"
  #include "axisfns.pro"
  #include "bjhash.pro"
  #include "common.pro"
  #include "convert.pro"
  #include "cr_proc.pro"
  #include "crash.pro"
#ifndef DEBUG
  #include "crashserver.pro"
#endif
  #include "cs_parse.pro"
  #include "cs_proc.pro"
  #include "customize.pro"
  #include "DBGBRK.pro"
  #include "debug.pro"
  #include "dictionary.pro"
  #include "dispdbg.pro"
  #include "display.pro"
  #include "dtoa.pro"
  ////lude "editctrl.pro"
  #include "editfcn.pro"
  #include "errmsg.pro"
  #include "except.pro"
  #include "execdfn.pro"
  #include "execfns.pro"
  #include "execmfn.pro"
  #include "fastbool.pro"
  #include "fh_parse.pro"
  #include "fh_proc.pro"
  #include "fmtspec.pro"
  #include "fpcho_com.pro"
  #include "free.pro"
  #include "getreent.pro"
  #include "getfns.pro"
  #include "goto.pro"
  #include "hc_proc.pro"
  #include "hungarian_dbl.pro"
  #include "hungarian_int.pro"
  #include "hungarian_rat.pro"
  #include "hungarian_vfp.pro"
  #include "identfns.pro"
  #include "immexec.pro"
  #include "inifile.pro"
  #include "iniparser.pro"
  #include "initdata.pro"
  #include "main.pro"
  #include "malloc.pro"
  #include "mpci.pro"
  #include "mphi.pro"
  #include "mpoi.pro"
  #include "mpcf.pro"
  #include "mphf.pro"
  #include "mpof.pro"
  #include "mpcho_com.pro"
  #include "mpfns.pro"
  #include "mpifns.pro"
  #include "parseline.pro"
  #include "perfmon.pro"
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
  #include "pf_dotdot.pro"
  #include "pf_dshoe.pro"
  #include "pf_dstile.pro"
  #include "pf_dtack.pro"
  #include "pf_dtackjot.pro"
  #include "pf_epsilon.pro"
  #include "pf_epsund.pro"
  #include "pf_equal.pro"
  #include "pf_equalund.pro"
  #include "pf_index.pro"
  #include "pf_iota.pro"
  #include "pf_iotaund.pro"
  #include "pf_lcaret.pro"
  #include "pf_lcaretund.pro"
  #include "pf_lshoe.pro"
  #include "pf_lshoeund.pro"
  #include "pf_ltack.pro"
  #include "pf_ndcaret.pro"
  #include "pf_nequal.pro"
  #include "pf_nucaret.pro"
  #include "pf_pi.pro"
  #include "pf_plus.pro"
  #include "pf_qdot.pro"
  #include "pf_query.pro"
  #include "pf_rcaret.pro"
  #include "pf_rcaretund.pro"
  #include "pf_rho.pro"
  #include "pf_root.pro"
  #include "pf_rshoe.pro"
  #include "pf_rshoeund.pro"
  #include "pf_rtack.pro"
  #include "pf_section.pro"
  #include "pf_slash.pro"
  #include "pf_slope.pro"
  #include "pf_squad.pro"
  #include "pf_star.pro"
  #include "pf_stile.pro"
  #include "pf_tilde.pro"
  #include "pf_times.pro"
  #include "pf_uarrow.pro"
  #include "pf_ucaret.pro"
  #include "pf_ushoe.pro"
  #include "pf_ustile.pro"
  #include "pf_utack.pro"
  #include "pf_utackjot.pro"
  #include "pl_proc.pro"
  #include "pn_parse.pro"
  #include "pn_proc.pro"
  #include "po_cmiddot.pro"
  #include "po_comb.pro"
  #include "po_dicircle.pro"
  #include "po_didtack.pro"
  #include "po_dieresis.pro"
  #include "po_dijot.pro"
  #include "po_distar.pro"
  #include "po_ditilde.pro"
  #include "po_dot.pro"
  #include "po_jot.pro"
  #include "po_jotdot.pro"
  #include "po_multiset.pro"
  #include "po_quadjot.pro"
  #include "po_slash.pro"
  #include "po_slope.pro"
  #include "po_variant.pro"
  #include "primfns.pro"
  #include "primspec.pro"
  #include "progbar.pro"
  #include "qdebug.pro"
  #include "qf_a.pro"
  #include "qf_at.pro"
  #include "qf_av.pro"
  #include "qf_cr.pro"
  #include "qf_cs.pro"
  #include "qf_d.pro"
  #include "qf_dc.pro"
  #include "qf_dl.pro"
  #include "qf_dr.pro"
  #include "qf_ea.pro"
  #include "qf_ec.pro"
  #include "qf_em.pro"
  #include "qf_error.pro"
  #include "qf_es.pro"
  #include "qf_et.pro"
  #include "qf_ex.pro"
  #include "qf_fmt.pro"
  #include "qf_fx.pro"
  #include "qf_lc.pro"
  #include "qf_mf.pro"
  #include "qf_nc.pro"
  #include "qf_nfns.pro"
  #include "qf_nl.pro"
  #include "qf_si.pro"
  #include "qf_strace.pro"
  #include "qf_sysid.pro"
  #include "qf_sysver.pro"
  #include "qf_t.pro"
  #include "qf_tc.pro"
  #include "qf_tf.pro"
  #include "qf_ts.pro"
  #include "qf_ucs.pro"
  #include "qf_vr.pro"
  #include "qf_wa.pro"
  #include "rebar.pro"
  #include "refcnt.pro"
  #include "savefcn.pro"
  #include "sc_common.pro"
  #include "sc_copy.pro"
#ifdef DEBUG
  #include "sc_debug.pro"
#endif
  #include "sc_drop.pro"
  #include "sc_erase.pro"
  #include "sc_fnov.pro"
  #include "sc_in.pro"
  #include "sc_inascii.pro"
  #include "sc_lib.pro"
  #include "sc_load.pro"
  #include "sc_out.pro"
  #include "sc_reset.pro"
  #include "sc_save.pro"
  #include "sc_si.pro"
  #include "sc_wsid.pro"
  #include "sc_symb.pro"
  #include "sessman.pro"
  #include "sort.pro"
  #include "statusbar.pro"
  #include "strand.pro"
  #include "symtab.pro"
  #include "symtrans.pro"
  #include "syscmds.pro"
  #include "sysvars.pro"
  #include "tabctrl.pro"
  #include "tokenize.pro"
  #include "translate.pro"
  #include "typemote.pro"
  #include "updates.pro"
  #include "ver.pro"
  #include "webcolors.pro"
  #include "yyfns.pro"
  #if RESDEBUG
    #include "resdebug.pro"
  #endif
#endif

//***************************************************************************
//  End of File: compro.h
//***************************************************************************
