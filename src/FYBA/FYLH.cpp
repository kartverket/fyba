/*
CH FYLH        AR-900503                                  BIBLIOTEK
CD =================================================================
CD
CD Eier.......: KARTVERKET / FYSAK-prosjektet
CD Ansvarlig..: Georg Langerak / Andreas Røstad
CD
CD Rutiner for å handtere hodet på SOSI-filer når dette ligger
CD som ginfo i RB.
CD  ==============================================================
*/

#include "stdafx.h"

#include <math.h>
#include <time.h>
#include <ctype.h>
#include <fcntl.h>



/*
AR:2000-10-07
CH LC_PutTransEx                                 Legger inn ..TRANSPAR i hodet
CD ==========================================================================
CD Formål:
CD Legger inn innholdet under ..TRANSPAR i ginfo i aktuell gruppe.
CD OBS! Forutsetter at aktuell gruppe er et SOSI-filhode versjon 3.x.
CD
CD Må velge mellom KOORDSYS, TRANSSYS eller GEOSYS.
CD Kun en av disse kan benyttes i filhodet. 
CD KOORDSYS er den mest vanlige måte å definere referansesystem. 
CD 
CD GEOKOORD skal benyttes for GEOSYS og for TRANSSYS 
CD
CD Må velge mellom VERT-DATUM eller VERT-INT.
CD VERT-DATUM er den mest vanlige beskrivelsesmåten. 
CD
CD Følgende kompaktifisering brukes:
CD     ..TRANSPAR 
CD     ...KOORDSYS <SYSKODE> <DATUM> <PROJEK> 
CD     ...TRANSSYS <TILSYS> <KONSTA1> <KONSTB1> <KONSTA2> <KONSTB2> <KONSTC1> <KONSTC2> 
CD     ...GEOSYS <GEO-DATUM> <GEO-PROJ> <GEO-SONE> 
CD     ...GEOKOORD <GEOKOORD> 
CD     ...ORIGO-NØ <ORIGO-N> <ORIGO-Ø> 
CD     ...ENHET <ENHET> 
CD     ...ENHET-H <ENHET-H> 
CD     ...ENHET-D <ENHET-D> 
CD     ...VERT-DATUM <HØYDE-REF> <DYBDE-REF> <FRISEIL-REF> <HØYDE-TYPE> 
CD     ...VERT-INT <H-REF-INT> <D-REF-INT> <F-REF-INT> 
CD     ...VERT-DELTA <V-DELTA-MIN> <V-DELTA-MAX>
CD
CD
CD Parametre:
CD Type           Navn    I/U  Forklaring
CD --------------------------------------------------------------------------
CD unsigned short usMaske  i   Maske som styrer hvilke deler av TRANSPAR som brukt
CD                             Følgende konstanter er definert:       
CD                              LC_TR_KOORDSYS - Koordsys             
CD                              LC_TR_TRANSSYS - Transsys             
CD                              LC_TR_GEOSYS - Geosys                 
CD                              LC_TR_GEOKOORD - Geokoord             
CD                              LC_TR_ORIGO - Origo-nø                
CD                              LC_TR_ENHET - Enhet                   
CD                              LC_TR_ENHETH - Enhet-h                
CD                              LC_TR_ENHETD - Enhet-d                
CD                              LC_TR_VERTDATUM - Vert-datum          
CD                              LC_TR_VERTINT - Vert-int              
CD                              LC_TR_VERTDELTA - Vert-delta          
CD
CD LC_TRANSPAR *   pTrans   i   Peker til struktur med ..TRANSPAR informasjonen.
CD short          ngi      r   Antall GINFO-linjer etter oppdateringen.
CD
CD Bruk:
CD LC_TRANSPAR Trans;
CD unsigned short usMaske = LC_TR_KOORDSYS | LC_TR_ORIGO | LC_TR_ENHET;
CD Trans.sKoordsys = 32;
CD Trans.dOrigoAust = 0.0;
CD Trans.dOrigoNord = 0.0;
CD Trans.dEnhet = 1.0;
CD ngi = LC_PutTransEx(usMaske,&Trans);
CD ==========================================================================
*/
short CFyba::LC_PutTransEx(unsigned short usMaske, LC_TRANSPAR * pTrans)
{
   short i,lin,ngi;
   long nko;
   unsigned short info;
   wchar_t *cp = NULL;
   wchar_t szGiLin[LC_MAX_SOSI_LINJE_LEN];


   /* Ingen aktuell gruppe */
   if (Sys.GrId.lNr == INGEN_GRUPPE) {
      LC_Error(49,L"(LC_PutTrans)",L"");
      return Sys.pGrInfo->ngi;
   }

   /* Aktuell gruppe er ikke .HODE */
   if (Sys.pGrInfo->gnavn != L_HODE) { 
      LC_Error(94,L"(LC_PutTrans)",L"");
      return Sys.pGrInfo->ngi;
   }


   /* ----- Fjerner gammel transpar ----- */
   LC_GetGrPara(&ngi,&nko,&info);
   lin=2;
   if (LC_GetGP(L"..TRANSPAR",&lin,ngi) != NULL) {      
      i = lin;
      do {
         i++;
         if (i <= ngi)  cp = LC_GetGi(i);
      } while ( i <= ngi  &&  cp[2] == '.' );
      /* i peker nå til første linje etter hele ..TRANSPAR med undernivåer */

      LC_DelGiL(lin,(short)(i-lin));
   }

   /* ----- Legger inn ny transpar ----- */
   LC_PutGi(LC_AppGiL(),L"..TRANSPAR");

   /* Koordsys */
   if ((usMaske & LC_TR_KOORDSYS) != 0) {
      UT_SNPRINTF(szGiLin, LC_MAX_SOSI_LINJE_LEN, L"...KOORDSYS %hd %s %s",pTrans->sKoordsys,pTrans->szKoordsysDatum,pTrans->szKoordsysProjek);
      LC_PutGi(LC_AppGiL(),szGiLin);
   }

   /* Transsys */
   if ((usMaske & LC_TR_TRANSSYS) != 0) {
      UT_SNPRINTF(szGiLin, LC_MAX_SOSI_LINJE_LEN, L"...TRANSSYS %hd %f %f %f %f %f %f", pTrans->sTranssysTilsys,
              pTrans->dTranssysKonstA1, pTrans->dTranssysKonstB1,
              pTrans->dTranssysKonstA2, pTrans->dTranssysKonstB2,
              pTrans->dTranssysKonstC1, pTrans->dTranssysKonstC2);
      LC_PutGi(LC_AppGiL(),szGiLin);
   }

   /* Geosys */
   if ((usMaske & LC_TR_GEOSYS) != 0) {
      if (pTrans->sGeosysProj  != LC_TR_GEOSYS_INGEN_VERDI)
      {
         UT_SNPRINTF(szGiLin, LC_MAX_SOSI_LINJE_LEN, L"...GEOSYS %hd %hd %hd", pTrans->sGeosysDatum,
                 pTrans->sGeosysProj, pTrans->sGeosysSone);
      }
      else
      {
         UT_SNPRINTF(szGiLin, LC_MAX_SOSI_LINJE_LEN, L"...GEOSYS %hd", pTrans->sGeosysDatum);
      }
     
      LC_PutGi(LC_AppGiL(),szGiLin);
   }

   /* Geokoord */ 
   if ((usMaske & LC_TR_GEOKOORD) != 0) {
      UT_SNPRINTF(szGiLin, LC_MAX_SOSI_LINJE_LEN, L"...GEOKOORD %hd", pTrans->sGeoKoord);
      LC_PutGi(LC_AppGiL(),szGiLin);
   }

   /* Origo */
   if ((usMaske & LC_TR_ORIGO) != 0) {
      UT_SNPRINTF(szGiLin, LC_MAX_SOSI_LINJE_LEN, L"...ORIGO-NØ %.0f  %.0f", pTrans->Origo.dNord, pTrans->Origo.dAust);
      LC_PutGi(LC_AppGiL(),szGiLin);
   }

   /* Enhet */
   if ((usMaske & LC_TR_ENHET) != 0) {
      LC_PutGi(LC_AppGiL(),LB_FormaterEnhet(szGiLin,LC_MAX_SOSI_LINJE_LEN, L"...ENHET", pTrans->dEnhet));
   }

   /* Enhet-h */
   if ((usMaske & LC_TR_ENHETH) != 0) {
      /*
      * Hvis enhet og enhet_h er like
      * skal det ikke legges inn ENHET-H
      */
      if (fabs(pTrans->dEnhet-pTrans->dEnhet_h) > 0.0000001) {
         LC_PutGi(LC_AppGiL(),LB_FormaterEnhet(szGiLin,LC_MAX_SOSI_LINJE_LEN,L"...ENHET-H",pTrans->dEnhet_h));
      }
   }

   /* Enhet-d */
   if ((usMaske & LC_TR_ENHETD) != 0) {
      /*
      * Hvis enhet og enhet_d er like
      * skal det ikke legges inn ENHET-D
      */
      if (fabs(pTrans->dEnhet-pTrans->dEnhet_d) > 0.0000001) {
         LC_PutGi(LC_AppGiL(),LB_FormaterEnhet(szGiLin,LC_MAX_SOSI_LINJE_LEN,L"...ENHET-D",pTrans->dEnhet_d));
      }
   }

   /* Vert-datum */
   if ((usMaske & LC_TR_VERTDATUM) != 0) {
      UT_SNPRINTF(szGiLin, LC_MAX_SOSI_LINJE_LEN, L"...VERT-DATUM %s %s %s %s", pTrans->szVertdatHref,
              pTrans->szVertdatDref, pTrans->szVertdatFref, pTrans->szVertdatHtyp);
      LC_PutGi(LC_AppGiL(),szGiLin);
   }
      
   /* Vert-int */
   if ((usMaske & LC_TR_VERTINT) != 0) {
      UT_SNPRINTF(szGiLin, LC_MAX_SOSI_LINJE_LEN, L"...VERT-INT %hd %hd %hd", pTrans->sVertintHref,
              pTrans->sVertintDref, pTrans->sVertintFref);
      LC_PutGi(LC_AppGiL(),szGiLin);
   }

   /* Vert-delta */
   if ((usMaske & LC_TR_VERTDELTA) != 0) {
      UT_SNPRINTF(szGiLin, LC_MAX_SOSI_LINJE_LEN, L"...VERT-DELTA %hd %hd", pTrans->sVdeltaMin, pTrans->sVdeltaMax);
      LC_PutGi(LC_AppGiL(),szGiLin);
   }

   return Sys.pGrInfo->ngi;
}


/*
AR-910920
CH LC_PutTrans                                 Legger inn ..TRANSPAR i hodet
CD ==========================================================================
CD Formål:
CD Legger inn transformasjonsparametrene i ginfo i aktuell gruppe.
CD Forutsetter at aktuell gruppe er et SOSI-filhode versjon 3.x.
CD
CD OBS! Denne rutinen opprettholdes bare for bakoverkompatibilitet.
CD      For nye programmer bør LC_PutTransEx benyttes. LC_PutTransEx er
CD      kompatibel med nye versjoner av SOSI.
CD
CD Parametre:
CD Type     Navn      I/U   Forklaring
CD --------------------------------------------------------------------------
CD short    koosys     i    Koordinatsystem
CD double   origo_a    i    Origo øst
CD double   origo_n    i    Origo nord
CD double   enhet      i    Enhet
CD double   enhet_h    i    Enhet-H
CD double   enhet_d    i    Enhet-D
CD short    ngi        r    Antall GINFO-linjer etter oppdateringen.
CD
CD Bruk:
CD     ngi = LC_PutTrans(koosys,origo_a,origo_n,enhet,enhet_h,enhet_d);
CD ==========================================================================
*/
short CFyba::LC_PutTrans(short koosys,double origo_a,double origo_n,
                  double enhet,double enhet_h,double enhet_d)
{
   short i,ngi,linje_enhet;
   wchar_t c[80];

   if (Sys.GrId.lNr != INGEN_GRUPPE) {           /* Aktuell gruppe OK */
       if (Sys.pGrInfo->gnavn == L_HODE) {        /* Aktuell gruppe .HODE */
           UT_SNPRINTF(c,80,L"%d",koosys);
           ngi = LC_PutGP(L"...KOORDSYS",c,&i);
           UT_SNPRINTF(c,80,L" %.0f  %.0f",origo_n,origo_a);
           ngi = LC_PutGP(L"...ORIGO-NØ",c,&i);
           linje_enhet = 2;              
           LC_GetGP(L"...ENHET",&linje_enhet,ngi);
           LC_PutGi(linje_enhet,LB_FormaterEnhet(c,80,L"...ENHET",enhet));
        
           /*
            * Hvis enhet og enhet_h er like
            * skal det ikke legges inn ENHET-H,
            * eventuell gammel linje fjernes.
            */

           if (fabs(enhet-enhet_h) < 0.0000001) {
              i = 2;
              if (LC_GetGP(L"...ENHET-H",&i,ngi) != NULL) {
                 LC_DelGiL(i,1);
              }

           } else {
            
              i = 2;              
              if (LC_GetGP(L"...ENHET-H",&i,ngi) == NULL) {
                i = linje_enhet + 1;
                ngi = LC_InsGiL(i,1);         /* Ikke funnet, tildel ny linje */
              }
              LC_PutGi(i,LB_FormaterEnhet(c,80,L"...ENHET-H",enhet_h));
           }

           /*
            * Hvis enhet og enhet_d er like
            * skal det ikke legges inn ENHET-D,
            * eventuell gammel linje fjernes.
            */
           if (fabs(enhet-enhet_d) < 0.000001) {
              i = 2;
              if (LC_GetGP(L"...ENHET-D",&i,ngi) != NULL) {
                 LC_DelGiL(i,1);
              }

           } else {
              i = 2;              
              if (LC_GetGP(L"...ENHET-D",&i,ngi) == NULL) {
                i = linje_enhet + 1;
                ngi = LC_InsGiL(i,1);         /* Ikke funnet, tildel ny linje */
             }
             LC_PutGi(i,LB_FormaterEnhet(c,80,L"...ENHET-D",enhet_d));

           }

       } else{                              /* Gruppen er ikke filhode */
           LC_Error(94,L"(LC_PutTrans)",L"");
       }

   } else{                              /* Ingen aktuell gruppe */
       LC_Error(49,L"(LC_PutTrans)",L"");
   }

   return Sys.pGrInfo->ngi;
}


/*
AR:2000-10-07
CH LC_GetTransEx                                 Henter ..TRANSPAR fra hodet
CD ==========================================================================
CD Formål:
CD Henter ut innholdet under ..TRANSPAR fra ginfo i aktuell gruppe.
CD OBS! Forutsetter at aktuell gruppe er et SOSI-filhode.
CD
CD Må velge mellom KOORDSYS, TRANSSYS eller GEOSYS. Kun en av disse kan benyttes i filhodet. 
CD KOORDSYS er den mest vanlige måte å definere referansesystem. 
CD 
CD GEOKOORD skal benyttes for GEOSYS og for TRANSSYS 
CD
CD Må velge mellom VERT-DATUM eller VERT-INT.
CD VERT-DATUM er den mest vanlige beskrivelsesmåten. 
CD

CD Parametre:
CD Type            Navn      I/U   Forklaring
CD --------------------------------------------------------------------------
CD unsigned short *pusMaske  iu   [Inn] Styrer hvilke deler av TRANSPAR som skal hentes
CD                                [Ut]  Viser hvilke deler av TRANSPAR som er funnet/hentet.
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
CD ist = LC_GetTransEx(&usMaske,&Trans);
CD ==========================================================================
*/
short CFyba::LC_GetTransEx(unsigned short *pusMaske, LC_TRANSPAR * pTrans)
{
   short lin,ngi,itxi;
   long nko;
   unsigned short us;
   wchar_t *cp;
   short ist = UT_TRUE;
   unsigned short usMaskeInn = *pusMaske;


   /* Nullstiller pTrans */
   memset(pTrans,0,sizeof(LC_TRANSPAR));

   /* Nullstiller masken */
   *pusMaske = 0;

   LC_GetGrPara(&ngi,&nko,&us);


   /* ----- Div. kontroller ----- */

   /* Ingen aktuell gruppe */
   if (Sys.GrId.lNr == INGEN_GRUPPE) {
      LC_Error(49,L"(LC_GetTransEx)",L"");
      return  UT_FALSE;
   }
   /* Gruppen er ikke filhode */
   if (Sys.pGrInfo->gnavn != L_HODE) {        
      LC_Error(94,L"(LC_GetTransEx)",L"");
      return  UT_FALSE;
   }
   /* Transpar */
   lin=2;
   if (LC_GetGP(L"..TRANSPAR",&lin,ngi) == NULL) {      
      LC_Error(14,L"(LC_GetTransEx)",L"");
      return  UT_FALSE;
   }


   /* ----- Henter verdier ----- */

   /* Koordsys */
   if ((usMaskeInn & LC_TR_KOORDSYS) != 0) {
      lin = 2;
      cp = LC_GetGP(L"...KOORDSYS",&lin,ngi);
      if (cp == NULL) {
          lin=2;
          cp = LC_GetGP(L"..KOORDSYS",&lin,ngi);
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
      if ((cp = LC_GetGP(L"...TRANSSYS",&lin,ngi)) != NULL) {
         *pusMaske |= LC_TR_TRANSSYS;
         UT_StrShort(cp,0,&itxi,&pTrans->sTranssysTilsys);
         UT_StrDbl(cp,itxi,&itxi,'.',&pTrans->dTranssysKonstA1);
         UT_StrDbl(cp,itxi,&itxi,'.',&pTrans->dTranssysKonstB1);
         UT_StrDbl(cp,itxi,&itxi,'.',&pTrans->dTranssysKonstA2);
         UT_StrDbl(cp,itxi,&itxi,'.',&pTrans->dTranssysKonstB2);
         UT_StrDbl(cp,itxi,&itxi,'.',&pTrans->dTranssysKonstC1);
         UT_StrDbl(cp,itxi,&itxi,'.',&pTrans->dTranssysKonstC2);
      }
   }

   /* Geosys */
   if ((usMaskeInn & LC_TR_GEOSYS) != 0) {
      lin = 2;
      if ((cp = LC_GetGP(L"...GEOSYS",&lin,ngi)) != NULL) {
         *pusMaske |= LC_TR_GEOSYS;
         UT_StrShort(cp,0,&itxi,&pTrans->sGeosysDatum);
         UT_StrShort(cp,itxi,&itxi,&pTrans->sGeosysProj);
         UT_StrShort(cp,itxi,&itxi,&pTrans->sGeosysSone);
      }
   }

   /* Geokoord */ 
   if ((usMaskeInn & LC_TR_GEOKOORD) != 0) {
      lin = 2;
      if ((cp = LC_GetGP(L"...GEOKOORD",&lin,ngi)) != NULL) {
         *pusMaske |= LC_TR_GEOKOORD;
         UT_StrShort(cp,0,&itxi,&pTrans->sGeoKoord);
      }
   }

   /* Origo */
   if ((usMaskeInn & LC_TR_ORIGO) != 0) {
      //pTrans->dOrigoAust = 0.0;                           
      //pTrans->dOrigoNord = 0.0;                           
      lin = 2;
      if ((cp = LC_GetGP(L"...ORIGO-NØ",&lin,ngi)) != NULL) {
         *pusMaske |= LC_TR_ORIGO;
         UT_StrDbl(cp,0,&itxi,'.',&pTrans->Origo.dNord);
         UT_StrDbl(cp,itxi,&itxi,'.',&pTrans->Origo.dAust);
      }
   }

   /* Enhet */
   if ((usMaskeInn & LC_TR_ENHET) != 0) {
      lin = 2;
      if ((cp = LC_GetGP(L"...ENHET",&lin,ngi)) != NULL) {
         *pusMaske |= LC_TR_ENHET;
         pTrans->dEnhet = wcstod(cp,&cp);
      }
   }

   /* Enhet-h */
   if ((usMaskeInn & LC_TR_ENHETH) != 0) {
      lin=2;
      if ((cp = LC_GetGP(L"...ENHET-H",&lin,ngi)) == NULL) {
        pTrans->dEnhet_h = pTrans->dEnhet;
      } else {
        *pusMaske |= LC_TR_ENHETH;
        pTrans->dEnhet_h = wcstod(cp,&cp);
      }
   }

   /* Enhet-d */
   if ((usMaskeInn & LC_TR_ENHETD) != 0) {
      lin=2;
      if ((cp = LC_GetGP(L"...ENHET-D",&lin,ngi)) == NULL) {
        pTrans->dEnhet_d = pTrans->dEnhet;
      } else {
        *pusMaske |= LC_TR_ENHETD;
        pTrans->dEnhet_d = wcstod(cp,&cp);
      }
   }

   /* Vert-datum */
   if ((usMaskeInn & LC_TR_VERTDATUM) != 0) {
      lin = 2;
      if ((cp = LC_GetGP(L"...VERT-DATUM",&lin,ngi)) != NULL) {
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
      if ((cp = LC_GetGP(L"...VERT-INT",&lin,ngi)) != NULL) {
         *pusMaske |= LC_TR_VERTINT;
         UT_StrShort(cp,0,&itxi,&pTrans->sVertintHref);
         UT_StrShort(cp,itxi,&itxi,&pTrans->sVertintDref);
         UT_StrShort(cp,itxi,&itxi,&pTrans->sVertintFref);
      }
   }

   /* Vert-delta */
   if ((usMaskeInn & LC_TR_VERTDELTA) != 0) {
      lin = 2;
      if ((cp = LC_GetGP(L"...VERT-DELTA",&lin,ngi)) != NULL) {
         *pusMaske |= LC_TR_VERTDELTA;
         UT_StrShort(cp,0,&itxi,&pTrans->sVdeltaMin);
         UT_StrShort(cp,itxi,&itxi,&pTrans->sVdeltaMax);
      }
   }


   /* ----- Div. sluttkontroll ----- */

   /* Kontroller at det er funnet Koordsys, Transsys eller Geosys */
   if ((usMaskeInn & LC_TR_KOORDSYS) != 0  ||
       (usMaskeInn & LC_TR_TRANSSYS) != 0  ||
       (usMaskeInn & LC_TR_GEOSYS) != 0      ) {
      if ((*pusMaske & LC_TR_KOORDSYS) == 0  &&
          (*pusMaske & LC_TR_TRANSSYS) == 0  &&
          (*pusMaske & LC_TR_GEOSYS) == 0  ) {
         /* Ikke noe koordinatsystem funnet */
         LC_Error(15,L"(LC_GetTransEx)",L"");
         ist = UT_FALSE;
      }
   }

   /* Kontroller at det er funnet Origo */
   if ((usMaskeInn & LC_TR_ORIGO) != 0  &&  
       (*pusMaske & LC_TR_ORIGO) == 0  ) {
      /* Origo mangler */
      LC_Error(16,L"(LC_GetTransEx)",L"");
      ist = UT_FALSE;
   }

   /* Kontroller at det er funnet Enhet */
   if ((usMaskeInn & LC_TR_ENHET) != 0  &&  
       (*pusMaske & LC_TR_ENHET) == 0 ) {
      /* Enhet mangler */
      LC_Error(17,L"(LC_GetTransEx)",L"");
      ist = UT_FALSE;
   }

   return ist;
}


/*
GL-880427
AR-910920
CH LC_GetTrans                                     Finner ..TRANSPAR i hodet
CD ==========================================================================
CD Formål:
CD Henter ut transformasjonsparametrene fra ginfo i aktuell gruppe.
CD Forutsetter at aktuell gruppe er et SOSI-filhode.
CD
CD OBS! Denne rutinen opprettholdes bare for bakoverkompatibilitet.
CD      For nye programmer bør LC_GetTransEx benyttes. LC_GetTransEx er
CD      kompatibel med nye versjoner av SOSI.
CD
CD
CD Parametre:
CD Type     Navn      I/U   Forklaring
CD --------------------------------------------------------------------------
CD short   *koosys     u    Koordinatsystem
CD double  *origo_a    u    Origo øst
CD double  *origo_n    u    Origo nord
CD double  *enhet      u    Enhet
CD double  *enhet_h    u    ...ENHET-H
CD double  *enhet_d    u    ...ENHET-D
CD short    ist        r    status: UT_TRUE=OK, UT_FALSE=feil (navn er ikke funnet)
CD
CD Bruk:
CD     ist = LC_GetTrans(&koosys,&origo_a,&origo_n,&enhet,&enhet_h,&enhet_d);
CD ==========================================================================
*/
short CFyba::LC_GetTrans(short *koosys,double *origo_a,double *origo_n,double *enhet,
					 double *enhet_h,double *enhet_d)
{
   short lin,ngi;
   long nko;
   unsigned short us;
   wchar_t *cp;
   short ist = UT_TRUE;

   if (Sys.GrId.lNr != INGEN_GRUPPE) {           /* Aktuell gruppe OK */
       if (Sys.pGrInfo->gnavn == L_HODE) {        /* Aktuell gruppe .HODE */
           LC_GetGrPara(&ngi,&nko,&us);

           lin=2;
           if (LC_GetGP(L"..TRANSPAR",&lin,ngi) == NULL) {      /* Transpar */
               LC_Error(14,L"(LC_GetTrans)",L"");
               ist = UT_FALSE;
           } else{
               *koosys=0;                                      /* Koordsys */
               lin=2;
               cp = LC_GetGP(L"...KOORDSYS",&lin,ngi);
               if (cp == NULL){
                   lin=2;
                   cp = LC_GetGP(L"..KOORDSYS",&lin,ngi);
               }
               if (cp == NULL){
                   LC_Error(15,L"(LC_GetTrans)",L"");
                   ist = UT_FALSE;
               } else{
                   *koosys = (short)wcstol(cp,&cp,10);
                   *origo_a = 0.0;                            /* Origo */
                   *origo_n = 0.0;
                   lin = 2;
                   cp = LC_GetGP(L"...ORIGO-NØ",&lin,ngi);
                   if (cp == NULL) {
                       LC_Error(16,L"(LC_GetTrans)",L"");
                       ist = UT_FALSE;
                   } else{
                       *origo_n=wcstod(cp,&cp);
                       *origo_a=wcstod(cp,&cp);

                       *enhet  = 1.0;                         /* Enhet */
                       lin = 2;
                       cp = LC_GetGP(L"...ENHET",&lin,ngi);
                       if (cp == NULL){
                           LC_Error(17,L"(LC_GetTrans)",L"");
                           ist = UT_FALSE;
                       } else{
                           *enhet  = wcstod(cp,&cp);
                       }
                       lin=2;
                       cp = LC_GetGP(L"...ENHET-H",&lin,ngi);
                       if (cp == NULL){
                          *enhet_h = *enhet;
                       } else {
                          *enhet_h = wcstod(cp,&cp);
                       }
                       lin=2;
                       cp = LC_GetGP(L"...ENHET-D",&lin,ngi);
                       if (cp == NULL){
                          *enhet_d = *enhet;
                       } else{
                          *enhet_d = wcstod(cp,&cp);
                       }
                   }
               }
           }

       } else{                              /* Gruppen er ikke filhode */
           LC_Error(94,L"(LC_GetTrans)",L"");
           ist = UT_FALSE;
       }

   } else{                              /* Ingen aktuell gruppe */
       LC_Error(49,L"(LC_GetTrans)",L"");
       ist = UT_FALSE;
   }

   return ist;
}


/*
AR:2013-12-03
CH LC_BaEnhet                                                     Hent enhet
CD ==========================================================================
CD Formål:
CD Henter enhet fra første fil i basen.
CD
CD Parametre:
CD Type     Navn      I/U   Forklaring
CD --------------------------------------------------------------------------
CD double  *enhet      u    Enhet grunnriss
CD short    ist        r    status: UT_TRUE=OK, UT_FALSE=feil (navn er ikke funnet)
CD
CD Bruk:
CD     ist = LC_GetBaEnhet(&enhet);
CD ==========================================================================
*/
short CFyba::LC_GetBaEnhet(double *enhet)
{
   if (Sys.pAktBase->pForsteFil != NULL)
   {
      *enhet = Sys.pAktBase->pForsteFil->TransPar.dEnhet;
      return UT_TRUE;
   }

   return UT_FALSE;
}


/*
AR-920401
CH LC_GetTegnsett                                            Finner tegnsett
CD ==========================================================================
CD Formål:
CD Finne tegnsett i ginfo i aktuell gruppe.
CD OBS! Forutsetter at aktuell gruppe er et SOSI-filhode.
CD
CD Parametre:
CD Type    Navn       I/U  Forklaring
CD --------------------------------------------------------------------------
CD short  *psTegnsett  u   Tegnsett, konstanter definert:
CD                            TS_DOSN8   = DOS norsk 8-bits(standardverdi)
CD                            TS_ND7     = Norsk Data 7-bits
CD                            TS_ISO8859 = ISO8859-10 norsk/samisk
CD                            TS_DECM8   = DEC multinasjonal 8-bits
CD                            TS_DECN7   = DEC norsk 7-bits
CD                            TS_UTF8    = UTF-8 tegnsett
CD short   sStatus     r   Status: UT_TRUE  = Funnet
CD                                 UT_FALSE = Ikke funnet
CD
CD Bruk:
CD      sStatus = LC_GetTegnsett(&sTegnsett);
CD ==========================================================================
*/
short CFyba::LC_GetTegnsett(short *psTegnsett)
{
   short lin,ngi;
   long nko;
   unsigned short us;
   wchar_t *cp;
   short ist = UT_FALSE;

   *psTegnsett = TS_DOSN8;

   if (Sys.GrId.lNr != INGEN_GRUPPE) {           /* Aktuell gruppe OK */
      if (Sys.pGrInfo->gnavn == L_HODE) {        /* Aktuell gruppe .HODE */
         LC_GetGrPara(&ngi,&nko,&us);
         lin=2;
         if ((cp = LC_GetGP(L"..TEGNSETT",&lin,ngi)) != NULL) {  /* Tegnsett */
            ist = UT_TRUE;
            UT_StrUpper(cp);
            if (wcscmp(cp,L"ISO8859-10") == 0) {
               *psTegnsett = TS_ISO8859;

            } else if (wcscmp(cp,L"UTF-8") == 0) {
               *psTegnsett = TS_UTF8;

            } else if (wcscmp(cp,L"ISO8859-1") == 0) {
               *psTegnsett = TS_ISO8859;

            } else if (wcscmp(cp,L"ANSI") == 0) {
               *psTegnsett = TS_ISO8859;

            } else if (wcscmp(cp,L"DOSN8") == 0) {
               *psTegnsett = TS_DOSN8;

            } else if (wcscmp(cp,L"ND7") == 0) {
               *psTegnsett = TS_ND7;

            } else if (wcscmp(cp,L"DECN7") == 0) {
               *psTegnsett = TS_DECN7;

            } else if (wcscmp(cp,L"DECM8") == 0) {
               *psTegnsett = TS_DECM8;
            }
            else
            {
               LC_Error(165, L"(LC_GetTegnsett)", cp);
               ist = UT_FALSE;
            }
         }

      } else {                              /* Gruppen er ikke filhode */
         LC_Error(94,L"(LC_GetTegnsett)",L"");
         ist = UT_FALSE;
      }

   } else {                              /* Ingen aktuell gruppe */
       LC_Error(49,L"(LC_GetTegnsett)",L"");
       ist = UT_FALSE;
   }

   return ist;
}


/*
AR-920401
CH LH_GetNgisLag                                           Finner NGIS-LAG
CD ==========================================================================
CD Formål:
CD Finne NGIS-LAG i ginfo i aktuell gruppe.
CD OBS! Forutsetter at aktuell gruppe er et SOSI-filhode.
CD
CD Parametre:
CD Type   Navn        I/U  Forklaring
CD --------------------------------------------------------------------------
CD wchar_t*  pszNgisLag   r   NGIS-lag. 
CD                           Tom streng = ..NGIS-LAG er ikke funnet eller parameter mangler
CD                           ..NGIS-LAG 0 = Bare leseaksess
CD
CD Bruk:
CD      pszNgisLag = LH_GetNgisLag();
CD ==========================================================================
*/
wchar_t * CFyba::LH_GetNgisLag(void)
{
   wchar_t *cp;
   short lin = 2;


   if (Sys.GrId.lNr != INGEN_GRUPPE) {           /* Aktuell gruppe OK */
      if (Sys.pGrInfo->gnavn == L_HODE) {        /* Aktuell gruppe .HODE */
         if ((cp = LC_GetGP(L"..NGIS-LAG",&lin,Sys.pGrInfo->ngi)) != NULL) {
            return cp; 
         } else {
            return  L"";
         }

      } else {                              /* Gruppen er ikke filhode */
         LC_Error(94,L"(LH_GetNgisLag)",L"");
      }

   } else {                              /* Ingen aktuell gruppe */
       LC_Error(49,L"(LH_GetNgisLag)",L"");
   }

   return  L"";
}


/*
AR-910920
CH LC_PutOmr                                     Legger inn ..OMRÅDE i hodet
CD ==========================================================================
CD Formål:
CD Legger inn område i ginfo i aktuell gruppe.
CD Hvis område ikke har noen utstrekning justeres
CD dette med 1 meter i hver retning. 
CD OBS! Forutsetter at aktuell gruppe er et SOSI-filhode av ny type.
CD
CD Parametre:
CD Type     Navn      I/U   Forklaring
CD --------------------------------------------------------------------------
CD double   nv_a       i    Område
CD double   nv_n       i
CD double   oh_a       i
CD double   oh_n       i
CD short    ist        r    status: UT_TRUE=OK, UT_FALSE=feil
CD
CD Bruk:
CD     ist = LC_PutOmr(nv_a,nv_n,oh_a,oh_n);
CD ==========================================================================
*/
short CFyba::LC_PutOmr(double nv_a,double nv_n,double oh_a,double oh_n)
{
   short i;
   wchar_t c[80];
   short ist = UT_FALSE;
   double dNV_N, dNV_A, dOH_A, dOH_N;


   if (Sys.GrId.lNr != INGEN_GRUPPE) {           /* Aktuell gruppe OK */
       if (Sys.pGrInfo->gnavn == L_HODE) {        /* Aktuell gruppe .HODE */

           //UT_SNPRINTF(c,80,L" %ld     %ld",UT_RoundDL(floor(nv_n)),UT_RoundDL(floor(nv_a)));
           dNV_N = UT_RoundDD(floor(nv_n));
           dNV_A = UT_RoundDD(floor(nv_a));
 
           dOH_N = UT_RoundDD(ceil(oh_n));
           dOH_A = UT_RoundDD(ceil(oh_a));

           // Hvis nødvendig justeres område
           if (fabs(dOH_N-dNV_N) < 0.00000001) {
               dNV_N -= 1.0;
               dOH_N += 1.0;
           }
           if (fabs(dOH_A-dOH_A) < 0.00000001) {
               dNV_A -= 1.0;
               dOH_A += 1.0;
           }

           UT_SNPRINTF( c,80, L" %.0f  %.0f", dNV_N, dNV_A );
           if (LC_PutGP(L"...MIN-NØ",c,&i)) {

               UT_SNPRINTF( c, 80, L" %.0f  %.0f", dOH_N, dOH_A );
               if (LC_PutGP(L"...MAX-NØ",c,&i)){
                   ist = UT_TRUE;
               }
           }

       } else{                              /* Gruppen er ikke filhode */
           LC_Error(94,L"(LC_PutOmr)",L"");
       }

   } else{                              /* Ingen aktuell gruppe */
       LC_Error(49,L"(LC_PutOmr)",L"");
   }

   return ist;
}


/*
AR-910920
CH LC_GetOmr                                         Finner ..OMRÅDE i hodet
CD ==========================================================================
CD Formål:
CD Henter ut område fra ginfo i aktuell gruppe.
CD OBS! Forutsetter at aktuell gruppe er et SOSI-filhode.
CD
CD Parametre:
CD Type     Navn      I/U   Forklaring
CD --------------------------------------------------------------------------
CD double  *nv_a       u    Område
CD double  *nv_n       u
CD double  *oh_a       u
CD double  *oh_n       u
CD short    ist        r    status: UT_TRUE=OK, UT_FALSE=feil (navn er ikke funnet)
CD
CD Bruk:
CD     ist = LC_GetOmr(&nv_a,&nv_n,&oh_a,&oh_n);
CD ==========================================================================
*/
short CFyba::LC_GetOmr(double *nv_a,double *nv_n,double *oh_a,double *oh_n)
{
   short lin,i,ngi;
   long nko;
   unsigned short info;
   wchar_t *cp;
   short ist = UT_TRUE;

   if (Sys.GrId.lNr != INGEN_GRUPPE) {           /* Aktuell gruppe OK */
       if (Sys.pGrInfo->gnavn == L_HODE) {        /* Aktuell gruppe .HODE */
           LC_GetGrPara(&ngi,&nko,&info);
           lin=2;
           if (LC_GetGP(L"..OMRÅDE",&lin,ngi) == NULL) { 
               LC_Error(7,L"(LC_GetOmr)",L"");
               *nv_n = -9999999.0;
               *nv_a = -9999999.0;
               *oh_n =  9999999.0;
               *oh_a =  9999999.0;
               ist = UT_FALSE;
           } else {
               /* Min-NØ */
               i = lin;
               cp = LC_GetGP(L"...MIN-NØ",&i,ngi);
               if (cp == NULL){
                  LC_Error(8,L"(LC_GetOmr)",L"");
                  ist = UT_FALSE;
                  *nv_n = -9999999.0;
                  *nv_a = -9999999.0;
               } else{
                   *nv_n = wcstod(cp,&cp);
                   *nv_a = wcstod(cp,&cp);
                   /* Max-NØ */
                   i = lin;
                   cp = LC_GetGP(L"...MAX-NØ",&i,ngi);
                   if (cp == NULL){
                      LC_Error(9,L"(LC_GetOmr)",L"");
                      ist = UT_FALSE;
                      *oh_n = 9999999.0;
                      *oh_a = 9999999.0;

                   } else{
                      *oh_n = wcstod(cp,&cp);
                      *oh_a = wcstod(cp,&cp);
                   }
               }
           }

       } else{                              /* Gruppen er ikke filhode */
           LC_Error(94,L"(LC_GetOmr)",L"");
           ist = UT_FALSE;
       }

   } else{                              /* Ingen aktuell gruppe */
       LC_Error(49,L"(LC_GetOmr)",L"");
       ist = UT_FALSE;
   }

   return ist;
}


/*
AR-910920
CH LC_NyttHode                                               Lager nytt hode
CD ==========================================================================
CD Formål:
CD Legger inn et standard SOSI-filhode i ginfo i aktuell gruppe.
CD
CD Parametre:
CD     ingen
CD
CD Bruk:
CD     LC_NyttHode();
CD ==========================================================================
*/
void CFyba::LC_NyttHode(void)
{
   short ngi;
   long nko;
   unsigned short us;
   wchar_t szTx[100];

   if (Sys.GrId.lNr != INGEN_GRUPPE) {           /* Aktuell gruppe OK */
                                           /* Tildel plass */
       LC_GetGrPara(&ngi,&nko,&us);
       if (ngi < 10){
           LC_InsGiL((short)(ngi+1),(short)(10-ngi));
       } else if (ngi > 10){
           LC_DelGiL(11,(short)(ngi-10));
       }

       Sys.pGrInfo->gnavn = L_HODE;       /* Aktuell gruppe .HODE */

                                           /* Generer nytt hode */
       LC_PutGi(1,L".HODE");
       LC_PutGi(2,L"..TEGNSETT ISO8859-10");
       LC_PutGi(3,L"..TRANSPAR");
       LC_PutGi(4,L"...KOORDSYS 0");
       LC_PutGi(5,L"...ORIGO-NØ  0  0");
       LC_PutGi(6,L"...ENHET 0.01");
       LC_PutGi(7,L"..OMRÅDE");
       LC_PutGi(8,L"...MIN-NØ   -99999   -99999");
       LC_PutGi(9,L"...MAX-NØ  1999999  1999999");

       //LC_PutGi(10,L"..SOSI-VERSJON  3.0");
       UT_SNPRINTF(szTx,100,L"..SOSI-VERSJON %.2f",((double)FYBA_SOSI_VERSJON)/100.0);
       LC_PutGi(10,szTx);

       LC_PutSn(0L);          /* Standard serienummer 0 for hodet*/
   }
}


/*
AR-910920
CH LC_TestHode                                             Tester SOSI-hodet
CD ==========================================================================
CD Formål:
CD Sjekker at ginfo i aktuell gruppe er et lovlig SOSI-filhode.
CD
CD Parametre:
CD  Type    Navn    I/U   Forklaring
CD --------------------------------------------------------------------------
CD  short   ist      r    status: UT_TRUE=OK, UT_FALSE=feil
CD
CD Bruk:
CD     ist = LC_TestHode();
CD ==========================================================================
*/
short CFyba::LC_TestHode(void)
{
   double d;
   short ist;
   unsigned short usMaske = LC_TR_ALLT;
   LC_TRANSPAR Trans;


   ist = LC_GetTransEx(&usMaske,&Trans);

   if (ist == UT_TRUE) {
       ist =  LC_GetOmr(&d,&d,&d,&d);
   }

   return ist;
}

