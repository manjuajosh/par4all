#!/bin/sh
#
# SCCS Stuff
# $RCSfile: make-pipsrc.sh,v $ ($Date: 1995/08/09 12:58:37 $, ) version $Revision$
#
# to derive the Shell version of pipsrc.ref
#


{
  cat <<-!
	#	   --------------------------------------------------------
	#	   --------------------------------------------------------
	#
	#				    WARNING
	#
	#		  THIS FILE HAS BEEN AUTOMATICALLY GENERATED
	#
	#			       DO NOT MODIFY IT
	#
	#	   --------------------------------------------------------
	#	   --------------------------------------------------------
	!

   cat pipsrc.ref

   echo export `sed -n '/^[^\[{}#]*=/s/\([^ \t]*\)[ \t]*=.*/\1/p' pipsrc.ref | sort -u | tr '\012' ' '` 

} > pipsrc.sh

#
# that's all
#
