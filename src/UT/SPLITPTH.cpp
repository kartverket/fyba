/*------------------------------------------------------------------------
 * filename - spltpath.c
 *-----------------------------------------------------------------------*/

#include "stdafx.h"
#include <string.h>

#ifdef WIN32
#include <windows.h>
#endif

#include "fyut.h"

/*
AR-930423
CH UT_splitpath                                       Splitt filnavn
CD ==================================================================
CD Formål:
CD UT_splitpath splitter et fullstendig filnavn i enkelte komponenter.
CD Filnavnet:  X:\DIR\SUBDIR\NAME.EXT
CD blir til:      X er drive
CD                \DIR\SUBDIR\ er gitt av dir
CD                NAME.EXT er gitt av name og ext
CD
CD PARAMETERLISTE:
CD Type         Navn       I/U  Merknad
CD --------------------------------------------------------------
CD wchar_t        *pszPath   i   Komplett filnavn
CD const wchar_t  *pszDrive  u   Disk
CD const wchar_t  *pszDir    u   Katalog
CD const wchar_t  *pszNavn   u   Navn
CD const wchar_t  *pszExt    u   Extension
CD
CD Bruk:  UT_splitpath(szPath,szDrive,szDir,szNavn,szExt);
   ==================================================================
*/
SK_EntPnt_UT void UT_splitpath(const wchar_t *pathP, wchar_t *driveP, wchar_t *dirP, wchar_t *nameP, wchar_t *extP)
{
   _wsplitpath_s(pathP, driveP, (driveP)? _MAX_DRIVE:0, dirP, (dirP)? _MAX_DIR:0, nameP, (nameP)? _MAX_FNAME:0, extP, (extP)? _MAX_EXT:0);
}


/*
AR:2013-11-15
CH UT_HarPathExtension                               Sjekk extension
CD ==================================================================
CD Formål:
CD Sjekker om path (filnavn) har extension.
CD
CD PARAMETERLISTE:
CD Type            Navn      I/U  Merknad
CD --------------------------------------------------------------
CD const wchar_t * path     i   Komplett filnavn
CD bool            bExtension  u   true = Har extension.
CD                                false = Har ikke extension
CD
CD Bruk:  bool status = UT_HarPathExtension(path);

   ==================================================================
*/
SK_EntPnt_UT bool UT_HarPathExtension(const wchar_t *path)
{
   wchar_t ext[_MAX_EXT];
   UT_splitpath(path, NULL, NULL, NULL, ext);

   return (*ext != L'\0');
}
