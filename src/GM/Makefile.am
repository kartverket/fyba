AM_CPPFLAGS = --pedantic -Wall -O2 -D_FILE_OFFSET_BITS=64 -DUNIX -DLINUX -fPIC -Wno-write-strings
ACLOCAL_AMFLAGS = -I m5  

lib_LTLIBRARIES = libfygm.la
libfygm_la_SOURCES = fygm.h GM.cpp stdafx.cpp stdafx.h
libfygm_la_LDFLAGS = -version-info 0:0:0

library_includedir=$(includedir)/fyba
library_include_HEADERS = fygm.h 

