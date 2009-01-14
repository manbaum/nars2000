# This makefile generates the .pro files and is
#   called from VC Express as a pre-build event.

.SUFFIXES: .y

WS=WineHQ\                      # Wine source

!ifndef O
!error makefile.pro:  Macro O is not defined.
!endif

!include makefile.src

{}.y{$(O)}.c:
        @$(PARGEN) -o$(O)$(@B).c -v -r all -l %s

{}.c{$(O)}.pro:
        @echo $(@F)
        @php makepro.php %s $@

{$(O)}.c{$(O)}.pro:
        @echo $(@F)
        @php makepro.php %s $@

{$(WS)}.c{$(O)}.pro:
        @echo $(@F)
        @php makepro.php %s $@

ALL:$(SRC:???=pro)

$(O)cs_parse.c:         $(@B).y
$(O)fh_parse.c:         $(@B).y
$(O)pl_parse.c:         $(@B).y
$(O)pn_parse.c:         $(@B).y

