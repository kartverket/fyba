/*------------------------------------------------------------------------
 * filename - spltpath.c
 *
 * function(s)
 *        DotFound - checks for special directory names
 *        UT_splitpath - split a full path name (MSC compatible)
 *-----------------------------------------------------------------------*/

/*
 *      C/C++ Run Time Library - Version 5.0
 *
 *      Copyright (c) 1987, 1992 by Borland International
 *      All Rights Reserved.
 *
 */

#include "stdafx.h"
#include <string.h>

#ifdef BORLAND
#include <windows.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif


#include "fyut.h"


/*---------------------------------------------------------------------*

Name            DotFound - checks for special dir name cases

Usage           int DotFound(char *pB);

Prototype in    local to this module

Description     checks for special directory names

*---------------------------------------------------------------------*/
static size_t DotFound(char *pB)
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

/*---------------------------------------------------------------------*

Name            UT_splitpath - splits a full path name into its components

Usage           #include <fyut.h>
                void UT_splitpath(const char *path, char * drive, char * dir,
                             char * name, char * ext);

Related
functions usage void UT_makepath(char *path, const char *drive, const char *dir,
                            const char *name, const char *ext);

Prototype in    fyut.h

Description     UT_splitpath takes a file's full path name (path) as a string
                in the form

                        X:\DIR\SUBDIR\NAME.EXT

                and splits path into its four components. It then stores
                those components in the strings pointed to by drive, dir,
                name and ext. (Each component is required but can be a
                NULL, which means the corresponding component will be
                parsed but not stored.)

                The maximum sizes for these strings are given by the
                constants _MAX_DRIVE, _MAX_DIR, _MAX_PATH, _MAX_NAME and _MAX_EXT,
                (defined in fyut.h) and each size includes space for the
                null-terminator.

                UT_splitpath assumes that there is enough space to store each
                non-NULL component. fnmerge assumes that there is enough
                space for the constructed path name. The maximum constructed
                length is _MAX_PATH.

                When UT_splitpath splits path, it treats the punctuation as
                follows:

                * drive keeps the colon attached (C:, A:, etc.)

                * dir keeps the leading and trailing backslashes
                  (\turboc\include\,\source\, etc.)

                * ext keeps the dot preceding the extension (.c, .exe, etc.)

                UT_splitpath and _makepath are invertible; if you
                split a given path with UT_splitpath, then
                merge the resultant components with UT_makepath, you end up
                with path.

Return value    UT_splitpath does not return a value.

*---------------------------------------------------------------------*/


/*
AR-930423
CH UT_splitpath                                       Splitt filnavn
CD ==================================================================
CD Purpose:
CD UT_splitpath splitter et fullstendig filnavn i enkelte komponenter.
CD Filnavnet:  X:\DIR\SUBDIR\NAME.EXT
CD blir til:      X er drive
CD                \DIR\SUBDIR\ er gitt av dir
CD                NAME.EXT er gitt av name og ext
CD
CD Parameters:
CD Type         Name       I/O  Explanation
CD --------------------------------------------------------------
CD char        *pszPath   i   Komplett filnavn
CD const char  *pszDrive  o   Disk
CD const char  *pszDir    o   Katalog
CD const char  *pszNavn   o   Navn
CD const char  *pszExt    o   Extension
CD
CD Usage:  UT_splitpath(szPath,szDrive,szDir,szNavn,szExt);
   ==================================================================
*/
SK_EntPnt_UT void UT_splitpath(const char *pathP, char *driveP, char *dirP,
                  char *nameP, char *extP)
{
        char   *pB;
        size_t Wrk;
        int    ExtFunnet;

        char buf[ _MAX_PATH+2 ];

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

        if ((Wrk = strlen(pathP)) >= _MAX_PATH)
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
                                Wrk = DotFound(pB);
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
