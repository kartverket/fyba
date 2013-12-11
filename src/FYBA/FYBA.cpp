#include "StdAfx.h"
#include "Fyba.h"




//////////////////////////////////////////////////////////////////////////
/*
CD short sBaseType     Basetype. Konstanter definert:
CD                             LC_BASE  = (default)  Vanlig base. 
CD                             LC_KLADD = Kladdebase. Brukes bare i spesielle tilfeller
CD                                        hvis ingen fil åpnes som LC_BASE_xx.
*/
//////////////////////////////////////////////////////////////////////////
CFyba::CFyba(LcBasetype basetype)
{
   fytab_open = 0;

   memset(&Sys, 0, sizeof(LC_SYSTEMADM));

   /* Husker aktuelle versjonsnummer */
   UT_StrCopy(Sys.szBaseVer,FYBA_IDENT,LC_BASEVER_LEN);
   UT_StrCopy(Sys.szIdxVer,FYBA_INDEKS_VERSJON,5);

   /* Ingen aktuell gruppe */
   Sys.GrId.lNr = INGEN_GRUPPE;
   Sys.sGrEndra = END_UENDRA;

   Sys.sResPlass = 0;
   Sys.lMaxSkriv = 0L;
   Sys.lAntSkriv = 0L;
   Sys.sNGISmodus = NGIS_NORMAL;
   Sys.sUtvidModus = LC_UTVID_SIKKER;

   /* Allokerer buffer */
   Sys.Hode.pszTx   = (wchar_t*)malloc(LC_MAX_GINFO_BUFFER * sizeof(wchar_t));
   Sys.Ginfo.pszTx  = (wchar_t*)malloc(LC_MAX_GINFO_BUFFER * sizeof(wchar_t));
   Sys.pszPinfo     = (wchar_t*)malloc(LC_MAX_PINFO_BUFFER * sizeof(wchar_t));
   Sys.pdAust       = (double *)malloc(LC_MAX_KOORD * sizeof(double));
   Sys.pdNord       = (double *)malloc(LC_MAX_KOORD * sizeof(double));
   Sys.pInfo        = (LB_INFO *)malloc(LC_MAX_KOORD * sizeof(LB_INFO));
   Sys.pszTx8Buffer = (char *) malloc(LC_TX8_BUFFER_LEN * sizeof(char));

   /* Initierer lesebuffer for HO-rutinene */
   Sys.BufAdm.sStatus = LESEBUFFER_TOM;

   /* Initierer navnetabell for HO-rutinene */
   LN_InitTab(&(Sys.SosiNavn));

   Sys.usMerkRefGr = 0;

   // Ingen base er åpnet
   Sys.pForsteBase = NULL;
   Sys.pAktBase = NULL;

   // Åpner base
   LC_OpenBase(basetype);
}


//////////////////////////////////////////////////////////////////////////
CFyba::~CFyba(void)
{
   LC_BASEADM *pBase, *pNesteBase;
   
   /* Stenger eventuelle åpne baser. */
   for (pBase=Sys.pForsteBase; pBase!=NULL; pBase=pNesteBase) {
      pNesteBase = pBase->pNesteBase;
      LC_CloseBase(pBase,RESET_IDX);
   }

   /* Ingen aktuell gruppe */
   Sys.GrId.lNr = INGEN_GRUPPE;

   /* Frigir buffer */
   free(Sys.Hode.pszTx);
   free(Sys.Ginfo.pszTx);
   free(Sys.pszPinfo);
   free(Sys.pdAust);
   free(Sys.pdNord);
   free(Sys.pInfo);       
   free(Sys.pszTx8Buffer);

   /* Ingen base er åpnet */
   Sys.pForsteBase = NULL;
   Sys.pAktBase = NULL;
}


/*
CH  FYBA                                             FYsak-Basesystem
CD  ==================================================================
CD  Rutiner for å lese, endre og skrive SOSI-filer.
CD  ==================================================================
CD
CH  INSTALLERINGS- OG BRUKS-OPPLYSNINGER:
CD
CD  Bibliotek..: fybale.lib
CD  Kildefiler.: fyba.c, fylo.c, fyln.c, fylr.c, fyls.c, fylx.c, fyli.c
CD               fylh.c, fyho.c, fyle.c, fyba.h, fybax.h
CD  Versjon....: D01
CD  Eier.......: KARTVERKET / FYSAK-prosjektet
CD  Ansvarlig..: Andreas Røstad, Georg Langerak
CD
CD  Kompilator.: Microsoft C versjon 6.0
CD  Optioner...: /c /AL /J /FPi /G2t /W4
CD  Memory-mod.: Large
CD  Floating-p.: Emulation
CD  Processor..: 80286
CD
CD  #include...: fyba.h
CD  Linkes med.: utle.lib    >=  versjon D
CD              +llibce.lib  >=  versjon 6.0
CD
CD  ==================================================================
*/

#include "stdafx.h"


/*
CH LC_InqVer                                                   Identifikasjon
CD =============================================================================
CD Formål:
CD Henter versjons-identifikasjon for dette biblioteket.
   =============================================================================
*/
wchar_t * CFyba::LC_InqVer(void)
{
   return FYBA_IDENT;
}
