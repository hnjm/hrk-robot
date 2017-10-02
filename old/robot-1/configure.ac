AC_INIT([robot],[0.1],[harkaitzv@gmail.com])
AC_CONFIG_MACRO_DIRS([.m4])
AC_CONFIG_AUX_DIR([.aux])
AM_INIT_AUTOMAKE([])
AC_CANONICAL_HOST
AC_PROG_CC
		if test -f $SCRIPTDIR/hrkconfig.sh;then
		  source $SCRIPTDIR/hrkconfig.sh
		elif test -f ./hrkconfig.sh;then
		  source ./hrkconfig.sh
		fi
AC_CONFIG_HEADERS([src/config.h])
AC_DEFINE([HAS_ROBOT],[],[For code only usefull in this project.])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
])
AC_OUTPUT
