TARGET	= tpips
bin_PROGRAMS=tpips
tpips_SOURCES=tp_yacc.y tp_lex.l tpips.c wrapper.c revisions.c main_tpips.c
tpips_LDADD=../../Libs/libpipslibs.la
tpips_LDFLAGS=$(READLINE_LIBS) $(NCURSES_LIBS)\
	$(NEWGENLIBS_LIBS) $(LINEARLIBS_LIBS)
nodist_include_HEADERS=completion_list.h $(TARGET).h
dist_noinst_SCRIPTS=make_completion_lists.sh


AM_YFLAGS=-d -p tp_
AM_LFLAGS=-p tp_

# tpips completion

completion_list.h :	\
			make_completion_lists.sh \
			../../Documentation/pipsmake/phases.h \
			../../Documentation/pipsmake/resources.h \
			../../Documentation/pipsmake/printable_resources.h \
			../../Documentation/pipsmake/properties.rc
	$(AM_V_GEN)sh $(srcdir)/make_completion_lists.sh \
		../../Documentation/pipsmake/phases.h \
		../../Documentation/pipsmake/resources.h \
		../../Documentation/pipsmake/printable_resources.h \
		../../Documentation/pipsmake/properties.rc \
		> $@

BUILT_SOURCES=$(TARGET).h completion_list.h tp_yacc.h
AM_CPPFLAGS=\
	-I../../Libs/gnulib \
	-I$(top_srcdir)/src/Documentation/constants\
	-I../../Documentation/pipsmake\
	-I../../Documentation/newgen\
	-I../../Libs/newgen\
	-I../../Libs/misc\
	-I../../Libs/ri-util\
	-I../../Libs/pipsdbm\
	-I../../Libs/pipsmake\
	-I../../Libs/properties\
	-I../../Libs/top-level\
	-I../../Libs/preprocessor\
	$(READLINE_CFLAGS) $(NCURSES_CFLAGS)\
	$(NEWGENLIBS_CFLAGS) $(LINEARLIBS_CFLAGS)

include $(top_srcdir)/makes/cproto.mk
DISTCLEANFILES=completion_list.h
CLEANFILES=tp_yacc.[ch] tp_lex.c
