#
# $Id$
#
# updates if no wpips on a pips architecture
# wpips was never compiled with aix for instance, hence

#debug_output := $(shell echo no_wpips.mk  > /dev/tty)

# Skip compiling WPips:
PIPS_NO_WPIPS = 1
