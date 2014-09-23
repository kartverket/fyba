INCLUDE_DIRECTORIES(../GM ../UT)

IF(UNIX)
	ADD_DEFINITIONS(-pedantic -Wno-long-long -Wall -O2 -D_FILE_OFFSET_BITS=64 -DUNIX -DLINUX -fPIC -Wno-write-strings)
ENDIF(UNIX)

ADD_LIBRARY(fyba STATIC
	Fyba_Callback.cpp
	FYBA_DLL.cpp
	FYHO.cpp
	FYLD.cpp
	FYLH.cpp
	fyln.cpp
	FYLP.cpp
	FYLS.cpp
	FYLX.cpp
	stdafx.cpp
	FYBA.cpp
	Fyba_melding.cpp
	FYLB.cpp
	FYLE.cpp
	FYLI.cpp
	FYLO.cpp
	FYLR.cpp
	FYLU.cpp
	FYTA.cpp
)

INSTALL(FILES fyba.h DESTINATION include/fyba)
INSTALL(TARGETS fyba ARCHIVE DESTINATION lib)
