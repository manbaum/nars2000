############################# APLCHARS EXTENSION #####################

EXNAM=aplchars                      # Extension name
CHR=chrome\content\$(EXNAM)\        # Directory for chrome files
LCL=chrome\locale\en-US\$(EXNAM)\   # ...           locale ...
SKN=chrome\skin\classic\$(EXNAM)\   # ...           skin   ...
###ZIPPRG=                          # Path and name of zip program (7-zip)

!ifndef ZIPPRG
!error Environment variable ZIPPRG not defined.
!endif

ALL:    $(EXNAM).xpi                        \
        ..\..\htdocs\$(EXNAM).xpi           \
        ..\..\htdocs\update.rdf

chrome\$(EXNAM).jar:    $(CHR)*.*           \
                        $(LCL)*.*           \
                        $(SKN)*.*           \
                        upd.inp
        @php ..\incver.php install.rdf $(CHR)about.xul update.rdf
        @cd chrome
        @if exist $(EXNAM).jar del $(EXNAM).jar
        @$(ZIPPRG) a -tzip -r $(EXNAM).jar *.* -x@..\upd.inp
        @cd ..

$(EXNAM).xpi:           chrome.manifest     \
                        install.rdf         \
                        LICENSE.TXT         \
                        upd.inp             \
                        defaults\*.*        \
                        chrome\$(EXNAM).jar \
                        makefile
        @if exist $(EXNAM).xpi del $(EXNAM).xpi
        @$(ZIPPRG) a -tzip $(EXNAM).xpi *.* -x@upd.inp
        @$(ZIPPRG) a -tzip -r $(EXNAM).xpi defaults\*.*
        @$(ZIPPRG) a -tzip $(EXNAM).xpi chrome\$(EXNAM).jar

..\..\htdocs\$(EXNAM).xpi:  $(EXNAM).xpi
        @copy %s $@ >NUL

..\..\htdocs\update.rdf:    update.rdf
        @copy %s $@ >NUL

