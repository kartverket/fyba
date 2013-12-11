/*
AR-920214
CH FYHO                                       "Direkte" hoderutiner
CD =================================================================
CD
CD Eier.......: KARTVERKET / FYSAK-prosjektet
CD Ansvarlig..: Georg Langerak / Andreas Røstad
CD
CD Rutiner for å handtere hodet på SOSI-filer direkte på filen.
CD  ==============================================================
*/

#include "stdafx.h"

#include <math.h>
#include <time.h>
#include <ctype.h>
#include <fcntl.h>
              

/*
AR:2000-10-07
CH HO_GetTransEx                                       Finner .TRANSPAR i hodet
CD =============================================================================
CD Formål:
CD Henter ut innholdet under ..TRANSPAR fra fra filhodet.
CD
CD Parametre:
CD Type            Navn      I/U  Forklaring
CD --------------------------------------------------------------------------
CD wchar_t           *pszFil     i   Fullstendig filnavn
CD unsigned short *pusMaske  iu   [Inn] Styrer hvilke deler av TRANSPAR som skal hentes
CD                                [Ut] Viser hvilke deler av TRANSPAR som er funnet/hentet.
CD                                Følgende konstanter er definert:
CD                                  LC_TR_ALLT - Alle deler av ..TRANSPAR hentes
CD                                  LC_TR_KOORDSYS - Koordsys
CD                                  LC_TR_TRANSSYS - Transsys
CD                                  LC_TR_GEOSYS - Geosys
CD                                  LC_TR_GEOKOORD - Geokoord
CD                                  LC_TR_ORIGO - Origo-nø
CD                                  LC_TR_ENHET - Enhet
CD                                  LC_TR_ENHETH - Enhet-h
CD                                  LC_TR_ENHETD - Enhet-d
CD                                  LC_TR_VERTDATUM - Vert-datum
CD                                  LC_TR_VERTINT - Vert-int
CD                                  LC_TR_VERTDELTA - Vert-delta
CD
CD LC_TRANSPAR *    pTrans    iu   Peker til struktur som skal motta ..TRANSPAR informasjonen.
CD short           sStatus   r    Status: UT_TRUE=OK, UT_FALSE=feil (ikke funnet).
CD
CD Bruk:
CD unsigned short usMaske = LC_TR_ALLT;
CD LC_TRANSPAR Trans;
CD ist = HO_GetTransEx(L"Test.sos",&usMaske,&Trans);
CD ==========================================================================
*/
short CFyba::HO_GetTransEx(const wchar_t *pszFil,unsigned short *pusMaske, LC_TRANSPAR * pTrans)
{
   short sStatus;
   FILE * pFil;


   /* Åpner filen */
   pFil = UT_OpenFile(pszFil,L"SOS",UT_READ,UT_OLD,&sStatus);

   /* Åpnet OK ? */
   if (sStatus == UT_OK) {
      /* Hent verdier */
      sStatus = ho_GetTransEx(pFil,pusMaske,pTrans);
      fclose (pFil);

	/* Åpningsfeil på kladdefilen */
   } else {
      wchar_t szError[256];
      UT_strerror(szError,256,sStatus);
      UT_SNPRINTF(err.tx,LC_ERR_LEN,L" %s - %s",pszFil,szError);
      LC_Error(101,L"(HO_GetTransEx)",err.tx);
      memset(pTrans,0,sizeof(LC_TRANSPAR));
      *pusMaske = 0;
      sStatus = UT_FALSE;
   }

   return sStatus;
}


/*
AR:2000-10-07
CH ho_GetTransEx                                         Finner .TRANSPAR i hodet
CD =============================================================================
CD Henter ut innholdet under ..TRANSPAR fra fra filhodet.
CD
CD Parametre:
CD Type            Navn      I/U  Forklaring
CD --------------------------------------------------------------------------
CD FILE           *pFil       i   Filpeker til sosi-fil.
CD unsigned short *pusMaske  iu   [Inn] Styrer hvilke deler av TRANSPAR som skal hentes
CD                                [Ut] Viser hvilke deler av TRANSPAR som er funnet/hentet.
CD                                Følgende konstanter er definert:
CD                                  LC_TR_ALLT - Alle deler av ..TRANSPAR hentes
CD                                  LC_TR_KOORDSYS - Koordsys
CD                                  LC_TR_TRANSSYS - Transsys
CD                                  LC_TR_GEOSYS - Geosys
CD                                  LC_TR_GEOKOORD - Geokoord
CD                                  LC_TR_ORIGO - Origo-nø
CD                                  LC_TR_ENHET - Enhet
CD                                  LC_TR_ENHETH - Enhet-h
CD                                  LC_TR_ENHETD - Enhet-d
CD                                  LC_TR_VERTDATUM - Vert-datum
CD                                  LC_TR_VERTINT - Vert-int
CD                                  LC_TR_VERTDELTA - Vert-delta
CD
CD LC_TRANSPAR *    pTrans    iu   Peker til struktur som skal motta ..TRANSPAR informasjonen.
CD short           sStatus   r    Status: UT_TRUE=OK, UT_FALSE=feil (ikke funnet).
CD
CD Bruk:
CD unsigned short usMaske = LC_TR_ALLT;
CD LC_TRANSPAR Trans;
CD ist = ho_GetTransEx(pFil,&usMaske,&Trans);
   =============================================================================
*/
short CFyba::ho_GetTransEx(FILE *pFil,unsigned short *pusMaske, LC_TRANSPAR * pTrans)
{
   short lin,itxi;
   wchar_t *cp;
   short ist = UT_TRUE;
   unsigned short usMaskeInn = *pusMaske;


   /* Nullstiller pTrans */
   memset(pTrans,0,sizeof(LC_TRANSPAR));

   /* Nullstiller masken */
   *pusMaske = 0;

   /* Sjekk hvilket tegnsett som skal brukes */
	ho_GetTegnsett(pFil,&Sys.BufAdm.sTegnsett);

   /* ----- Div. kontroller ----- */
   /* Transpar */
   lin=2;
   if ( ! ho_GetVal(pFil,L"..TRANSPAR",&lin)) {
       LC_Error(14,L" (HO_GetTrans) ",L" ");
       return UT_FALSE;
   }

   /* ----- Henter verdier ----- */

   /* Koordsys */
   if ((usMaskeInn & LC_TR_KOORDSYS) != 0) {
      lin = 2;
      cp = ho_GetVal(pFil,L"...KOORDSYS",&lin);
      if (cp == NULL) {
         lin=2;
         ho_GetVal(pFil,L"..KOORDSYS",&lin);
      }
      if (cp != NULL) {
         *pusMaske |= LC_TR_KOORDSYS;
         UT_StrShort(cp,0,&itxi,&pTrans->sKoordsys);
         UT_StrToken(cp,itxi,&itxi,36,pTrans->szKoordsysDatum);
         UT_StrToken(cp,itxi,&itxi,36,pTrans->szKoordsysProjek);
      }
   }

   /* Transsys */
   if ((usMaskeInn & LC_TR_TRANSSYS) != 0) {
      lin = 2;
      if ((cp = ho_GetVal(pFil,L"...TRANSSYS",&lin)) != NULL) {
         *pusMaske |= LC_TR_TRANSSYS;
         UT_StrShort(cp,0,&itxi,&pTrans->sTranssysTilsys);
         UT_StrDbl(cp,itxi,&itxi,L'.',&pTrans->dTranssysKonstA1);
         UT_StrDbl(cp,itxi,&itxi,L'.',&pTrans->dTranssysKonstB1);
         UT_StrDbl(cp,itxi,&itxi,L'.',&pTrans->dTranssysKonstA2);
         UT_StrDbl(cp,itxi,&itxi,L'.',&pTrans->dTranssysKonstB2);
         UT_StrDbl(cp,itxi,&itxi,L'.',&pTrans->dTranssysKonstC1);
         UT_StrDbl(cp,itxi,&itxi,L'.',&pTrans->dTranssysKonstC2);
      }
   }

   /* Geosys */
   if ((usMaskeInn & LC_TR_GEOSYS) != 0) {
      lin = 2;
      if ((cp = ho_GetVal(pFil,L"...GEOSYS",&lin)) != NULL) {
         *pusMaske |= LC_TR_GEOSYS;
         UT_StrShort(cp,0,&itxi,&pTrans->sGeosysDatum);
         UT_StrShort(cp,itxi,&itxi,&pTrans->sGeosysProj);
         UT_StrShort(cp,itxi,&itxi,&pTrans->sGeosysSone);
      }
   }

   /* Geokoord */ 
   if ((usMaskeInn & LC_TR_GEOKOORD) != 0) {
      lin = 2;
      if ((cp = ho_GetVal(pFil,L"...GEOKOORD",&lin)) != NULL) {
         *pusMaske |= LC_TR_GEOKOORD;
         UT_StrShort(cp,0,&itxi,&pTrans->sGeoKoord);
      }
   }

   /* Origo */
   if ((usMaskeInn & LC_TR_ORIGO) != 0) {
      lin = 2;
      if ((cp = ho_GetVal(pFil,L"...ORIGO-NØ",&lin)) != NULL) {
         *pusMaske |= LC_TR_ORIGO;
         UT_StrDbl(cp,0,&itxi,L'.',&pTrans->Origo.dNord);
         UT_StrDbl(cp,itxi,&itxi,L'.',&pTrans->Origo.dAust);
      }
   }

   /* Enhet */
   if ((usMaskeInn & LC_TR_ENHET) != 0) {
      lin = 2;
      if ((cp = ho_GetVal(pFil,L"...ENHET",&lin)) != NULL) {
         *pusMaske |= LC_TR_ENHET;
         pTrans->dEnhet = wcstod(cp,&cp);
      }
   }

   /* Enhet-h */
   if ((usMaskeInn & LC_TR_ENHETH) != 0) {
      lin=2;
      if ((cp = ho_GetVal(pFil,L"...ENHET-H",&lin)) == NULL) {
        pTrans->dEnhet_h = pTrans->dEnhet;
      } else {
        *pusMaske |= LC_TR_ENHETH;
        pTrans->dEnhet_h = wcstod(cp,&cp);
      }
   }

   /* Enhet-d */
   if ((usMaskeInn & LC_TR_ENHETD) != 0) {
      /* Enhet-d */
      lin=2;
      if ((cp = ho_GetVal(pFil,L"...ENHET-D",&lin)) == NULL) {
        pTrans->dEnhet_d = pTrans->dEnhet;
      } else {
        *pusMaske |= LC_TR_ENHETD;
        pTrans->dEnhet_d = wcstod(cp,&cp);
      }
   }

   /* Vert-datum */
   if ((usMaskeInn & LC_TR_VERTDATUM) != 0) {
      lin = 2;
      if ((cp = ho_GetVal(pFil,L"...VERT-DATUM",&lin)) != NULL) {
         *pusMaske |= LC_TR_VERTDATUM;
         UT_StrToken(cp,0,&itxi,7,pTrans->szVertdatHref);
         UT_StrToken(cp,itxi,&itxi,6,pTrans->szVertdatDref);
         UT_StrToken(cp,itxi,&itxi,6,pTrans->szVertdatFref);
         UT_StrToken(cp,itxi,&itxi,2,pTrans->szVertdatHtyp);
      }
   }
   
   /* Vert-int */
   if ((usMaskeInn & LC_TR_VERTINT) != 0) {
      lin = 2;
      if ((cp = ho_GetVal(pFil,L"...VERT-INT",&lin)) != NULL) {
         *pusMaske |= LC_TR_VERTINT;
         UT_StrShort(cp,0,&itxi,&pTrans->sVertintHref);
         UT_StrShort(cp,itxi,&itxi,&pTrans->sVertintDref);
         UT_StrShort(cp,itxi,&itxi,&pTrans->sVertintFref);
      }
   }

   /* Vert-delta */
   if ((usMaskeInn & LC_TR_VERTDELTA) != 0) {
      lin = 2;
      if ((cp = ho_GetVal(pFil,L"...VERT-DELTA",&lin)) != NULL) {
         *pusMaske |= LC_TR_VERTDELTA;
         UT_StrShort(cp,0,&itxi,&pTrans->sVdeltaMin);
         UT_StrShort(cp,itxi,&itxi,&pTrans->sVdeltaMax);
      }
   }


   /* ----- Div. sluttkontroll ----- */

   /* Kontroller at det er funnet Koordsys, Transsys eller Geosys */
   if ((usMaskeInn & LC_TR_KOORDSYS) != 0  ||
       (usMaskeInn & LC_TR_TRANSSYS) != 0  ||
       (usMaskeInn & LC_TR_GEOSYS) != 0     ) {
      if ((*pusMaske & LC_TR_KOORDSYS) == 0  &&
          (*pusMaske & LC_TR_TRANSSYS) == 0  &&
          (*pusMaske & LC_TR_GEOSYS) == 0 ) {
         /* Ikke noe koordinatsystem funnet */
         LC_Error(15,L"(ho_GetTransEx)",L"");
         ist = UT_FALSE;
      }
   }

   /* Kontroller at det er funnet Origo */
   if ((usMaskeInn & LC_TR_ORIGO) != 0 &&  
       (*pusMaske & LC_TR_ORIGO) == 0 ) {
      /* Origo mangler */
      LC_Error(16,L"(ho_GetTransEx)",L"");
      ist = UT_FALSE;
   }

   /* Kontroller at det er funnet Enhet */
   if ((usMaskeInn & LC_TR_ENHET) != 0  &&  
       (*pusMaske & LC_TR_ENHET) == 0 ) {
      /* Enhet mangler */
      LC_Error(17,L"(ho_GetTransEx)",L"");
      ist = UT_FALSE;
   }

   return ist;
}



/*
AR:1999-07-12
CH HO_GetTrans                                         Finner .TRANSPAR i hodet
CD =============================================================================
CD Formål:
CD Henter transformasjonsparametrene fra filhodet.
CD
CD OBS! Denne rutinen opprettholdes bare for bakoverkompatibilitet.
CD      For nye programmer bør HO_GetTransEx benyttes. HO_GetTransEx er
CD      kompatibel med nye versjoner av SOSI.
CD
CD Parametre:
CD Type     Navn       I/U   Forklaring
CD -----------------------------------------------------------------------------
CD wchar_t    *pszFil      i   Fullstendig filnavn
CD short   *koosys      u   Koordinatsystem
CD double  *origo_a     u   ..ORIGO-AUST
CD double  *origo_n     u   ..ORIGO-NORD
CD double  *enhet       u   ...ENHET
CD double  *enhet_h     u   ...ENHET-H
CD double  *enhet_d     u   ...ENHET-D
CD short    sStatus     r   UT_TRUE, eller UT_FALSE.
CD
CD Bruk:
CD sStatus = HO_GetTrans(fil,&koosys,&origo_a,&origo_n,&enhet,&enhet_h,&enhet_d);
   =============================================================================
*/
short CFyba::HO_GetTrans(const wchar_t *pszFil,short *koosys,double *origo_a,
					  double *origo_n,double *enhet,double *enhet_h,double *enhet_d)
{
   short sStatus;
   FILE * pFil;

   /* Åpner filen */
   pFil = UT_OpenFile(pszFil,L"",UT_READ,UT_OLD,&sStatus);

   /* Åpnet OK ? */
   if (sStatus == UT_OK) {
      /* Hent verdier */
      sStatus = ho_GetTrans(pFil,koosys,origo_a,origo_n,enhet,enhet_h,enhet_d);
      fclose (pFil);

	/* Åpningsfeil på kladdefilen */
   } else {
      wchar_t szError[256];
      UT_strerror(szError,256,sStatus);
      UT_SNPRINTF(err.tx,LC_ERR_LEN,L" %s - %s",pszFil,szError);
      LC_Error(101,L"(HO_GetTrans)",err.tx);
      sStatus = UT_FALSE;
   }

   return sStatus;
}


/*
GL-880427
AR-900314
CH ho_GetTrans                                         Finner .TRANSPAR i hodet
CD =============================================================================
CD Formål:
CD Henter transformasjonsparametrene fra filhodet.
CD
CD Parametre:
CD Type     Navn        I/U   Forklaring
CD -----------------------------------------------------------------------------
CD FILE    *pFil         i   Filpeker til sosi-fil.
CD short   *koosys       u   Koordinatsystem
CD double  *origo_a      u   ..ORIGO-AUST
CD double  *origo_n      u   ..ORIGO-NORD
CD double  *enhet        u   ...ENHET
CD double  *enhet_h      u   ...ENHET-H
CD double  *enhet_d      u   ...ENHET-D
CD short    sStatus      r   UT_TRUE, eller UT_FALSE.
CD
CD Bruk:
CD ho_GetTrans(pFil,&koosys,&origo_a,&origo_n,&enhet,&enhet_h,&enhet_d);
   =============================================================================
*/
short CFyba::ho_GetTrans(FILE *pFil,short *koosys,double *origo_a,
					  double *origo_n,double *enhet,double *enhet_h,double *enhet_d)
{
   short lin;
   wchar_t *cp;


   /* Sjekk hvilket tegnsett som skal brukes */
	ho_GetTegnsett(pFil,&Sys.BufAdm.sTegnsett);

   lin=2;
   if ( ! ho_GetVal(pFil,L"..TRANSPAR",&lin)) {
       LC_Error(14,L" (HO_GetTrans) ",L" ");
       return UT_FALSE;
   }

   
   *koosys=0;
   lin=2;
   cp = ho_GetVal(pFil,L"...KOORDSYS",&lin);
   if (cp == NULL) {
   lin=2;
   ho_GetVal(pFil,L"..KOORDSYS",&lin);
   }
   if (cp == NULL) {
    LC_Error(15,L" (HO_GetTrans) ",L" ");
    return UT_FALSE;

   } else {
     wchar_t *ep;
     *koosys = (short)wcstol(cp,&ep,10);
   }

   lin=2;
   if ((cp = ho_GetVal(pFil,L"...ORIGO-NØ",&lin)) == NULL) {
    LC_Error(16,L" (HO_GetTrans) ",L" ");
    return UT_FALSE;

   } else {
     *origo_n = wcstod(cp,&cp);
     *origo_a = wcstod(cp,&cp);
     lin=2;
     if ((cp = ho_GetVal(pFil,L"...ENHET",&lin)) == NULL) {
        LC_Error(17,L" (HO_GetTrans) ",L" ");
        return UT_FALSE;
     } else {
        *enhet = wcstod(cp,&cp);
     }
     lin=2;
     if ((cp = ho_GetVal(pFil,L"...ENHET-H",&lin)) == NULL) {
         *enhet_h = *enhet;
     } else {
         *enhet_h = wcstod(cp,&cp);
     }
     lin=2;
     if ((cp = ho_GetVal(pFil,L"...ENHET-D",&lin)) == NULL) {
         *enhet_d = *enhet;
     } else {
         *enhet_d = wcstod(cp,&cp);
     }
   }

   return UT_TRUE;
}


/*
AR-890823
CH HO_GetOmr                                            Finner ..OMRÅDE i hodet
CD =============================================================================
CD Formål:
CD Henter områdeangivelsen fra filhodet.
CD
CD Parametre:
CD Type     Navn        I/U   Forklaring
CD -----------------------------------------------------------------------------
CD wchar_t    *pszFil      i   Fullstendig filnavn
CD double  *nv_a        u
CD double  *nv_n        u
CD double  *oh_a        u
CD double  *oh_n        u
CD short    sStatus     r   UT_TRUE, eller UT_FALSE.
CD
CD Bruk:
CD sStatus = HO_GetOmr(pszFil,&nv_a,&nv_n,&oh_a,&oh_n);
	=============================================================================
*/
short CFyba::HO_GetOmr(const wchar_t * pszFil,double *nv_a,double *nv_n,double *oh_a,double *oh_n)
{
   short sStatus;
   FILE * pFil;

   /* Åpner filen */
   pFil = UT_OpenFile(pszFil,L"",UT_READ,UT_OLD,&sStatus);

   /* Åpnet OK ? */
   if (sStatus == UT_OK) {
      /* Hent verdier */
      ho_GetOmr(pFil,nv_a,nv_n,oh_a,oh_n);
      fclose (pFil);
      sStatus = UT_TRUE;

	/* Åpningsfeil */
   } else {
      wchar_t szError[256];
      UT_strerror(szError,256,sStatus);
      UT_SNPRINTF(err.tx,LC_ERR_LEN,L" %s - %s",pszFil,szError);
      LC_Error(101,L"(HO_GetOmr)",err.tx);
      sStatus = UT_FALSE;
   }

   return sStatus;
}


/*
AR-890823
CH ho_GetOmr                                            Finner ..OMRÅDE i hodet
CD =============================================================================
CD Formål:
CD Henter områdeangivelsen fra filhodet.
CD
CD Parametre:
CD Type     Navn        I/U   Forklaring
CD -----------------------------------------------------------------------------
CD FILE    *fil          i    Filpeker til sosi-fil.
CD double  *nv_a         u
CD double  *nv_n         u
CD double  *oh_a         u
CD double  *oh_n         u
CD short    sStatus      r    UT_TRUE, eller UT_FALSE.
CD
CD Bruk:
CD sStatus = ho_GetOmr(fil,&nv_a,&nv_n,&oh_a,&oh_n);
	=============================================================================
*/
short CFyba::ho_GetOmr(FILE *fil,double *nv_a,double *nv_n,double *oh_a,double *oh_n)
{
   short lin,i;
   wchar_t *cp;
   short sStatus = UT_TRUE;


   /* Sjekk hvilket tegnsett som skal brukes */
   ho_GetTegnsett(fil,&Sys.BufAdm.sTegnsett);


   lin=2;
   if (ho_GetVal(fil,L"..OMRÅDE",&lin) == NULL) {
      LC_Error(7,L"(HO_GetOmr)",L"");
      *nv_n = -9999999.0;
      *nv_a = -9999999.0;
      *oh_n =  9999999.0;
      *oh_a =  9999999.0;
      sStatus = UT_FALSE;


   } else {
      /* Min-NØ */
      i = lin;
	   if ((cp = ho_GetVal(fil,L"...MIN-NØ",&i)) == NULL ) {
         LC_Error(8,L"(HO_GetOmr)",L"");
         *nv_n = -9999999.0;
         *nv_a = -9999999.0;
         sStatus = UT_FALSE;
      
      } else {
         *nv_n = wcstod(cp,&cp);
         *nv_a = wcstod(cp,&cp);
      }
      
		/* Max-NØ */
      i = lin;
      if ((cp = ho_GetVal(fil,L"...MAX-NØ",&i)) == NULL) {
         LC_Error(9,L"(HO_GetOmr)",L"");
         *oh_n = 999999.0;
         *oh_a = 9999999.0;
         sStatus = UT_FALSE;
      
      } else{
         *oh_n = wcstod(cp,&cp);
         *oh_a = wcstod(cp,&cp);
      }
   }

   /* Sjekker at området har utstrekning */
   //if (*oh_a-*nv_a < 0.001  ||  *oh_n-*nv_n < 0.001) {
   //   LC_Error(104,L"(HO_GetOmr)",L"");
   //   *nv_n = -9999999.0;
   //   *nv_a = -9999999.0;
   //   *oh_n =  9999999.0;
   //   *oh_a =  9999999.0;
	//}

   return sStatus;
}

 
/*
AR:1999-07-12
CH HO_GetKvalitet                                 Finner kvalitetsopplysninger
CD =============================================================================
CD Formål:
CD Finne kvalitetsopplysninger i filhode.
CD (Ikke aktuellt etter SOSI v. 4.00.)
CD
CD Parametre:
CD Type    Navn              I/U  Forklaring
CD -----------------------------------------------------------------------------
CD wchar_t   *pszFil             i  Fullstendig filnavn
CD short  *psMetode           u  Hvordan data er registrert.
CD                                 KVAL_MET_UNDEF  metode er udefinert.
CD long   *pLNnoyaktighet     u  Registreringsnøyaktighet
CD                                 KVAL_NOY_UKJENT  nøyaktighet er ukjent.
CD short  *psSynbarhet        u  Synbarhet i bilde
CD                                 KVAL_SYN_UNDEF  synbarhet er udefinert.
CD short  *psHoydeMetode      u  Hvordan høyden er registrert.
CD                                 KVAL_MET_UNDEF  metode er udefinert.
CD long   *plHoydeNoyaktighet u  Registreringsnøyaktighet
CD                                 KVAL_NOY_UKJENT  nøyaktighet er ukjent.
CD short   ist                r  Statusvariabel: UT_TRUE  - OK, ..KVALITET er funnet
CD                                               UT_FALSE - ikke funnet
CD
CD Bruk:
CD      ist = HO_GetKvalitet(fil,&sMetode,&lNoyaktighet,&sSynbarhet,
CD                           &sHoydeMetode,&lHoydeNoyaktighet);
CD =============================================================================
*/
short CFyba::HO_GetKvalitet(const wchar_t *pszFil,short *psMetode,long *plNoyaktighet,
                     short *psSynbarhet,short *psHoydeMetode,long *plHoydeNoyaktighet)
{
   short sStatus;
   FILE * pFil;

   /* Åpner filen */
   pFil = UT_OpenFile(pszFil,L"",UT_READ,UT_OLD,&sStatus);

   /* Åpnet OK ? */
   if (sStatus == UT_OK) {
      /* Hent verdier */
      sStatus = ho_GetKvalitet(pFil,psMetode,plNoyaktighet,psSynbarhet,psHoydeMetode,plHoydeNoyaktighet);
      fclose (pFil);

	/* Åpningsfeil */
   } else {
      wchar_t szError[256];
      UT_strerror(szError,256,sStatus);
      UT_SNPRINTF(err.tx,LC_ERR_LEN,L" %s - %s",pszFil,szError);
      LC_Error(101,L"(HO_GetKvalitet)",err.tx);
      sStatus = UT_FALSE;
   }

   return sStatus;
}
  

/*
OJ-891123
CH ho_GetKvalitet                                 Finner kvalitetsopplysninger
CD =============================================================================
CD Formål:
CD Finne kvalitetsopplysninger i filhode.
CD (Ikke aktuellt etter SOSI v. 4.00.)
CD
CD Parametre:
CD Type    Navn              I/U  Forklaring
CD -----------------------------------------------------------------------------
CD FILE   *pFil               i  Filpeker til sosi-fil.
CD short  *psMetode           u  Hvordan data er registrert.
CD                                 KVAL_MET_UNDEF  metode er udefinert.
CD long   *pLNnoyaktighet     u  Registreringsnøyaktighet
CD                                 KVAL_NOY_UKJENT  nøyaktighet er ukjent.
CD short  *psSynbarhet        u  Synbarhet i bilde
CD                                 KVAL_SYN_UNDEF  synbarhet er udefinert.
CD short  *psHoydeMetode      u  Hvordan høyden er registrert.
CD                                 KVAL_MET_UNDEF  metode er udefinert.
CD long   *plHoydeNoyaktighet u  Registreringsnøyaktighet
CD                                 KVAL_NOY_UKJENT  nøyaktighet er ukjent.
CD short   ist                r  Statusvariabel: UT_TRUE  - OK, ..KVALITET er funnet
CD                                               UT_FALSE - ikke funnet
CD
CD Bruk:
CD      ist = ho_GetKvalitet(fil,&sMetode,&lNoyaktighet,&sSynbarhet,
CD                           &sHoydeMetode,&lHoydeNoyaktighet);
CD =============================================================================
*/
short CFyba::ho_GetKvalitet(FILE *pFil,short *psMetode,long *plNoyaktighet,
                     short *psSynbarhet,short *psHoydeMetode,long *plHoydeNoyaktighet)
{
	short lin;
   wchar_t *cp;
   short ist = UT_FALSE;


   /* Sjekk hvilket tegnsett som skal brukes */
   ho_GetTegnsett(pFil,&Sys.BufAdm.sTegnsett);

   lin=2;
   if ((cp = ho_GetVal(pFil,L"..KVALITET",&lin)) != NULL) {     /* Kvalitet */
      ist = UT_TRUE;
   }

   /* Tolk strengen til tallverdier */
   LN_TolkKvalitet(cp,psMetode,plNoyaktighet,psSynbarhet,
                   psHoydeMetode,plHoydeNoyaktighet);


   /* Handter manglende høyde-kvalitet spesiellt */
	if (*psHoydeMetode == KVAL_MET_UNDEF)      *psHoydeMetode      = *psMetode;
   if (*plHoydeNoyaktighet == KVAL_NOY_UKJENT) *plHoydeNoyaktighet = *plNoyaktighet;

   return ist;
}


/*
AR-920331
CH HO_GetTegnsett                                            Finner tegnsett
CD ==========================================================================
CD Formål:
CD Finne tegnsett i filhodet.
CD
CD Parametre:
CD Type      Navn       I/U  Forklaring
CD --------------------------------------------------------------------------
CD wchar_t  *pszFil      i   Fullstendig filnavn
CD short    *psTegnsett  u   Tegnsett, konstanter definert:
CD                             TS_UKJENT  = Fikk ikke sjekket tegnsett
CD                             TS_DOSN8   = DOS norsk 8-bits(standardverdi)
CD                             TS_ND7     = Norsk Data 7-bits
CD                             TS_DECM8   = DEC multinasjonal 8-bits
CD                             TS_ISO8859 = ISO8859-10 Norsk/samisk tegnsett
CD                             TS_DECN7   = DEC norsk 7-bits
CD                             TS_UTF8    = UTF-8
CD short     sStatus     r   Status: UT_TRUE  = Funnet
CD                                   UT_FALSE = Ikke funnet
CD
CD Bruk:
CD      sStatus = HO_GetTegnsett(pszFil,&sTegnsett);
CD ==========================================================================
*/
short CFyba::HO_GetTegnsett(const wchar_t *pszFil,short *psTegnsett)
{
   short sStatus;
   FILE * pFil;

   /* Åpner filen */
   pFil = UT_OpenFile(pszFil,L"",UT_READ,UT_OLD,&sStatus);

   /* Åpnet OK ? */
   if (sStatus == UT_OK) {
      /* Hent verdier */
      sStatus = ho_GetTegnsett(pFil,psTegnsett);
      fclose (pFil);

	/* Åpningsfeil */
   } else {
      wchar_t szError[256];
      UT_strerror(szError,256,sStatus);
      UT_SNPRINTF(err.tx,LC_ERR_LEN,L" %s - %s",pszFil,szError);
      LC_Error(101,L"(HO_GetTegnsett)",err.tx);
      sStatus = UT_FALSE;
   }

   return sStatus;
}


/*
AR-920331
CH ho_GetTegnsett                                            Finner tegnsett
CD ==========================================================================
CD Formål:
CD Finne tegnsett i filhodet.
CD
CD Parametre:
CD Type     Navn      I/U  Forklaring
CD --------------------------------------------------------------------------
CD FILE    *fil        i   Filpeker til sosi-fil.
CD short  *psTegnsett  u   Tegnsett, konstanter definert:
CD                            TS_UKJENT  = Fikk ikke sjekket tegnsett
CD                            TS_DOSN8   = DOS norsk 8-bits(standardverdi)
CD                            TS_ND7     = Norsk Data 7-bits
CD                            TS_DECM8   = DEC multinasjonal 8-bits
CD                            TS_ISO8859 = ISO8859-10 Norsk/samisk tegnsett
CD                            TS_DECN7   = DEC norsk 7-bits
CD                            TS_UTF8    = UTF-8
CD short   sStatus     r   Status: UT_TRUE  = Funnet
CD                                 UT_FALSE = Ikke funnet
CD
CD Bruk:
CD      sStatus = ho_GetTegnsett(fil,&sTegnsett);
CD ==========================================================================
*/
short CFyba::ho_GetTegnsett(FILE *pFil,short *psTegnsett)
{
   char tx[LC_MAX_SOSI_LINJE_LEN];
   UT_INT64 startpos;
   char *cp, *ce;
   short ierr;
   short ist = UT_FALSE;
   bool bFerdig = false;

   *psTegnsett = TS_UKJENT;       
 

   // Søk fram til .HODE
   if (ho_FinnHode(pFil, &startpos) == UT_TRUE)
   {
      // Leser hodet
      UT_SetPos_i64(pFil,startpos);
      UT_ReadLine(pFil,LC_MAX_SOSI_LINJE_LEN,tx);

      // Skann resten av hodet
      do 
      {
         // Les en linje - Overser blanke, fyll (!!!!!!) og kommentar
         do
         {
            if ((ierr = UT_ReadLine(pFil,LC_MAX_SOSI_LINJE_LEN,tx)) != UT_OK)
            {
               return UT_FALSE;     // ===>  Retur pga. lesefeil
            }
         } while (ho_TestFyllKommentar(tx) == UT_TRUE);

         // Hopp over ledende blanke
         cp = &tx[0];
         while (UT_IsSpace(*cp))  ++cp;

         // Sjekk linjen om det er ..TEGNSETT
         if (strncmp(cp,"..TEGNSETT ",11) == 0) 
         {
            bFerdig = true;
                     
            // Hopp fram til første ikkje-blanke etter navnet
            cp += 11;
            while (UT_IsSpace(*cp))  ++cp;

            // Hvis strengen ikke er ferdigbehandla
            if (*cp != '\0')
            {  

               // Avgrensa av doble hermeteikn
               if (*cp == '"')
               {
                  ce = cp + 1;
                  while (*ce != '\0'  && *ce != '"')  ++ce;
                  if (*ce == '"')  ++cp;
                  *ce = '\0';
               } 

               // Avgrensa av enkle hermeteikn
               else if (*cp == '\'')
               {
                  ce = cp + 1;
                  while (*ce != '\0'  && *ce != '\'')  ++ce;
                  if (*ce == '\'')  ++cp;
                  *ce = '\0';
               } 

               // Vanlig ord
               else
               {
                  ce = cp + 1;
                  while (*ce != '\0'  &&  ( ! UT_IsSpace(*ce)))  ++ce;
                  *ce = '\0';
               }
            }
         }
      } while ( ! bFerdig );


      if (*cp != '\0')
      {
         if (strcmp(cp,"ISO8859-10") == 0) {
            *psTegnsett = TS_ISO8859;
            ist = UT_TRUE;

         } else if (strcmp(cp,"DOSN8") == 0) {
            *psTegnsett = TS_DOSN8;
            ist = UT_TRUE;

         } else if (strcmp(cp,"ISO8859-1") == 0) {
            *psTegnsett = TS_ISO8859;
            ist = UT_TRUE;

         } else if (strcmp(cp,"ANSI") == 0) {
            *psTegnsett = TS_ISO8859;
            ist = UT_TRUE;

         } else if (strcmp(cp,"ND7") == 0) {
            *psTegnsett = TS_ND7;
            ist = UT_TRUE;

         } else if (strcmp(cp,"DECN7") == 0) {
            *psTegnsett = TS_DECN7;
            ist = UT_TRUE;

         } else if (strcmp(cp,"DECM8") == 0) {
            *psTegnsett = TS_DECM8;
            ist = UT_TRUE;

         } else if (strcmp(cp,"UTF-8") == 0) {
            *psTegnsett = TS_UTF8;
            ist = UT_TRUE;
         }
         else
         {
            wchar_t wszTegnsett[100];
            UT_KonverterTegnsett_8_16(TS_ISO8859,cp,wszTegnsett);
            LC_Error(165, L"(HO_GetTegnsett)", wszTegnsett);
            ist = UT_FALSE;
         }

      }
   }

   return ist;
}


/*
AR:1999-07-14
CH HO_GetVal                                      Finn verdien til et SOSI-navn
CD =============================================================================
CD Formål:
CD Henter parametrene til et SOSI-navn.
CD Strengen ligger i et felles "returbuffer" for alle get-rutiner i fyba.
CD Dette blir ødelagt ved neste kall til en "get-rutine". For å ta vare på
CD strengen må den kopieres over til egen streng. (Bruk strcpy).
CD
CD Parametre:
CD Type     Navn        I/U  Forklaring
CD -----------------------------------------------------------------------------
CD wchar_t    *pszFil       i   Fullstendig filnavn
CD wchar_t    *sosi_navn    i   SOSI-navn det skal finnes verdi til
CD short   *sett_nr     i/u  i: "Sett nummer"(linjenummer) for start søking (min 1)
CD                           u: Ved tilslag returneres "Sett nummer" for
CD                              tilslaget.
CD wchar_t    *para_peker   r   Peker til parameter-streng avslutta med '\0'.
CD                           Hvis SOSI-navnet ikke er funnet returneres NULL.
CD
CD Bruk:
CD para_peker = HO_GetVal(fil,sosi_navn,&sett_nr);
	=============================================================================
*/
wchar_t * CFyba::HO_GetVal(const wchar_t *pszFil,wchar_t *sosi_navn,short *sett_nr)
{
   short sStatus;
   FILE * pFil;
   wchar_t *rp = NULL;               /* Retur peker */


   /* Åpner filen */
   pFil = UT_OpenFile(pszFil,L"",UT_READ,UT_OLD,&sStatus);

   /* Åpnet OK ? */
   if (sStatus == UT_OK) {
      /* Hent verdier */
      rp = ho_GetVal(pFil,sosi_navn,sett_nr);
      fclose (pFil);

	/* Åpningsfeil */
   } else {
      wchar_t szError[256];
      UT_strerror(szError,256,sStatus);
      UT_SNPRINTF(err.tx,LC_ERR_LEN,L" %s - %s",pszFil,szError);
      LC_Error(101,L"(HO_GetVal)",err.tx);
      sStatus = UT_FALSE;
   }

   return rp;
}


/*
GL:1988-04-27
AR:1989-08-23
CH ho_GetVal                                      Finn verdien til et SOSI-navn
CD =============================================================================
CD Formål:
CD Henter parametrene til et SOSI-navn.
CD Strengen ligger i et felles "returbuffer" for alle get-rutiner i fyba.
CD Dette blir ødelagt ved neste kall til en "get-rutine". For å ta vare på
CD strengen må den kopieres over til egen streng. (Bruk strcpy).
CD
CD Parametre:
CD Type     Navn        I/U   Forklaring
CD -----------------------------------------------------------------------------
CD FILE    *pFil         i   Filpeker til sosi-fil.
CD wchar_t *sosi_navn    i   SOSI-navn det skal finnes verdi til
CD short   *sett_nr     i/u  i: "Sett nummer"(linjenummer) for start søking (min 1)
CD                           u: Ved tilslag returneres "Sett nummer" for
CD                              tilslaget.
CD wchar_t    *para_peker   r   Peker til parameter-streng avslutta med '\0'.
CD                           Hvis SOSI-navnet ikke er funnet returneres NULL.
CD
CD Bruk:
CD para_peker = ho_GetVal(pFil,sosi_navn,&sett_nr);
	=============================================================================
*/
wchar_t * CFyba::ho_GetVal(FILE *pFil,wchar_t *sosi_navn,short *sett_nr)
{
   UT_INT64 startpos;
   short ant_par,navn_nr,type;
   short funnet = 0;
   short ferdig = 0;
   short sett = 0;
   LB_LESEBUFFER * pLb = &(Sys.BufAdm);
   wchar_t *rp = NULL;               /* Retur peker */


   // Søk fram til .HODE
   if (ho_FinnHode(pFil, &startpos) == UT_TRUE) {
      // Finn riktig info
      // SOSI-navnet
      LN_PakkNavn(&(Sys.SosiNavn),sosi_navn,&navn_nr,&ant_par);

      /* Sikrer at ny lesing startes */
      pLb->sStatus = LESEBUFFER_TOM;
      UT_SetPos_i64(pFil,startpos);

      do {
         sett++;
         /* Hent "sett" */
         type = LB_GetSet(pFil,pLb,&(Sys.SosiNavn));
         if (type >= 0  ||  type == LEST_GINFO) {
            if (sett > 1  &&  pLb->cur_niv == 1) {
               ferdig = 1;

            } else {
               if (sett >= *sett_nr) {
                  if (pLb->cur_navn[pLb->cur_niv-1] == navn_nr) {
                     funnet = 1;
                     rp = pLb->pp;
                     *sett_nr = sett;
                  }
               }
            }
         }
         pLb->set_brukt = SET_BRUKT;
      } while ( ! ferdig  &&  ! funnet);       /* Søk etter navnet */
   }

   pLb->sStatus = LESEBUFFER_TOM;
   return rp;
}


/*
GL-880303
AR-891124
CH  HO_New                                                   Lager nytt hode
CD  =========================================================================
CD Formål:
CD Genererer et nytt SOSI-filhode.
CD Hvis område ikke har noen utstrekning justeres
CD dette med 1 meter i hver retning. 
CD
CD Parametre:
CD Type     Navn         I/U    Forklaring
CD --------------------------------------------------------------------------
CD wchar_t    *pszFil        i    Fullstendig filnavn
CD short    koosys        i    Koordinatsystem
CD double   origo_a       i    Origo øst
CD double   origo_n       i    Origo nord
CD double   enhet         i    Enhet
CD double   enhet_h       i    Enhet-H
CD double   enhet_d       i    Enhet-D
CD double   nv_a          i    Område:  Nedre venstre hjørne
CD double   nv_n          i
CD double   oh_a          i             Øvre høyre hjørne
CD double   oh_n          i
CD short    sStatus       r    Status: UT_TRUE  = Funnet
CD                                     UT_FALSE = Ikke funnet
CD
CD Bruk:
CD sStatus = HO_New(fil,koosys,origo_a,origo_n,enhet,enhet_h-enhet_d,
CD                  nv_a,nv_n,oh_a,oh_n);
CD =============================================================================
*/
short CFyba::HO_New(const wchar_t *pszFil,short koosys,double origo_a,double origo_n,
            double enhet,double enhet_h,double enhet_d,
            double nv_a,double nv_n,double oh_a,double oh_n)
{

   short sStatus = UT_TRUE;
   FILE * pFil;


   /* Åpner filen */
   pFil = UT_OpenFile(pszFil,L"",UT_UPDATE,UT_UNKNOWN,&sStatus);

   /* Åpnet OK ? */
   if (sStatus == UT_OK) {
      /* Skriv nytt hode */
      ho_New(pFil, koosys, origo_a, origo_n, enhet, enhet_h, enhet_d,
             nv_a, nv_n, oh_a, oh_n);

      fclose (pFil);
      sStatus = UT_TRUE;

	/* Åpningsfeil */
   } else {
      wchar_t szError[256];
      UT_strerror(szError,256,sStatus);
      UT_SNPRINTF(err.tx,LC_ERR_LEN,L" %s - %s",pszFil,szError);
      LC_Error(101,L"(HO_New)",err.tx);
      sStatus = UT_FALSE;
   }

   return sStatus;
}


/*
GL-880303
AR-891124
CH  ho_New                                                   Lager nytt hode
CD  =========================================================================
CD Formål:
CD Genererer et nytt SOSI-filhode.
CD Hvis område ikke har noen utstrekning justeres
CD dette med 1 meter i hver retning. 
CD
CD Parametre:
CD Type     Navn         I/U    Forklaring
CD --------------------------------------------------------------------------
CD FILE    *fil           i   Filpeker til sosi-fil.
CD short    koosys        i    Koordinatsystem
CD double   origo_a       i    Origo øst
CD double   origo_n       i    Origo nord
CD double   enhet         i    Enhet
CD double   enhet_h       i    Enhet-H
CD double   enhet_d       i    Enhet-D
CD double   nv_a          i    Område:  Nedre venstre hjørne
CD double   nv_n          i
CD double   oh_a          i             Øvre høyre hjørne
CD double   oh_n          i
CD
CD Bruk:
CD     ho_New(fil,koosys,origo_a,origo_n,enhet,enhet_h-enhet_d,
CD            nv_a,nv_n,oh_a,oh_n);
CD =============================================================================
*/
void CFyba::ho_New(FILE *fil,short koosys,double origo_a,double origo_n,
            double enhet,double enhet_h,double enhet_d,
            double nv_a,double nv_n,double oh_a,double oh_n)
{
   wchar_t tx[LC_MAX_SOSI_LINJE_LEN];


   UT_SetPos_i64(fil,0);

   LB_WriteLine(fil,LC_INTERNT_TEGNSETT,L".HODE\r\n");
   LB_WriteLine(fil,LC_INTERNT_TEGNSETT,L"..TEGNSETT ISO8859-10\r\n");
   LB_WriteLine(fil,LC_INTERNT_TEGNSETT,L"..TRANSPAR\r\n");
   UT_SNPRINTF(tx,LC_MAX_SOSI_LINJE_LEN,L"...KOORDSYS  %d\r\n",koosys);
   LB_WriteLine(fil,LC_INTERNT_TEGNSETT,tx);
   UT_SNPRINTF(tx,LC_MAX_SOSI_LINJE_LEN,L"...ORIGO-NØ  %.0f  %.0f\r\n",origo_n,origo_a);
   LB_WriteLine(fil,LC_INTERNT_TEGNSETT,tx);
   
   LB_FormaterEnhet(tx,LC_MAX_SOSI_LINJE_LEN,L"...ENHET",enhet);
   UT_StrCat(tx,L"\r\n",LC_MAX_SOSI_LINJE_LEN);
   LB_WriteLine(fil,LC_INTERNT_TEGNSETT,tx);
   
   if (fabs(enhet-enhet_h)>0.000001) {
      LB_FormaterEnhet(tx,LC_MAX_SOSI_LINJE_LEN,L"...ENHET-H",enhet_h);
      UT_StrCat(tx,L"\r\n",LC_MAX_SOSI_LINJE_LEN);
      LB_WriteLine(fil,LC_INTERNT_TEGNSETT,tx);
   }
   
   if (fabs(enhet-enhet_d)>0.000001) {
      LB_FormaterEnhet(tx,LC_MAX_SOSI_LINJE_LEN,L"...ENHET-D",enhet_d);
      UT_StrCat(tx,L"\r\n",LC_MAX_SOSI_LINJE_LEN);
      LB_WriteLine(fil,LC_INTERNT_TEGNSETT,tx);
   }
   LB_WriteLine(fil,LC_INTERNT_TEGNSETT,L"..OMRÅDE\r\n");
   
   // Hvis nødvendig justeres område
   if (fabs(oh_n-nv_n) < 0.000001) {
      nv_n -= 1.0;
      oh_n += 1.0;
   }
   if (fabs(oh_a-nv_a) < 0.000001) {
      nv_a -= 1.0;
      oh_a += 1.0;
   }

   UT_SNPRINTF(tx,LC_MAX_SOSI_LINJE_LEN,L"...MIN-NØ   %.0f     %.0f\r\n",nv_n,nv_a);
   LB_WriteLine(fil,LC_INTERNT_TEGNSETT,tx);
   UT_SNPRINTF(tx,LC_MAX_SOSI_LINJE_LEN,L"...MAX-NØ   %.0f     %.0f\r\n",oh_n,oh_a);
   LB_WriteLine(fil,LC_INTERNT_TEGNSETT,tx);

   UT_SNPRINTF(tx,LC_MAX_SOSI_LINJE_LEN,L"..SOSI-VERSJON %.2f\r\n",((double)FYBA_SOSI_VERSJON)/100.0);
   LB_WriteLine(fil,LC_INTERNT_TEGNSETT,tx);

   UT_StrCopy(tx,L"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n",LC_MAX_SOSI_LINJE_LEN);
   LB_WriteLine(fil,LC_INTERNT_TEGNSETT,tx);
   LB_WriteLine(fil,LC_INTERNT_TEGNSETT,tx);
   LB_WriteLine(fil,LC_INTERNT_TEGNSETT,tx);
   LB_WriteLine(fil,LC_INTERNT_TEGNSETT,L".SLUTT\r\n");

   /* chsize(fileno(fil),ftell(fil)-1); */    /* Sett filstørrelse */
}


/*
AR:1999-07-14
CH HO_TestSOSI                                              Tester SOSI-filen
CD =============================================================================
CD Formål:
CD Sjekker at filen er en SOSI-fil, og finner posisjonen for .SLUTT.
CD
CD Parametre:
CD Type      Navn       I/U   Forklaring
CD -----------------------------------------------------------------------------
CD wchar_t     *pszFil      i    Fullstendig filnavn
CD UT_INT64 *sluttpos    u    Posisjon for .SLUTT
CD short     ist         r    Status: UT_TRUE  = OK
CD                                  UT_FALSE = feil
CD
CD Bruk:
CD     ist = HO_TestSOSI(pszFil,&sluttpos);
CD =============================================================================
*/
short CFyba::HO_TestSOSI(const wchar_t *pszFil,UT_INT64 *sluttpos)
{
   short sStatus;
   FILE * pFil;


   /* Åpner filen */
   pFil = UT_OpenFile(pszFil,L"",UT_READ,UT_OLD,&sStatus);

   /* Åpnet OK ? */
   if (sStatus == UT_OK) {
      /* Sjekk filen */
      sStatus = ho_TestSOSI(pFil,sluttpos);
      fclose (pFil);

	/* Åpningsfeil */
   } else {
      wchar_t szError[256];
      UT_strerror(szError,256,sStatus);
      UT_SNPRINTF(err.tx,LC_ERR_LEN,L" %s - %s",pszFil,szError);
      LC_Error(101,L"(HO_TestSOSI)",err.tx);
      sStatus = UT_FALSE;
   }

   return sStatus;
}


/*
AR-891205
CH ho_TestSOSI                                              Tester SOSI-filen
CD =============================================================================
CD Formål:
CD Sjekker at filen er en SOSI-fil, og finner posisjonen for .SLUTT.
CD
CD Parametre:
CD Type      Navn      I/U   Forklaring
CD -----------------------------------------------------------------------------
CD FILE     *pFil       i    Filpeker til sosi-fil.
CD UT_INT64 *sluttpos   u    Posisjon for .SLUTT
CD short     ist        r    Status: UT_TRUE  = OK
CD                                 UT_FALSE = feil
CD
CD Bruk:
CD     ist = ho_TestSOSI(pFil,&sluttpos);
CD =============================================================================
*/
short CFyba::ho_TestSOSI(FILE *pFil,UT_INT64 *sluttpos)
{
   short ferdig;
   char tx[LC_MAX_SOSI_LINJE_LEN];
   UT_INT64 startpos,filpos;
   double nv_a,nv_n,oh_a,oh_n;
   short ist = UT_FALSE;

   *sluttpos = 0;
   
   // ----- Sjekk at filen starter med .HODE og søk fram til .HODE
   if (ho_FinnHode(pFil, &startpos) == UT_TRUE)
   {
      // Skann siste del av filen for å finne .SLUTT
      ferdig = 0;
      _fseeki64(pFil,-200,SEEK_END);
      UT_GetPos_i64(pFil,&filpos);

      while (!ferdig  &&  UT_ReadLine(pFil,LC_MAX_SOSI_LINJE_LEN,tx) == UT_OK)
      {
         if (*tx == '.' &&  *(tx+1) == 'S')
         {
            if (strncmp(tx,".SLUTT",6) == 0)
            {  /* .SLUTT er funnet */
               *sluttpos = filpos;
               ferdig = 1;
               ist = UT_TRUE;
            }
         }
         UT_GetPos_i64(pFil,&filpos);
      }
                 
      if (ist == UT_FALSE)
      {
         // .SLUTT ikke er funnet, skann hele filen fra hodet 
         ferdig = 0;
         filpos = startpos;
         UT_SetPos_i64(pFil,filpos);
  
         while (!ferdig  &&  UT_ReadLine(pFil,LC_MAX_SOSI_LINJE_LEN,tx) == UT_OK)
         {
            if (*tx == '.' &&  *(tx+1) == 'S') 
            {
               if (strncmp(tx,".SLUTT",6) == 0)
               {  /* .SLUTT er funnet */
                  *sluttpos = filpos;
                  ferdig = 1;
                  ist = UT_TRUE;
               }
            }
            UT_GetPos_i64(pFil,&filpos);
         }
      }
   }


   /* Sjekk at hodet har transpar og fornuftig område */   
   if (ist == UT_TRUE) {
      unsigned short usMaske = LC_TR_ALLT;
      LC_TRANSPAR Trans;
      ist = ho_GetTransEx(pFil,&usMaske,&Trans);

      if (ist == UT_TRUE) {
         ist = ho_GetOmr(pFil,&nv_a,&nv_n,&oh_a,&oh_n);
      }
   }

   return ist;
}


/*
AR:2004-05-05
!---------------------------------------------------------------!
! ho_TestFyllKommentar - Tester om en streng er fyll/kommentar. !
!                                                               !
! Retur:  UT_TRUE  = linjen er fyll/kommentar                   !
!         UT_FALSE = linjen inneholder annen informasjon        !
!                                                               !
!---------------------------------------------------------------!
*/
short CFyba::ho_TestFyllKommentar(const char *pszTx)
{
   for (; *pszTx; ++pszTx) {
      if (!UT_IsSpace(*pszTx)  &&  *pszTx != '!')  return (UT_FALSE);
      if (*pszTx == '!')  return (UT_TRUE);
   }

   return (UT_TRUE);
}


/*
AR:2004-05-05
!---------------------------------------------------------------------!
! ho_FinnHode - Finner filposisjonen til .HODE                        !
!                                                                     !
! Retur:  UT_TRUE  = Lovlig hode er funnet                            !
!         UT_FALSE = .HODE er ikke funnet,                            !
!                    eller .HODE er ikke første logiske info i filen. !
!                                                                     !
!---------------------------------------------------------------------!
*/
short CFyba::ho_FinnHode(FILE *pFil, UT_INT64 *n64Hodepos)
{
   char tx[LC_MAX_SOSI_LINJE_LEN], *cp;
   short ierr;

   *n64Hodepos = 0L;
   
   // ----- Sjekk at filen starter med .HODE
   UT_SetPos_i64(pFil,0);

   do
   {
      // Husk filposisjonen
      UT_GetPos_i64(pFil,n64Hodepos);

      // Les
      if ((ierr = UT_ReadLine(pFil,LC_MAX_SOSI_LINJE_LEN,tx)) != UT_OK)
      {
         return UT_FALSE;     // ===>  Retur pga. lesefeil
      }
   } while (ho_TestFyllKommentar(tx) == UT_TRUE);


   // ----- Har nå funnet en linje som inneholder logisk informasjon

   // Hopp over blanke på starten av linjen
   cp = &tx[0];
   while (UT_IsSpace(*cp))
   {
      ++cp;
      ++(*n64Hodepos);
   }

   // Starten av filen, hopp over eventuelt byte order mark (BOM)
   if (*n64Hodepos == 0)
   {
      if (strlen(cp) > 2  &&
         (unsigned char)(*cp) == 0xEF  &&
         (unsigned char)(*(cp+1)) == 0xBB  &&
         (unsigned char)(*(cp+2)) == 0xBF )
      {
         cp += 3;
         (*n64Hodepos) += 3;
      }
   }

   if (strncmp(cp,".HODE",5) == 0) 
   {
         return UT_TRUE;      // ===>  Retur, .HODE er funnet
   }
         
   return UT_FALSE;   // ===>  Retur, .HODE er ikke funnet
}
