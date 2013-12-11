/*-----------------------------------------------------------------------*
 * Fil - makepath.c
 *
 * funksjon 
 *        UT_makepath - bygg opp filnavn
 *-----------------------------------------------------------------------*/

#include "stdafx.h"
#include "fyut.h"

#ifdef WIN32
#	include <windows.h>
#endif

static wchar_t *_stpcpy(wchar_t *til, const wchar_t *fra);


/*---------------------------------------------------------------------*

Name            UT_makepath - bygg opp filnavn

Bruk            #include <fyut.h>
                void UT_makepath(wchar_t *path, const wchar_t * drive, const wchar_t * dir,
                                 const wchar_t * name, const wchar_t * ext);

Prototype i     fyut.h

Beskrivelse     UT_makepath bygger opp et fullstendig filnavn ut fra dets deler.
                Det nye filnavnet blir

                        X:\DIR\SUBDIR\NAME.EXT

                hvor

                        X er drive
                        \DIR\SUBDIR\ er gitt av dir
                        NAME.EXT er gitt av name og ext

                If the drive, dir, name, or ext parameters are null or empty,
                they are not inserted in the path string.  Otherwise, if
                the drive doesn't end a colon, one is inserted in the path.
                If the dir doesn't end in a slash, one is inserted in the
                path.  If the ext doesn't start with a dot, one is inserted
                in the path.

                The maximum sizes for the path string is given by the
                constant _MAX_PATH (defined in stdlib.h), which includes space
                for the null-terminator.

                UT_splitpath and UT_makepath are invertible; if you split a given
                path with UT_splitpath, then merge the resultant components
                with UT_makepath, you end up with path.

Return value    None

*---------------------------------------------------------------------*/


/*
AR-930423
CH UT_makepath                                    Slå sammen filnavn
CD ==================================================================
CD Formål:
CD UT_makepath bygger opp et fullstendig filnavn ut fra dets deler.
CD Det nye filnavnet blir:  X:\DIR\SUBDIR\NAME.EXT
CD hvor:                    X er drive
CD                          \DIR\SUBDIR\ er gitt av dir
CD                          NAME.EXT er gitt av name og ext
CD
CD PARAMETERLISTE:
CD Type         Navn       I/U  Merknad
CD --------------------------------------------------------------
CD wchar_t        *pszPath   u   Komplett filnavn
CD const wchar_t  *pszDrive  i   Disk
CD const wchar_t  *pszDir    i   Katalog
CD const wchar_t  *pszNavn   i   Navn
CD const wchar_t  *pszExt    i   Extension
CD
CD Bruk:  UT_makepath(szPath,szDrive,szDir,szNavn,szExt);
   ==================================================================
*/
SK_EntPnt_UT void  UT_makepath(wchar_t *pathP, const wchar_t *driveP, const wchar_t *dirP,
                  const wchar_t *nameP, const wchar_t *extP)
{
#ifdef UNIX
   if (dirP && *dirP) {
      if(*(dirP)!=UT_SLASH) {
         *pathP++=UT_SLASH;
      }
      pathP = _stpcpy(pathP,dirP);
      if (*(pathP-1) != '\\' && *(pathP-1) != '/')  *pathP++ = UT_SLASH;
   }

   if (nameP)  pathP = _stpcpy(pathP,nameP);

   if (extP && *extP) {
      if (*extP != '.')  *pathP++ = '.';
      pathP = _stpcpy(pathP,extP);
   }

   *pathP = '\0';

#else

   if (driveP && *driveP) {
      *pathP++ = *driveP;
      *pathP++ = ':';
   }

   if (dirP && *dirP) {
      pathP = _stpcpy(pathP,dirP);
      if (*(pathP-1) != '\\' && *(pathP-1) != '/')  *pathP++ = UT_SLASH;
   }

   if (nameP)  pathP = _stpcpy(pathP,nameP);

   if (extP && *extP) {
      if (*extP != '.')  *pathP++ = '.';
      pathP = _stpcpy(pathP,extP);
   }

   *pathP = '\0';
#endif
}


//====================================================
static wchar_t *_stpcpy(wchar_t *til, const wchar_t *fra)
{
   while (*fra) {
      *til = *fra;
      til++;
      fra++;
   } /* endwhile */

   return til;
}
