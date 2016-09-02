AC_PREREQ(2.52)
AC_INIT([agtype], 1.0, [ktlee@bitneint.net]) 
LT_INIT
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CHECK_HEADERS([sql.h])
AC_ARG_ENABLE([odbc],
[AS_HELP_STRING([--enable-odbc],
  [includes ODBC APIs])],
[case "${enableval}" in
  yes) odbc=true ;;
  no)  odbc=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-odbc]) ;;
esac],[odbc=false])
AM_CONDITIONAL([ODBC], [test x$odbc = xtrue])
AC_CONFIG_FILES([
Makefile
test/Makefile
agtype.pc
])
AC_CONFIG_SUBDIRS([json-c-0.12.1])
AC_OUTPUT
