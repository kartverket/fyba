AM_CPPFLAGS = --pedantic -Wno-long-long -Wall -O2 -D_FILE_OFFSET_BITS=64 -DUNIX -DLINUX -fPIC -Wno-write-strings -I$(srcdir)/../GM -I$(srcdir)/../UT
ACLOCAL_AMFLAGS = -I m5

lib_LTLIBRARIES = libfyba.la
libfyba_la_SOURCES = fyba.h Fyba_Callback.cpp  FYBA_DLL.cpp      FYHO.cpp  FYLD.cpp  FYLH.cpp  fyln.cpp  FYLP.cpp  FYLS.cpp  FYLX.cpp  stdafx.cpp FYBA.cpp           Fyba_melding.cpp  FYLB.cpp  FYLE.cpp  FYLI.cpp  FYLO.cpp  FYLR.cpp  FYLU.cpp  FYTA.cpp fyba.h  fyba_strings.h  fybax.h  stdafx.h
libfyba_la_LDFLAGS = -version-info 0:0:0

library_includedir=$(includedir)/fyba
library_include_HEADERS = fyba.h     
