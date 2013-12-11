/******************************************************************************
*
* KARTVERKET  -  FYSAK
*
* Filename: fyut.h
*
* Content: Prototyper for generelle hjelperutiner og operativsystemavhengige rutiner.
*
* Copyright (c) 1990-2011 Kartverket
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
******************************************************************************/

#pragma once

#include <stdio.h>
#include <string>

#ifdef WIN32
#  include<windows.h>
#endif

#ifndef SK_EntPnt_UT
#  define SK_EntPnt_UT
#endif


/* ----------------------------------------- Konstanter */

#   ifdef WIN32

#   ifdef _DEBUG
#      pragma comment (lib, "UtD.lib")
#   else
#      pragma comment (lib, "Ut.lib")
#   endif

/* For DOS/OS2/Windows */
#   define UT_SLASH  L'\\'
#   define UT_STR_SLASH  L"\\"
#   define UT_INT64  __int64

#   ifdef _UNICODE
#     define UT_SNPRINTF _snwprintf_s
#     define UT_FPRINTF UT_fwprintfUTF8
#   else
#     define UT_SNPRINTF _snprintf_s
#     define UT_FPRINTF fprintf_s
#   endif

#   define UT_GETPID   _getpid

#else
          /* For UNIX */
#  ifdef LINUX /* specifically the LINUX */
#    include <stdint.h>
#    include <inttypes.h>
#    define UT_INT64 int64_t
#  endif

#   define UT_SNPRINTF snprintf
#   define UT_FPRINTF fprintf
#   define UT_GETPID   getpid

#   define UT_SLASH  '/'
#   define UT_STR_SLASH  "/"
#   define cdecl
#   define EZERO 0

#  ifdef  _FILE_OFFSET_BITS
#    undef _FILE_OFFSET_BITS
#  endif
#  define _FILE_OFFSET_BITS 64
#  include <limits.h>
#  include <sys/types.h>
#  include <unistd.h>
#  include <stdio.h>

#  define _TRUNCATE 0
#  define _fseeki64 fseek
#  define _ftelli64 ftell
#  define _strcmpi  strcasecmp

#endif

#define UT_FALSE 0
#define UT_TRUE  1
#define UT_OVRFLW 2

#define UT_NULL 0L

#define UT_OK  0
#define UT_EOF 1
#define UT_ERROR -1
#define UT_READ   0
#define UT_WRITE  1
#define UT_UPDATE  2
#define UT_UNKNOWN 0
#define UT_OLD     1
#define UT_NEW     2

// Konstanter for UT_KonverterTegnsett
#define TS_UKJENT   0  // Ukjent tegnsett
#define TS_DOSN8    1  // DOS norsk 8-bits (standardverdi)
#define TS_ND7      2  // Norsk Data 7-bits
#define TS_DECM8    4  // DEC multinasjonal 8-bits
#define TS_DECN7    8  // DEC norsk 7-bits
#define TS_ISO8859  16 // ISO8859-10 (Samisk og norsk mm. )
#define TS_UTF8     32 

/* Div lengder for filnavn; jfr. stdlib.h */
#define _MAX_PATH   260  /* max. length of full pathname */
#define _MAX_DRIVE    3  /* max. length of drive component */
#define _MAX_DIR    256  /* max. length of path component */
#define _MAX_FNAME  256  /* max. length of file name component */
#define _MAX_EXT    256  /* max. length of extension component */

// UTF-8
#define UTF8_ULOVLIG    -1 // Ulovlig UTF-8
#define UTF8_IKKE       0  // Ikke UTF-8
#define UTF8_LOVLIG     1  // UTF-8

// UTF-8 filtest
#define FIL_UTF8_IKKE   0  // Ikke UTF-8 fil
#define FIL_UTF8_BOM    1  // UTF-8 fil med BOM
#define FIL_UTF8        2  // UTF-8 fil uten BOM


/* ----------------------------------------- Makroer */
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))



/*
 *!--------------------------------------------------------------!
 *! Oppdateringstid for fil                                      !
 *!--------------------------------------------------------------!
 */
typedef struct dFTID {
   unsigned short usAar;
   unsigned short usMnd;
   unsigned short usDag;
   unsigned short usTime;
   unsigned short usMin;
   unsigned short usSek;
} FTID;

typedef FTID *PFTID;


/* ----------------------------------------- Prototyper */

SK_EntPnt_UT void  UT_makepath(wchar_t *pathP,const wchar_t *driveP,const wchar_t *dirP,const wchar_t *nameP,const wchar_t *extP);
SK_EntPnt_UT void  UT_splitpath(const wchar_t *pathP,wchar_t *driveP,wchar_t *dirP,wchar_t *nameP,wchar_t *extP);
SK_EntPnt_UT bool  UT_HarPathExtension(const wchar_t *path);
SK_EntPnt_UT short UT_InqPathSize(const wchar_t *pszPath,long *plSize);
SK_EntPnt_UT short UT_InqPathSize_i64(const wchar_t *pszPath,UT_INT64 *n64Size);
SK_EntPnt_UT short UT_SetPathSize(const wchar_t *pszPath,long lSize);
SK_EntPnt_UT short UT_SetPathSize_i64(const wchar_t *pszPath, UT_INT64 n64Size);
SK_EntPnt_UT short UT_InqPathTid(const wchar_t *pszPath,PFTID pFilTid);
SK_EntPnt_UT short UT_InqAvailSize(const wchar_t *pszPath,unsigned long *pulLedigPlass);
SK_EntPnt_UT short UT_FullPath(wchar_t *pszBuffer, const wchar_t *pszPath, size_t maxlen);
SK_EntPnt_UT short UT_DeleteFile(const wchar_t *pszPath);
SK_EntPnt_UT short UT_DeleteDir(const wchar_t *pszPath);
SK_EntPnt_UT short UT_CreateDir(const wchar_t *pszPath);
SK_EntPnt_UT short UT_FilnavnCmp(const wchar_t *pszFil1, const wchar_t *pszFil2);
SK_EntPnt_UT short UT_CopyFile(const wchar_t *pszFraFilnavn, const wchar_t *pszTilFilnavn, short sFeilHvisEksisterer);


/* Framhenting frå streng */
SK_EntPnt_UT wchar_t *UT_GetToken (wchar_t *str, short *l_tok);
SK_EntPnt_UT wchar_t  UT_StrToken (const wchar_t *str, short ii, short *iu, short mt, wchar_t *token);
SK_EntPnt_UT wchar_t  UT_StrWord (const wchar_t *str, short ii, short *iu, short mw, wchar_t *word);
SK_EntPnt_UT short UT_StrInt (const wchar_t *str, short ii, short *iu, int *_int);
SK_EntPnt_UT short UT_StrInt64 (const wchar_t *str, short ii, short *iu, UT_INT64 *i64);
SK_EntPnt_UT short UT_StrShort (const wchar_t *str, short ii, short *iu, short *s);
SK_EntPnt_UT short UT_StrLong (const wchar_t *str, short ii, short *iu, long *_long);
SK_EntPnt_UT short UT_StrUlong (const wchar_t *str, short ii, short *iu, unsigned long *_ulong);
SK_EntPnt_UT short UT_StrDbl (const wchar_t *str, short ii, short *iu, wchar_t decpt,double *dbl);
SK_EntPnt_UT void  UT_StrCopy (wchar_t *tx_to, const wchar_t *tx_from, int maxlen);
SK_EntPnt_UT short UT_StrCat (wchar_t *dst,const wchar_t *src, const int maxlen);
SK_EntPnt_UT int UT_memcpy(void *dest,size_t numberOfElements,const void *src,size_t count);
SK_EntPnt_UT int UT_strerror(wchar_t *buffer,size_t numberOfElements,int errnum);
SK_EntPnt_UT wchar_t *UT_strtok(wchar_t *strToken,const wchar_t *strDelimit,wchar_t **context);


/* Konvertering */
#ifdef OS216
SK_EntPnt_UT    short UT_AtoI (wchar_t *str, int *_int);
SK_EntPnt_UT    short UT_AtoS (wchar_t str[], short *s);
SK_EntPnt_UT    short UT_AtoL (wchar_t str[], long *_long);
#endif
SK_EntPnt_UT short UT_AtoD (wchar_t *str, wchar_t decpt, double *d);
SK_EntPnt_UT short UT_ItoA (int i, short sl, wchar_t *str);
SK_EntPnt_UT short UT_StoA (short s, short mstr, wchar_t *str);
SK_EntPnt_UT short UT_LtoA (long l, short sl, wchar_t *str);
SK_EntPnt_UT short UT_DtoA (double d, short dec, wchar_t decpt, short sl, wchar_t *str);

/* Strengmanipulering ellers */
SK_EntPnt_UT wchar_t *UT_ClrCrlf (wchar_t *str);
SK_EntPnt_UT wchar_t *UT_ClrTrailsp(wchar_t *str);
SK_EntPnt_UT char    *UT_ClrTrailsp(char *str);
SK_EntPnt_UT wchar_t *UT_ClrExtrasp(wchar_t *str);
SK_EntPnt_UT void  UT_JustStr (wchar_t justmode, wchar_t fill, short lstr, wchar_t *str);
SK_EntPnt_UT short UT_Index (wchar_t *str, wchar_t *substr);
SK_EntPnt_UT void  UT_GetSubstr (wchar_t ctx[],short fra,short til,wchar_t sub[],short maxlen);
SK_EntPnt_UT wchar_t *UT_StrLower (wchar_t *tx);
SK_EntPnt_UT wchar_t *UT_StrUpper (wchar_t *tx);
SK_EntPnt_UT int   UT_StrCmpi(const wchar_t *pszTx1, const wchar_t *pszTx2);
SK_EntPnt_UT int   UT_StrColli(const wchar_t *pszTx1, const wchar_t *pszTx2);
SK_EntPnt_UT char *UT_Ascii7ToAscii8 (char *tx);
SK_EntPnt_UT char *UT_Ascii8ToAscii7 (char *tx);
SK_EntPnt_UT char *UT_ISO8859ToAscii8(char *tx);
SK_EntPnt_UT char *UT_Ascii8ToISO8859(char *tx);
SK_EntPnt_UT char *UT_ISO8859ToAscii7(char *tx);
SK_EntPnt_UT char *UT_Ascii7ToISO8859(char *tx);
SK_EntPnt_UT wchar_t  UT_Utf8ToWChar(const char *tx, int *numBytes); 
SK_EntPnt_UT size_t   UT_GetUtf8Len(const wchar_t *wtx);
SK_EntPnt_UT bool UT_ErUtf8(const char *pszTx);
SK_EntPnt_UT bool UT_TestNormalUtf8_2byte(const char *pszTx);
SK_EntPnt_UT bool UT_TestNormalUtf8_3byte(const char *pszTx);
SK_EntPnt_UT short UT_ErNormalUtf8(const char *pszTx);
SK_EntPnt_UT short UT_ErUtf8Fil(const wchar_t *pszFilnavn);

SK_EntPnt_UT char    *UT_WCharToUtf8(wchar_t wc, char *dest);
SK_EntPnt_UT wchar_t *UT_ISO8859ToWChar(const char *tx, wchar_t *wtx);
SK_EntPnt_UT char    *UT_WCharToISO8859(const wchar_t *wtx, char *tx);
SK_EntPnt_UT void UT_KonverterTegnsett_8_8(short sFraTegnsett, short sTilTegnsett, char *pszTx);
SK_EntPnt_UT void UT_KonverterTegnsett_8_16 (short sFraTegnsett, char *pszTx, wchar_t *pwszResultat);
SK_EntPnt_UT void UT_KonverterTegnsett_8_16_UTF8 (const std::string strTx, std::wstring &wstrResultat); // Spesielt for GML
SK_EntPnt_UT void UT_KonverterTegnsett_16_8 (short sTilTegnsett, const wchar_t *pwszTx, char *pszResultat, size_t maxBytes);
SK_EntPnt_UT void UT_FormaterInt64(wchar_t *pszBuffer, size_t sBufferLen, __int64 value);

SK_EntPnt_UT void UT_StrTilWstr(const std::string &rstrFraTx, std::wstring &rwstrTilTx);
SK_EntPnt_UT void UT_WstrTilStr(const std::wstring &rwstrFraTx, std::string &rstrTilTx);

/* Andre teiknhandterings rutiner */
SK_EntPnt_UT int UT_IsSpace (char c);
SK_EntPnt_UT int UT_IsSpace (wchar_t c);
SK_EntPnt_UT int UT_IsPrint (wchar_t ch);
SK_EntPnt_UT int UT_IsLower (wchar_t ch);
SK_EntPnt_UT int UT_IsUpper (wchar_t ch);
SK_EntPnt_UT wchar_t UT_ToUpper (wchar_t ch);
SK_EntPnt_UT wchar_t UT_ToLower (wchar_t ch);

/* Matematiske funksjonar */
SK_EntPnt_UT double UT_RoundDD (double d);
SK_EntPnt_UT double UT_RoundHalfUpDD (double d);
SK_EntPnt_UT int    UT_RoundDI (double d);
SK_EntPnt_UT unsigned int UT_RoundDUI (double d);
SK_EntPnt_UT long   UT_RoundDL (double d);
SK_EntPnt_UT long long UT_RoundDLL (double d);
SK_EntPnt_UT short  UT_RoundDS (double d);

/* Kalender/klokke-rutiner */
SK_EntPnt_UT void UT_InqDate (short *date);
SK_EntPnt_UT wchar_t *UT_InqDateString(void);
SK_EntPnt_UT wchar_t *UT_InqDatetimeString(void);
SK_EntPnt_UT void UT_InqTime (short *time);

/* Høgnivå */
SK_EntPnt_UT FILE *UT_OpenFile     (const wchar_t *filnavn,const wchar_t *ftyp,
                                    short facc,short exist,short *ierr);

SK_EntPnt_UT short UT_ReadLine     (FILE *pfil,short llin, wchar_t clin[]);
SK_EntPnt_UT short UT_ReadLine     (FILE *pfil,short llin, char *clin);

SK_EntPnt_UT short UT_ReadLineCrlf (FILE *pfil,short llin, wchar_t clin[]);
SK_EntPnt_UT short UT_ReadLineNoComm(FILE *pfil,short llin, wchar_t *clin);

SK_EntPnt_UT short UT_ReadFile     (FILE *pfil, long lbufChar, long &lbufWchar, wchar_t *buff);
SK_EntPnt_UT short UT_ReadFile     (FILE *pfil, long lbuf, unsigned char *buff);
SK_EntPnt_UT short UT_ReadFile     (FILE *pfil, long lbuf, char *buff);

SK_EntPnt_UT short UT_ReadXMLLine  (FILE *pfil, std::wstring &wstrXMLLin);
SK_EntPnt_UT short UT_ReadXMLLine  (FILE *pfil, std::string &strXMLLin);

SK_EntPnt_UT short UT_WriteLine    (FILE *pfil,wchar_t clin[]);
SK_EntPnt_UT short UT_WriteLine    (FILE *pfil,char clin[]);
SK_EntPnt_UT short UT_WriteLineCrlf(FILE *pfil,wchar_t clin[]);
SK_EntPnt_UT short UT_WriteLineUTF8(FILE* fi, const wchar_t *pwszTx);
SK_EntPnt_UT short UT_WriteUTF8(FILE* fi, const wchar_t *pwszTx);
SK_EntPnt_UT short UT_WriteUTF8(FILE* fi, const wchar_t *pwszTx, const bool bNewline);

SK_EntPnt_UT short UT_WriteBomUTF8(FILE* fi);

SK_EntPnt_UT short UT_fwprintfUTF8(FILE * _File, const wchar_t * _Format, ...);

SK_EntPnt_UT short UT_SetPos       (FILE *fi,long lpos);
SK_EntPnt_UT short UT_SetPos_i64   (FILE *fi,UT_INT64 n64FilPos);
SK_EntPnt_UT short UT_GetPos       (FILE *fi,long *lpos);
SK_EntPnt_UT short UT_GetPos_i64(FILE *fi,UT_INT64 *n64FilPos);
SK_EntPnt_UT short UT_Save         (FILE *pfil);

/* Div. rutiner */
SK_EntPnt_UT void UT_Bell (void);
SK_EntPnt_UT void UT_Sound (short freq, short dur);
SK_EntPnt_UT void UT_Pause (short dur);
SK_EntPnt_UT unsigned long UT_InqFreemem (void);
SK_EntPnt_UT wchar_t *UT_GetEnv (wchar_t *var, short mstr, wchar_t *str);
SK_EntPnt_UT wchar_t *UT_MakeFysak ( wchar_t *str, short mstr, wchar_t *filename );
SK_EntPnt_UT wchar_t *UT_MakeFysakPath ( wchar_t *str, short mstr, const wchar_t *path, CONST wchar_t *filename );
SK_EntPnt_UT void UT_SortD(double *db_arr, short size);

SK_EntPnt_UT unsigned long *UT_InitFlag(short sNlin);
SK_EntPnt_UT void UT_CloseFlag(unsigned long *plFlag);
SK_EntPnt_UT bool UT_SetFlag(unsigned long *plFlag,short sLine);
SK_EntPnt_UT short UT_GetFlag(unsigned long *plFlag,short sLine);

SK_EntPnt_UT short UT_StartProsess(const wchar_t *pszCommandLine, short sVent, int nCmdSh = SW_SHOWNORMAL,
                                   HANDLE hStdInput = NULL, HANDLE hStdOutput = NULL, HANDLE hStdError = NULL);

SK_EntPnt_UT short UT_PutAnfTegn(wchar_t *pszTx, wchar_t cTegn, size_t sMaksLen);

/* --- Makroer -- */
#define UT_VENT 1
#define UT_FORTSETT 2



