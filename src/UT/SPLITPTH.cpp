///////////////////////////////////////////////////////////////////////////////////
// SPLITPATH.cpp
//
// Funksjoner
//        PrikkFunnet - sjekker etter spesielle kataloger
//        UT_splitpath - split a full path name (MSC compatible)
//

#include "stdafx.h"
#include <string.h>

#ifdef BORLAND
#include <windows.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include "fyut.h"

#ifdef LINUX
#define wchar_t char
#define wcslen  strlen
#endif

///////////////////////////////////////////////////////////////////////
//
// PrikkFunnet - sjekker etter spesielle kataloger

static size_t PrikkFunnet(wchar_t *pB)
{
        if (*(pB-1) == '.')
                pB--;
        switch (*--pB) {
           case ':'  :
              if (*(pB-2) != '\0') {
                 break;
              } else {
                 return UT_TRUE;
              }
           case '/'  :
           case '\\' :
           case '\0' :
              return UT_TRUE;
        }
        return UT_FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////
// UT_splitpath - splits a full path name into its components

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
        wchar_t   *pB;
        size_t Wrk;
        int    ExtFunnet;

        wchar_t buf[ _MAX_PATH+2 ];

        /*
         * Set all string to default value zero
         */
        ExtFunnet = UT_FALSE;
        if (driveP)
                *driveP = 0;
        if (dirP)
                *dirP = 0;
        if (nameP)
                *nameP = 0;
        if (extP)
                *extP = 0;

        /*
         * Copy filename into template up to _MAX_PATH characters
         */
        pB = buf;
        while (*pathP == ' ')
                pathP++;

        if ((Wrk = wcslen(pathP)) >= _MAX_PATH)
                Wrk = _MAX_PATH - 1;
        *pB++ = 0;
        UT_StrCopy(pB, pathP, Wrk+1);

        *(pB += Wrk) = 0;

        /*
         * Split the filename and fill corresponding nonzero pointers
         */
        Wrk = 0;
        for (; ; ) {
                switch (*--pB) {
                case '.'  :
                        if (!Wrk && (*(pB+1) == '\0'))
                                Wrk = PrikkFunnet(pB);
                        if ((!Wrk) && (!ExtFunnet)) {
                                ExtFunnet = UT_TRUE;
                                UT_StrCopy(extP, pB, _MAX_EXT);
                                *pB = 0;
                        }
                        continue;
                case ':'  :
                        if (pB != &buf[2])
                                continue;
                case '\0' :
                        if (Wrk) {
                                pB++;
                                UT_StrCopy(dirP, pB, _MAX_DIR);
                                *pB-- = 0;
                                break;
                        }
                case '/'  :
                case '\\' :
                        if (!Wrk) {
                                Wrk++;
                                pB++;
                                UT_StrCopy(nameP, pB, _MAX_FNAME);
                                *pB-- = 0;
                                if (*pB == 0 || (*pB == ':' && pB == &buf[2]))
                                        break;
                        }
                        continue;

                case '*'  :
                case '?'  :
                        continue;

                default :
                        continue;
                }
                break;
        }

        if (*pB == ':') {
                UT_StrCopy(driveP, &buf[1], _MAX_DRIVE);
        }
}
