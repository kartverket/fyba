/***************************************************************************
*                                                                           *
*       Hjelpebiblioteket   U T  (Utilities)                                *
*       Georg Langerak, KARTVERKET / FYSAK-prosjektet, januar 1989    *
*       Fil: UT1.C : Filhandtering                                          *
*                                                                           *
****************************************************************************/

/*
CH  UT1                                                 Leser-på-Disk
CD  ==================================================================
CD  Rutiner for å åpne, posisjonere og lese/skrive på filer åpnet
CD  for binær les/skriv som stream.
CD  ==================================================================
CD
CH  INSTALLERINGS- OG BRUKS-OPPLYSNINGER:
CD
CD  Bibliotek..: UTLE.LIB
CD  Kildefiler.: UT1.C + fyut.h
CD  Versjon....: E00
CD  Eier.......: KARTVERKET / FYSAK-prosjektet
CD  Ansvarlig..: Georg Langerak / Andreas Røstad
CD
CD  #include...: fyut.h
CD  ==================================================================
*/ 

#include "stdafx.h"

#ifdef OS2
#   define INCL_DOSFILEMGR
#   define INCL_DOSERRORS
#   include <os2.h>
#endif

#ifdef UNIX
#  ifndef _INCLUDE_POSIX_SOURCE
#     define _INCLUDE_POSIX_SOURCE
#  endif
#  ifndef _HPUX_SOURCE
#     define _HPUX_SOURCE
#  endif
#  include<sys/stat.h>
#endif

#ifdef LINUX /*This is just a quick fix. The two parameter versions of these are just not portable.*/
#   define strcpy_s strcpy
#   define strcat_s strcat 
#endif

#ifdef WIN32
#  include <sys/stat.h>
#endif

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#ifndef LINUX
  #include <share.h>
#endif

#include "fyut.h"

/*******************************************************************************
*                                                                              *
*       RUTINER FOR VERSJONSKONTROLL                                         *
*                                                                              *
*******************************************************************************/

/*
GL-890215
AR-911006
CH UT_OpenFile                                          Åpner og sjekker fil
CD ==========================================================================
CD Bruk:
CD  fp = UT_OpenFile(L"TULLFILA",L"DAT",READ,OLD,&ierr);
CD
CD Parametre:
CD Type           Navn      I/U   Forklaring
CD --------------------------------------------------------------------------
CD const wchar_t *fnam       i    Filnavn inkl. type (Max 46 tegn inkl. '\0')
CD const wchar_t *ftyp       i    Filtype (extention) (Max 4 tegn inkl. '\0')
CD short          facc       i    Aksess (0=UT_READ,1=UT_WRITE,2=UT_UPDATE)
CD short          exist      i    Åpningstatus (0=UT_UNKNOWN,1=UT_OLD,2=UT_NEW)
CD short         &ierr       u    (UT_OK,UT_ERROR eller skriv printf(strerror(ierr));
CD FILE           filpeker   r    filpeker.
CD
CD Formål:
CD Åpner en fil med angitt aksess og åpningstatus.
CD Returnerer status som kan benyttes videre eller utskrives
	==========================================================================
*/
SK_EntPnt_UT FILE *UT_OpenFile(const wchar_t *filnavn,const wchar_t *ftyp,short facc,short exist,short *ierr)
{
	FILE *fp;
	wchar_t mode[4];
	wchar_t drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	wchar_t path[_MAX_PATH];
	int filslutt = 0;

#  ifdef OS232
		FILESTATUS3 PathInfoBuf;
		APIRET rc;
#  endif

#  ifdef OS216
		FILESTATUS PathInfoBuf;
		int rc;
#  endif

#  ifdef UNIX
		struct stat buf;
		int status;
#  endif

#ifdef WIN32
		//struct _stat buf;
        struct _stat32i64 buf32i64;
		int status;
#endif

   /* Lag fullstendig filnavn */
	UT_FullPath(path,filnavn,_MAX_PATH);

	/* Splitt filnavnet */
	UT_splitpath(path,drive,dir,fname,ext);

	/* Ikke gitt filtype */
	if (*ext == L'\0') {
		if (ftyp != NULL) {
			if (*ftyp != L'\0') {
				UT_StrCopy(ext, L".", _MAX_EXT);
				UT_StrCat(ext, ftyp, _MAX_EXT);
			}
		}
	}

	/* Bygg opp filnavnet igjen */
	UT_makepath(path,drive,dir,fname,ext);

	mode[0] = L'\0';

	/* UT_FPRINTF(stderr,"\nFilnavn \"%s\"\n",path); */

	/* Hent filopplysninger */
#  ifdef OS232
		rc = DosQueryPathInfo(path,FIL_STANDARD,&PathInfoBuf,sizeof(FILESTATUS3));
		/* UT_FPRINTF(stderr,"Fra DosQeryPathInfo rc = %d\n",rc); */
#  endif
#  ifdef OS216
		rc = DosQPathInfo(path,FIL_STANDARD,(PBYTE)&PathInfoBuf,sizeof(FILESTATUS),0);
#  endif
#  ifdef UNIX
		status =  stat(path,&buf);
#  endif
#  ifdef WIN32
		//status = _stat(path,&buf);
      status = _wstat32i64(path,&buf32i64);
      /*
      if (status == -1){
         wchar_t szError[256];
         UT_strerror(szError,256,errno);
         szError[255] = '\0';
      }
      */

#  endif


	/* Setter sammen flagg for åpningsmodus */
	if (facc == UT_READ && exist == UT_UNKNOWN) {
		/* Hvis filen IKKE eksisterer */
#     ifdef OS2
		if (rc == ERROR_FILE_NOT_FOUND) {
#     endif
#     ifdef UNIX
		if (status == -1  &&  errno == ENOENT) {
#     endif
#     ifdef WIN32
		if (status == -1  &&  errno == ENOENT) {
#     endif
			UT_StrCopy(mode, L"wb+", 4);
		} else {      /* Filen finnes fra før */
			UT_StrCopy(mode, L"rb", 4);
		}
	} else if (facc == UT_READ  && exist == UT_OLD) {
		/* Filen finnes ikke */

#     ifdef OS2
		if (rc == ERROR_FILE_NOT_FOUND) {
#     endif
#     ifdef UNIX
		if (status == -1  &&  errno == ENOENT) {
#     endif
#     ifdef WIN32
		if (status == -1  &&  errno == ENOENT) {
#     endif
			*ierr = ENOENT;
			return (NULL);     /* ===> RETUR */
		}

		UT_StrCopy(mode, L"rb", 4);

	} else if (facc == UT_READ  && exist == UT_NEW) {
		/* Sjekk at filen IKKE eksisterer */
#     ifdef OS2
		if (rc == ERROR_FILE_NOT_FOUND) {
#     endif
#     ifdef UNIX
		if (status == -1  &&  errno == ENOENT) {
#     endif
#     ifdef WIN32
		if (status == -1  &&  errno == ENOENT) {
#     endif
			UT_StrCopy(mode, L"wb+", 4);
		} else {
			*ierr = EEXIST;
			return (NULL);     /* ===> RETUR */
		}

	} else if (facc == UT_WRITE && exist == UT_UNKNOWN) {
		UT_StrCopy(mode, L"wb+", 4);

	} else if (facc == UT_WRITE && exist == UT_OLD) {
		/* Filen finnes ikke */
#     ifdef OS2
		if (rc == ERROR_FILE_NOT_FOUND) {
#     endif
#     ifdef UNIX
		if (status == -1  &&  errno == ENOENT) {
#     endif
#     ifdef WIN32
		if (status == -1  &&  errno == ENOENT) {
#     endif
			*ierr = ENOENT;
			return (NULL);     /* ===> RETUR */
		}

		/* Hvis filen er OK */
#     ifdef OS2
		if (rc == NO_ERROR) {
#     endif
#     ifdef UNIX
		if (status == 0) {
#     endif
#     ifdef WIN32
		if (status == 0) {
#     endif
			UT_StrCopy(mode, L"wb+", 4);
		}

	} else if (facc == UT_WRITE && exist == UT_NEW) {
		/* Sjekk at filen IKKE eksisterer */
#     ifdef OS2
		if (rc == ERROR_FILE_NOT_FOUND) {
#     endif
#     ifdef UNIX
		if (status == -1  &&  errno == ENOENT) {
#     endif
#     ifdef WIN32
		if (status == -1  &&  errno == ENOENT) {
#     endif
			UT_StrCopy(mode, L"wb+", 4);
		}

	} else if (facc == UT_UPDATE && exist == UT_UNKNOWN) {
		/* Hvis filen IKKE eksisterer */
#     ifdef OS2
			if (rc == ERROR_FILE_NOT_FOUND) {
#     endif
#     ifdef UNIX
		if (status == -1  &&  errno == ENOENT) {
#     endif
#     ifdef WIN32
		if (status == -1  &&  errno == ENOENT) {
#     endif
			UT_StrCopy(mode, L"wb+", 4);

		} else {      /* Filen finnes fra før */
			UT_StrCopy(mode, L"rb+", 4);
			filslutt=1;
		}

	} else if (facc == UT_UPDATE  &&  exist == UT_OLD) {
		/* Filen finnes ikke */
#     ifdef OS2
		if (rc == ERROR_FILE_NOT_FOUND) {
#     endif
#     ifdef UNIX
		if (status == -1  &&  errno == ENOENT) {
#     endif
#     ifdef WIN32
		if (status == -1  &&  errno == ENOENT) {
#     endif
			*ierr = ENOENT;
			return (NULL);     /* ===> RETUR */
		}

		UT_StrCopy(mode, L"rb+", 4);
		filslutt=1;

	} else if (facc == UT_UPDATE  &&  exist == UT_NEW) {
		/* Sjekk at filen IKKE eksisterer */
#     ifdef OS2
		if (rc == ERROR_FILE_NOT_FOUND) {
#     endif
#     ifdef UNIX
		if (status == -1  &&  errno == ENOENT) {
#     endif
#     ifdef WIN32
		if (status == -1  &&  errno == ENOENT) {
#     endif
			UT_StrCopy(mode, L"wb+", 4);
		}

	} else {                                    /* Ugyldig kombinasjon */
		*ierr=UT_ERROR;
		return(NULL);
	}

/*
_SH_COMPAT : Sets compatibility mode
_SH_DENYRW : Denies read and write access to file
_SH_DENYWR : Denies write access to file
_SH_DENYRD : Denies read access to file
_SH_DENYNO : Permits read and write access
_SH_SECURE : Sets secure mode (shared read, exclusive write access).
*/

																
   // ----- Selve filåpningen
	if (mode[0] != L'\0') {
#ifdef UNIX
         fp = fopen(path,mode); 
#else
      if (mode[0] == L'w' || mode[2] == L'+')
      {
         fp = _wfsopen(path,mode,_SH_DENYWR);   // Sperrer mot at andre åpner filen for skriving, godtar lesing
         if (fp == NULL)
         {
            // Feil, venter 1/10 sekund og prøver en gang til, 5 ganger
            for (int i=0; fp==NULL && i<5; ++i)
            {

               UT_FPRINTF(stderr, L"%d> (UT_OpenFile: _wfsopen, _SH_DENYWR) Venter 0.1 sek.\n", GetCurrentThreadId());

               Sleep(100);
               fp = _wfsopen(path,mode,_SH_DENYWR);
            }
         }
      }           
      else
      {
         fp = _wfsopen(path,mode,_SH_DENYNO);
         if (fp == NULL)
         {
            // Feil, venter 1/10 sekund og prøver en gang til, 5 ganger
            for (int i=0; fp==NULL && i<5; ++i)
            {
               UT_FPRINTF(stderr, L"%d> (UT_OpenFile: _wfsopen, _SH_DENYNO) Venter 0.1 sek.\n", GetCurrentThreadId());

               Sleep(100);
               fp = _wfsopen(path,mode,_SH_DENYNO);
            }
         }
      }
#endif


      if (fp  == NULL)
      {
			*ierr = errno;
		}

      else
      {
			*ierr =UT_OK;
			if (filslutt)
         {
				// Posisjoner på slutten av filen
#           ifdef OS2
					UT_SetPos(fp,(long)PathInfoBuf.cbFile);
#           endif
#           ifdef UNIX
					UT_SetPos(fp,(long)buf.st_size);
#           endif
#           ifdef WIN32
					UT_SetPos_i64(fp,buf32i64.st_size);
#           endif
			}
		}

		/* UT_FPRINTF(stderr,"Ierr: %d\n",*ierr); */
		/* UT_FPRINTF(stderr,strerror(*ierr)); */
	}

   else 
   {
		*ierr = UT_ERROR;
		return (NULL);
	}

	if (*ierr != UT_OK)  fp=NULL;
	
   return fp;
}


/*
GL-890105
CH UT_ReadLineCrlf                     Leser en linje inkl cl/lf
CD ==================================================================
CD Formål......:  Leser en linje inkl. Crlf fra current posisjon.
CD
CD Aktivisering:  ist = UT_ReadLineCrlf(fil,llin,clin);
CD
CD Retur-verdier: short  UT_OK  - Lesing er ok
CD                short  UT_EOF - Du har lest End Of File
CD                short  UT_ERROR - Du har fått feil ved lesingen
CD
CD PARAMETERLISTE:
CD Navn      Type    i/U Merknad
CD --------------------------------------------------------------
CD pfil      FILE     i   Peker til filstruktur (def i stdio.h)
CD llin      short    i   Max lengde på clin inkl (\r\n\0)
CD clin      *wchar_t    u   Streng les. Må være 3 større enn behov.
	==================================================================
*/
SK_EntPnt_UT short UT_ReadLineCrlf(FILE *pfil,short llin, wchar_t *clin)
{
   wchar_t *cp = clin;
   wchar_t *siste = clin + llin - 1;
   short ierr = UT_OK;
   wchar_t c;

   while (cp < siste) {
      c = getwc(pfil);
      if (c == EOF) {
	      if (feof(pfil)) {
            if (cp == clin)  ierr = UT_EOF;
         } else if (ferror(pfil)) {
            ierr = UT_ERROR;
         }
         break;

      } else if (c == L'\n') {
         *cp = c;
         cp++;
         break;

      } else if (c == L'\0') {
         *cp = L' ';

      } else {
         *cp = c;
      }

      cp++;
   }

   *cp = L'\0';

   if ((int)*clin == 26) ierr = UT_EOF;      /* LS-890928 */


   return (ierr);
}


/*
GL-890105
CH UT_ReadLine                        Leser en linje og fjerner crlf
CD ==================================================================
CD Formål:
CD Leser en linje og fjerner crlf.
CD
CD PARAMETERLISTE:
CD Navn    Type      i/u  Merknad
CD -----------------------------------------------------------
CD pfil    FILE       i   Peker til filstruktur (def i stdio.h)
CD llin    short      i   Max lengde på clin inkl '\0'
CD clin   *wchar_t    u   Streng lest. Må være 3 større enn faktisk lest.
CD short    status    r   UT_OK  - Lesing er ok
CD                        UT_EOF - Du har lest End Of File
CD                        UT_ERROR - Du har fått feil ved lesingen
CD
CD Bruk:
CD ist = UT_ReadLine(fil,llin,clin);
==================================================================
*/
SK_EntPnt_UT short UT_ReadLine(FILE *pfil,short llin, wchar_t *pwcsLin)
{
   char *pszBuffer = (char *)malloc(llin*sizeof(char));
   char *bp = pszBuffer;

   UT_INT64 n64FilPos;
   UT_GetPos_i64(pfil,&n64FilPos);

   short sStatus = UT_ReadLine(pfil, llin, pszBuffer);

   if (sStatus == UT_OK)
   {
      // Starten av filen, hopp over eventuelt byte order mark (BOM)
      if (n64FilPos == 0)
      {
         if (strlen(pszBuffer) > 2  &&
             (unsigned char)pszBuffer[0] == 0xEF  &&
             (unsigned char)pszBuffer[1] == 0xBB  &&
             (unsigned char)pszBuffer[2] == 0xBF )
         {
            bp += 3;
         }
      }

      // Konvertering til wchar_t
      if (UT_ErNormalUtf8(bp) == UTF8_LOVLIG)
      {
         UT_KonverterTegnsett_8_16(TS_UTF8, bp, pwcsLin);
      }
      else
      {
         UT_KonverterTegnsett_8_16(TS_ISO8859, bp, pwcsLin);
      }
   }
   else
   { 
      *pwcsLin = L'\0';
   }
   free(pszBuffer);

   return (sStatus);
}


//////////////////////////////////////////////////////////////////////////
SK_EntPnt_UT short UT_ReadLine(FILE *pfil,short llin, char *pszLin)
{
   char *cp = pszLin;
   char *siste = pszLin + llin - 1;
   short ierr = UT_OK;
   char c;

   while (cp < siste) {
      c = getc(pfil);
      if (c == EOF) {
         if (feof(pfil)) {
            if (cp == pszLin)  ierr = UT_EOF;
         } else if (ferror(pfil)) {
            ierr = UT_ERROR;
         }
         break;

      } else if (c == '\n') {
         *cp = c;
         cp++;
         break;

      } else if (c == '\0') {
         *cp = ' ';

      } else {
         *cp = c;
      }

      cp++;
   }

   *cp = '\0';

   if ((int)*pszLin == 26) ierr = UT_EOF;      /* LS-890928 */


   /* Fjerner Crlf */
   if (cp-pszLin > 1) {
      if (*(cp-2) == '\r') {
         *(cp-2) = *(cp-1) = '\0'; /* crlf */
      } else if (*(cp-1) == '\n') {
         *(cp-1) = '\0';             /* lf */
      }
   }

   return (ierr);
}


/*
GL-890308
CH UT_ReadLineNoComm             Leser linjer og fjerner kommentarer
CD ==================================================================
CD Aktivisering :  ist = UT_ReadLineNoComm(fil,llin,clin);
CD
CD Retur-verdier: short  UT_OK  - Lesing er ok
CD                short  UT_EOF - Du har lest End Of File
CD                short  UT_ERROR - Du har fått feil ved lesingen
CD
CD PARAMETERLISTE:
CD Type     Navn    i/U Merknad
CD ------------------------------------------------------------------
CD FILE     fi       i   Peker til filstruktur (def i stdio.h)
CD short    llin     i   Max lengde på clin inkl \r\n\0.(cr+lf)
CD wchar_t    *clin     u   Streng lest. (Uten \r\n. (cr+lf))
CD
CD Formål:
CD Leser en linje fra filen, og fjerner CrLf. Hvis linjen er blank
CD eller første "ikke-blanke-tegn" er utropstegn leses påfølgende linje(r).
	==================================================================
*/
SK_EntPnt_UT short UT_ReadLineNoComm(FILE *pfil, short llin, wchar_t *clin)
{
	short ierr;
	register wchar_t *cp=NULL;

	do{
		 ierr = UT_ReadLine(pfil,llin,clin);
		 if (ierr == UT_OK){
			  for (cp=clin;  UT_IsSpace(*cp) && *cp;  cp++){
					;
			  }
		 }
	} while (ierr == UT_OK && (!*cp || *cp==L'!'));

	/* Fjern blanke på slutten */
	if (ierr == UT_OK) {
		UT_ClrTrailsp(clin);
	}

	return (ierr);
}


/*
PG-120314
CH UT_ReadFile                                           Leser en fil
CD ==================================================================
CD Formål:
CD Leser en fil.
CD
CD PARAMETERLISTE:
CD Navn     Type       i/u  Merknad
CD ------------------------------------------------------------------------
CD pfil      FILE       i   Peker til filstruktur (def i stdio.h)
CD lbufChar  long       i   Max lengde på buff inkl '\0'
CD lbufWchar long       i   Lengde på returnert wchar-buffer
CD buff     *wchar_t    u   Streng lest
CD short     status     r   UT_OK  - Lesing er ok
CD                          UT_EOF - Du har lest End Of File
CD                          UT_ERROR - Du har fått feil ved lesingen
CD
CD Bruk:
CD ist = UT_ReadFile(pfil, lbufChar, lbufWchar, buff);
============================================================================
*/
SK_EntPnt_UT short UT_ReadFile(FILE *pfil, long lbufChar, long &lbufWchar, wchar_t *pwcsBuf)
{
   char *pszBuffer = (char *)malloc(lbufChar*sizeof(char));

   short sStatus = UT_ReadFile(pfil, lbufChar, pszBuffer);

   char *psz = pszBuffer; 

   // Hopp over ev. byte order mark (BOM)
   if (strlen(pszBuffer) > 2  &&
      (unsigned char)pszBuffer[0] == 0xEF  &&
      (unsigned char)pszBuffer[1] == 0xBB  &&
      (unsigned char)pszBuffer[2] == 0xBF )
   {
      psz += 3;
   }

   if (sStatus == UT_OK)
   {
      if (UT_ErNormalUtf8(psz) == UTF8_LOVLIG)
      {
         UT_KonverterTegnsett_8_16(TS_UTF8, psz, pwcsBuf);
      }
      else
      {
         UT_KonverterTegnsett_8_16(TS_ISO8859, psz, pwcsBuf);
      }
   }
   else
   { 
      *pwcsBuf = L'\0';
   }
   // Lengde på den returnerte wchar-strengen
   lbufWchar = wcslen(pwcsBuf);

   free(pszBuffer);

   return (sStatus);
}

//////////////////////////////////////////////////////////////////////////
SK_EntPnt_UT short UT_ReadFile(FILE *pfil, long lbuf, char *pszBuf)
{
   char *cp = pszBuf;
   char *siste = pszBuf + lbuf - 1;
   short ierr = UT_OK;
   char c;

   while (cp < siste) {
      c = getc(pfil);
      if (c == EOF) {
         if (feof(pfil)) {
            if (cp == pszBuf)  ierr = UT_EOF;
         } else if (ferror(pfil)) {
            ierr = UT_ERROR;
         }
         break;

      /*} else if (c == '\n') {
         *cp = c;
         cp++;
         break;*/

      } else if (c == '\0') {
         *cp = ' ';

      } else {
         *cp = c;
      }

      cp++;
   }

   *cp = '\0';

   if ((int)*pszBuf == 26) ierr = UT_EOF;      /* LS-890928 */

   /* Fjerner Crlf */
   //if (cp-pszLin > 1) {
      //if (*(cp-2) == '\r') {
         //*(cp-2) = *(cp-1) = '\0'; /* crlf */
      //} else if (*(cp-1) == '\n') {
         //*(cp-1) = '\0';             /* lf */
      //}
   //}

   return (ierr);
}

//////////////////////////////////////////////////////////////////////////
SK_EntPnt_UT short UT_ReadFile(FILE *pfil, long lbuf, unsigned char *puszBuf)
{
   unsigned char *cp = puszBuf;
   unsigned char *siste = puszBuf + lbuf - 1;
   short ierr = UT_OK;
   char c;

   while (cp < siste) {
      c = getc(pfil);

      if (c == EOF) {
         if (feof(pfil)) {
            if (cp == puszBuf)  ierr = UT_EOF;
         } else if (ferror(pfil)) {
            ierr = UT_ERROR;
         }
         break;

      /*} else if (c == '\n') {
         *cp = c;
         cp++;
         break;*/

      } else if (c == '\0') {
         *cp = ' ';

      } else {
         *cp = c;
      }

      cp++;
   }

   *cp = '\0';

   if ((int)*puszBuf == 26) ierr = UT_EOF;      /* LS-890928 */


   /* Fjerner Crlf */
   //if (cp-puszLin > 1) {
      //if (*(cp-2) == '\r') {
         //*(cp-2) = *(cp-1) = '\0'; /* crlf */
      //} else if (*(cp-1) == '\n') {
         //*(cp-1) = '\0';             /* lf */
      //}
   //}

   return (ierr);
}

/*
GL-890105
PG-121029
CH UT_ReadXMLLine                        Leser en XML-linje uten crlf
CD ==================================================================
CD Formål:
CD Leser en linje uten crlf.
CD
CD PARAMETERLISTE:
CD Navn    Type      i/u  Merknad
CD -----------------------------------------------------------
CD pfil    FILE       i   Peker til filstruktur (def i stdio.h)
CD wstring wstrGMLLin u   Streng lest.
CD short   status     r   UT_OK  - Lesing er ok
CD                        UT_EOF - Du har lest End Of File
CD                        UT_ERROR - Du har fått feil ved lesingen
CD
CD Bruk:
CD ist = UT_ReadXMLLine(fil,wstrXMLLin);
==================================================================
*/
SK_EntPnt_UT short UT_ReadXMLLine(FILE *pfil, std::wstring &wstrXMLLin)
{
   UT_INT64 n64FilPos;
   UT_GetPos_i64(pfil, &n64FilPos);

   std::string strXMLLin;
   strXMLLin.reserve(256); // Setter av plass til 256 tegn
   short sStatus = UT_ReadXMLLine(pfil, strXMLLin);
   
   if (sStatus == UT_OK)
   {
      // Starten av filen, hopp over eventuelt byte order mark (BOM)
      if (n64FilPos == 0)
      {
         if (strXMLLin.length() > 2  &&
            (unsigned char)strXMLLin[0] == 0xEF  &&
            (unsigned char)strXMLLin[1] == 0xBB  &&
            (unsigned char)strXMLLin[2] == 0xBF )
         {
            strXMLLin.erase(0,3);
         }
      }
      // Konvertering til wchar_t. Skal alltid være UTF-8
      UT_KonverterTegnsett_8_16_UTF8(strXMLLin, wstrXMLLin);  
   }
   else
   { 
      wstrXMLLin = L'\0';
   }

   return (sStatus);
}

//////////////////////////////////////////////////////////////////////////
SK_EntPnt_UT short UT_ReadXMLLine(FILE *pfil, std::string &strXMLLin)
{
   short ierr = UT_OK;
   char c, cSiste = '\0'; // Dummy
   bool bLeserTegn = false;

   while (ierr == UT_OK) {
      c = getc(pfil);
      if (c == EOF) {
         if (feof(pfil)) {
            ierr = UT_EOF;
         } else if (ferror(pfil)) {
            ierr = UT_ERROR;
         }
         break;

      } else if ((c == ' ') || (c == '\t')) {
         if(bLeserTegn) // Fjerner ' ' og '\t' først på linja
         {
            cSiste = ' ';
            strXMLLin += ' ';
         }

      } else if (c == '>') { // Sjekker om neste tegn er '<' for å avslutte linje
         bLeserTegn = true;
         cSiste = '>';
         strXMLLin += c;
         // Håndterer space og tab mellom '>' og '<'
         char ch;
         bool bNyLinje = false;
         UT_INT64 lCurPos;
         UT_GetPos_i64(pfil, &lCurPos); // Finner posisjon i fila
         do {
            ch = getc(pfil);
            if(ch == '<')
            {
               bNyLinje = true; // Lager ny linje ved "><"
               break;
            }
         }  while((ch == ' ') || (ch == '\t'));

         UT_SetPos_i64(pfil, lCurPos); // Setter filpekeren tilbake

         if(bNyLinje)
            break;
       
      } else if ((c == '\r') || (c == '\n')) {
         if((cSiste == '\0') && (bLeserTegn)) // Ikke avsluttet objekt med '>' eller ' ' bakerst, må legge inn ' ' før neste linje - dersom ikke tom linje
            strXMLLin += ' ';
         break;

      } else if (c == '\0') {
         cSiste = ' ';
         strXMLLin += ' ';

      } else {
         bLeserTegn = true;
         cSiste = '\0'; // Dummy
         strXMLLin += c;
      }
   }
   strXMLLin += '\0';

   return (ierr);
}

/*
GL-890107
CH UT_WriteLineCrlf                      Skriver en linje incl crlf
CD ==================================================================
CD Formål......:  Skriver en linje som alt HAR Crlf på slutten .
CD
CD Aktivisering:  ist = UT_WriteLineCrlf(fil,clin);
CD
CD Retur-verdier: short  UT_OK  - Lesing er ok
CD                short  UT_ERROR - Du har fått feil ved lesingen
CD
CD PARAMETERLISTE:
CD Navn      Type    i/U Merknad
CD --------------------------------------------
CD pfil      FILE     i   Peker til filstruktur (def i stdio.h)
CD clin      wchar_t[]   i   Streng som skal skrives (må ha \r\n\0 )
   ==================================================================
*/
SK_EntPnt_UT short UT_WriteLineCrlf(FILE *pfil,wchar_t clin[])
{
   fputws(clin,pfil);
   if (ferror(pfil))  return UT_ERROR;
   return UT_OK;
}

/*
GL-890107
CH UT_WriteLine                           Skriver en linje uten crlf
CD ==================================================================
CD Formål......:  Skriver en linje og legger på Crlf etterpå .
CD
CD Aktivisering:  ist = UT_WriteLine(fil,clin);
CD
CD Retur-verdier: short  UT_OK  - Lesing er ok
CD                short  UT_ERROR - Du har fått feil ved lesingen
CD
CD PARAMETERLISTE:
CD Navn      Type    i/U Merknad
CD --------------------------------------------
CD pfil      FILE     i   Peker til filstruktur (def i stdio.h)
CD clin      wchar_t[]   i   Streng som skal skrives.
   ==================================================================
*/
SK_EntPnt_UT short UT_WriteLine(FILE *pfil,wchar_t clin[])
{
   fputws(clin,pfil);
   fputws( L"\r\n", pfil);

   if (ferror(pfil))  return(UT_ERROR);
   
   return(UT_OK);
}


//////////////////////////////////////////////////////////////////////////
SK_EntPnt_UT short UT_WriteLine(FILE *pfil,char clin[])
{
   fputs(clin, pfil);
   fputs( "\r\n", pfil);
   
   if (ferror(pfil))  return(UT_ERROR);
   
   return(UT_OK);
}
 
/*
AR:2012-03-12
CH UT_WriteLineUTF8                       Skriver en linje som UTF-8
CD ==================================================================
CD Formål:
CD Konverterer linje til UTF-8 og skriver den til fil. 
CD Legger også på CrLf på slutten.
CD
CD PARAMETERLISTE:
CD Type     Navn      I/U  Merknad
CD --------------------------------------------
CD FILE    *pfil      i    Peker til fil
CD wchar_t *pwszTx    i    Streng som skal skrives.
CD short    status    r    Status:  UT_OK    - Skrevet OK
CD                                  UT_ERROR - Skrivefeil
CD
CD Bruk:
CD ist = UT_WriteLineUTF8(fil,clin);
   ==================================================================
*/
SK_EntPnt_UT short UT_WriteLineUTF8(FILE* fi, const wchar_t *pwszTx)
{
   return UT_WriteUTF8(fi, pwszTx, true);
}


/*
AR:2012-03-12
CH UT_WriteUTF8                           Skriver en streng som UTF-8
CD ==================================================================
CD Formål:
CD Konverterer streng til UTF-8 og skriver den til fil - uten newline. 
CD
CD PARAMETERLISTE:
CD Type     Navn      I/U  Merknad
CD --------------------------------------------
CD FILE    *pfil      i    Peker til fil
CD wchar_t *pwszTx    i    Streng som skal skrives.
CD short    status    r    Status:  UT_OK    - Skrevet OK
CD                                  UT_ERROR - Skrivefeil
CD
CD Bruk:
CD ist = UT_WriteUTF8(fil, clin, bNewline);
   ==================================================================
*/
SK_EntPnt_UT short UT_WriteUTF8(FILE* fi, const wchar_t *pwszTx)
{  
   return UT_WriteUTF8(fi, pwszTx, false);
}

/*
AR:2012-03-12
CH UT_WriteUTF8                           Skriver en streng som UTF-8
CD ==================================================================
CD Formål:
CD Konverterer streng til UTF-8 og skriver den til fil. 
CD Kan legge på CrLf på slutten.
CD
CD PARAMETERLISTE:
CD Type     Navn      I/U  Merknad
CD --------------------------------------------
CD FILE    *pfil      i    Peker til fil
CD wchar_t *pwszTx    i    Streng som skal skrives.
CD bool     bNewline  i    Om newline skal legges på
CD short    status    r    Status:  UT_OK    - Skrevet OK
CD                                  UT_ERROR - Skrivefeil
CD
CD Bruk:
CD ist = UT_WriteUTF8(fil, clin, bNewline);
   ==================================================================
*/
SK_EntPnt_UT short UT_WriteUTF8(FILE* fi, const wchar_t *pwszTx, const bool bNewline)
{
   // Allokerer buffer og konverterer til UTF-8
   size_t len = UT_GetUtf8Len(pwszTx) + 1;
   if (len > 1)
   {
      char *pszBuffer = (char*)malloc(len);
      UT_KonverterTegnsett_16_8 (TS_UTF8,pwszTx,pszBuffer,len);

      // Skriver
      fputs(pszBuffer,fi);

      free (pszBuffer);
   }

   if(bNewline)
   {
      // Legger på CrLf
      fputs("\r\n", fi);
   }
   
   if (ferror(fi))  return(UT_ERROR);
   
   return(UT_OK);
}

/*
AR:2012-03-12
CH UT_WriteBomUTF8                                 Skriver UTF-8 BOM
CD ==================================================================
CD Formål:
CD Skriver UTF-8 BOM (byte order mark) til starten av tom fil. 
CD BOM er tre byte (0xEF, 0xBB, 0xBF)i starten av filen.
CD Dette viser at filen er UTF-8
CD
CD PARAMETERLISTE:
CD Type     Navn      I/U  Merknad
CD --------------------------------------------
CD FILE    *pfil      i    Peker til fil
CD short    status    r    Status:  UT_OK    - Skrevet OK
CD                                  UT_ERROR - Skrivefeil
CD
CD Bruk:
CD ist = UT_WriteBomUTF8(fil);
   ==================================================================
*/
SK_EntPnt_UT short UT_WriteBomUTF8(FILE* fi)
{
   unsigned char szBOM[4] = {0xEF, 0xBB, 0xBF, 0x00};

   // Går til enden av fila
   fseek(fi, 0, SEEK_END);

   // Finner posisjon i fila
   long lpos = ftell(fi);

   // Skriver UTF-8 BOM dersom fila er tom
   if(lpos == 0)
   {
      fputs((char*)szBOM, fi); // Skriver 3 bytes
      if (ferror(fi))  return(UT_ERROR);
   }
   return(UT_OK);
}


/*
GL-880809
CH UT_Save                             Berger diskbuffer ut på disk.
CD ==================================================================
CD Formål......:  Sikre at at operativsystemets filbuffer
CD                blir disklagret. (Sikre mot strømstans etc.)
CD
CD Aktivisering:  UT_Save(fil);
CD
CD Retur-verdier: short  UT_OK  - Lesing er ok
CD                short  UT_ERROR - Du har fått feil ved flushingen
CD
CD PARAMETERLISTE:
CD Navn      Type  Array i/U Merknad
CD ------------------------------------------------------------------
CD pfil      FILE         i   Peker til filstruktur (def i stdio.h)
   ==================================================================
*/
SK_EntPnt_UT short UT_Save (FILE *pfil)
{
   if (fflush(pfil) == EOF)  return (UT_ERROR);
 
   return(UT_OK);
}


/*
GL-871127
CH UT_SetPos                               Sette current filposisjon
CD ==================================================================
CD Formål......:  Setter filposisjon hvor det skal begynnes å lese fra
CD
CD Aktivisering:  ist = UT_SetPos(fil,lpos);
CD
CD Retur-verdier: short  UT_OK  - Lesing er ok
CD                short  UT_ERROR - Du har fått feil.
CD
CD PARAMETERLISTE:
CD Navn      Type  Array i/U Merknad
CD ------------------------------------------------------------------
CD pfil      FILE         i   Peker til filstruktur (def i stdio.h)
CD lpos      long         i   Fil-posisjon
   ==================================================================
*/
SK_EntPnt_UT short UT_SetPos  (FILE *fi,long lpos)
{
   if (fseek(fi,lpos,SEEK_SET)) return (UT_ERROR);
   return(UT_OK);
}


/*
CH UT_SetPos_i64                             Sette current filposisjon
CD ==================================================================
CD Formål......:  Setter filposisjon hvor det skal begynnes å lese fra
CD
CD Aktivisering:  ist = UT_SetPos_i64(fil,pos);
CD
CD Retur-verdier: short  UT_OK  - Utført OK.
CD                short  UT_ERROR - Du har fått feil.
CD
CD PARAMETERLISTE:
CD Type      Navn      i/U  Merknad
CD ------------------------------------------------------------------
CD FILE      pfil       i   Peker til filstruktur (def i stdio.h)
CD UT_INT64  n64FilPos  i   Fil-posisjon
   ==================================================================
*/
SK_EntPnt_UT short UT_SetPos_i64 (FILE *fi,UT_INT64 n64FilPos)
{
   if (_fseeki64(fi,n64FilPos,SEEK_SET)) return (UT_ERROR);
   return(UT_OK);
}


/*
GL-871127
CH UT_GetPos                                Finner neste filposisjon
CD ==================================================================
CD Formål......:  Finner NESTE posisjon på filen.
CD                Dette vil bli neste linje etter UT_Readxxx og,
CD                UT_Writexxxx.
CD
CD Aktivisering:  ist = UT_GetPos(fil,&lpos);
CD
CD Retur-verdier: short  UT_OK  - Lesing er ok
CD                short  UT_ERROR - Du har fått feil.
CD
CD PARAMETERLISTE:
CD Navn      Type  Array i/U Merknad
CD ------------------------------------------------
CD pfil      FILE         i   Peker til filstruktur (def i stdio.h)
CD lpos      long         u   Peker til filposisjon
   ==================================================================
*/
SK_EntPnt_UT short UT_GetPos(FILE *fi,long *lpos)
{
   *lpos = ftell(fi);
   if (*lpos == -1L ) return (UT_ERROR);
   return(UT_OK);
}


/*
CH UT_GetPos_i64                            Finner neste filposisjon
CD ==================================================================
CD Formål......:  Finner NESTE posisjon på filen.
CD                Dette vil bli neste linje etter UT_Readxxx og,
CD                UT_Writexxxx.
CD
CD Aktivisering:  ist = UT_GetPos_i64(fil,&n64FilPos);
CD
CD Retur-verdier: short  UT_OK  - Utført OK
CD                short  UT_ERROR - Du har fått feil.
CD
CD PARAMETERLISTE:
CD Type  Navn        i/U Merknad
CD ------------------------------------------------
CD FILE  pfil              i   Peker til filstruktur (def i stdio.h)
CD long  n64FilPos         u   Peker til filposisjon
   ==================================================================
*/
SK_EntPnt_UT short UT_GetPos_i64(FILE *fi,UT_INT64 *n64FilPos)
{
   *n64FilPos = _ftelli64(fi);
   if (*n64FilPos == -1L ) return (UT_ERROR);
   return(UT_OK);
}

short UT_fwprintfUTF8(FILE * _File, const wchar_t * _Format, ...)
{
   va_list args;
   int len;
   wchar_t * buffer;

   va_start( args, _Format );
   len = _vscwprintf( _Format, args ) // _vscprintf doesn't count
                                 + 1; // terminating '\0'
   buffer = (wchar_t*)malloc( len * sizeof(wchar_t) );
   vswprintf_s( buffer, len, _Format, args );

   short status = UT_WriteUTF8(_File, buffer);

   free( buffer );

   return status;
}

/*
   CString strFormat, strRest;
   wchar_t szOutput[1024];
   
   strFormat = _Format;
   bool bFormat = false;

   for(int i=0, j=0; _Format[i] != L'\0'; ++i, ++j)
   {
      // If escape character
      if(_Format[i] == L'\\') // Slår ikke til på \r\n - har ikke sjekket veldig godt på hva denne gjør...
      {
         ++i;
         ++j;
         continue;
      }
      // if not a substitutal character
      if(_Format[i] != L'%')
         continue;

      bFormat = true;
      ++i;
      ++j;
      CString strBackup = strFormat; // Husker hele _Format strengen - for å legge på reststrenger bak aktuell formatstreng lenger nede...
      CString strTekst = strFormat.Left(j); // Teksten før aktuell formatstreng
      strFormat = strFormat.Mid(j); // Selve aktuell formatstreng - bak %   
      int nType = strFormat.FindOneOf(L"cCdiouxXeEfgGaAnpsSZ"); // Henter ut selve aktuelle formatstreng
      strFormat = strFormat.Left(nType+1);
      //strFormat = strFormat.SpanIncluding(L"0123456789cCdiouxXeEfgGaAnpsSZlh.");
      short strFormatLen = strFormat.GetLength(); //Finner lengden på formatstrengen, siden den kan variere
      wchar_t szType = strFormat[strFormatLen-1]; //Finner riktig datatype
      strFormat = strTekst+strFormat; // Legger på alt foran formatstrengen
      strRest = strBackup.Mid(strFormat.GetLength()); // Leser ut rest bak formatstrengen

      switch(szType)
      {
         // string
         case L's':
         case L'S':
         case L'Z':
         {
            wchar_t* s = va_arg(argptr, wchar_t*); // Henter ut riktig argument i rekken
            swprintf_s(szOutput, strFormat, s);
            strFormat = szOutput;
            j = strFormat.GetLength()-strFormatLen;
            break;
         }  
         // character
         case L'c':
         case L'C':
         {
            wchar_t c = (wchar_t) va_arg(argptr, int); // Henter ut riktig argument i rekken
            swprintf_s(szOutput, strFormat, c);
            strFormat = szOutput;
            j = strFormat.GetLength()-strFormatLen;
            break;            
         }
         // integer
         case L'd':
         case L'i':
         case L'o':
         case L'u':
         case L'x':
         case L'X':
         case L'n':
         case L'p':
         {
            int d = va_arg(argptr, int); // Henter ut riktig argument i rekken
            swprintf_s(szOutput, strFormat, d);
            strFormat = szOutput;
            j = strFormat.GetLength()-strFormatLen;
            break;
         }
         // double
         case L'e':
         case L'E':
         case L'f':
         case L'g':
         case L'G':
         case L'a':
         case L'A':
         {
            double f = va_arg(argptr, double); // Henter ut riktig argument i rekken
            swprintf_s(szOutput, strFormat, f);
            strFormat = szOutput;
            j = strFormat.GetLength()-strFormatLen;
            break;
         }
      }
      UT_StrCat(szOutput, (wchar_t *)((LPCTSTR)strRest), 1024); // Lager ferdig output-streng
      strFormat += strRest;
   }
   va_end(argptr);

   if(!bFormat) // Ingen formatering, bare enkel streng
      UT_StrCopy(szOutput, strFormat, 1024);

   return UT_WriteUTF8(_File, szOutput);
}
*/

