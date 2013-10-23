/* ------------------------------
 * Fil: CopyFile.c
 * ------------------------------ */

#include "stdafx.h"

#ifdef UNIX
#  ifndef _INCLUDE_POSIX_SOURCE
#    define _INCLUDE_POSIX_SOURCE
#  endif
#  ifndef _HPUX_SOURCE
#    define _HPUX_SOURCE
#  endif
#endif

#ifdef OS2
#endif

#ifdef WIN32
#  include<windows.h>
#endif

#ifdef BORLAND
#endif

//#include "StdAfx.h"
#include "fyut.h"

/*
AR-971118
CH UT_CopyFile                                             Copy file
CD ==================================================================
CD Purpose:
CD Copy an existing file to a new file.
CD
CD Parameters:
CD Type   Name               I/O Explanation
CD ------------------------------------------------------------------
CD char  *pszFromFilename     i  Copy from this file
CD char  *pszToFilename       i  Copy to this file
CD short  sErrorIfExist       i  Action to perform if the target file exists:
CD                                  UT_TRUE = Abort with error
CD                                  UT_FALSE = Overwrite 
CD short  sStatus             r  Status:
CD                                  UT_TRUE = OK
CD                                  UT_FALSE = Error.
CD
CD Usage:  sStatus = UT_CopyFile(pszFromFilename,pszToFilename,UT_TRUE);
   ==================================================================
*/
SK_EntPnt_UT short UT_CopyFile(char *pszFromFilename,char *pszToFilename,short sErrorIfExist)
{
#ifdef WIN32
   if (CopyFile( pszFromFilename, pszToFilename, sErrorIfExist)) {
      return UT_TRUE;
   } else {
      return UT_FALSE;
   }
#endif


#ifdef UNIX
      return UT_FALSE; //TODO
#endif

#ifdef OS232
      return UT_FALSE; //TODO
#endif

#ifdef OS216
      return UT_FALSE; //TODO
#endif

#ifdef BORLAND
      return UT_FALSE; //TODO
#endif

}
