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

#define wchar_t char
#define wcschr  strchr

/////////////////////////////////////////////////////////////////////////////////////// 
// FULLPATH.cpp

///////////////////////////////////////////////////////////////////////////////////////
// ErSlash()
int ErSlash (int chr)
{
    return (chr == '\\' || chr == '/');
}

///////////////////////////////////////////////////////////////////////////////////////
// FullPath()
wchar_t * FullPath(wchar_t *Buffer,
                 const wchar_t *PathName,
                 size_t nMaxlen
                 )
{
	 wchar_t *TempBuf;
    wchar_t *dst, *src;
    int c, Drive;
    size_t nLen;


    /* Allocate a temporary buffer to hold the fully qualified path.
	  */
	 if ((TempBuf = (wchar_t*)malloc(_MAX_PATH*2+1)) == NULL)
		  return (NULL);
    
    
     Drive = 7;
     src = (wchar_t *)PathName; 
    
    
	 /* If supplied path is relative, append it to the drivename
     * and its current directory.  Otherwise append it to the
     * drivename only.
     */
    if (!ErSlash(src[0])) {              /* path is relative? */
        /* Get drivename and its current directory.
         */
		  if (getcwd(TempBuf,_MAX_PATH*2+1) == NULL) {
				free(TempBuf);
				return (NULL);
        }
        dst = &TempBuf[strlen(TempBuf)];
        if (!ErSlash(*(dst-1)))         /* if directory doesn't end in slash */
            *dst++ = UT_SLASH;              /* append one */

    } else {
        /* Path is absolute.  Store the drivename only.*/
        dst = TempBuf;
    }
    strcpy(dst,src);                    /* concatenate supplied path */

    /* Scan the path, squeezing out "../" and "./", and
     * squeezing out the previous directory when "../" is found.
	  */
    src = dst = TempBuf;
    
    for (;;) {
        /* If this the end of the path, or end of a directory,
         * we must check for "." or ".."
         */
        if ((c = *src++) == '\0' || ErSlash(c)) {
            /* If last directory copied was "/.", back up over it.
             * Skip test if we are still at the beginning of TempBuf.
             */
            if (src != (TempBuf+1)) {
			  if (*(dst-1) == '.' && ErSlash(*(dst-2))) {
                dst -= 2;

            /* If last directory copied was "/..", back up over it
             * AND the previous directory.
             */
              } else if (*(dst-1) == '.' && *(dst-2) == '.' && ErSlash(*(dst-3))) {
                dst -= 3;                /* back up over "/.." */
					 if (*(dst-1) == ':') {   /* can't back up over drivename */
						  free(TempBuf);
						  return(NULL);
					 }
					 while (!ErSlash(*--dst))
                    ;                   /* back up to start of prev. dir. */
			  }

              if (c == '\0') {             /* end of path? */
                if (ErSlash(*(dst-1)))  /* if last wchar_t is slash */
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
    nLen = strlen(TempBuf) + 1;                  /* length of path and null */
    if (Buffer != NULL) {
		  if (nLen > nMaxlen) {                     /* user buffer too small? */
				free(TempBuf);
				return (NULL);

        } else {
				strcpy(Buffer,TempBuf);
				free(TempBuf);
            return (Buffer);
        }

    } else {
        return (wchar_t*)(realloc(TempBuf,nLen));          /* shrink the buffer */
	 }
}
#endif


/*
AR-930423
CH UT_FullPath                              Finn fullstendig filnavn
CD ==================================================================
CD Formål:
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
CD PARAMETERLISTE:
CD Type             Navn       I/U  Merknad
CD ------------------------------------------------------------------
CD wchar_t            *pszBuffer u   Komplett filnavn
CD const wchar_t      *pszPath   i   Forkortet filnavn
CD size_t           maxlen    i   Max lengde av pszBuffer
CD short            sStatus   r   Status; 0=OK, annen verdi er feil.
CD
CD Bruk:  sStatus = UT_FullPath(szBuffer,szPath,maxlen);
	==================================================================
*/
SK_EntPnt_UT short  UT_FullPath(wchar_t *pszBuffer, const wchar_t *pszPath, size_t maxlen)
{
	wchar_t szFilnavn[_MAX_PATH];
	wchar_t *pszStart,*pszSlutt;
	wchar_t *env;
#ifdef BORLAND
	wchar_t  *pszOrgPath;
#endif

	/* Søk start- og sluttparantes */
	UT_StrCopy(szFilnavn,pszPath,_MAX_PATH);
	pszStart = wcschr(szFilnavn,'(');
	pszSlutt = wcschr(szFilnavn,')');

	/* Både start- og sluttparantes er funnet,
      og starten er først i strengen */
   if (pszStart != NULL  &&  pszSlutt != NULL  &&  pszStart < pszSlutt) {
      *pszStart++ = '\0';
		*pszSlutt++ = '\0';
#ifdef LINUX
      env = getenv( UT_StrUpper(pszStart));
#else      
      size_t len;
      _wdupenv_s(&env, &len, UT_StrUpper(pszStart));
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
   return  (short)(FullPath(pszBuffer,szFilnavn,maxlen) != NULL)?  0 : 1;
#endif

#ifdef OS232
	return  (short) DosQueryPathInfo(szFilnavn,FIL_QUERYFULLNAME,pszBuffer,maxlen);
#endif

#ifdef OS216
	return  (short) DosQPathInfo(szFilnavn,FIL_QUERYFULLNAME,(PBYTE)pszBuffer,(USHORT)maxlen,0);
#endif

#ifdef WIN32
   return  (short)(_wfullpath(pszBuffer,szFilnavn,maxlen) != NULL)?  0 : 1;
#endif

#ifdef BORLAND
	pszOrgPath = FullPath(pszBuffer,szFilnavn,maxlen);

	if (pszOrgPath != NULL)
		return((short)0);
	else
		return ((short)1);
#endif

}

