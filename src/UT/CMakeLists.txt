IF(UNIX)
	ADD_DEFINITIONS(-pedantic -Wno-long-long -Wall -O2 -D_FILE_OFFSET_BITS=64 -DUNIX -DLINUX -fPIC -Wno-write-strings)
ENDIF(UNIX)

ADD_LIBRARY(ut STATIC
	ANFORSEL.cpp
	DELDIR.cpp
	FILNACMP.cpp
	INQTID.cpp
	UT1.cpp
	UT4.cpp
	CopyFile.cpp
	DELFILE.cpp
	MAKEPATH.cpp
	stdafx.cpp
	UT2.cpp
	CREDIR.cpp
	DISKINFO.cpp
	INQSIZE.cpp
	SETSIZE.cpp
	StrtPros.cpp
	UT3.cpp
	stdafx.h
	FULLPATH.cpp
	SPLITPTH.cpp
)

INSTALL(FILES fyut.h DESTINATION include/fyba)
INSTALL(TARGETS ut ARCHIVE DESTINATION lib)
