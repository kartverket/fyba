/* ------------------------------ 
 * File: DelFile.c
 * ------------------------------ */

#include "stdafx.h"

#ifdef LINUX
#  include <unistd.h>
#  define UT_access access
#else
#  include <io.h>
#  define UT_access _access
#endif

#include "fyut.h"

/*
AR-921013
CH  UT_DeleteFile                                      Deletes a file
CD  ==================================================================
CD  Purpose:
CD  Deletes a file.
CD
CD  Parameters:
CD  Type      Name     I/O  Explanation
CD  ------------------------------------------------------------------
CD  char     *pszPath   i   Filename
CD  short     sStatus   r   Status; 0=OK, other value on error.
CD
CD  Usage:  sStatus = UT_DeleteFile(szPath);
    ==================================================================
*/

SK_EntPnt_UT short UT_DeleteFile(char *pszPath)
{
   // Check that the file exists
   if( (UT_access( pszPath, 0 )) == 0 ) {
      /* Delete file if exists */
      return remove(pszPath); 
   }
   
   return  0;
}
