/* ------------------------------
 * Fil: FullPath.c
 * ------------------------------ */

#include "stdafx.h"
#include <stdlib.h>
#include <string.h>

#ifdef LINUX
#  include <unistd.h>
#endif

#ifdef UNIX
#  include <ctype.h>
#endif

#ifdef OS2
#  define INCL_DOSFILEMGR
#  define INCL_DOSERRORS
#  include <os2.h>
#endif

#ifdef BORLAND
#  include <windows.h>
#endif

#ifdef WIN32
#  include <windows.h>
#endif

#include "fyut.h"


#ifdef UNIX

/*-----------------------------------------------------------------------* 
 * filename - fullpath.c
 *
 * function(s)
 *        isSlash   - check for directory separator character
 *        _fullpath - make an absolute path name from a relative path name
 *-----------------------------------------------------------------------*/

/*
 *      C/C++ Run Time Library - Version 5.0
 *
 *      Copyright (c) 1987, 1992 by Borland International
 *      All Rights Reserved.
 *
 */


/*---------------------------------------------------------------------*

Name            isSlash - check for directory separator character

Usage           int isSlash(int c);

Prototype in    local

Description     isSlash returns true if the character c is a valid
                directory separator character (\ or / on DOS, / on UNIX).
                It returns false otherwise.

Return value    Describe above.

*---------------------------------------------------------------------*/
static int isSlash (int c)
{
    return (c == '\\' || c == '/');
}

/*---------------------------------------------------------------------*

Name            _fullpath - makes new file name

Usage           #include <dir.h>
                char *_fullpath(char *buffer, const char * pathname,
                                size_t maxlen);

Prototype in    stdlib.h

Description     _fullpath converts the relative path name name 'pathname'
                to a fully qualified pathname, stored in 'buffer'.  The
					 relative path can contain ".\" and "..".

                The maximum size of the supplied buffer is 'maxlen'.
                If the fully qualified path is longer than 'maxlen',
                NULL is returned.  The buffer should be at least _MAX_PATH
                bytes long (this constant is defined in stdlib.h).

                If 'buffer' is NULL, a buffer to store the fully qualified
                path is allocated and is returned.  The calling program
                must free this buffer with GlobalFree(() when it is no longer needed.

                If the pathname does not specify a disk drive, the current
                drive is used.

Return value    A pointer to the buffer containing the fully qualified
                path is returned.  If there is an error, NULL is returned.

Note            This function is a rewrite of the FExpand procedure
                in the Turbo Pascal RTL, with modifications for MSC
					 compatibility.

*---------------------------------------------------------------------*/
char * _fullpath(char *buffer,
                 const char *pathname,
                 size_t maxlen
                 )
{
	 char *tempbuf;
    char *dst, *src;
    int c, driveletter;
    size_t len;


    /* Allocate a temporary buffer to hold the fully qualified path.
	  */
	 if ((tempbuf = (char*)UT_MALLOC(_MAX_PATH*2+1)) == NULL)
		  return (NULL);
    
    
     driveletter = 7;
     src = (char *)pathname; 
    
    
	 /* If supplied path is relative, append it to the drivename
     * and its current directory.  Otherwise append it to the
     * drivename only.
     */
    if (!isSlash(src[0])) {              /* path is relative? */
        /* Get drivename and its current directory.
         */
		  if (getcwd(tempbuf,_MAX_PATH*2+1) == NULL) {
				UT_FREE(tempbuf);
				return (NULL);
        }
        dst = &tempbuf[strlen(tempbuf)];
        if (!isSlash(*(dst-1)))         /* if directory doesn't end in slash */
            *dst++ = UT_SLASH;              /* append one */

    } else {
        /* Path is absolute.  Store the drivename only.*/
        dst = tempbuf;
    }
    strcpy(dst,src);                    /* concatenate supplied path */

    /* Scan the path, squeezing out "../" and "./", and
     * squeezing out the previous directory when "../" is found.
	  */
    src = dst = tempbuf;
    
    for (;;) {
        /* If this the end of the path, or end of a directory,
         * we must check for "." or ".."
         */
        if ((c = *src++) == '\0' || isSlash(c)) {
            /* If last directory copied was "/.", back up over it.
             * Skip test if we are still at the beginning of tempbuf.
             */
            if (src != (tempbuf+1)) {
			  if (*(dst-1) == '.' && isSlash(*(dst-2))) {
                dst -= 2;

            /* If last directory copied was "/..", back up over it
             * AND the previous directory.
             */
              } else if (*(dst-1) == '.' && *(dst-2) == '.' && isSlash(*(dst-3))) {
                dst -= 3;                /* back up over "/.." */
					 if (*(dst-1) == ':') {   /* can't back up over drivename */
						  UT_FREE(tempbuf);
						  return(NULL);
					 }
					 while (!isSlash(*--dst))
                    ;                   /* back up to start of prev. dir. */
			  }

              if (c == '\0') {             /* end of path? */
                if (isSlash(*(dst-1)))  /* if last char is slash */
                    dst--;              /*  back up over it */
                if (*(dst-1) == ':')    /* if path is just a drivename */
                    *dst++ = '/';      /*  append a slash */
                *dst = '\0';            /* append null terminator */
                break;
            
              } else {
					 *dst++ = c;             /* copy the slash */
              }
		    }

        } else {
            *dst++ = c;                 /* copy the character */
        }
    }

	 /* Copy the temp buffer to the user's buffer, if present.
     * Otherwise shrink the temp buffer and return a pointer to it.
     */
    len = strlen(tempbuf) + 1;                  /* length of path and null */
    if (buffer != NULL) {
		  if (len > maxlen) {                     /* user buffer too small? */
				UT_FREE(tempbuf);
				return (NULL);

        } else {
				strcpy(buffer,tempbuf);
				UT_FREE(tempbuf);
            return (buffer);
        }

    } else {
        return (char*)(realloc(tempbuf,len));          /* shrink the buffer */
	 }
}
#endif


/*
AR-930423
CH UT_FullPath                              Finn fullstendig filnavn
CD ==================================================================
CD Purpose:
CD Lag absolutt path navn fra relativt path navn.
CD I tilleg tolker denne environment-variabler inn i filnavnet.
CD Environment-varialen skrives i parantes.
CD
CD Eks:
CD    SET FKB=D:\DATA\SOSI\FKB
CD
CD   Filnavnet   (FKB)\CV03851V.SOS
CD   pakkes ut til  D:\DATA\SOSI\FKB\CV03851V.SOS
CD
CD Parameters:
CD Type             Navn       I/U  Merknad
CD ------------------------------------------------------------------
CD char            *pszBuffer u   Komplett filnavn
CD const char      *pszPath   i   Forkortet filnavn
CD size_t           maxlen    i   Max lengde av pszBuffer
CD short            sStatus   r   Status; 0=OK, annen verdi er feil.
CD
CD Usage:  sStatus = UT_FullPath(szBuffer,szPath,maxlen);
	==================================================================
*/
SK_EntPnt_UT short  UT_FullPath(char *pszBuffer, const char *pszPath, size_t maxlen)
{
	char szFilnavn[_MAX_PATH];
	char *pszStart,*pszSlutt;
	char *env;
#ifdef BORLAND
	char  *pszOrgPath;
#endif

	/* Søk start- og sluttparantes */
	UT_StrCopy(szFilnavn,pszPath,_MAX_PATH);
	pszStart = strchr(szFilnavn,'(');
	pszSlutt = strchr(szFilnavn,')');

	/* Både start- og sluttparantes er funnet,
      og starten er først i strengen */
   if (pszStart != NULL  &&  pszSlutt != NULL  &&  pszStart < pszSlutt) {
      *pszStart++ = '\0';
		*pszSlutt++ = '\0';
#ifdef LINUX
      env = getenv( UT_StrUpper(pszStart));
#else      
      size_t len;
      _dupenv_s(&env, &len, UT_StrUpper(pszStart));
#endif

      /* Navnet er ikke funnet */
      if (env == NULL) {
         UT_StrCopy(szFilnavn,pszPath,_MAX_PATH);

		} else {
 		  UT_StrCat(szFilnavn,env,_MAX_PATH);
        UT_ClrTrailsp(szFilnavn);
        UT_StrCat(szFilnavn,pszPath+(pszSlutt-szFilnavn),_MAX_PATH);
		}
	}

	/* Hent filopplysninger */
#ifdef UNIX
   return  (short)(_fullpath(pszBuffer,szFilnavn,maxlen) != NULL)?  0 : 1;
#endif

#ifdef OS232
	return  (short) DosQueryPathInfo(szFilnavn,FIL_QUERYFULLNAME,pszBuffer,maxlen);
#endif

#ifdef OS216
	return  (short) DosQPathInfo(szFilnavn,FIL_QUERYFULLNAME,(PBYTE)pszBuffer,(USHORT)maxlen,0);
#endif

#ifdef WIN32
   return  (short)(_fullpath(pszBuffer,szFilnavn,maxlen) != NULL)?  0 : 1;
#endif

#ifdef BORLAND
	pszOrgPath = _fullpath(pszBuffer,szFilnavn,maxlen);

	if (pszOrgPath != NULL)
		return((short)0);
	else
		return ((short)1);
#endif

}

