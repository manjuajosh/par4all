#
# $RCSfile: config.makefile,v $ (version $Revision$)
# $Date: 1997/04/10 17:50:23 $, 

SOURCES=	pipsmake-rc.tex \
		make-pips-menu \
		make-builder-map

INSTALL_SHR=	epips_view_menu_layout.el \
		epips_transform_menu_layout.el \
		pipsmake.rc \
		wpips.rc

DERIVED_INC=	resources.h \
		printable_resources.h \
		phases.h \
		builder_map.h \
		wpips_transform_menu_layout.h \
		wpips_view_menu_layout.h

INSTALL_INC=	$(DERIVED_INC)
INSTALL_DOC=	pipsmake-rc.ps
INSTALL_HTM=	pipsmake-rc 

DERIVED_FILES=	$(INSTALL_SHR) $(DERIVED_INC) $(INSTALL_DOC) pipsmake-rc.html

AUTO =	$(PIPS_ROOT)/Include/auto

all: $(DERIVED_FILES)

pipsmake.rc: pipsmake-rc.tex
	#
	# building pipsmake.rc
	#
	sed 's,	,    ,g;s/ *$$//;/^alias /d' $< | filter_verbatim | \
		cat $(AUTO).h - > $@

wpips.rc: pipsmake-rc.tex
	#
	# buidling $@
	#
	{ cat $(AUTO)-dash.h ;\
	  sed 's,	,    ,g;s/ *$$//;/^alias /!d' $< ; } > $@

resources.h: pipsmake.rc
	#
	# building $@
	# 
	cpp $< | \
	sed '/>/!d;s/^.*MODULE\.//;s/^.*PROGRAM\.//;\
		s/^.*ALL\.//;s/^.*MAIN\.//;' | \
	tr '[a-z]' '[A-Z]' | sort -u | sed 's/.*/#define DBR_& "&"/' | \
	cat $(AUTO).h - > $@

printable_resources.h: resources.h
	#
	# building $@
	#
	sed '/define.*FILE/!d' $< | cat $(AUTO).h - > $@

phases.h: pipsmake.rc
	#
	# building $@
	#
	cpp $< | sed '/^[a-z]/!d;s/ .*//g;' | tr '[a-z]' '[A-Z]' | sort -u | \
	sed 's/.*/#define BUILDER_& "&"/' | cat $(AUTO).h - > $@

builder_map.h: pipsmake.rc
	#
	# building builder_map.h
	# 
	{ cat $(AUTO).h ; sh ./make-builder-map < $< ;} > $@


epips_transform_menu_layout.el wpips_transform_menu_layout.h epips_view_menu_layout.el wpips_view_menu_layout.h:  pipsmake-rc.tex
	# 
	# building menu layout files:
	#
	generate_all_menu_documentation -layout < $<

clean: local-clean

local-clean:
	$(RM) -r $(DERIVED_FILES) *.aux *.log *.ind *.idx *.toc *.ilg *.dvi

# end of $RCSfile: config.makefile,v $
#
