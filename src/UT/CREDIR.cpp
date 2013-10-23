/* ------------------------------ 
 * Fil: CreDir.c
 * ------------------------------ */

#include "stdafx.h"

#ifdef UNIX
#  ifndef _INCLUDE_POSIX_SOURCE
#     define _INCLUDE_POSIX_SOURCE
#  endif

#  ifndef _HPUX_SOURCE
#     define _HPUX_SOURCE
#  endif

#  include<sys/types.h>
#  include <sys/stat.h>
#endif

#ifdef OS232
#  define INCL_DOSFILEMGR
#  define INCL_DOSERRORS
#  include <os2.h>
#endif

#ifdef OS216
#  include <direct.h>
#endif

#ifdef WIN32
#  include <direct.h>
#endif

#ifdef BORLAND
#  include <dir.h>
#endif

//#include "StdAfx.h"
#include "fyut.h"

/*
AR-921013
CH UT_CreateDir                                 Creates a directory
CD ==================================================================
CD Purpose:
CD Creates a directory.
CD
CD Parameters:
CD Type      Name     I/O  Explanation
CD ------------------------------------------------------------------
CD char     *pszPath   i   Directory-name
CD short     sStatus   r   Status; 0=OK, other value on error.
CD
CD Usage:  sStatus = UT_CreateDir(szPath);
	==================================================================
*/
SK_EntPnt_UT short UT_CreateDir(char *pszPath)
{
#ifdef UNIX
	mode_t modus;
	/* Sets all permissions for the user, none for others */
	modus = (S_IRUSR  | S_IWUSR  | S_IXUSR);
	return  (short)mkdir(pszPath,modus);
#endif

#ifdef OS232
	return  (short)DosCreateDir(pszPath,NULL);
#endif

#ifdef OS216
   return  (short)mkdir(pszPath);
#endif

#ifdef WIN32
   return  (short)_mkdir(pszPath);
#endif

#ifdef BORLAND
	return  (short)mkdir(pszPath);
#endif
}
