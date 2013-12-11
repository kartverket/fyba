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

#ifdef WIN32
#  include <windows.h>
#endif

#include "fyut.h"


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
   return  (short)(_fullpath(pszBuffer,szFilnavn,maxlen) != NULL)?  0 : 1;
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

}

