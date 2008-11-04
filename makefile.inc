# Include file common to makefile.w32 and makefile.w64


CFLAGS_WEXE = /nologo /GA /W3 /WX /Zp1 $(CFLAGS_DEBUG) /D $(WDEF) /c /D _CRT_SECURE_NO_WARNINGS
!if "$(WP)" == "64"
CFLAGS_WEXE=$(CFLAGS_WEXE) /Wp64 /GS-
!endif
LFLAGS_WEXE = /NOLOGO /MAP
LIBS_WEXE = GDI32.LIB           \
            KERNEL32.LIB        \
            USER32.LIB          \
            VERSION.LIB         \
            COMDLG32.LIB        \
            SHELL32.LIB         \
            COMCTL32.LIB        \
            WININET.LIB         \
            OLDNAMES.LIB        \
            OLE32.LIB           \
            UUID.LIB            \
            WINSPOOL.LIB        \
            ADVAPI32.LIB        \
!if "$(DEBUG)" == "1"
            QDEBUG32.LIB        \
!endif
            libcmt.lib          \
            libgsl-double.a     \
            libgslcblas-double.a

!if "$(DEBUG)" == "1"
RCFLAGS = /D "DEBUG"
!else
RCFLAGS =
!endif

ASMOPTS=/coff

RUNFLAGS =
DEFFILE = main.DEF
OBJS_EXT =
MAPFILE = main.MAP
!if "$(DEBUG)" == "1"
RCDEFINES = /D $(WDEF) /D BUILD_BETA
!else
RCDEFINES = /D $(WDEF)
!endif

ASM=masm
AFLAGS=/Mu /DDEBUG=$(DEBUG)

!if "$(VS)" == "1"
O=DEBUG\                        # define output directory for intermediate files
B=$(BIN)D$(WP)\                 # ...                         binaries
!else
!if "$(DEBUG)" == "1"
O=RET\D$(WP)\                   # define output directory for intermediate files
B=$(BIN)D$(WP)\                 # ...                         binaries
!else
O=RET\ND$(WP)\                  # define output directory for intermediate files
B=$(BIN)ND$(WP)\                # ...                         binaries
!endif
!endif
OC=RET\COM\                     # Define output directory for common intermediate files

HLP=$(HCDIR)HCRTF.EXE           # Path to HCRTF
FONT=SImPL-Medium.ttf           # Default APL TrueType font


ALL:    DIR                 \
########PRO                 \
        DEP                 \
        EXE                 \
        MAKEVER             \
        MAKEPRO

EXE:                        \
        $(O)$(ENAME).EXE    \
        $(B)$(ENAME).EXE    \
!if "$(DEBUG)" == "0"
        $(R)$(ENAME).EXE    \
        $(PFNE)             \
!endif
        $(BC)$(ENAME).HLP

MAKEVER:                    \
        $(OC)$(MV).exe      \
        $(BC)$(MV).exe      \
        $(ENAME).ver

MAKEPRO:                    \
        $(OC)$(MP).exe      \
        $(BC)$(MP).exe

CLEAN:
        @del /Q $(O)*.*

DEL:
        @del /Q $(O)*.*

DELPRO:
        @del /Q $(O)*.pro

DIR:
        @Set PATH=$(BC)
        @vdir -q -c $(O) $(OC) $(B) $(BC) >NUL
        @Set PATH=$(OLDPATH)

{}.rc{$(O)}.res:
        @Set INCLUDE=$(I98);$(O)
        @if exist $@ del $@
        @$(RC) $(RCFLAGS) $(RCDEFINES) -r -fo$@ %s

{}.y{$(O)}.c:
        @Set INCLUDE=$(I98);$(O)
        bison -o$(O)$(@B).c -v -r all -l %s
########bison -o$(O)$(@B).c           -l %s

{}.c{$(O)}.obj:
        @Set INCLUDE=$(I98);$(O)
        @if exist $@ del $@
        @$(CC) $(CFLAGS_WEXE) /Fo$@ %s

{}.c{$(OC)}.obj:
        @Set INCLUDE=$(I98);$(O)
        @if exist $@ del $@
        @$(CC) $(CFLAGS_WEXE) /Fo$@ %s

{$(O)}.c{$(O)}.obj:
        @Set INCLUDE=$(I98);$(O)
        @if exist $@ del $@
        @$(CC) $(CFLAGS_WEXE) /Fo$@ %s

{$(WS)}.c{$(O)}.obj:
        @Set INCLUDE=$(WS).;$(WS)Include;$(WS)Include\Wine;$(WS)Include\msvcrt;.;$(O);$(SDK)INCLUDE
        @if exist $@ del $@
        @$(CC) $(CFLAGS_WEXE) /Fo$@ /D "UNICODE" %s

####################### DEPENDENCY GENERATION #################

DEP:                    \
$(O)assign.DEP          \
$(O)axisfns.DEP         \
$(O)bjhash.DEP          \
$(O)common.DEP          \
$(O)convert.DEP         \
$(O)crash.DEP           \
$(O)cs_parse.DEP        \
$(O)cs_proc.DEP         \
$(O)customize.DEP       \
$(O)debug.DEP           \
$(O)dispdbg.DEP         \
$(O)display.DEP         \
$(O)dtoa.DEP            \
$(O)editctrl.DEP        \
$(O)editctrl.DEP        \
$(O)editfcn.DEP         \
$(O)editmat.DEP         \
$(O)editvec.DEP         \
$(O)errmsg.DEP          \
$(O)except.DEP          \
$(O)execdfn.DEP         \
$(O)execfns.DEP         \
$(O)execfns.DEP         \
$(O)execmfn.DEP         \
$(O)fastbool.DEP        \
$(O)fh_parse.DEP        \
$(O)free.DEP            \
$(O)g_fmt.DEP           \
$(O)getfns.DEP          \
$(O)goto.DEP            \
$(O)immexec.DEP         \
$(O)inifile.DEP         \
$(O)initdata.DEP        \
$(O)main.DEP            \
$(O)perfmon.DEP         \
$(O)pf_bar.DEP          \
$(O)pf_circle.DEP       \
$(O)pf_colonbar.DEP     \
$(O)pf_comma.DEP        \
$(O)pf_cslope.DEP       \
$(O)pf_cstile.DEP       \
$(O)pf_cstar.DEP        \
$(O)pf_darrow.DEP       \
$(O)pf_dcaret.DEP       \
$(O)pf_deltastile.DEP   \
$(O)pf_domino.DEP       \
$(O)pf_dshoe.DEP        \
$(O)pf_dstile.DEP       \
$(O)pf_dtack.DEP        \
$(O)pf_dtackjot.DEP     \
$(O)pf_epsilon.DEP      \
$(O)pf_equal.DEP        \
$(O)pf_equalund.DEP     \
$(O)pf_index.DEP        \
$(O)pf_iota.DEP         \
$(O)pf_lcaret.DEP       \
$(O)pf_lcaretund.DEP    \
$(O)pf_lshoe.DEP        \
$(O)pf_ltack.DEP        \
$(O)pf_ndcaret.DEP      \
$(O)pf_nequal.DEP       \
$(O)pf_nucaret.DEP      \
$(O)pf_plus.DEP         \
$(O)pf_qdot.DEP         \
$(O)pf_query.DEP        \
$(O)pf_rcaret.DEP       \
$(O)pf_rcaretund.DEP    \
$(O)pf_rho.DEP          \
$(O)pf_rshoe.DEP        \
$(O)pf_rtack.DEP        \
$(O)pf_slash.DEP        \
$(O)pf_slope.DEP        \
$(O)pf_squad.DEP        \
$(O)pf_star.DEP         \
$(O)pf_stile.DEP        \
$(O)pf_tilde.DEP        \
$(O)pf_times.DEP        \
$(O)pf_uarrow.DEP       \
$(O)pf_ucaret.DEP       \
$(O)pf_ustile.DEP       \
$(O)pf_utack.DEP        \
$(O)pf_utackjot.DEP     \
$(O)pl_parse.DEP        \
$(O)po_cmiddot.DEP      \
$(O)po_dieresis.DEP     \
$(O)po_dijot.DEP        \
$(O)po_ditilde.DEP      \
$(O)po_dot.DEP          \
$(O)po_jot.DEP          \
$(O)po_jotdot.DEP       \
$(O)po_slash.DEP        \
$(O)po_slope.DEP        \
$(O)primfns.DEP         \
$(O)primspec.DEP        \
$(O)qf_av.DEP           \
$(O)qf_cr.DEP           \
$(O)qf_dl.DEP           \
$(O)qf_dm.DEP           \
$(O)qf_dr.DEP           \
$(O)qf_ea.DEP           \
$(O)qf_em.DEP           \
$(O)qf_error.DEP        \
$(O)qf_es.DEP           \
$(O)qf_et.DEP           \
$(O)qf_ex.DEP           \
$(O)qf_fx.DEP           \
$(O)qf_lc.DEP           \
$(O)qf_mf.DEP           \
$(O)qf_nc.DEP           \
$(O)qf_nl.DEP           \
$(O)qf_size.DEP         \
$(O)qf_sysid.DEP        \
$(O)qf_sysver.DEP       \
$(O)qf_tc.DEP           \
$(O)qf_tf.DEP           \
$(O)qf_ts.DEP           \
$(O)qf_type.DEP         \
$(O)qf_ucs.DEP          \
$(O)refcnt.DEP          \
$(O)resdebug.DEP        \
$(O)savefcn.DEP         \
$(O)sc_common.DEP       \
$(O)sc_copy.DEP         \
$(O)sc_drop.DEP         \
$(O)sc_erase.DEP        \
$(O)sc_fnov.DEP         \
$(O)sc_in.DEP           \
$(O)sc_inascii.DEP      \
$(O)sc_lib.DEP          \
$(O)sc_load.DEP         \
$(O)sc_out.DEP          \
$(O)sc_reset.DEP        \
$(O)sc_save.DEP         \
$(O)sc_si.DEP           \
$(O)sc_wsid.DEP         \
$(O)sessman.DEP         \
$(O)sort.DEP            \
$(O)strand.DEP          \
$(O)symtab.DEP          \
$(O)symtrans.DEP        \
$(O)syscmds.DEP         \
$(O)sysvars.DEP         \
$(O)tabctrl.DEP         \
$(O)tokenize.DEP        \
$(O)translate.DEP       \
$(O)typemote.DEP        \
$(O)ver.DEP             \
$(O)main.DEV            \
$(O)ver.DEV             \
$(O)version.DEV         \
$(O)yyfns.DEP

{$(O)}.C{$(O)}.DEP:
    @Set INCLUDE=$(I98);$(O)
    @Set PATH=$(BC)
    @     depscan -q -s$(PROJPATH)$(O)=$$(O) %s > $@
    @Set PATH=$(OLDPATH)

{$(WS)}.C{$(O)}.DEP:
    @Set INCLUDE=$(WS).;$(WS)Include;$(WS)Include\Wine;$(WS)Include\msvcrt;.
    @Set PATH=$(BC)
    @     depscan -q -s$(PROJPATH)$(O)=$$(O),$(WS)=$$(WS) %s > $@
    @Set PATH=$(OLDPATH)

{}.C{$(O)}.DEP:
    @Set INCLUDE=$(I98);$(O)
    @Set PATH=$(BC)
    @     depscan -q -s$(PROJPATH)$(O)=$$(O) %s > $@
    @Set PATH=$(OLDPATH)

{}.RC{$(O)}.DEV:
    @Set INCLUDE=$(I98);$(O)
    @Set PATH=$(BC)
    @     depscan -q -s$(PROJPATH)$(O)=$$(O) %s > $@
    @Set PATH=$(OLDPATH)

!if exist ($(O)assign.DEP)
!       include $(O)assign.DEP
!endif
!if exist ($(O)axisfns.DEP)
!       include $(O)axisfns.DEP
!endif
!if exist ($(O)bjhash.DEP)
!       include $(O)bjhash.DEP
!endif
!if exist ($(O)common.DEP)
!       include $(O)common.DEP
!endif
!if exist ($(O)convert.DEP)
!       include $(O)convert.DEP
!endif
!if exist ($(O)crash.DEP)
!       include $(O)crash.DEP
!endif
!if exist ($(O)cs_parse.DEP)
!       include $(O)cs_parse.DEP
!endif
!if exist ($(O)cs_proc.DEP)
!       include $(O)cs_proc.DEP
!endif
!if exist ($(O)customize.DEP)
!       include $(O)customize.DEP
!endif
!if exist ($(O)debug.DEP)
!       include $(O)debug.DEP
!endif
!if exist ($(O)dispdbg.DEP)
!       include $(O)dispdbg.DEP
!endif
!if exist ($(O)display.DEP)
!       include $(O)display.DEP
!endif
!if exist ($(O)dtoa.DEP)
!       include $(O)dtoa.DEP
!endif
!if exist ($(O)editctrl.DEP)
!       include $(O)editctrl.DEP
!endif
!if exist ($(O)editfcn.DEP)
!       include $(O)editfcn.DEP
!endif
!if exist ($(O)editmat.DEP)
!       include $(O)editmat.DEP
!endif
!if exist ($(O)editvec.DEP)
!       include $(O)editvec.DEP
!endif
!if exist ($(O)errmsg.DEP)
!       include $(O)errmsg.DEP
!endif
!if exist ($(O)except.DEP)
!       include $(O)except.DEP
!endif
!if exist ($(O)execdfn.DEP)
!       include $(O)execdfn.DEP
!endif
!if exist ($(O)execfns.DEP)
!       include $(O)execfns.DEP
!endif
!if exist ($(O)execmfn.DEP)
!       include $(O)execmfn.DEP
!endif
!if exist ($(O)fastbool.DEP)
!       include $(O)fastbool.DEP
!endif
!if exist ($(O)fh_parse.DEP)
!       include $(O)fh_parse.DEP
!endif
!if exist ($(O)free.DEP)
!       include $(O)free.DEP
!endif
!if exist ($(O)getreent.DEP)
!       include $(O)getreent.DEP
!endif
!if exist ($(O)g_fmt.DEP)
!       include $(O)g_fmt.DEP
!endif
!if exist ($(O)getfns.DEP)
!       include $(O)getfns.DEP
!endif
!if exist ($(O)goto.DEP)
!       include $(O)goto.DEP
!endif
!if exist ($(O)immexec.DEP)
!       include $(O)immexec.DEP
!endif
!if exist ($(O)inifile.DEP)
!       include $(O)inifile.DEP
!endif
!if exist ($(O)initdata.DEP)
!       include $(O)initdata.DEP
!endif
!if exist ($(O)main.DEP)
!       include $(O)main.DEP
!endif
!if exist ($(O)perfmon.DEP)
!       include $(O)perfmon.DEP
!endif
!if exist ($(O)pf_bar.DEP)
!       include $(O)pf_bar.DEP
!endif
!if exist ($(O)pf_circle.DEP)
!       include $(O)pf_circle.DEP
!endif
!if exist ($(O)pf_colonbar.DEP)
!       include $(O)pf_colonbar.DEP
!endif
!if exist ($(O)pf_comma.DEP)
!       include $(O)pf_comma.DEP
!endif
!if exist ($(O)pf_cslope.DEP)
!       include $(O)pf_cslope.DEP
!endif
!if exist ($(O)pf_cstile.DEP)
!       include $(O)pf_cstile.DEP
!endif
!if exist ($(O)pf_cstar.DEP)
!       include $(O)pf_cstar.DEP
!endif
!if exist ($(O)pf_darrow.DEP)
!       include $(O)pf_darrow.DEP
!endif
!if exist ($(O)pf_dcaret.DEP)
!       include $(O)pf_dcaret.DEP
!endif
!if exist ($(O)pf_deltastile.DEP)
!       include $(O)pf_deltastile.DEP
!endif
!if exist ($(O)pf_domino.DEP)
!       include $(O)pf_domino.DEP
!endif
!if exist ($(O)pf_dshoe.DEP)
!       include $(O)pf_dshoe.DEP
!endif
!if exist ($(O)pf_dstile.DEP)
!       include $(O)pf_dstile.DEP
!endif
!if exist ($(O)pf_dtack.DEP)
!       include $(O)pf_dtack.DEP
!endif
!if exist ($(O)pf_dtackjot.DEP)
!       include $(O)pf_dtackjot.DEP
!endif
!if exist ($(O)pf_epsilon.DEP)
!       include $(O)pf_epsilon.DEP
!endif
!if exist ($(O)pf_equal.DEP)
!       include $(O)pf_equal.DEP
!endif
!if exist ($(O)pf_equalund.DEP)
!       include $(O)pf_equalund.DEP
!endif
!if exist ($(O)pf_index.DEP)
!       include $(O)pf_index.DEP
!endif
!if exist ($(O)pf_iota.DEP)
!       include $(O)pf_iota.DEP
!endif
!if exist ($(O)pf_lcaret.DEP)
!       include $(O)pf_lcaret.DEP
!endif
!if exist ($(O)pf_lcaretund.DEP)
!       include $(O)pf_lcaretund.DEP
!endif
!if exist ($(O)pf_lshoe.DEP)
!       include $(O)pf_lshoe.DEP
!endif
!if exist ($(O)pf_ltack.DEP)
!       include $(O)pf_ltack.DEP
!endif
!if exist ($(O)pf_ndcaret.DEP)
!       include $(O)pf_ndcaret.DEP
!endif
!if exist ($(O)pf_nequal.DEP)
!       include $(O)pf_nequal.DEP
!endif
!if exist ($(O)pf_nucaret.DEP)
!       include $(O)pf_nucaret.DEP
!endif
!if exist ($(O)pf_plus.DEP)
!       include $(O)pf_plus.DEP
!endif
!if exist ($(O)pf_qdot.DEP)
!       include $(O)pf_qdot.DEP
!endif
!if exist ($(O)pf_query.DEP)
!       include $(O)pf_query.DEP
!endif
!if exist ($(O)pf_rcaret.DEP)
!       include $(O)pf_rcaret.DEP
!endif
!if exist ($(O)pf_rcaretund.DEP)
!       include $(O)pf_rcaretund.DEP
!endif
!if exist ($(O)pf_rho.DEP)
!       include $(O)pf_rho.DEP
!endif
!if exist ($(O)pf_rshoe.DEP)
!       include $(O)pf_rshoe.DEP
!endif
!if exist ($(O)pf_rtack.DEP)
!       include $(O)pf_rtack.DEP
!endif
!if exist ($(O)pf_slash.DEP)
!       include $(O)pf_slash.DEP
!endif
!if exist ($(O)pf_slope.DEP)
!       include $(O)pf_slope.DEP
!endif
!if exist ($(O)pf_squad.DEP)
!       include $(O)pf_squad.DEP
!endif
!if exist ($(O)pf_star.DEP)
!       include $(O)pf_star.DEP
!endif
!if exist ($(O)pf_stile.DEP)
!       include $(O)pf_stile.DEP
!endif
!if exist ($(O)pf_tilde.DEP)
!       include $(O)pf_tilde.DEP
!endif
!if exist ($(O)pf_times.DEP)
!       include $(O)pf_times.DEP
!endif
!if exist ($(O)pf_uarrow.DEP)
!       include $(O)pf_uarrow.DEP
!endif
!if exist ($(O)pf_ucaret.DEP)
!       include $(O)pf_ucaret.DEP
!endif
!if exist ($(O)pf_ustile.DEP)
!       include $(O)pf_ustile.DEP
!endif
!if exist ($(O)pf_utack.DEP)
!       include $(O)pf_utack.DEP
!endif
!if exist ($(O)pf_utackjot.DEP)
!       include $(O)pf_utackjot.DEP
!endif
!if exist ($(O)pl_parse.DEP)
!       include $(O)pl_parse.DEP
!endif
!if exist ($(O)po_cmiddot.DEP)
!       include $(O)po_cmiddot.DEP
!endif
!if exist ($(O)po_dieresis.DEP)
!       include $(O)po_dieresis.DEP
!endif
!if exist ($(O)po_dijot.DEP)
!       include $(O)po_dijot.DEP
!endif
!if exist ($(O)po_ditilde.DEP)
!       include $(O)po_ditilde.DEP
!endif
!if exist ($(O)po_dot.DEP)
!       include $(O)po_dot.DEP
!endif
!if exist ($(O)po_jot.DEP)
!       include $(O)po_jot.DEP
!endif
!if exist ($(O)po_jotdot.DEP)
!       include $(O)po_jotdot.DEP
!endif
!if exist ($(O)po_slash.DEP)
!       include $(O)po_slash.DEP
!endif
!if exist ($(O)po_slope.DEP)
!       include $(O)po_slope.DEP
!endif
!if exist ($(O)primfns.DEP)
!       include $(O)primfns.DEP
!endif
!if exist ($(O)primspec.DEP)
!       include $(O)primspec.DEP
!endif
!if exist ($(O)qf_av.DEP)
!       include $(O)qf_av.DEP
!endif
!if exist ($(O)qf_cr.DEP)
!       include $(O)qf_cr.DEP
!endif
!if exist ($(O)qf_dl.DEP)
!       include $(O)qf_dl.DEP
!endif
!if exist ($(O)qf_dm.DEP)
!       include $(O)qf_dm.DEP
!endif
!if exist ($(O)qf_dr.DEP)
!       include $(O)qf_dr.DEP
!endif
!if exist ($(O)qf_ea.DEP)
!       include $(O)qf_ea.DEP
!endif
!if exist ($(O)qf_em.DEP)
!       include $(O)qf_em.DEP
!endif
!if exist ($(O)qf_error.DEP)
!       include $(O)qf_error.DEP
!endif
!if exist ($(O)qf_es.DEP)
!       include $(O)qf_es.DEP
!endif
!if exist ($(O)qf_et.DEP)
!       include $(O)qf_et.DEP
!endif
!if exist ($(O)qf_ex.DEP)
!       include $(O)qf_ex.DEP
!endif
!if exist ($(O)qf_fx.DEP)
!       include $(O)qf_fx.DEP
!endif
!if exist ($(O)qf_lc.DEP)
!       include $(O)qf_lc.DEP
!endif
!if exist ($(O)qf_mf.DEP)
!       include $(O)qf_mf.DEP
!endif
!if exist ($(O)qf_nc.DEP)
!       include $(O)qf_nc.DEP
!endif
!if exist ($(O)qf_nl.DEP)
!       include $(O)qf_nl.DEP
!endif
!if exist ($(O)qf_size.DEP)
!       include $(O)qf_size.DEP
!endif
!if exist ($(O)qf_sysid.DEP)
!       include $(O)qf_sysid.DEP
!endif
!if exist ($(O)qf_sysver.DEP)
!       include $(O)qf_sysver.DEP
!endif
!if exist ($(O)qf_tc.DEP)
!       include $(O)qf_tc.DEP
!endif
!if exist ($(O)qf_tf.DEP)
!       include $(O)qf_tf.DEP
!endif
!if exist ($(O)qf_ts.DEP)
!       include $(O)qf_ts.DEP
!endif
!if exist ($(O)qf_type.DEP)
!       include $(O)qf_type.DEP
!endif
!if exist ($(O)qf_ucs.DEP)
!       include $(O)qf_ucs.DEP
!endif
!if exist ($(O)refcnt.DEP)
!       include $(O)refcnt.DEP
!endif
!if exist ($(O)resdebug.DEP)
!       include $(O)resdebug.DEP
!endif
!if exist ($(O)savefcn.DEP)
!       include $(O)savefcn.DEP
!endif
!if exist ($(O)sc_common.DEP)
!       include $(O)sc_common.DEP
!endif
!if exist ($(O)sc_copy.DEP)
!       include $(O)sc_copy.DEP
!endif
!if exist ($(O)sc_drop.DEP)
!       include $(O)sc_drop.DEP
!endif
!if exist ($(O)sc_erase.DEP)
!       include $(O)sc_erase.DEP
!endif
!if exist ($(O)sc_fnov.DEP)
!       include $(O)sc_fnov.DEP
!endif
!if exist ($(O)sc_in.DEP)
!       include $(O)sc_in.DEP
!endif
!if exist ($(O)sc_inascii.DEP)
!       include $(O)sc_inascii.DEP
!endif
!if exist ($(O)sc_lib.DEP)
!       include $(O)sc_lib.DEP
!endif
!if exist ($(O)sc_load.DEP)
!       include $(O)sc_load.DEP
!endif
!if exist ($(O)sc_out.DEP)
!       include $(O)sc_out.DEP
!endif
!if exist ($(O)sc_reset.DEP)
!       include $(O)sc_reset.DEP
!endif
!if exist ($(O)sc_save.DEP)
!       include $(O)sc_save.DEP
!endif
!if exist ($(O)sc_si.DEP)
!       include $(O)sc_si.DEP
!endif
!if exist ($(O)sc_wsid.DEP)
!       include $(O)sc_wsid.DEP
!endif
!if exist ($(O)sessman.DEP)
!       include $(O)sessman.DEP
!endif
!if exist ($(O)sort.DEP)
!       include $(O)sort.DEP
!endif
!if exist ($(O)strand.DEP)
!       include $(O)strand.DEP
!endif
!if exist ($(O)symtab.DEP)
!       include $(O)symtab.DEP
!endif
!if exist ($(O)symtrans.DEP)
!       include $(O)symtrans.DEP
!endif
!if exist ($(O)syscmds.DEP)
!       include $(O)syscmds.DEP
!endif
!if exist ($(O)sysvars.DEP)
!       include $(O)sysvars.DEP
!endif
!if exist ($(O)tabctrl.DEP)
!       include $(O)tabctrl.DEP
!endif
!if exist ($(O)tokenize.DEP)
!       include $(O)tokenize.DEP
!endif
!if exist ($(O)translate.DEP)
!       include $(O)translate.DEP
!endif
!if exist ($(O)typemote.DEP)
!       include $(O)typemote.DEP
!endif
!if exist ($(O)ver.DEP)
!       include $(O)ver.DEP
!endif
!if exist ($(O)main.DEV)
!       include $(O)main.DEV
!endif
!if exist ($(O)ver.DEV)
!       include $(O)ver.DEV
!endif
!if exist ($(O)version.DEV)
!       include $(O)version.DEV
!endif
!if exist ($(O)yyfns.DEP)
!       include $(O)yyfns.DEP
!endif


####################### PROTOTYPE GENERATION #################

PRO:                    \
DIR                     \
$(O)assign.PRO          \
$(O)axisfns.PRO         \
$(O)bjhash.PRO          \
$(O)common.PRO          \
$(O)convert.PRO         \
$(O)crash.PRO           \
$(O)cs_parse.PRO        \
$(O)cs_proc.PRO         \
$(O)customize.PRO       \
$(O)debug.PRO           \
$(O)dispdbg.PRO         \
$(O)display.PRO         \
$(O)dtoa.PRO            \
$(O)editctrl.PRO        \
$(O)editfcn.PRO         \
$(O)editmat.PRO         \
$(O)editvec.PRO         \
$(O)errmsg.PRO          \
$(O)except.PRO          \
$(O)execdfn.PRO         \
$(O)execfns.PRO         \
$(O)execmfn.PRO         \
$(O)fastbool.PRO        \
$(O)fh_parse.PRO        \
$(O)free.PRO            \
$(O)g_fmt.PRO           \
$(O)getreent.PRO        \
$(O)getfns.PRO          \
$(O)goto.PRO            \
$(O)immexec.PRO         \
$(O)inifile.PRO         \
$(O)initdata.PRO        \
$(O)main.PRO            \
$(O)perfmon.PRO         \
$(O)pf_bar.PRO          \
$(O)pf_circle.PRO       \
$(O)pf_colonbar.PRO     \
$(O)pf_comma.PRO        \
$(O)pf_cslope.PRO       \
$(O)pf_cstile.PRO       \
$(O)pf_cstar.PRO        \
$(O)pf_darrow.PRO       \
$(O)pf_dcaret.PRO       \
$(O)pf_deltastile.PRO   \
$(O)pf_domino.PRO       \
$(O)pf_dshoe.PRO        \
$(O)pf_dstile.PRO       \
$(O)pf_dtack.PRO        \
$(O)pf_dtackjot.PRO     \
$(O)pf_epsilon.PRO      \
$(O)pf_equal.PRO        \
$(O)pf_equalund.PRO     \
$(O)pf_index.PRO        \
$(O)pf_iota.PRO         \
$(O)pf_lcaret.PRO       \
$(O)pf_lcaretund.PRO    \
$(O)pf_lshoe.PRO        \
$(O)pf_ltack.PRO        \
$(O)pf_ndcaret.PRO      \
$(O)pf_nequal.PRO       \
$(O)pf_nucaret.PRO      \
$(O)pf_plus.PRO         \
$(O)pf_qdot.PRO         \
$(O)pf_query.PRO        \
$(O)pf_rcaret.PRO       \
$(O)pf_rcaretund.PRO    \
$(O)pf_rho.PRO          \
$(O)pf_rshoe.PRO        \
$(O)pf_rtack.PRO        \
$(O)pf_slash.PRO        \
$(O)pf_slope.PRO        \
$(O)pf_squad.PRO        \
$(O)pf_star.PRO         \
$(O)pf_stile.PRO        \
$(O)pf_tilde.PRO        \
$(O)pf_times.PRO        \
$(O)pf_uarrow.PRO       \
$(O)pf_ucaret.PRO       \
$(O)pf_ustile.PRO       \
$(O)pf_utack.PRO        \
$(O)pf_utackjot.PRO     \
$(O)pl_parse.PRO        \
$(O)po_cmiddot.PRO      \
$(O)po_dieresis.PRO     \
$(O)po_dijot.PRO        \
$(O)po_ditilde.PRO      \
$(O)po_dot.PRO          \
$(O)po_jot.PRO          \
$(O)po_jotdot.PRO       \
$(O)po_slash.PRO        \
$(O)po_slope.PRO        \
$(O)primfns.PRO         \
$(O)primspec.PRO        \
$(O)qf_av.PRO           \
$(O)qf_cr.PRO           \
$(O)qf_dl.PRO           \
$(O)qf_dm.PRO           \
$(O)qf_dr.PRO           \
$(O)qf_ea.PRO           \
$(O)qf_em.PRO           \
$(O)qf_error.PRO        \
$(O)qf_es.PRO           \
$(O)qf_et.PRO           \
$(O)qf_ex.PRO           \
$(O)qf_fx.PRO           \
$(O)qf_lc.PRO           \
$(O)qf_mf.PRO           \
$(O)qf_nc.PRO           \
$(O)qf_nl.PRO           \
$(O)qf_size.PRO         \
$(O)qf_sysid.PRO        \
$(O)qf_sysver.PRO       \
$(O)qf_tc.PRO           \
$(O)qf_tf.PRO           \
$(O)qf_ts.PRO           \
$(O)qf_type.PRO         \
$(O)qf_ucs.PRO          \
$(O)refcnt.PRO          \
$(O)resdebug.PRO        \
$(O)savefcn.PRO         \
$(O)sc_common.PRO       \
$(O)sc_copy.PRO         \
$(O)sc_drop.PRO         \
$(O)sc_erase.PRO        \
$(O)sc_fnov.PRO         \
$(O)sc_in.PRO           \
$(O)sc_inascii.PRO      \
$(O)sc_lib.PRO          \
$(O)sc_load.PRO         \
$(O)sc_out.PRO          \
$(O)sc_reset.PRO        \
$(O)sc_save.PRO         \
$(O)sc_si.PRO           \
$(O)sc_wsid.PRO         \
$(O)sessman.PRO         \
$(O)sort.PRO            \
$(O)strand.PRO          \
$(O)symtab.PRO          \
$(O)symtrans.PRO        \
$(O)syscmds.PRO         \
$(O)sysvars.PRO         \
$(O)tabctrl.PRO         \
$(O)tokenize.PRO        \
$(O)translate.PRO       \
$(O)typemote.PRO        \
$(O)ver.PRO             \
$(O)yyfns.PRO

{}.C{$(O)}.PRO:
        @Set INCLUDE=$(I98);$(O)
        @echo %s
########@php makepro.php {%s} {$@}
        @$(BC)$(MP) {$(CC)} {$(CFLAGS_WEXE) /Zg /DPROTO} {%s} {$@}

{$(O)}.C{$(O)}.PRO:
        @Set INCLUDE=$(I98);$(O)
        @echo %s
########@php makepro.php {%s} {$@}
        @$(BC)$(MP) {$(CC)} {$(CFLAGS_WEXE) /Zg /DPROTO} {%s} {$@}

{$(WS)}.C{$(O)}.PRO:
        @Set INCLUDE=$(WS).;$(WS)Include;$(WS)Include\Wine;$(WS)Include\msvcrt;.
        @echo %s
########@php makepro.php {%s} {$@}
        @$(BC)$(MP) {$(CC)} {$(CFLAGS_WEXE) /Zg /DPROTO /DUNICODE} {%s} {$@}

########################## UTILITY ROUTINES ##########################

$(OC)DBGBRK.OBJ:        DBGBRK.ASM
        @if exist $@ del $@
        ml /DIS_$(WIN) /c /W3 /nologo $(ASMOPTS) /Fo $@ %s


############################# MAIN APP ROUTINES ######################

ICONS=$(A)About.ico     \
      $(A)Close.ico     \
      $(A)MF.ico        \
      $(A)SM.ico        \
      $(A)DB.ico        \
      $(A)FE.ico        \
      $(A)ME.ico        \
      $(A)VE.ico        \
      $(A)CC.ico        \
      $(A)LineCont.bmp

## Cygwin-specific files
$(O)getreent.OBJ:       $(@B).C         $(GETREENT_DEP)

## FP-specific files
$(O)dtoa.OBJ:           $(@B).C         $(DTOA_DEP)
$(O)g_fmt.OBJ:          $(@B).C         $(G_FMT_DEP)

## Bison-generated files
$(O)cs_parse.C:         $(@B).Y
$(O)cs_parse.OBJ:       $(O)$(@B).C     $(CS_PARSE_DEP)
$(O)fh_parse.C:         $(@B).Y
$(O)fh_parse.OBJ:       $(O)$(@B).C     $(FH_PARSE_DEP)
$(O)pl_parse.C:         $(@B).Y
$(O)pl_parse.OBJ:       $(O)$(@B).C     $(PL_PARSE_DEP)

## WineHQ-specific files
$(O)editctrl.OBJ:       $(WS)$(@B).C    $(EDITCTRL_DEP)

## NARS-specific files
$(O)assign.OBJ:         $(@B).C         $(ASSIGN_DEP)
$(O)axisfns.OBJ:        $(@B).C         $(AXISFNS_DEP)
$(O)bjhash.OBJ:         $(@B).C         $(BJHASH_DEP)
$(O)common.OBJ:         $(@B).C         $(COMMON_DEP)
$(O)convert.OBJ:        $(@B).C         $(CONVERT_DEP)
$(O)crash.OBJ:          $(@B).C         $(CRASH_DEP)
$(O)cs_proc.OBJ:        $(@B).C         $(CS_PROC_DEP)
$(O)customize.OBJ:      $(@B).C         $(CUSTOMIZE_DEP)
$(O)debug.OBJ:          $(@B).C         $(DEBUG_DEP)
$(O)dispdbg.OBJ:        $(@B).C         $(DISPDBG_DEP)
$(O)display.OBJ:        $(@B).C         $(DISPLAY_DEP)
$(O)editfcn.OBJ:        $(@B).C         $(EDITFCN_DEP)
$(O)editmat.OBJ:        $(@B).C         $(EDITMAT_DEP)
$(O)editvec.OBJ:        $(@B).C         $(EDITVEC_DEP)
$(O)errmsg.OBJ:         $(@B).C         $(ERRMSG_DEP)
$(O)except.OBJ:         $(@B).C         $(EXCEPT_DEP)
$(O)execdfn.OBJ:        $(@B).C         $(EXECDFN_DEP)
$(O)execfns.OBJ:        $(@B).C         $(EXECFNS_DEP)
$(O)execmfn.OBJ:        $(@B).C         $(EXECMFN_DEP)
$(O)fastbool.OBJ:       $(@B).C         $(FASTBOOL_DEP)
$(O)free.OBJ:           $(@B).C         $(FREE_DEP)
$(O)getfns.OBJ:         $(@B).C         $(GETFNS_DEP)
$(O)goto.OBJ:           $(@B).C         $(GOTO_DEP)
$(O)immexec.OBJ:        $(@B).C         $(IMMEXEC_DEP)
$(O)inifile.OBJ:        $(@B).C         $(INIFILE_DEP)
$(O)initdata.OBJ:       $(@B).C         $(INITDATA_DEP)
$(O)main.OBJ:           $(@B).C         $(MAIN_DEP)
$(O)perfmon.OBJ:        $(@B).C         $(PERFMON_DEP)
$(O)pf_bar.OBJ:         $(@B).C         $(PF_BAR_DEP)
$(O)pf_circle.OBJ:      $(@B).C         $(PF_CIRCLE_DEP)
$(O)pf_colonbar.OBJ:    $(@B).C         $(PF_COLONBAR_DEP)
$(O)pf_comma.OBJ:       $(@B).C         $(PF_COMMA_DEP)
$(O)pf_cslope.OBJ:      $(@B).C         $(PF_CSLOPE_DEP)
$(O)pf_cstile.OBJ:      $(@B).C         $(PF_CSTILE_DEP)
$(O)pf_cstar.OBJ:       $(@B).C         $(PF_CSTAR_DEP)
$(O)pf_darrow.OBJ:      $(@B).C         $(PF_DARROW_DEP)
$(O)pf_dcaret.OBJ:      $(@B).C         $(PF_DCARET_DEP)
$(O)pf_deltastile.OBJ:  $(@B).C         $(PF_DELTASTILE_DEP)
$(O)pf_domino.OBJ:      $(@B).C         $(PF_DOMINO_DEP)
$(O)pf_dshoe.OBJ:       $(@B).C         $(PF_DSHOE_DEP)
$(O)pf_dstile.OBJ:      $(@B).C         $(PF_DSTILE_DEP)
$(O)pf_dtack.OBJ:       $(@B).C         $(PF_DTACK_DEP)
$(O)pf_dtackjot.OBJ:    $(@B).C         $(PF_DTACKJOT_DEP)
$(O)pf_epsilon.OBJ:     $(@B).C         $(PF_EPSILON_DEP)
$(O)pf_equal.OBJ:       $(@B).C         $(PF_EQUAL_DEP)
$(O)pf_equalund.OBJ:    $(@B).C         $(PF_EQUALUND_DEP)
$(O)pf_index.OBJ:       $(@B).C         $(PF_INDEX_DEP)
$(O)pf_iota.OBJ:        $(@B).C         $(PF_IOTA_DEP)
$(O)pf_lcaret.OBJ:      $(@B).C         $(PF_LCARET_DEP)
$(O)pf_lcaretund.OBJ:   $(@B).C         $(PF_LCARETUND_DEP)
$(O)pf_lshoe.OBJ:       $(@B).C         $(PF_LSHOE_DEP)
$(O)pf_ltack.OBJ:       $(@B).C         $(PF_LTACK_DEP)
$(O)pf_ndcaret.OBJ:     $(@B).C         $(PF_NDCARET_DEP)
$(O)pf_nequal.OBJ:      $(@B).C         $(PF_NEQUAL_DEP)
$(O)pf_nucaret.OBJ:     $(@B).C         $(PF_NUCARET_DEP)
$(O)pf_plus.OBJ:        $(@B).C         $(PF_PLUS_DEP)
$(O)pf_qdot.OBJ:        $(@B).C         $(PF_QDOT_DEP)
$(O)pf_query.OBJ:       $(@B).C         $(PF_QUERY_DEP)
$(O)pf_rcaret.OBJ:      $(@B).C         $(PF_RCARET_DEP)
$(O)pf_rcaretund.OBJ:   $(@B).C         $(PF_RCARETUND_DEP)
$(O)pf_rho.OBJ:         $(@B).C         $(PF_RHO_DEP)
$(O)pf_rshoe.OBJ:       $(@B).C         $(PF_RSHOE_DEP)
$(O)pf_rtack.OBJ:       $(@B).C         $(PF_RTACK_DEP)
$(O)pf_slash.OBJ:       $(@B).C         $(PF_SLASH_DEP)
$(O)pf_slope.OBJ:       $(@B).C         $(PF_SLOPE_DEP)
$(O)pf_squad.OBJ:       $(@B).C         $(PF_SQUAD_DEP)
$(O)pf_star.OBJ:        $(@B).C         $(PF_STAR_DEP)
$(O)pf_stile.OBJ:       $(@B).C         $(PF_STILE_DEP)
$(O)pf_tilde.OBJ:       $(@B).C         $(PF_TILDE_DEP)
$(O)pf_times.OBJ:       $(@B).C         $(PF_TIMES_DEP)
$(O)pf_uarrow.OBJ:      $(@B).C         $(PF_UARROW_DEP)
$(O)pf_ucaret.OBJ:      $(@B).C         $(PF_UCARET_DEP)
$(O)pf_ustile.OBJ:      $(@B).C         $(PF_USTILE_DEP)
$(O)pf_utack.OBJ:       $(@B).C         $(PF_UTACK_DEP)
$(O)pf_utackjot.OBJ:    $(@B).C         $(PF_UTACKJOT_DEP)
$(O)po_cmiddot.OBJ:     $(@B).C         $(PO_CMIDDOT_DEP)
$(O)po_dieresis.OBJ:    $(@B).C         $(PO_DIERESIS_DEP)
$(O)po_dijot.OBJ:       $(@B).C         $(PO_DIJOT_DEP)
$(O)po_ditilde.OBJ:     $(@B).C         $(PO_DITILDE_DEP)
$(O)po_dot.OBJ:         $(@B).C         $(PO_DOT_DEP)
$(O)po_jot.OBJ:         $(@B).C         $(PO_JOT_DEP)
$(O)po_jotdot.OBJ:      $(@B).C         $(PO_JOTDOT_DEP)
$(O)po_slash.OBJ:       $(@B).C         $(PO_SLASH_DEP)
$(O)po_slope.OBJ:       $(@B).C         $(PO_SLOPE_DEP)
$(O)primfns.OBJ:        $(@B).C         $(PRIMFNS_DEP)
$(O)primspec.OBJ:       $(@B).C         $(PRIMSPEC_DEP)
$(O)qf_av.OBJ:          $(@B).C         $(QF_AV_DEP)
$(O)qf_cr.OBJ:          $(@B).C         $(QF_CR_DEP)
$(O)qf_dl.OBJ:          $(@B).C         $(QF_DL_DEP)
$(O)qf_dm.OBJ:          $(@B).C         $(QF_DM_DEP)
$(O)qf_dr.OBJ:          $(@B).C         $(QF_DR_DEP)
$(O)qf_ea.OBJ:          $(@B).C         $(QF_EA_DEP)
$(O)qf_em.OBJ:          $(@B).C         $(QF_EM_DEP)
$(O)qf_error.OBJ:       $(@B).C         $(QF_ERROR_DEP)
$(O)qf_es.OBJ:          $(@B).C         $(QF_ES_DEP)
$(O)qf_et.OBJ:          $(@B).C         $(QF_ET_DEP)
$(O)qf_ex.OBJ:          $(@B).C         $(QF_EX_DEP)
$(O)qf_fx.OBJ:          $(@B).C         $(QF_FX_DEP)
$(O)qf_lc.OBJ:          $(@B).C         $(QF_LC_DEP)
$(O)qf_mf.OBJ:          $(@B).C         $(QF_MF_DEP)
$(O)qf_nc.OBJ:          $(@B).C         $(QF_NC_DEP)
$(O)qf_nl.OBJ:          $(@B).C         $(QF_NL_DEP)
$(O)qf_size.OBJ:        $(@B).C         $(QF_SIZE_DEP)
$(O)qf_sysid.OBJ:       $(@B).C         $(QF_SYSID_DEP)
$(O)qf_sysver.OBJ:      $(@B).C         $(QF_SYSVER_DEP)
$(O)qf_tc.OBJ:          $(@B).C         $(QF_TC_DEP)
$(O)qf_tf.OBJ:          $(@B).C         $(QF_TF_DEP)
$(O)qf_ts.OBJ:          $(@B).C         $(QF_TS_DEP)
$(O)qf_type.OBJ:        $(@B).C         $(QF_TYPE_DEP)
$(O)qf_ucs.OBJ:         $(@B).C         $(QF_UCS_DEP)
$(O)refcnt.OBJ:         $(@B).C         $(REFCNT_DEP)
!if "$(RESDEBUG)" == "1"
$(O)resdebug.OBJ:       $(@B).C         $(RESDEBUG_DEP)
!endif
$(O)savefcn.OBJ:        $(@B).C         $(SAVEFCN_DEP)
$(O)sc_common.OBJ:      $(@B).C         $(SC_COMMON_DEP)
$(O)sc_copy.OBJ:        $(@B).C         $(SC_COPY_DEP)
$(O)sc_drop.OBJ:        $(@B).C         $(SC_DROP_DEP)
$(O)sc_erase.OBJ:       $(@B).C         $(SC_ERASE_DEP)
$(O)sc_fnov.OBJ:        $(@B).C         $(SC_FNOV_DEP)
$(O)sc_in.OBJ:          $(@B).C         $(SC_IN_DEP)
$(O)sc_inascii.OBJ:     $(@B).C         $(SC_INASCII_DEP)
$(O)sc_lib.OBJ:         $(@B).C         $(SC_LIB_DEP)
$(O)sc_load.OBJ:        $(@B).C         $(SC_LOAD_DEP)
$(O)sc_out.OBJ:         $(@B).C         $(SC_OUT_DEP)
$(O)sc_reset.OBJ:       $(@B).C         $(SC_RESET_DEP)
$(O)sc_save.OBJ:        $(@B).C         $(SC_SAVE_DEP)
$(O)sc_si.OBJ:          $(@B).C         $(SC_SI_DEP)
$(O)sc_wsid.OBJ:        $(@B).C         $(SC_WSID_DEP)
$(O)sessman.OBJ:        $(@B).C         $(SESSMAN_DEP)
$(O)sort.OBJ:           $(@B).C         $(SORT_DEP)
$(O)strand.OBJ:         $(@B).C         $(STRAND_DEP)
$(O)symtab.OBJ:         $(@B).C         $(SYMTAB_DEP)
$(O)symtrans.OBJ:       $(@B).C         $(SYMTRANS_DEP)
$(O)syscmds.OBJ:        $(@B).C         $(SYSCMDS_DEP)
$(O)sysvars.OBJ:        $(@B).C         $(SYSVARS_DEP)
$(O)tabctrl.OBJ:        $(@B).C         $(TABCTRL_DEP)
$(O)tokenize.OBJ:       $(@B).C         $(TOKENIZE_DEP)
$(O)translate.OBJ:      $(@B).C         $(TRANSLATE_DEP)
$(O)typemote.OBJ:       $(@B).C         $(TYPEMOTE_DEP)
$(O)ver.OBJ:            $(@B).C         $(VER_DEP)
$(O)yyfns.OBJ:          $(@B).C         $(YYFNS_DEP)

main.RC:                                $(MAIN_RCDEP)   $(ICONS)

ver.RC:                                 $(VER_RCDEP)

$(O)main.RES:           $(@B).RC        $(MAIN_RCDEP)
        @Set INCLUDE=$(I98);$(O)
        @if exist $@ del $@
        @$(RC) $(RCFLAGS) $(RCDEFINES) -r -fo$@ %s

$(BC)$(ENAME).HLP:      $(ENAME).RTF
        $(HLP) -o $@ -xr %s

$(WS)local.h:           editctrl.h   resource.h

# Build the Win32 version
OBJS=                       \
     $(O)assign.OBJ         \
     $(O)axisfns.OBJ        \
     $(O)bjhash.OBJ         \
     $(O)common.OBJ         \
     $(O)convert.OBJ        \
     $(O)crash.OBJ          \
     $(O)cs_parse.OBJ       \
     $(O)cs_proc.OBJ        \
     $(O)customize.OBJ      \
     $(O)debug.OBJ          \
     $(O)dispdbg.OBJ        \
     $(O)display.OBJ        \
     $(O)dtoa.obj           \
     $(O)editctrl.OBJ       \
     $(O)editfcn.OBJ        \
     $(O)editmat.OBJ        \
     $(O)editvec.OBJ        \
     $(O)errmsg.OBJ         \
     $(O)except.OBJ         \
     $(O)execdfn.OBJ        \
     $(O)execfns.OBJ        \
     $(O)execmfn.OBJ        \
     $(O)fastbool.OBJ       \
     $(O)fh_parse.OBJ       \
     $(O)free.OBJ           \
     $(O)g_fmt.obj          \
     $(O)getreent.OBJ       \
     $(O)getfns.obj         \
     $(O)goto.obj           \
     $(O)immexec.OBJ        \
     $(O)inifile.OBJ        \
     $(O)initdata.OBJ       \
     $(O)main.OBJ           \
     $(O)perfmon.OBJ        \
     $(O)pf_bar.OBJ         \
     $(O)pf_circle.OBJ      \
     $(O)pf_colonbar.OBJ    \
     $(O)pf_comma.OBJ       \
     $(O)pf_cslope.OBJ      \
     $(O)pf_cstile.OBJ      \
     $(O)pf_cstar.OBJ       \
     $(O)pf_darrow.OBJ      \
     $(O)pf_dcaret.OBJ      \
     $(O)pf_deltastile.OBJ  \
     $(O)pf_domino.OBJ      \
     $(O)pf_dshoe.OBJ       \
     $(O)pf_dstile.OBJ      \
     $(O)pf_dtack.OBJ       \
     $(O)pf_dtackjot.OBJ    \
     $(O)pf_epsilon.OBJ     \
     $(O)pf_equal.OBJ       \
     $(O)pf_equalund.OBJ    \
     $(O)pf_index.OBJ       \
     $(O)pf_iota.OBJ        \
     $(O)pf_lcaret.OBJ      \
     $(O)pf_lcaretund.OBJ   \
     $(O)pf_lshoe.OBJ       \
     $(O)pf_ltack.OBJ       \
     $(O)pf_ndcaret.OBJ     \
     $(O)pf_nequal.OBJ      \
     $(O)pf_nucaret.OBJ     \
     $(O)pf_plus.OBJ        \
     $(O)pf_qdot.OBJ        \
     $(O)pf_query.OBJ       \
     $(O)pf_rcaret.OBJ      \
     $(O)pf_rcaretund.OBJ   \
     $(O)pf_rho.OBJ         \
     $(O)pf_rshoe.OBJ       \
     $(O)pf_rtack.OBJ       \
     $(O)pf_slash.OBJ       \
     $(O)pf_slope.OBJ       \
     $(O)pf_squad.OBJ       \
     $(O)pf_star.OBJ        \
     $(O)pf_stile.OBJ       \
     $(O)pf_tilde.OBJ       \
     $(O)pf_times.OBJ       \
     $(O)pf_uarrow.OBJ      \
     $(O)pf_ucaret.OBJ      \
     $(O)pf_ustile.OBJ      \
     $(O)pf_utack.OBJ       \
     $(O)pf_utackjot.OBJ    \
     $(O)pl_parse.OBJ       \
     $(O)po_cmiddot.OBJ     \
     $(O)po_dieresis.OBJ    \
     $(O)po_dijot.OBJ       \
     $(O)po_ditilde.OBJ     \
     $(O)po_dot.OBJ         \
     $(O)po_jot.OBJ         \
     $(O)po_jotdot.OBJ      \
     $(O)po_slash.OBJ       \
     $(O)po_slope.OBJ       \
     $(O)primfns.OBJ        \
     $(O)primspec.OBJ       \
     $(O)qf_av.OBJ          \
     $(O)qf_cr.OBJ          \
     $(O)qf_dl.OBJ          \
     $(O)qf_dm.OBJ          \
     $(O)qf_dr.OBJ          \
     $(O)qf_ea.OBJ          \
     $(O)qf_em.OBJ          \
     $(O)qf_error.OBJ       \
     $(O)qf_es.OBJ          \
     $(O)qf_et.OBJ          \
     $(O)qf_ex.OBJ          \
     $(O)qf_fx.OBJ          \
     $(O)qf_lc.OBJ          \
     $(O)qf_mf.OBJ          \
     $(O)qf_nc.OBJ          \
     $(O)qf_nl.OBJ          \
     $(O)qf_size.OBJ        \
     $(O)qf_sysid.OBJ       \
     $(O)qf_sysver.OBJ      \
     $(O)qf_tc.OBJ          \
     $(O)qf_tf.OBJ          \
     $(O)qf_ts.OBJ          \
     $(O)qf_type.OBJ        \
     $(O)qf_ucs.OBJ         \
     $(O)refcnt.OBJ         \
!if "$(RESDEBUG)" == "1"
     $(O)resdebug.OBJ       \
!endif
     $(O)savefcn.OBJ        \
     $(O)sc_common.OBJ      \
     $(O)sc_copy.OBJ        \
     $(O)sc_drop.OBJ        \
     $(O)sc_erase.OBJ       \
     $(O)sc_fnov.OBJ        \
     $(O)sc_in.OBJ          \
     $(O)sc_inascii.OBJ     \
     $(O)sc_lib.OBJ         \
     $(O)sc_load.OBJ        \
     $(O)sc_out.OBJ         \
     $(O)sc_reset.OBJ       \
     $(O)sc_save.OBJ        \
     $(O)sc_si.OBJ          \
     $(O)sc_wsid.OBJ        \
     $(O)sessman.OBJ        \
     $(O)sort.OBJ           \
     $(O)strand.OBJ         \
     $(O)symtab.OBJ         \
     $(O)symtrans.OBJ       \
     $(O)syscmds.OBJ        \
     $(O)sysvars.OBJ        \
     $(O)tabctrl.OBJ        \
     $(O)tokenize.OBJ       \
     $(O)translate.OBJ      \
     $(O)typemote.OBJ       \
     $(O)ver.OBJ            \
     $(O)yyfns.OBJ          \
     $(OC)DBGBRK.OBJ        \
     $(O)main.RES

$(O)$(ENAME).EXE:       $(OBJS)         \
                        $(DEFFILE)      \
                        makefile.inc    \
                        makefile.$(MFEXT)
        @echo >NUL @<<$(O)main.CRF
$(OBJS)
/OUT:$(O)$(ENAME).EXE
/MAP:$(O)$(MAPFILE)
$(LIBS_WEXE) $(LIBS_EXT)
/DEF:$(DEFFILE)
/NODEFAULTLIB
<<KEEP
        @Set LIB=$(L98)
        @if exist $@ del $@
        @$(LD) $(LFLAGS_WEXE) @$(O)main.CRF
        @del $(O)$(@B).exp
        @del $(O)$(@B).lib
        @copy $@ $(ENAME).EXE
        @7za a -tzip $(B)$(ENAME).zip $(ENAME).EXE LICENSE $(FONT)
        @del $(ENAME).EXE
        @copy $(O)$(MAPFILE) $(B)$(@B).MAP >NUL

$(B)$(ENAME).EXE:       $(O)$(ENAME).EXE
        @copy %s $@ >NUL
        @dir /ON $(B)$(ENAME).*|grep "$(ENAME)\."

$(R)$(ENAME).EXE:       $(O)$(ENAME).EXE
        @copy %s $@ >NUL

$(PFNE):                $(O)$(ENAME).EXE
        @copy %s $(PFNE) >NUL


############################# MAKEVER EXECUTABLE #####################

$(OC)$(MV).obj:         $(@B).c

$(OC)$(MV).exe:         $(OC)$(MV).obj
        @echo >NUL @<<$(OC)$(MV).crf
$**
/OUT:$(OC)$(MV).exe
/MAP:$(OC)$(MV).map
kernel32.lib user32.lib version.lib libcmt.lib
/DEF:$(MV).def
/NODEFAULTLIB
<<KEEP
        @Set LIB=$(L98)
        @if exist $@ del $@
        @$(LD) $(LFLAGS_WEXE) @$(OC)$(MV).crf
        @del $(OC)$(@B).exp
        @del $(OC)$(@B).lib

$(BC)$(MV).exe:         $(OC)$(MV).exe
        @copy    %s     $@ >NUL

$(ENAME).ver:           $(O)$(ENAME).exe
        @$(OC)$(MV).exe $(O)$(ENAME).exe


############################# MAKEPRO EXECUTABLE #####################

$(OC)$(MP).obj:         $(@B).c

$(OC)$(MP).exe:         $(OC)$(MP).obj   $(OC)DbgBrk.obj
        @echo >NUL @<<$(OC)$(MP).crf
$**
/OUT:$(OC)$(MP).exe
/MAP:$(OC)$(MP).map
kernel32.lib libcmt.lib user32.lib pcre.lib
/NODEFAULTLIB
/DEF:$(MP).def
<<KEEP
        @Set LIB=$(L98)
        @if exist $@ del $@
        @$(LD) $(LFLAGS_WEXE) @$(OC)$(MP).crf
        @del $(OC)$(@B).exp
        @del $(OC)$(@B).lib

$(BC)$(MP).exe:         $(OC)$(MP).exe
        @copy    %s     $@ >NUL
