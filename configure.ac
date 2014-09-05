#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT(fyba, 4.1.0, thomas.hirsch@statkart.no)
AM_INIT_AUTOMAKE(fyba, 4.1.0)
LT_INIT()
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([include/fygm.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h float.h inttypes.h limits.h locale.h memory.h stdint.h stdlib.h string.h sys/ioctl.h sys/statvfs.h sys/time.h sys/vfs.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_MODE_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRCOLL
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor getcwd memmove memset mkdir modf pow rmdir sqrt strchr strerror strpbrk strstr strtol strtoul])

AC_OUTPUT
AC_OUTPUT(fyba.pc Makefile src/GM/Makefile src/UT/Makefile src/FYBA/Makefile doc/Makefile)
