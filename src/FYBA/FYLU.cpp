/* === AR-920613 ========================================================== */
/*  KARTVERKET  -  FYSAK-PC                                           */
/*  Fil: fylu.c                                                             */
/*  Innhold: Rutiner for utvalg                                             */
/* ======================================================================== */

#include "stdafx.h"

#include <ctype.h>
#include <math.h>

using namespace std;

//#define U_PARA_LEN    1024     /* Max lengde av parameterstreng */

/*  Funksjonsdefinisjoner for interne funksjoner */
int LU_compare (const void *arg1, const void *arg2);


/*
AR-911003
CH LC_OpenQuery                                                Initier query
CD ==========================================================================
CD Formål:
CD Initierer query mot GINFO/PINFO.
CD Tildeler administrasjonsblokk for utvalg.
CD
CD Parametre:
CD Type         Navn   I/U  Forklaring
CD -----------------------------------------------------------------------
CD LC_UT_ADM   *UtAdm   r   Peker til administrasjonsblokk for utvalg.
CD
CD Bruk:
CD pUtAdm = LC_OpenQuery();
   ==========================================================================
*/
LC_UT_ADM * CFyba::LC_OpenQuery(void)
{
   LC_UT_ADM *pUtAdm;

	/* Tildeler administrasjonsblokk */
	pUtAdm = (LC_UT_ADM *) malloc(sizeof(LC_UT_ADM));
   memset(pUtAdm, 0, sizeof(LC_UT_ADM));

   /* Nullstiller */
   memset(&(pUtAdm->Gruppe), 0, sizeof(LC_UTVALG_BLOKK));
   memset(&(pUtAdm->Punkt),  0, sizeof(LC_UTVALG_BLOKK));
   memset(&(pUtAdm->Pinfo),  0, sizeof(LC_UTVALG_BLOKK));

   return pUtAdm;
}


/*
AR-910718
CH LC_CloseQuery                                             Avslutter query
CD ==========================================================================
CD Formål:
CD Avslutter query mot GINFO/PINFO.
CD Frigir minne brukt til administrasjon og utvalgstabeller.
CD
CD Parametre:
CD Type       Navn    I/U   Forklaring
CD --------------------------------------------------------------------------
CD LC_UT_ADM *UtAdm    i    Peker til administrasjonsblokk for utvalg.
CD 
CD
CD Bruk:
CD LC_CloseQuery(pUtAdm);
   ==========================================================================
*/
void CFyba::LC_CloseQuery(LC_UT_ADM * pUtAdm)
{
   LC_UTVALG *pU,*pNesteU;
   LC_LAG *pLag,*pNesteLag; //JAØ-19980922


   if (pUtAdm != NULL)
   {
      /*
       * Frigir GRUPPE-UTVALG.
       */
      pU = pUtAdm->Gruppe.pForsteU;
      while (pU != NULL) {
         pNesteU = pU->pNesteU;
         LU_FrigiUtvalg(pU);

         pU = pNesteU;
      }

      /*
       * Frigir PUNKT-UTVALG.
       */
      pU = pUtAdm->Punkt.pForsteU;
      while (pU != NULL) {
         pNesteU = pU->pNesteU;
         LU_FrigiUtvalg(pU);
         pU = pNesteU;
      }

      /*
       * Frigir PINFO-UTVALG.
       */
      pU = pUtAdm->Pinfo.pForsteU;
      while (pU != NULL) {
         pNesteU = pU->pNesteU;
         LU_FrigiUtvalg(pU);
         pU = pNesteU;
      }

      /*
       * Frigir lag. JAØ-19980922
       */
      pLag = pUtAdm->pForsteLag;
      while (pLag != NULL) {
         pNesteLag = pLag->pNesteLag;
			if (pLag->pszLagNavn != NULL)  free(pLag->pszLagNavn);
			free(pLag);
         pLag = pNesteLag;
      }

      /*
       * Frigir Adm.blokken.
		 */
		free(pUtAdm);
   }
}


/*
AR-920521
CH LU_FrigiUtvalg                                            Frigi et utvalg
CD ==========================================================================
CD Formål:
CD Frigir et utvalg med alle underliggende utvalgselementer.
CD
CD Parameters:
CD Type        Navn  I/O  Forklaring
CD -------------------------------------------------------------------------
CD LC_UTVALG *  pU     i   Peker til utvalget.
CD
CD Bruk:
CD LU_FrigiUE(pU);
CD ==========================================================================
*/
void CFyba::LU_FrigiUtvalg (LC_UTVALG * pU)
{
   /* Frigi utvalgs-elementer */
   if (pU->pForsteUE != NULL)  LU_FrigiUE(pU->pForsteUE);

	/* Frigi navn og regel */
	if (pU->pszNavn != NULL)  free(pU->pszNavn);
	if (pU->pszRegel != NULL)  free(pU->pszRegel);

   /* Frigi toppblokken for utvalget */
	free(pU);
}


/*
AR-920521
CH LU_FrigiUE                                        Frigi utvalgselementer
CD ==========================================================================
CD Formål:
CD Frigir alle elementene (utvalgslinjene) på et nivå i kjeden av
CD utvalgselementer for et utvalg.
CD
CD Parameters:
CD Type                  Navn      I/O  Forklaring
CD -------------------------------------------------------------------------
CD LC_UTVALG_ELEMENT *  pForsteUE  i   Start utvalgs-kjede på dette nivå.
CD
CD Bruk:
CD LU_FrigiUE(pForsteUE);
CD ==========================================================================
*/
void CFyba::LU_FrigiUE (LC_UTVALG_ELEMENT * pUE)
{
   LC_UTVALG_ELEMENT *  pNesteUE;

   /* Går gjennom alle elementene på dette nivå */
   while (pUE != NULL) {
      /* Frigi elementer på underliggende nivå under dette objektet */
      if (pUE->pForsteUE != NULL)  LU_FrigiUE(pUE->pForsteUE);

      /* Husk hva som er neste element på dette nivå */ 
      pNesteUE = pUE->pNesteUE;

		/* Frigi dette elementet */
		if (pUE->min != NULL)  free(pUE->min);
		if (pUE->max != NULL)  free(pUE->max);

		free(pUE);

      pUE = pNesteUE;
   }
}


/*
AR-920527
CH LC_PutQueryLine                                   Legg inn en query-linje
CD ==========================================================================
CD Formål:
CD Legger inn og tolker en linje med query-tekst.
CD
CD Parametre:
CD Type       Navn  I/U   Forklaring
CD ------------------------------------------------------------------------
CD LC_UT_ADM *UtAdm  i    Peker til administrasjonsblokk for utvalg.
CD wchar_t      *qulin  i    Linje med query-tekst. (Uten prikker på første nivå).
CD short     sType   i    Gruppe eller Punkt (U_GRUPPE eller U_PUNKT).
CD short      ist    r    Status (UT_TRUE=OK, UT_FALSE=linjen er ikke OK)
CD
CD Bruk:
CD ist = LC_PutQueryLine(pUtAdm,qulin,sType);
   =============================================================================
*/
short CFyba::LC_PutQueryLine(LC_UT_ADM *pUtAdm,const wchar_t *qulin,short sType)
{
   const wchar_t *cp;
   LC_UTVALG_BLOKK *pUB;
   short sNiv = 0;
   short sStatus = UT_FALSE;

   if (pUtAdm != NULL)
   {
      /* Preparer strengen */
      cp = qulin;
      while (UT_IsSpace(*cp)) {
         cp++;
      }
      /* Tell opp antall prikker */
      while (*cp == '.') {
         sNiv++;
         cp++;
      }

      /* Første utvalg i blokken, alloker topp-blokk */
      if (sType == U_GRUPPE) {
         pUB = &pUtAdm->Gruppe;

      } else { // U_PUNKT
         pUB = &pUtAdm->Punkt;
      }

      if (pUB->pForsteU == NULL) {
         /* Legg til et nytt utvalg */
         LU_AppUtvalg(pUB,L"Query");
      }
      
      /* Alloker minne og tolk linjen */
      sStatus = LU_AppUE(pUB->pSisteU,sNiv,cp);

      // Ta vare på opplysning om HØYDE er brukt
      if (sStatus != UT_FALSE) {
         if (wcscmp(pUB->pSisteU->pSisteUE->sosi,L"HØYDE") == 0) {
            pUB->sHoydeBrukt = UT_TRUE;
         }
      }
   }

   return sStatus;
}


/*
AR-920522
CH LC_PutQueryRegel                                    Legg inn et regelnavn
CD ==========================================================================
CD Formål:
CD Legger inn et regelnavn på siste linje i utvalgstabellen.
CD (Navnet blir intern konvertert til "store" bokstaver.)
CD
CD Parametre:
CD Type         Navn I/U   Forklaring
CD ------------------------------------------------------------------------
CD LC_UTVALG   *pU    i    Peker til utvalg
CD wchar_t        *navn  i    Regelnavn. 
CD
CD Bruk:
CD LC_PutQueryRegel(pU,navn);
   =============================================================================
*/
void CFyba::LC_PutQueryRegel(LC_UTVALG * pU,const wchar_t *navn)
{
	/* Frigi eventuell gammel regel */
	if (pU->pszRegel != NULL)  free(pU->pszRegel);

   /* Legg inn ny regel */
   pU->pszRegel = _wcsdup(navn);  
	//pU->pszRegel = (wchar_t*)malloc((wcslen(navn)+1) * sizeof(wchar_t));
	//UT_StrCopy(pU->pszRegel, navn, wcslen(navn)+1);
	UT_StrUpper(pU->pszRegel);
}

/*
JAØ-19980921
CH LC_PutLag                                          Legg inn peker til lag
CD ==========================================================================
CD Formål:
CD Legger inn peker til det laget utvalget tilhører.
CD
CD Parametre:
CD Type         Navn  I/U   Forklaring
CD ------------------------------------------------------------------------
CD LC_UT_ADM   *UtAdm  i    Peker til administrasjonsblokk for utvalg.
CD LC_UTVALG   *pU     i    Peker til utvalg
CD wchar_t        *navn   i    Lag-navn. 
CD
CD Bruk:
CD LC_PutLag(pUB,pU,navn);
   =============================================================================
*/
void CFyba::LC_PutLag(LC_UT_ADM *pUtAdm,LC_UTVALG *pU,const wchar_t *navn)
{
   LC_LAG * pLag,*pNyttLag;
	short sFunnet = 0;


   if (pUtAdm != NULL)
   {
	   /* Det er allerede lagt inn lag på dette utvalget ? */
      if (pU->pLag != NULL) { 
         /* Annet lag enn forrige gang ? */
         if (wcscmp(navn,pU->pLag->pszLagNavn) != 0) {
            LC_Error(128,L"(LU_HuskPrior)",pU->pszNavn);
         }

      } else {
	      /* Tester om lag-navnet allerede er registrert. */
	      pLag = pUtAdm->pForsteLag;
	      while ((pLag != NULL) && (!sFunnet)) {
		      sFunnet = (wcscmp(navn,pLag->pszLagNavn) == 0);
		      if (!sFunnet) pLag = pLag->pNesteLag;
	      }
   	 
         if (pLag == NULL) { /* Nytt lag, må opprettes og settes inn i kjeden. */
		      pNyttLag = (LC_LAG *)malloc(sizeof(LC_LAG));
		      memset(pNyttLag,0,sizeof(LC_LAG));
		      pNyttLag->sLagAktiv = 1;
            pNyttLag->pszLagNavn = _wcsdup(navn);  
		      //pNyttLag->pszLagNavn = (wchar_t*)malloc((wcslen(navn)+1) * sizeof(wchar_t));
            //UT_StrCopy(pNyttLag->pszLagNavn,navn,wcslen(navn)+1);
		      pNyttLag->pNesteLag = NULL;
		      pU->pLag = pNyttLag;
   		   
		      if (pUtAdm->pForsteLag == NULL) {
			      pUtAdm->pForsteLag = pNyttLag;
		      } else {
			      pUtAdm->pSisteLag->pNesteLag = pNyttLag;
		      }
		      pUtAdm->pSisteLag = pNyttLag;
		      pUtAdm->pSisteLag->pNesteLag = NULL;
	      } else {
		      pU->pLag = pLag;
	      }
      }
   }
} /* END LC_PutLag */


/*
AR-890308
CH LC_LesUtvalg                                        Les utvalg i kom.filen
CD =============================================================================
CD Formål:
CD Leser og tolker gruppe og punktutvalg på kommandofilen og legger i tabell.
CD Forutsetter at filen er åpnet på forhånd.
CD
CD Parametre:
CD Type       Navn    I/U   Forklaring
CD -----------------------------------------------------------------------------
CD LC_UT_ADM *pUtAdm   i    Peker til administrasjonsblokk for utvalg.
CD FILE      *pKomFil  i    Peker til "handle" for åpnet kommandofil.
CD short      sStatus  r    UT_TRUE=OK, UT_FALSE=feil i linjen
CD
CD Bruk:
CD sStatus = LC_LesUtvalg(pUtAdm,pKomFil);
   =============================================================================
*/
short CFyba::LC_LesUtvalg(LC_UT_ADM *pUtAdm,const wchar_t *pszKomFil)
{
   FILE * pKomFil;
   short sFunnet;
   wchar_t szTx[100],ord[60],szNavn[50];
   short itxi,lesefeil,sNiv;
   short sForrigeMaxPrioritet,sPrioritet;
   LC_UTVALG_BLOKK  *pUB=NULL;
   short sStatus;
   

   if (pUtAdm != NULL)
   {
      /* Åpner filen */
      pKomFil = UT_OpenFile(pszKomFil,L"",UT_READ,UT_OLD,&sStatus);

	   /* Åpningsfeil */
      if (sStatus != UT_OK) {
         wchar_t szError[256];
         UT_strerror(szError,256,sStatus);
         UT_SNPRINTF(err.tx,LC_ERR_LEN,L" %s - %s",pszKomFil,szError);
         LC_Error(101,L"(LC_LesUtvalg)",err.tx);
         return UT_FALSE;
      }
       
      /* Starter først på filen og leser  */
      UT_SetPos(pKomFil,0L);

      lesefeil = LU_LesULinje(pKomFil,100,szTx,&sNiv);
      while (! lesefeil) {
         sFunnet = UT_FALSE;
                                          /* Hent kommandonavn */
         UT_StrToken(szTx,0,&itxi,60,ord);
         UT_StrUpper(ord);
         UT_StrToken(szTx,itxi,&itxi,60,szNavn);

         if (sNiv == 1  &&  wcscmp(ord,L"GRUPPE-UTVALG") == 0) {
            pUB = &pUtAdm->Gruppe;
            sFunnet = UT_TRUE;

         } else if(sNiv == 1  &&  wcscmp(ord,L"PUNKT-UTVALG") == 0) {
            pUB = &pUtAdm->Punkt;
            sFunnet = UT_TRUE;

         } else if(sNiv == 1  &&  wcscmp(ord,L"PINFO-UTVALG") == 0) {
            pUB = &pUtAdm->Pinfo;
            sFunnet = UT_TRUE;
         }

         if (sFunnet) {
            sForrigeMaxPrioritet = pUtAdm->sMaxPrior;

            /* Legg til et utvalg */
            LU_AppUtvalg(pUB,szNavn);

            lesefeil = LU_LesULinje(pKomFil,100,szTx,&sNiv);
            while (! lesefeil  && sNiv > 1) {
               UT_StrToken(szTx,0,&itxi,60,ord);
               UT_StrUpper(ord);

               /* PRIORITET */
               if (sNiv == 2  &&  wcscmp(ord,L"PRIORITET") == 0) {
                  UT_StrShort(szTx,itxi,&itxi,&sPrioritet);
                  pUB->pSisteU->sPrioritet = sPrioritet;
                  pUB->pSisteU->sOriginalPrioritet = sPrioritet;
                  if (sPrioritet > pUtAdm->sMaxPrior)  pUtAdm->sMaxPrior = sPrioritet;

               /* BRUK-REGEL */
               } else if (sNiv == 2  &&  wcscmp(ord,L"BRUK-REGEL") == 0) {
                  UT_StrToken(szTx,itxi,&itxi,61,ord);
                  LC_PutQueryRegel(pUB->pSisteU,ord);

               /* GRUPPE */  //JAØ-19980921
               } else if (sNiv == 2  &&  wcscmp(ord,L"LAG") == 0) {
                  UT_StrToken(szTx,itxi,&itxi,61,ord);
                  LC_PutLag(pUtAdm,pUB->pSisteU,ord);

               /* Vanlige utvalgslinjer */
               } else {
                  /* Legg inn linjen */
                  if (LU_AppUE(pUB->pSisteU,(short)(sNiv-2),szTx) == UT_FALSE) {

                     // Logisk feil i linjen.
                     LU_DelLastQuery(pUB);
                     pUtAdm->sMaxPrior = sForrigeMaxPrioritet;
                     fclose (pKomFil);
                     return UT_FALSE;               // Avbryter tolkingen ==>
                  }

                  /* Sjekk om HØYDE er brukt i utvalg */
                  if (wcscmp(pUB->pSisteU->pSisteUE->sosi,L"HØYDE") == 0) {
                     pUB->sHoydeBrukt = UT_TRUE;
                  }

                  /* PUNKT-UTVALG */
                  if (pUB == &pUtAdm->Punkt) {
                     /* "!" er brukt, må sjekke alle punkt */
                     if (pUB->pSisteU->pSisteUE->metode == LC_U_IKKE) {
                        pUB->sTestAllePi = UT_TRUE;
                     }
                  }
               }

               /* Les neste linje */
               lesefeil = LU_LesULinje(pKomFil,100,szTx,&sNiv);
            }

                            /* Fjern utvalg som ikker er avsluttet med regel */
            if (pUB->pSisteU->pszRegel == NULL) {
               LU_DelLastQuery(pUB);
               pUtAdm->sMaxPrior = sForrigeMaxPrioritet;
            }

         } else {                                    /* Hopp over ukjent linje */
            lesefeil = LU_LesULinje(pKomFil,100,szTx,&sNiv);
         }
      }

      fclose (pKomFil);

      /* Pakk prioritetene */
      LU_PakkPrioritet(pUtAdm);

      return UT_TRUE;
   }

   return UT_FALSE;
}


/*
AR-920610
CH LU_AppUE                                      Legg til en ny utvalgslinje
CD ==========================================================================
CD Formål:
CD Legg til et nytt element i kjeden utvalgslinjer.
CD
CD Parameters:
CD Type         Navn     I/O  Forklaring
CD -----------------------------------------------------------------------
CD LC_UTVALG   *pU        i   Aktuellt utvalg
CD short        sNiv      i   Nivå (antall prikker forran navnet)
CD wchar_t        *pszTx     i   Lest linje
CD short        sStatus   r   UT_TRUE=OK, UT_FALSE=feil i linjen
CD
CD Bruk:
CD    sStatus = LU_AppUE(pUB->pSisteU,sNiv,szTx);
CD =============================================================================
*/
short CFyba::LU_AppUE (LC_UTVALG *pU,short sNiv,const wchar_t *pszTx)
{
   LC_UTVALG_ELEMENT *pUE,*pNyUE,*pForrigeUE=NULL;
   short sAktNiv = 0;

   /* Finn rett posisjon i kjedene */
   pUE=pU->pSisteUE;
   while (sAktNiv < sNiv  &&  pUE != NULL) {
      pForrigeUE = pUE;
      pUE = pUE->pSisteUE;
      sAktNiv++;
   }

   //if (sAktNiv < sNiv-1) {
   if (sAktNiv < sNiv  ||  sNiv > sAktNiv) {
      LC_Error(126,L"(LU_AppUE)",pszTx);
      return  UT_FALSE;       /* ==> Retur når ulovlig sprang i nivå */
   }

	/* Alloker minne og initier */
	pNyUE = (LC_UTVALG_ELEMENT *)malloc(sizeof(LC_UTVALG_ELEMENT));
	memset(pNyUE,0,sizeof(LC_UTVALG_ELEMENT));
	pNyUE->min = NULL;
	pNyUE->max = NULL;

   pNyUE->pNesteUE = NULL;
   pNyUE->pForsteUE = NULL;
   pNyUE->pSisteUE = NULL;


   /* Heng inn i kjedene */
   if (sNiv == 0) {
      if (pU->pSisteUE == NULL) {
         pU->pForsteUE = pNyUE;
         pU->pSisteUE = pNyUE;

      } else {
         pU->pSisteUE->pNesteUE = pNyUE;
         pU->pSisteUE = pNyUE;
      }

   } else {
      if (pForrigeUE->pSisteUE == NULL) {
         pForrigeUE->pForsteUE = pNyUE;
         pForrigeUE->pSisteUE = pNyUE;

      } else {
         pForrigeUE->pSisteUE->pNesteUE = pNyUE;
         pForrigeUE->pSisteUE = pNyUE;
      }
   }

   /* Tolk linjen */
   if ( ! LU_TolkUtvalgslinje(pNyUE,pszTx)) {
      LC_Error(124,L"(LU_AppUE)",pszTx);
      return  UT_FALSE;       /* ==> Retur når ulovlig utvalgslinje */
   }
         
   return  UT_TRUE;
}


/*
AR-910718
CH LU_DelLastQery                           Fjern siste utvalg fra tabellen
CD ==========================================================================
CD Formål:
CD Fjerner siste utvalg fra tabellen. (Fram til forrige regelnavn.)
CD
CD Parametre:
CD Type              Navn  I/U   Forklaring
CD ------------------------------------------------------------------------
CD LC_UTVALG_BLOKK  *pUB    i    Peker til administrasjonsblokk for utvalg.
CD
CD Bruk:
CD LU_DelLastQuery(pUB);
   =============================================================================
*/
void CFyba::LU_DelLastQuery(LC_UTVALG_BLOKK *pUB)
{
   /* Husk aktuelt utvalg */
   LC_UTVALG * pU = pUB->pSisteU;
   
   /*
    * Ordne pekere
    */
   
   /* I toppblokken */
   pUB->pSisteU = pU->pForrigeU;
   if (pUB->pSisteU == NULL)  pUB->pForsteU = NULL;

   /* I kjeden av utvalg */
   if (pU->pForrigeU != NULL) {
      pU->pForrigeU->pNesteU = NULL;
   }

   /*
    * Frigi minne
    */
   LU_FrigiUtvalg(pU);
}


/*
ÅE-20040709 Forandret litt for at type skulle ta negative tall.
AR-920526
CH LU_TolkUtvalgslinje                         Tolk utvalgslinje i kom.filen
CD ==========================================================================
CD Formål:
CD Tolker en utvalgslinje fra kommandofilen og legger i tabell.
CD Forutsetter at linjen er lest.
CD
CD Parametre:
CD Type                Navn  I/U  Forklaring
CD -----------------------------------------------------------------------
CD LC_UTVALG_ELEMENT * pUE    i   Peker til utvalgselement
CD wchar_t               *pszTx  i   Linje med query-tekst. (Uten prikker i starten)
CD short               ist    r   Status (UT_TRUE=OK, UT_FALSE=linjen er ikke OK)
CD
CD Bruk:
CD ist = LU_TolkUtvalgslinje(pUE,pszTx);
   =============================================================================
*/
short CFyba::LU_TolkUtvalgslinje(LC_UTVALG_ELEMENT * pUE,const wchar_t *pszTx)
{
   wchar_t tx[100],ord[60];
   const wchar_t *cp;
   short itxi;
   short i;

   // ----- Preparer strengen
   cp = pszTx;
   while (UT_IsSpace(*cp)){
      ++cp;
   }
   UT_StrCopy(tx,cp,100);
   UT_StrUpper(tx);

                                       
   // ----- Hent kommandonavn
   UT_StrToken(tx,0,&itxi,60,ord);

   if (wcscmp(ord,L"VELG") == 0) {
      pUE->kommando = LC_U_ELLER;

   } else if (wcscmp(ord,L"ELLER") == 0) {
      pUE->kommando = LC_U_ELLER;

   } else if (wcscmp(ord,L"OG") == 0) {
      pUE->kommando = LC_U_OG;

   } else {
      return  UT_FALSE;                /* ===> Feil i utvalgslinjen */
   }

   // ----- Tolk resten av strengen
   pUE->metode = LC_U_ALLE;
   pUE->type = LC_U_TALL;
   pUE->ledd = 0;
   pUE->start = 0;
   pUE->slutt = 0;
   
   // ----- SOSI-navn
   UT_StrToken(tx,itxi,&itxi,60,ord);
   if (*ord) {
       UT_StrCopy(pUE->sosi,ord,LC_MAX_SOSINAVN_LEN);
   } else {
      return  UT_FALSE;                /* ===> Feil i utvalgslinjen */
   }

                       /* Metode */
   UT_StrToken(tx,itxi,&itxi,60,ord);
   UT_StrUpper(ord);

   if (*ord) {
      if (!wcscmp(ord,L"="))
         pUE->metode = LC_U_LIK;
      else if (!wcscmp(ord,L"<>"))
         pUE->metode = LC_U_FRATIL;
      else if (!wcscmp(ord,L"/"))
         pUE->metode = LC_U_DELELIG;
      else if (!wcscmp(ord,L"!/"))
         pUE->metode = LC_U_UDELELIG;
      else if (!wcscmp(ord,L"()"))
         pUE->metode = LC_U_CONTEIN;
      else if (!wcscmp(ord,L"!()"))
         pUE->metode = LC_U_IKKECONTEIN;
      else if (!wcscmp(ord,L"AL"))
         pUE->metode = LC_U_ALLE;
      else if (!wcscmp(ord,L"!="))
         pUE->metode = LC_U_IKKELIK;
      else if (!wcscmp(ord,L"!"))
         pUE->metode = LC_U_IKKE;
      else if (!wcscmp(ord,L"><"))
         pUE->metode = LC_U_UTENFOR;
      else if (!wcscmp(ord,L"<"))
         pUE->metode = LC_U_MINDRE;
      else if (!wcscmp(ord,L">"))
         pUE->metode = LC_U_STORRE;
      else if (! wcscmp(ord,L"IV"))
         pUE->metode = LC_U_IKKEVALGT;
      else if (!wcscmp(ord,L"FL"))
         pUE->metode = LC_U_FLERE;
      else if (!wcscmp(ord,L"!FL"))
         pUE->metode = LC_U_IKKEFLERE;
      else {
         return  UT_FALSE;                /* ===> Feil i utvalgslinjen */
      }
   }

   // ----- Min
   UT_StrToken(tx,itxi,&itxi,60,ord);
	if (*ord) {
      pUE->min = _wcsdup(ord);  
		//pUE->min = (wchar_t*)malloc((wcslen(ord)+1) * sizeof(wchar_t));
      //UT_StrCopy(pUE->min,ord,wcslen(ord)+1);

		// Sjekk type
      LU_SjekkDatatype(pUE->min,0,&pUE->type);

  	} else {
      pUE->min = _wcsdup(L"");  
		//pUE->min = (wchar_t*)malloc(sizeof(wchar_t));
		//UT_StrCopy(pUE->min,L"",1);
      pUE->type = LC_U_ALFA;
	}

                       
   // ----- Max
   UT_StrToken(tx,itxi,&itxi,60,ord);
	if (*ord) {
      pUE->max = _wcsdup(ord);  
		//pUE->max = (wchar_t*)malloc((wcslen(ord)+1) * sizeof(wchar_t));
      //UT_StrCopy(pUE->max, ord, wcslen(ord)+1);

      // Sjekk type
      if (pUE->type != LC_U_ALFA) {
         LU_SjekkDatatype(pUE->max,pUE->metode,&pUE->type);
      }

	} else {
      pUE->max = _wcsdup(ord);  
      //pUE->max = (wchar_t*)malloc(sizeof(wchar_t));
		//UT_StrCopy(pUE->max,L"",1);
   }
							  
   // ----- Type
	UT_StrToken(tx,itxi,&itxi,60,ord);
	if(*ord){
      if(*ord == 'T'){
          pUE->type = LC_U_TALL | LC_U_DEFINERT;
      } else if(*ord == 'A'){
          pUE->type = LC_U_ALFA | LC_U_DEFINERT;
      } else if(*ord == 'F'){
          pUE->type = LC_U_FLYT | LC_U_DEFINERT;
      } else{
         return  UT_FALSE;                /* ===> Feil i utvalgslinjen */
      }

      wchar_t *cp2 = ord + 1;
      for (i=0; i<2; ++i) {
         if (*cp2 == '#') {
            ++cp2;
            pUE->ledd = (char) wcstol(cp2,&cp2,10);          /* Ledd-nummer */
         }else if (*cp2 == '[') {
            ++cp2;
            pUE->start = (char) wcstol(cp2,&cp2,10);         /* Startposisjon */
            ++cp2;
            pUE->slutt = (char) wcstol(cp2,&cp2,10);         /* Sluttposisjon */
         }
      }
   }

	return UT_TRUE;
}


/*
AR-881115
CH LU_SjekkDatatype                                              Sjekk datatype
CD =============================================================================
CD Formål:
CD Finner datatype ut fra utvalgsparameter.
CD
CD Parametre:
CD Type      Navn       I/U   Forklaring
CD -----------------------------------------------------------------------------
CD wchar_t     *pszVerdi    i    Verdi
CD wchar_t     szMetode     i    Utvalgsmetode
CD short    *psType      iu   Type, Inn=foreløpig type, Ut=beregnet type
CD
CD Bruk:
CD LU_SjekkDatatype(pszVerdi,szMetode,sType);
   =============================================================================
*/
void CFyba::LU_SjekkDatatype(wchar_t *pszVerdi,wchar_t szMetode,short *psType)
{
   wchar_t *cp;
   short i=0;


	/* Sjekk typen */
	for (cp=pszVerdi; *cp!='\0'; ++cp)
   {
      // Første tegn:
      if (i==0)
      {
         if (*cp=='.')
         {
            *psType = LC_U_FLYT;
         } 
         else if (*cp!='+' && *cp!='-' && !iswdigit( *cp ))
         {
            *psType = LC_U_ALFA;
            break;
         }
      } 
      else if ( ! iswdigit( *cp ) ) 
      {
         // Resten av tegnene:
         if (*cp == '.')  // funnet '.' tidligere
         { 
            if ((*psType == LC_U_FLYT)&&(szMetode != LC_U_FRATIL)&&(szMetode != LC_U_UTENFOR)&&(szMetode != LC_U_DELELIG))
            {
               *psType = LC_U_ALFA;
               break;
            } 
            else 
            {
               *psType = LC_U_FLYT;
            }
         
         }
         else
         {
            *psType = LC_U_ALFA;
            break;
         }
      }
      ++i;
   }

   if (*psType == LC_U_TALL)
   {
      // Hvis tallet har for mange siffer til long må det håndteres som tekst
      long lTall = wcstol(pszVerdi,&pszVerdi,10);
      if (lTall == LONG_MAX  ||  lTall == LONG_MIN)
      {
         *psType = LC_U_ALFA;
      }
   }
}


/*
AR-881115
CH LC_GetUtRegelNavn                                      Henter regelnavn
CD =============================================================================
CD Formål:
CD Henter regelnavn for at programmet utenfor skal kunne sjekke
CD at det er tilgjengelig videre behandling av alle definerte navn.
CD
CD Parametre:
CD Type                         Navn I/U   Forklaring
CD -----------------------------------------------------------------------------
CD LC_UT_ADM *pUtAdm     i     Peker til administrasjonsblokk for utvalg.
CD short   *ist          iu    Status (Inn: 1=start,  0=neste)
CD                                    (Ut:  0=OK,    -1=ferdig);
CD wchar_t    *regelpeker   r     Peker til utvalgsnavn.
CD
CD Bruk:
CD regelpeker = LC_GetUtRegelNavn(pUtAdm,&ist);
   =============================================================================
*/
wchar_t * CFyba::LC_GetUtRegelNavn(LC_UT_ADM *pUtAdm,short *ist)
{
   if (pUtAdm != NULL)
   {
      if (*ist == 1) {
         pUtAdm->pAktUB = &pUtAdm->Gruppe;
         pUtAdm->pAktUB->pAktU = pUtAdm->pAktUB->pForsteU;

      } else {
         if (pUtAdm->pAktUB->pAktU != NULL) {
            pUtAdm->pAktUB->pAktU = pUtAdm->pAktUB->pAktU->pNesteU;
         }
      }

      /* Sjekk om dette er et lovlig utvalg */
      while (pUtAdm->pAktUB->pAktU == NULL) {
         /* Ferdig med GRUPPE-UTVALG ,sjekk PUNKT-UTVALG */
         if (pUtAdm->pAktUB == &(pUtAdm->Gruppe)) {
            pUtAdm->pAktUB = &pUtAdm->Punkt;
            pUtAdm->pAktUB->pAktU = pUtAdm->pAktUB->pForsteU;
         
         /* Ferdig med PUNKT-UTVALG ,sjekk PINFO-UTVALG */
         } else if (pUtAdm->pAktUB == &pUtAdm->Punkt) {
            pUtAdm->pAktUB = &pUtAdm->Pinfo;
            pUtAdm->pAktUB->pAktU = pUtAdm->pAktUB->pForsteU;

         } else {
            /* RETUR når det ikke er tilslag */
            *ist = -1;
            return NULL;
         }
      }

      /* RETUR ved tilslag */
      *ist = 0;
      return  pUtAdm->pAktUB->pAktU->pszRegel;
   }
   
   *ist = -1;
   return NULL;
}


/*
AR-920526
CH LC_FinnPinfoUtvalg
CD ==========================================================================
CD Formål:
CD Finner et PINFO-UTVALG i kjeden av slike utvalg.
CD
CD Parametre:
CD Type          Navn     I/U  Forklaring
CD --------------------------------------------------------------------------
CD LC_UT_ADM *    pUtAdm    i   Peker til administrasjonsblokk for utvalg.
CD wchar_t         *pszNavn   i   Utvalgsnavn
CD LC_UTVALG *    pUtvalg   r   Peker til utvalget. (NULL = ikke funnet)
CD
CD Bruk:
CD pUtvalg = LC_FinnPinfoUtvalg(pszNavn);
   ==========================================================================
*/
LC_UTVALG * CFyba::LC_FinnPinfoUtvalg(LC_UT_ADM * pUtAdm,const wchar_t *pszNavn)
{
   LC_UTVALG * pU;

   if (pUtAdm != NULL)
   {
      /* Søk i kjeden av pinfo-utvalg */
      for (pU=pUtAdm->Pinfo.pForsteU; pU != NULL; pU=pU->pNesteU) {
         if (wcscmp(pszNavn,pU->pszRegel) == 0) {
            return  pU;              /* ==> Funnet */
         }
      }
   }

   // Ikke funnet
   return NULL;
}


/*
AR-881130
CH LC_PunktUtvalg                                               PUNKT-utvalg
CD =============================================================================
CD Formål:
CD Sjekker PINFO-delen av aktuell gruppe for tilslag på PUNKT-UTVALG.
CD
CD Parametre:
CD Type       Navn       I/U   Forklaring
CD -----------------------------------------------------------------------------
CD LC_UT_ADM * pUtAdm      i    Peker til administrasjonsblokk for utvalg.
CD short      sPrior      i    Prioritet.
CD short     *psStat     iu    Søkestatus, Inn: 1=start søk, 0=fortsett søk
CD                                         Ut : 0=tilslag, -1=ikke tilslag
CD long       lPnr        i    Punktnummer som skal sjekkes.
CD wchar_t     **ppszRegel   u    Peker til regelnavn
CD
CD Bruk:
CD LC_PunktUtvalg(pUtAdm,sPrior,&psStat,lPnr,&ppszRegel);
   =============================================================================
*/
void CFyba::LC_PunktUtvalg(LC_UT_ADM *pUtAdm,short sPrior,short *psStat,long lPnr,wchar_t **ppszRegel)
{
   LC_UTVALG * pU;


   if (pUtAdm != NULL)
   {
      if (*psStat == 1) {               /* Initier søk */
         /* Utvalgsmetode "!" (ikke) er brukt, eller
            punktet har PINFO mm. */
         if (pUtAdm->Punkt.sTestAllePi == UT_TRUE  ||
             LC_TestPi(lPnr,pUtAdm->Punkt.sHoydeBrukt)) {
            pU = pUtAdm->Punkt.pForsteU;

         } else {
            pU = NULL;
         }

      } else {
         if (pUtAdm->Punkt.pAktU != NULL) {
            pU = pUtAdm->Punkt.pAktU->pNesteU;
         } else {
            pU = NULL;
         }
      }

                                      /* Søk */
      while (pU != NULL) {
         /* Rett prioritet ? */
         if (sPrior == LC_OVERSE_PRIORITET  ||  pU->sPrioritet == sPrior) {
            if (LC_PiTestUtvalg(pUtAdm,pU,lPnr)) {           /* Tilslag */
               pUtAdm->Punkt.pAktU = pU;
               *ppszRegel = pU->pszRegel;
               *psStat = 0;
               return;                     /* ==> Retur ved tilslag */
            }
         }

         /* Fortsett med neste utvalg */
         pU = pU->pNesteU;
      }

      pUtAdm->Punkt.pAktU = pU;
   }

   *psStat = -1;                                    /* Ikke tilslag */
   return;
}


/*
AR-920929
CH LC_PiTestUtvalg                                  Sjekk PUNKT/PINFO utvalg
CD ==========================================================================
CD Formål:
CD Sjekk om PINFO i aktuellt punkt tilfredstiller et punkt-utvalg.
CD
CD Parametre:
CD Type        Navn     I/U  Forklaring
CD --------------------------------------------------------------------------
CD LC_UT_ADM *  pUtAdm    i   Administrasjonsblokk
CD LC_UTVALG *  pU        i   Peker til utvalg
CD long        lPnr      i    Punktnummer som skal sjekkes.
CD short       sTilslag  r   Status: UT_TRUE=tilslag, UT_FALSE=ikke tilslag
CD
CD Bruk:
CD sTilslag = LC_PiTestUtvalg(pUtAdm,pU,lPnr);
   ==========================================================================
*/
short CFyba::LC_PiTestUtvalg(LC_UT_ADM * pUtAdm,LC_UTVALG * pU,long lPnr)
{
   short sTilslag = UT_FALSE;


   if (pUtAdm != NULL)
   {
      LC_UTVALG_ELEMENT * pUE = pU->pForsteUE;

      /* Søk */
      while (pUE != NULL) {
                                          /* Linjen må testes i disse tilfeller */
                                      /* Har tilslag, og metode er ..OG */
                                      /* Har ikke tilslag, og metode er ..ELLER */
         if (( sTilslag  &&  pUE->kommando == LC_U_OG) ||
             (!sTilslag  &&  pUE->kommando == LC_U_ELLER)) {
            sTilslag = LU_PiTestDelutvalg(pUtAdm,pUE,lPnr);
         }

         pUE = pUE->pNesteUE;
      }
   }

   return  sTilslag;
}


/*
AR-920617
CH LU_PiTestDelutvalg                              Sjekk en del av et utvalg
CD ==========================================================================
CD Formål:
CD Sjekk om PINFO i aktuellt punkt tilfredstiller en del av et
CD punkt-utvalg. Sjekker et utvalgselement samt underliggende elementer
CD under dette.
CD
CD Parametre:
CD Type                 Navn     I/U  Forklaring
CD --------------------------------------------------------------------------
CD LC_UT_ADM *           pUtAdm    i   Administrasjonsblokk
CD LC_UTVALG_ELEMENT *  pUE       i   Peker til utvalgselement
CD long                lPnr      i   Punktnummer som skal sjekkes.
CD short                sTilslag  r   Status: UT_TRUE=tilslag, UT_FALSE=ikke tilslag
CD
CD Bruk:
CD sTilslag = LU_PiTestDelutvalg(pUtAdm,pUE,lPnr);
   ==========================================================================
*/
short CFyba::LU_PiTestDelutvalg(LC_UT_ADM * pUtAdm,LC_UTVALG_ELEMENT * pUE,long lPnr)
{
   short sTilslag = UT_FALSE;
   short sForste = UT_TRUE;
                                   /* Søk */
   while (pUE != NULL) {
      if (sForste) {
         /*
          * Første element i delutvalget skal alltid testes.
          */
         sTilslag = LU_PiTestLinje(pUE,lPnr);

         pUE = pUE->pForsteUE;

         sForste = UT_FALSE;

      } else {
         /*
          * Elementet må testes i disse tilfeller:
          *  - Har tilslag, og metode er ..OG.
          *  - Har ikke tilslag, og metode er ..ELLER.
          * Elementet kan være toppen av et nytt delutvalg, kaller derfor
          * LU_PiTestDelutvalg for å teste dette elementet.
          */
         if (( sTilslag  &&  pUE->kommando == LC_U_OG) ||
            (!sTilslag  &&  pUE->kommando == LC_U_ELLER)) {

            sTilslag = LU_PiTestDelutvalg(pUtAdm,pUE,lPnr);
         }

         pUE = pUE->pNesteUE;
      }
   }

   return  sTilslag;
}


/*
AR-920617
CH LU_PiTestLinje                                      Sjekk en utvalgslinje
CD ==========================================================================
CD Formål:
CD Sjekk om PINFO i aktuellt punkt tilfredstiller en linje punkt-utvalg.
CD
CD Parametre
CD Type                Navn   I/U  Forklaring
CD --------------------------------------------------------------------------
CD LC_UTVALG_ELEMENT * pUE     i   Peker til administrasjonsblokk for utvalg.
CD long                lPnr    i   Punktnummer som skal sjekkes.
CD short               tilslag r   Status: 1=tilslag, 0=ikke tilslag.
CD
CD Bruk:
CD tilslag = LU_PiTestLinje(pUE,lPnr);
   ==========================================================================
*/
short CFyba::LU_PiTestLinje(LC_UTVALG_ELEMENT * pUE,long lPnr)
{
   wchar_t akt_para[LC_MAX_SOSI_LINJE_LEN];
   short tilslag;
   long lMaxAntall;
   wchar_t *para;
   short sSett = 1;
   short metode = pUE->metode;

                                   /* Metode IKKE "!" */
   if (metode == LC_U_IKKE){
      /* AR:2002-04-24 */
      /* if ((para = LC_GetPiVerdi(pUE->sosi,lPnr,&sSett)) == NULL  ||
          *para == '\0') {  */
      if (LC_GetPiVerdi(pUE->sosi,lPnr,&sSett) == NULL) {
         return UT_TRUE;
      }

   } else if (metode == LC_U_FLERE) {          /* Metode "FL" */
      /* Teller opp antall av dette SOSI-navnet */
      tilslag = 0;
      while (LC_GetPiVerdi(pUE->sosi,lPnr,&sSett) != NULL) {
         tilslag++;
         /* Tilslag hvis navnet finnes mer enn 1 gang */
         if (tilslag > 1)  return UT_TRUE;
         sSett++;
      }

   } else if (metode == LC_U_IKKEFLERE) {          /* Metode "!FL" */
      /* Teller opp antall av dette SOSI-navnet */
      wchar_t *ep;
      lMaxAntall = max(wcstol(pUE->min,&ep,10),1l);
      tilslag = 0;
      
      while (LC_GetPiVerdi(pUE->sosi,lPnr,&sSett) != NULL) {
         tilslag++;
         /* Har flere forekomster, avbryter */
         if (tilslag > lMaxAntall)  return UT_FALSE;
         sSett++;
      }
      /* Tilslag, ikke flere forekomster */
      return UT_TRUE;

   } else if (metode == LC_U_ALLE) {          /* Metode "AL" */
      if (LC_GetPiVerdi(pUE->sosi,lPnr,&sSett) != NULL) {
         return UT_TRUE;
      }

                                   /* Metode IKKE LIK "!=" */
   } else if (metode == LC_U_IKKELIK) {
      tilslag = 0;
      pUE->metode = LC_U_LIK;                         /* Sjekker først på likhet */
         /* Hent parameter */
      while ((para = LC_GetPiVerdi(pUE->sosi,lPnr,&sSett)) != NULL) {
         if (LU_ParaTest(pUE,para,akt_para,LC_MAX_SOSI_LINJE_LEN)) {              /* Tilslag? */
            tilslag = 1;
            break;          /* Vet nå at det ikke blir tilslag, hopper ut */
         }
         sSett++;
      }
      pUE->metode = LC_U_IKKELIK;
      if (! tilslag)              /* Tilslag når "=-testen" ikke ga tilslag */
         return UT_TRUE;

                                   /* Metode INNEHOLDER IKKE "!()" */
   } else if (metode == LC_U_IKKECONTEIN) {
      tilslag = 0;
      pUE->metode = LC_U_CONTEIN;            /* Sjekker først INNEHOLDER */
         /* Hent parameter */
      while ((para = LC_GetPiVerdi(pUE->sosi,lPnr,&sSett)) != NULL) {
         if (LU_ParaTest(pUE,para,akt_para,LC_MAX_SOSI_LINJE_LEN)) {              /* Tilslag? */
            tilslag = 1;
            break;          /* Vet nå at det ikke blir tilslag, hopper ut */
         }
         sSett++;
      }
      pUE->metode = LC_U_IKKECONTEIN;
      if (! tilslag)              /* Tilslag når "()-testen" ikke ga tilslag */
         return UT_TRUE;

                                   /* Andre utvalgsmetoder */
   } else {
      /* Hent parameter */
      while ((para = LC_GetPiVerdi(pUE->sosi,lPnr,&sSett)) != NULL) {
         if (LU_ParaTest(pUE,para,akt_para,LC_MAX_SOSI_LINJE_LEN)) {              /* Tilslag? */
            return UT_TRUE;
         }
         sSett++;
      }
   }

   return UT_FALSE;                                         /* Ikke tilslag */
}


/*
AR-881130
CH LC_GruppeUtvalg                                              GINFO-utvalg
CD ==========================================================================
CD Formål:
CD Sjekker GINFO-delen av aktuell gruppe mot alle gruppeutvalg fra fil.
CD
CD Parametre:
CD Type       Navn     I/U   Forklaring
CD -----------------------------------------------------------------------------
CD LC_UT_ADM *pUtAdm    i    Peker til administrasjonsblokk for utvalg.
CD short      sPrior    i    Prioritet.
CD                           LC_OVERSE_PRIORITET = Tar ikke hensyn til prioritet.
CD short     *sstat     iu   Søkestatus, Inn: 1=start søk, 0=fortsett søk
CD                                       Ut : 0=tilslag, -1=ikke tilslag
CD wchar_t     **regelnavn  u   Peker til regelnavn
CD wchar_t      *regelnavn  u   Peker til utvalgsnavn
CD
CD Bruk:
CD pszUtvalgsNavn = LC_GruppeUtvalg(pUtAdm.sPrior,&sstat,&regel);
   =============================================================================
*/
wchar_t * CFyba::LC_GruppeUtvalg(LC_UT_ADM *pUtAdm,short sPrior,short *sstat,wchar_t **regelnavn)
{
   LC_UTVALG * pU;

   if (pUtAdm != NULL)
   {
      if (*sstat == 1) {               /* Initier søk */
         pU = pUtAdm->Gruppe.pForsteU;
         pUtAdm->sGruppeValgt = UT_FALSE;

      } else {
         if (pUtAdm->Gruppe.pAktU != NULL) {
            pU = pUtAdm->Gruppe.pAktU->pNesteU;
         } else {
            pU = NULL;
         }
      }

                                      /* Søk */
      while (pU != NULL) {
         /* Rett prioritet ? */
         if (sPrior == LC_OVERSE_PRIORITET  ||  pU->sPrioritet == sPrior) {
            if (LU_GiTestUtvalg(pUtAdm,pU)) {           /* Tilslag */
               *regelnavn = pU->pszRegel;
               *sstat = 0;
               pUtAdm->sGruppeValgt = UT_TRUE;
               pUtAdm->Gruppe.pAktU = pU;

               return  pU->pszNavn;   /* ==> Retur ved tilslag */
            }
         }

         /* Fortsett med neste utvalg */
         pU = pU->pNesteU;
      }

      pUtAdm->Gruppe.pAktU = pU;
   }

   *sstat = -1;                                    /* Ikke tilslag */
   return  NULL;
}


/*
AR-881213
CH LC_GiQuery                                         Query mot aktuell ginfo
CD =============================================================================
CD Formål:
CD Sjekker GINFO-delen av aktuell gruppe mot aktuellt query-oppsett.
CD
CD Parametre:
CD Type       Navn     I/U   Forklaring
CD -----------------------------------------------------------------------------
CD LC_UT_ADM *pUtAdm    i    Peker til administrasjonsblokk for utvalg.
CD short      status    r    Søkestatus, UT_TRUE=tilslag, UT_FALSE=ikke tilslag
CD
CD Bruk:
CD ist = LC_GiQuery(pUtAdm);
   =============================================================================
*/
short CFyba::LC_GiQuery(LC_UT_ADM *pUtAdm)
{
   if (pUtAdm != NULL)
   {
      pUtAdm->sGruppeValgt = UT_FALSE;

      return  LU_GiTestUtvalg(pUtAdm,pUtAdm->Gruppe.pForsteU);
   }

   return UT_FALSE;
}

/*
JAØ-20000512
CH LC_FAGiKombinertFlateQuery   Finn alle ved query mot ginfo i flate og omkrets
CD =============================================================================
CD Formål:
CD Sjekker GINFO-delen av alle flater mot aktuell queryopsett. Finner de flatene
CD som har tilslag på utvalgsblokken pUtAdmFlate og har har referanser til grupper
CD som har tilslag på utvalgsblokken pUtAdmOmkrets.
CD Tilslag merkes i brukttabellen kolonne BT_GISOK (30).
CD [Esc] avbryter utvalget, antall tilslag settes da til -1.
CD
CD Parametre:
CD Type            Navn     I/U   Forklaring
CD -----------------------------------------------------------------------------
CD LC_UT_ADM  *pUtAdmFlate   i    Peker til administrasjonsblokk for utvalg for flata. 
CD LC_UT_ADM  *pUtAdmOmkrets i    Peker til administrasjonsblokk for utvalg for omkrets.
CD unsigned short  usLag     i    Velg hvilke "lag" det skal søkes i.
CD                                LC_FRAMGR og /eller LC_BAKGR
CD short           sAlle     i    Flagg for hvorvidt utvalg for omkrets må slå til på 
CD                                alle gruppene i omkretsen. TRUE/FALSE
CD short           antall    r    Antall tilslag på utvalget.
CD 
CD Bruk:
CD antall = LC_FAGiQuery(pUtAdm, LC_FRAMGR | LC_BAKGR);
CD
   =============================================================================
*/
long CFyba::LC_FAGiKombinertFlateQuery(LC_UT_ADM * pUtAdmFlate,LC_UT_ADM * pUtAdmOmkrets,
                                               unsigned short usLag,short sMetode)
{
	#define RED_MAX_REF   10

	short ngi,gnavn,tilslag,i,avbryt;
   long nko;
   long ant_ref;
	unsigned short info;
	LC_BGR Bgr,AktBgr,FlateBgr;
	short avbrutt = UT_FALSE;
	long *gs;

	LC_GRF_STATUS GrfStat;
	long gsnr[RED_MAX_REF];
	unsigned char ref_status[RED_MAX_REF];
	long antall = -1L;


   if (pUtAdmFlate != NULL  &&  pUtAdmOmkrets != NULL)
   {   
      antall = 0L;

      LC_GetGrNr(&AktBgr);

      /* Blanker brukttabellen */
	   LI_EraseBt(BT_GISOK,BT_GISOK);

	   LC_InitNextBgr(&FlateBgr);

	   // Sjekker alle grupper i aktuellt lag 
	   while (LC_NextBgr(&FlateBgr,usLag)  &&  !avbrutt) {
		   // Filhodet behandles ikke 
		   if (FlateBgr.lNr != 0L) {
			   gnavn = LC_RxGr(&FlateBgr,LES_OPTIMALT,&ngi,&nko,&info);
			   if (gnavn == L_FLATE) { // Er det en flate ?
				   if (ngi > 0){                    // Med koordinat ?
					   if (LC_GiQuery(pUtAdmFlate)){   // Tilslag på flata.

						   LC_InitGetRefFlate(&GrfStat);
						   ant_ref = LC_GetRefFlate(&GrfStat,GRF_YTRE|GRF_INDRE,gsnr,(unsigned char*)ref_status,RED_MAX_REF);
						   Bgr.pFil = FlateBgr.pFil;

						   tilslag = 0;
						   avbryt = 0;
						   do {
							   i = 0;
							   gs = gsnr;
							   while ((i < ant_ref) && !avbryt) {
								   if (!((ref_status[i] & GRF_START_OY) || (ref_status[i] & GRF_SLUTT_OY))) {
									   Bgr.lNr = labs(*gs);
									   LC_RxGr(&Bgr,LES_OPTIMALT,&ngi,&nko,&info);
									   tilslag = LC_GiQuery(pUtAdmOmkrets);
									   if (sMetode == LC_ALLE) {
										   avbryt = !tilslag;
									   } else if (sMetode == LC_INGEN) {
										   avbryt = tilslag;
									   } else { //(sMetode == LC_NOEN)
										   avbryt = tilslag;
									   }
								   }
								   i++;
								   gs++;

							   }

							   LC_RxGr(&FlateBgr,LES_OPTIMALT,&ngi,&nko,&info);
							   ant_ref   = LC_GetRefFlate(&GrfStat,GRF_YTRE|GRF_INDRE,gsnr,(unsigned char*)ref_status,RED_MAX_REF);
						   } while (ant_ref>0  && !avbryt);
   						
						   if (sMetode == LC_INGEN) tilslag = !tilslag;
						   if (tilslag) {
							   LC_RxGr(&FlateBgr,LES_OPTIMALT,&ngi,&nko,&info);
							   antall += LC_MerkGr(BT_GISOK,1);     // Tilslag 
						   }
					   }
				   }
			   }
		   }
		   avbrutt = LC_Cancel();     /* <ESC> avbryter lesing */
	   }
      
	   if (avbrutt)  antall = -1L;

	   if (AktBgr.lNr != INGEN_GRUPPE) {
		   LC_RxGr(&AktBgr,LES_OPTIMALT,&ngi,&nko,&info);
	   }
   }

	return antall;
}

/*
AR-900109
CH LC_FAGiQuery                                 Finn alle ved query mot ginfo
CD =============================================================================
CD Formål:
CD Sjekker GINFO-delen av alle grupper mot aktuell queryopsett.
CD Tilslag merkes i brukttabellen kolonne BT_GISOK (14).
CD [Esc] avbryter utvalget, antall tilslag settes da til -1.
CD
CD Parametre:
CD Type            Navn    I/U   Forklaring
CD -----------------------------------------------------------------------------
CD LC_UT_ADM      *pUtAdm   i    Peker til administrasjonsblokk for utvalg.
CD unsigned short  usLag    i    Velg hvilke "lag" det skal søkes i.
CD                                 LC_FRAMGR og /eller LC_BAKGR
CD short           antall   r    Antall tilslag på utvalget.
CD
CD Bruk:
CD antall = LC_FAGiQuery(pUtAdm, LC_FRAMGR | LC_BAKGR);
CD
   =============================================================================
*/
long CFyba::LC_FAGiQuery(LC_UT_ADM *pUtAdm,unsigned short usLag)
{
   short ngi;
   long nko;
   unsigned short info;
   LC_BGR Bgr,AktBgr;
   short avbrutt = UT_FALSE;
   long antall = -1L;


   if (pUtAdm != NULL)
   {
      antall = 0L;

      LC_GetGrNr(&AktBgr);

      /* Blanker brukttabellen */
      LI_EraseBt(BT_GISOK,BT_GISOK);

      LC_InitNextBgr(&Bgr);
      
      /* Sjekker alle grupper i aktuellt lag */
      while(LC_NextBgr(&Bgr,usLag)  &&  !avbrutt) {
         /* Filhodet behandles ikke */
         if (Bgr.lNr != 0L) {
            LC_RxGr(&Bgr,LES_OPTIMALT,&ngi,&nko,&info);
            if (ngi > 0){                    /* Finnes gruppen ? */
               if (LC_GiQuery(pUtAdm)){
                  antall += LC_MerkGr(BT_GISOK,1);     /* Tilslag */
               }
            }
         }
         avbrutt = LC_Cancel();     /* <ESC> avbryter lesing */
      }
      
      if (avbrutt)  antall = -1L;

      if (AktBgr.lNr != INGEN_GRUPPE) {
         LC_RxGr(&AktBgr,LES_OPTIMALT,&ngi,&nko,&info);
      }
   }

   return antall;
}

/*
AR-20040705
ÅE-20040705
CH LC_FAPiQuery                            Finn alle grupper ved query mot pinfo
CD =============================================================================
CD Formål:
CD Sjekker PINFO-delen av alle grupper mot aktuell queryopsett.
CD Tilslag merkes i brukttabellen kolonne BT_GISOK (14).
CD [Esc] avbryter utvalget, antall tilslag settes da til -1.
CD
CD Parametre:
CD Type            Navn    I/U   Forklaring
CD -----------------------------------------------------------------------------
CD LC_UT_ADM      *pUtAdm   i    Peker til administrasjonsblokk for utvalg.
CD unsigned short  usLag    i    Velg hvilke "lag" det skal søkes i.
CD                                 LC_FRAMGR og /eller LC_BAKGR
CD short           antall   r    Antall tilslag på utvalget.
CD
CD Bruk:
CD antall = LC_FAPiQuery(pUtAdm, LC_FRAMGR | LC_BAKGR);
CD
   =============================================================================
*/
long CFyba::LC_FAPiQuery(LC_UT_ADM *pUtAdm,unsigned short usLag)
{
   short ngi;
   long nko;
   unsigned short info;
   LC_BGR Bgr,AktBgr;
   long lPunkt;
   short avbrutt = UT_FALSE;
   short ustat;
   short sFunnet = UT_FALSE;
   wchar_t *regel;
   long antall = -1L;


   if (pUtAdm != NULL)
   {    
      antall = 0L;

      LC_GetGrNr(&AktBgr);

      /* Blanker brukttabellen */
      LI_EraseBt(BT_GISOK,BT_GISOK);

      LC_InitNextBgr(&Bgr);
      
      /* Sjekker alle grupper i aktuellt lag */
      while(LC_NextBgr(&Bgr,usLag)  &&  !avbrutt) {
         /* Filhodet behandles ikke */
         if (Bgr.lNr != 0L) {
            LC_RxGr(&Bgr,LES_OPTIMALT,&ngi,&nko,&info);
            if (nko > 0){                    /* Finnes gruppen ? */
               sFunnet=UT_FALSE;
               lPunkt=1;
               // Kontrollerer punkt i gruppa:
               while (lPunkt<=nko && sFunnet==UT_FALSE && !avbrutt) {
                  ustat = 1;
                  LC_PunktUtvalg(pUtAdm,LC_OVERSE_PRIORITET,&ustat,lPunkt,&regel);
                  if (ustat == 0 && sFunnet==UT_FALSE && !avbrutt) {
                     sFunnet = UT_TRUE;
                     antall += LC_MerkGr(BT_GISOK,1);     /* Tilslag */
                     break; // avbryter søk i denne gruppen ved første aktuelle pinfo
                  }
                  lPunkt++;
                  avbrutt = LC_Cancel();     /* <ESC> avbryter lesing */
               }
            }
         }
         avbrutt = LC_Cancel();     /* <ESC> avbryter lesing */
      }
      
      if (avbrutt)  antall = -1L;

      if (AktBgr.lNr != INGEN_GRUPPE) {
         LC_RxGr(&AktBgr,LES_OPTIMALT,&ngi,&nko,&info);
      }
   }

   return antall;
}


/*
AR-890904
CH LU_GiTestUtvalg                                           Sjekk et utvalg
CD ==========================================================================
CD Formål:
CD Sjekk om GINFO av aktuell gruppe tilfredstiller et gruppe-utvalg.
CD
CD Parametre:
CD Type        Navn     I/U  Forklaring
CD --------------------------------------------------------------------------
CD LC_UT_ADM *  pUtAdm    i   Administrasjonsblokk
CD LC_UTVALG *  pU        i   Peker til utvalg
CD short       sTilslag  r   Status: UT_TRUE=tilslag, UT_FALSE=ikke tilslag
CD
CD Bruk:
CD bTilslag = LU_GiTestUtvalg(pUtAdm,pU);
   ==========================================================================
*/
short CFyba::LU_GiTestUtvalg(LC_UT_ADM * pUtAdm,LC_UTVALG *  pU)
{
   short sTilslag = UT_FALSE;
   LC_UTVALG_ELEMENT * pUE = pU->pForsteUE;

	// Hvis utvalgsregelen er med i et lag og dette ikke 
	// skal tegnes ut, returneres FALSE. JAØ-19980922
	if (pU->pLag != NULL) {
		if (pU->pLag->sLagAktiv == 0) return 0;
	}
   // Innført at også utvalgsregler kan slås av for tegning. JAØ-20020927
   if (!pU->sTegnes) return 0;
                                   /* Søk */
   while (pUE != NULL) {
                                       /* Linjen må testes i disse tilfeller */
                                   /* Har tilslag, og metode er ..OG */
                                   /* Har ikke tilslag, og metode er ..ELLER */
      if (( sTilslag  &&  pUE->kommando == LC_U_OG) ||
          (!sTilslag  &&  pUE->kommando == LC_U_ELLER)) {
         sTilslag = LU_GiTestDelutvalg(pUtAdm,pUE);
      }

      pUE = pUE->pNesteUE;
   }

   return  sTilslag;
}



/*
AR-890904
CH LU_GiTestDelutvalg                              Sjekk en del av et utvalg
CD ==========================================================================
CD Formål:
CD Sjekk om GINFO av aktuell gruppe tilfredstiller en del av et
CD gruppe-utvalg. Sjekker et utvalgselement samt underliggende elementer
CD under dette.
CD
CD Parametre:
CD Type                 Navn     I/U  Forklaring
CD --------------------------------------------------------------------------
CD LC_UT_ADM *           pUtAdm    i   Administrasjonsblokk
CD LC_UTVALG_ELEMENT *  pUE       i   Peker til utvalgselement
CD short                sTilslag  r   Status: UT_TRUE=tilslag, UT_FALSE=ikke tilslag
CD
CD Bruk:
CD bTilslag = LU_GiTestDelutvalg(pUtAdm,pUE);
   ==========================================================================
*/
short CFyba::LU_GiTestDelutvalg(LC_UT_ADM * pUtAdm,LC_UTVALG_ELEMENT * pUE)
{
	wchar_t *apara;
   short gilin;
   short sTilslag = UT_FALSE;
   short sForste = UT_TRUE;
                                   /* Søk */
   while (pUE != NULL) {
      if (sForste) {
         /*
          * Første element i delutvalget skal alltid testes.
          */
         sTilslag = LU_GiTestLinje(pUtAdm,pUE,&gilin,&apara);

         pUE = pUE->pForsteUE;

#ifdef TEST
         if (pUE->pForsteUE != NULL) {
            pUE = pUE->pForsteUE;
         } else {
            pUE = pUE->pNesteUE;
         }
#endif

         sForste = UT_FALSE;

      } else {
         /*
          * Elementet må testes i disse tilfeller:
          *  - Har tilslag, og metode er ..OG.
          *  - Har ikke tilslag, og metode er ..ELLER.
          * Elementet kan være toppen av et nytt delutvalg, kaller derfor
          * LU_GiTestDelutvalg for å teste dette elementet.
          */
         if (( sTilslag  &&  pUE->kommando == LC_U_OG) ||
            (!sTilslag  &&  pUE->kommando == LC_U_ELLER)) {
            sTilslag = LU_GiTestDelutvalg(pUtAdm,pUE);
         }

         pUE = pUE->pNesteUE;
      }
   }

   return  sTilslag;
}


/*
AR-881215
CH LU_GiTestLinje                                      Sjekk en utvalgslinje
CD ==========================================================================
CD Formål:
CD Sjekk om GINFO av aktuell gruppe tilfredstiller en linje ginfo-utvalg.
CD
CD Parametre
CD Type                Navn   I/U  Forklaring
CD --------------------------------------------------------------------------
CD LC_UT_ADM *          pUtAdm  i   Peker til administrasjonsblokk for utvalg.
CD LC_UTVALG_ELEMENT * pUE     i   Peker til administrasjonsblokk for utvalg.
CD wchar_t               *gilin   u   GINFO-linje for funnet tilslag.
CD wchar_t              **apara   u   Peker til aktuell del av parameterstreng.
CD short               tilslag r   Status: 1=tilslag, 0=ikke tilslag.
CD
CD Bruk:
CD tilslag = LU_GiTestLinje(pUtAdm,pUE,&gilin,&apara);
   ==========================================================================
*/
short CFyba::LU_GiTestLinje(LC_UT_ADM * pUtAdm,LC_UTVALG_ELEMENT * pUE,
                            short *gilin,wchar_t **apara)
{
   wchar_t akt_para[LC_MAX_SOSI_LINJE_LEN];
   short metode,tilslag;
   long lMaxAntall;
   wchar_t *para;

   metode = pUE->metode;
   *gilin = 1;
   *apara = akt_para;

   /* Metode IKKE "!" */
   if (metode == LC_U_IKKE){
      /* AR:2002-04-24 */
      /* if ((para = LC_GetGP(pUE->sosi,gilin,9999)) == NULL  ||
          *para == '\0') { */
      if (LC_GetGP(pUE->sosi,gilin,9999) == NULL) {
         return UT_TRUE;
      }

   /* Metode "FL" */
   } else if (metode == LC_U_FLERE) { 
      /* Teller opp antall av dette SOSI-navnet */
      tilslag = 0;
      while ((*apara = LC_GetGP(pUE->sosi,gilin,9999)) != NULL) {
         tilslag++;
         /* Tilslag hvis navnet finnes mer enn 1 gang */
         if (tilslag > 1)  return UT_TRUE;
         (*gilin)++;
      }

   /* Metode "!FL" */
   } else if (metode == LC_U_IKKEFLERE) {
      /* Teller opp antall av dette SOSI-navnet */
      wchar_t *ep;
      lMaxAntall = max(wcstol(pUE->min,&ep,10),1l);
      tilslag = 0;
      while ((*apara = LC_GetGP(pUE->sosi,gilin,9999)) != NULL) {
         tilslag++;
         /* Har flere forekomster, avbryter */
         if (tilslag > lMaxAntall)  return UT_FALSE;
         (*gilin)++;
      }
      /* Tilslag, ikke flere forekomster */
      return UT_TRUE;

   /* Metode "AL" */
   } else if (metode == LC_U_ALLE) { 
      if ((*apara = LC_GetGP(pUE->sosi,gilin,9999)) != NULL) {
         return UT_TRUE;
      }

   /* Metode IKKE VALGT "IV" */
   } else if (metode == LC_U_IKKEVALGT) {
      if (! pUtAdm->sGruppeValgt)
         return  UT_TRUE;

   /* Metode IKKE LIK "!=" */
   } else if (metode == LC_U_IKKELIK) {
      tilslag = 0;
      pUE->metode = LC_U_LIK;                         /* Sjekker først på likhet */
      while ((para = LC_GetGP(pUE->sosi,gilin,9999)) != NULL) { /* Hent parameter */
         if (LU_ParaTest(pUE,para,akt_para,LC_MAX_SOSI_LINJE_LEN)) {              /* Tilslag? */
            tilslag = 1;
            break;          /* Vet nå at det ikke blir tilslag, hopper ut */
         }
         (*gilin)++;
      }
      pUE->metode = LC_U_IKKELIK;
      if (! tilslag)              /* Tilslag når "=-testen" ikke ga tilslag */
         return UT_TRUE;

   /* Metode INNEHOLDER IKKE "!()" */
   } else if (metode == LC_U_IKKECONTEIN) {
      tilslag = 0;
      pUE->metode = LC_U_CONTEIN;            /* Sjekker først INNEHOLDER */
      while ((para = LC_GetGP(pUE->sosi,gilin,9999)) != NULL) { /* Hent parameter */
         if (LU_ParaTest(pUE,para,akt_para,LC_MAX_SOSI_LINJE_LEN)) {              /* Tilslag? */
            tilslag = 1;
            break;          /* Vet nå at det ikke blir tilslag, hopper ut */
         }
         (*gilin)++;
      }
      pUE->metode = LC_U_IKKECONTEIN;
      if (! tilslag)              /* Tilslag når "=-testen" ikke ga tilslag */
         return UT_TRUE;


   /* Andre utvalgsmetoder */
   } else {
      while ((para = LC_GetGP(pUE->sosi,gilin,9999)) != NULL) { /* Hent parameter */
         if (LU_ParaTest(pUE,para,akt_para,LC_MAX_SOSI_LINJE_LEN)) {              /* Tilslag? */
            return UT_TRUE;
         }
         (*gilin)++;
      }
   }

   return 0;                                         /* Ikke tilslag */
}


/*
AR:2006-08-08
CH LU_ParaTest                                     Sjekk parameter for tilslag
CD =============================================================================
CD Formål:
CD Sjekk om parameteren tilfredsstiller denne utvalgslinjen.
CD
CD Parametre:
CD Type                Navn    I/U  Forklaring
CD -----------------------------------------------------------------------------
CD LC_UTVALG_ELEMENT * pUE      i   Peker til utvalgslinje
CD wchar_t               *para     i   Peker til parameterstreng
CD wchar_t               *akt_para i   Aktuell del av parameterstrengen.
CD short               sMaxLen  i   Maks lengde for akt_para
CD short               tilslag  r   Status: 1=tilslag, 0=ikke tilslag
CD
CD Bruk:
CD tilslag = LU_ParaTest(pUE,para,&akt_para,sMaxLen);
   =============================================================================
*/
short CFyba::LU_ParaTest(LC_UTVALG_ELEMENT * pUE,wchar_t *para,wchar_t *pszAktPara,short sMaxLen)
{
   long heltall;
   double flyttall,desimal;
   short metode = pUE->metode;

   
   /* Juster for ledd, og del av streng
    * når dette er gitt som del av utvalgslinjen
    * (og ikke som del av SOSI-navnet)
    */
   if((pUE->ledd > 0) || (pUE->start > 0) || (pUE->slutt > 0)) { // Ledd eller del av streng er angitt utenfor SOSI-navnet
      LU_JustPara(para,(short)pUE->ledd,(short)pUE->start,(short)pUE->slutt,
                   pszAktPara,LC_MAX_SOSI_LINJE_LEN);
   }
   else {
      UT_StrCopy(pszAktPara,para,sMaxLen);
   }

   // Sjekk først at sammenligningen kan gjøres med aktuell talltype
   short type = pUE->type;
   if ((type & LC_U_TALL) == LC_U_TALL)
   { 
      wchar_t *ep;
      heltall = wcstol(pszAktPara,&ep,10);   // OBS! Denne brukes i selve sammenligningen lenger nede i koden
      if (heltall == LONG_MIN  ||  heltall == LONG_MAX)
      {
         // Tallet har for mange siffer, må håndtere sammenligningen som tekst
         type = LC_U_ALFA;
      }
   }

   // ----- Utfør testen
   switch (metode) {
      case LC_U_LIK:                                             /* Lik  "=" */
         if ((type & LC_U_TALL) == LC_U_TALL){              /* Heltall */
            wchar_t *ep;
            if (heltall == wcstol(pUE->min,&ep,10) )
               return (1);
         } else if ((type & LC_U_FLYT) == LC_U_FLYT){        /* Flyttall */
            wchar_t *ep;
            if (wcstod(pszAktPara,&ep) == wcstod(pUE->min,&ep) )
               return (1);
         } else{                               /* Streng */
            UT_StrUpper(pszAktPara);
            if (wcscmp(pszAktPara,pUE->min) == 0)
               return (1);
         }
         break;

      case LC_U_FRATIL:                                   /* Fra - til  "<>" */
         if ((type & LC_U_TALL) == LC_U_TALL){             /* Heltall */
            wchar_t *ep;
            if (heltall >= wcstol(pUE->min,&ep,10)  &&  heltall <= wcstol(pUE->max,&ep,10))
               return (1);
         } else if ((type & LC_U_FLYT) == LC_U_FLYT){       /* Flyttall */
            wchar_t *ep;
            flyttall = wcstod(pszAktPara,&ep);
            if (flyttall >= wcstod(pUE->min,&ep)  &&  flyttall <= wcstod(pUE->max,&ep))
               return (1);
         } else{                              /* Streng */
            UT_StrUpper(pszAktPara);
            if (wcscmp(pszAktPara,pUE->min) >= 0  &&  wcscmp(pszAktPara,pUE->max) <= 0)
               return (1);
         }
         break;

      case LC_U_DELELIG:                 /* Delelig "/" */
         // 2001-03-19: Endret slik at algoritmen for desimaltall alltid brukes.
         //if (type == U_FLYT){             /* Flyttall */
            wchar_t *ep;
            desimal = modf(wcstod(pszAktPara,&ep) / wcstod(pUE->min,&ep), &flyttall);
            if (fabs(fabs(desimal*wcstod(pUE->min,&ep)) - fabs(wcstod(pUE->max,&ep))) < 1.0E-6) {
               return (1);
            }

         //} else{                              /* Heltall eller streng */
         //   if ((wcstol(pszAktPara) % wcstol(pUE->min)) == wcstol(pUE->max)) {
         //      return (1);
         //   }
         //}
         break;
      
      case LC_U_UDELELIG:                /* Ikke delellig "!/" */
         {
         // 2001-03-19: Endret slik at algoritmen for desimaltall alltid brukes.
         //if (type == U_FLYT){             /* Flyttall */
            wchar_t *ep;
            desimal = modf(wcstod(pszAktPara,&ep) / wcstod(pUE->min,&ep), &flyttall);
            if (fabs(desimal) >= 1.0E-6)
               return (1);

         //} else{                              /* Heltall eller streng */
         //   if ((wcstol(pszAktPara) % wcstol(pUE->min)) != 0L)
         //      return (1);
         //}
         }
         break;

      case LC_U_CONTEIN:                 /* Inneholder "()" */
         UT_StrUpper(pszAktPara);
         if (wcsstr(pszAktPara,pUE->min) != NULL)
            return (1);
         break;

      case LC_U_UTENFOR:                 /* Utenfor "><" */
         if ((type & LC_U_TALL) == LC_U_TALL){             /* Heltall */
            wchar_t *ep;
            if (heltall < wcstol(pUE->min,&ep,10)  ||  heltall > wcstol(pUE->max,&ep,10))
               return (1);
         } else if ((type & LC_U_FLYT) == LC_U_FLYT){             /* Flyttall */
            wchar_t *ep;
            flyttall = wcstod(pszAktPara,&ep);
            if (flyttall < wcstod(pUE->min,&ep)  ||  flyttall > wcstod(pUE->max,&ep))
               return (1);
         } else{                              /* Streng */
            UT_StrUpper(pszAktPara);
            if (wcscmp(pszAktPara,pUE->min) < 0  ||  wcscmp(pszAktPara,pUE->max) > 0)
               return (1);
         }
         break;

      case LC_U_MINDRE:                  /* Mindre enn "<" */
         if ((type & LC_U_TALL) == LC_U_TALL){             /* Heltall */
            wchar_t *ep;
            if (heltall < wcstol(pUE->min,&ep,10))
               return (1);
         } else if ((type & LC_U_FLYT) == LC_U_FLYT){       /* Flyttall */
            wchar_t *ep;
            if (wcstod(pszAktPara,&ep) < wcstod(pUE->min,&ep))
               return (1);
         } else{                              /* Streng */
            UT_StrUpper(pszAktPara);
            if (wcscmp(pszAktPara,pUE->min) < 0)
               return (1);
         }
         break;

      case LC_U_STORRE:                  /* Større enn ">" */
         if ((type & LC_U_TALL) == LC_U_TALL){             /* Heltall */
            wchar_t *ep;
            if (heltall > wcstol(pUE->min,&ep,10))
               return (1);
         } else if ((type & LC_U_FLYT) == LC_U_FLYT){      /* Flyttall */
            wchar_t *ep;
            if (wcstod(pszAktPara,&ep) > wcstod(pUE->min,&ep))
               return (1);
         } else{                              /* Streng */
            UT_StrUpper(pszAktPara);
            if (wcscmp(pszAktPara,pUE->min) > 0)
               return (1);
         }
         break;
   }

   return (0);                             /* Ikke tilslag */
}


/*
AR-891206
CH LU_JustPara                          Juster parameter for ledd og posisjon
CD =============================================================================
CD Formål:
CD Juster parameteren for leddnummer og del av streng.
CD
CD Parametre:
CD Type       Navn    I/U   Forklaring
CD -----------------------------------------------------------------------------
CD wchar_t      *para     i    Parameterstreng som skal behandles
CD short      ledd     i    Leddnummer
CD short      start    i    Startposisjon i strengen (0=hele strengen)
CD short      slutt    i    Sluttposisjon i strengen (0=resten)
CD wchar_t      *akt_para iu   Ny behandla parameterstreng
CD short      max_len  i    Max lengde på akt_para
CD
CD Bruk:
CD LU_JustPara(para,ledd,start,slutt,akt_para,max_len);
   =============================================================================
*/
void CFyba::LU_JustPara(wchar_t *para,short ledd,short start,short slutt,
                         wchar_t *akt_para,short max_len)
{
   wchar_t *cp,*nt;
                                    /* Juster for ledd */
   cp = UT_strtok(para,L" ",&nt);
   while(cp != NULL  &&  --ledd > 0){
      cp = UT_strtok(NULL,L" ",&nt);
   }

                                    /* Juster for delstreng */
   if (cp != NULL){
      if (start != 0){
         if (slutt != 0){
            slutt = min(slutt,((short)wcslen(cp)));
            *(cp+slutt) = '\0';
         }
         start = min(start,((short)wcslen(cp)));
         cp += (start-1);
      }
      UT_StrCopy(akt_para,cp,max_len);

   } else{
      *akt_para = '\0';
   }
}


/*
AR-891207
CH LC_QueryGP                                       Søk i ginfo og finn verdi
CD =============================================================================
CD Formål:
CD Bruker query-tekst for å finne linje der parameter skal hentes.
CD
CD Parametre:
CD Type     Navn   I/U   Forklaring
CD ---------------------------------------------------------------------------
CD wchar_t    *qulin   i    Linje med query-tekst.
CD unsigned short    iniv    i    Nivå: Det er definert konstanter som henges
CD                             sammen med "|".
CD                             LC_GINFO = søk i GINFO på aktuell gruppe
CD                             LC_HODE = søk i filhodet
CD                             Hvis begge er brukt søkes det først i GINFO.
CD unsigned short   *univ    u    Nivå: LC_GINFO = parameter er fra GINFO
CD                             LC_HODE = parameter er fra filhodet
CD short   *ulin    u    GINFO-linjenummer for tilslaget.
CD wchar_t   **para    u    Funnet parameter.
CD short     funnet  r    Status: UT_TRUE=funnet, UT_FALSE=ikke funnet
CD
CD Bruk:
CD funnet = LC_QueryGP(qulin,LC_GINFO | LC_HODE,&univ,&ulin,&para);
   =============================================================================
*/
short CFyba::LC_QueryGP(wchar_t *qulin,unsigned short iniv,unsigned short *univ,short *ulin,wchar_t **para)
{
   LC_BGR Bgr,Hode;
   short ngi;
   long nko;
   unsigned short info;
   short sFunnet = UT_FALSE;
   LC_UT_ADM *pUtAdm;

   /* Åpne query */
   pUtAdm = LC_OpenQuery();
   /* Tolk linjen */
   if (LC_PutQueryLine(pUtAdm,qulin,U_GRUPPE)) {
      LC_PutQueryRegel(pUtAdm->Gruppe.pSisteU,L"S");

      /* Sjekk GINFO */
      /* Initier søk */
      pUtAdm->Gruppe.pAktU = pUtAdm->Gruppe.pForsteU;
      pUtAdm->sGruppeValgt = UT_FALSE;

      /* Søk */
      sFunnet = LU_GiTestLinje(pUtAdm,pUtAdm->Gruppe.pForsteU->pForsteUE,ulin,para);
      *univ = LC_GINFO;

      /*
       * Ikke funnet, og konstant er satt for at hodet skal sjekkes.
       * Sjekk hodet.
       */
      if ( (! sFunnet)  &&  (iniv & LC_HODE)) {
         LC_GetGrNr(&Bgr);    /* Husk aktuell gruppe */
         Hode.pFil = Bgr.pFil;
         Hode.lNr = 0L;
         /* Les filhode */
         LC_RxGr(&Hode,LES_OPTIMALT,&ngi,&nko,&info);

         /* Initier søk */
         pUtAdm->Gruppe.pAktU = pUtAdm->Gruppe.pForsteU;
         pUtAdm->sGruppeValgt = UT_FALSE;

         /* Søk */
         sFunnet = LU_GiTestLinje(pUtAdm,pUtAdm->Gruppe.pForsteU->pForsteUE,ulin,para);   
         *univ = LC_HODE;

         /* Les tilbake opprinnelig gruppe */
         LC_RxGr(&Bgr,LES_OPTIMALT,&ngi,&nko,&info);
      }
   }

   LC_CloseQuery(pUtAdm);
   return sFunnet;
}


/*
AR-920524
CH LU_LesULinje                       Les en utvalgs-linje fra kommandofilen
CD ==========================================================================
CD Formål:
CD Les en utvalgs-linje fra kommandofilen.
CD
CD Parameters:
CD Type     Name     I/O  Forklaring
CD ----------------------------------------------------------------
CD FILE    *pKomFil   i   Filpeker for beskrivelsesfil
CD short    sMaxTxLen i   Max lengde av pszTx
CD wchar_t    *pszTx     i   Lest linje
CD short   *psNiv     u   Nivå (antall prikker forran navnet)
CD short    lesefeil  r   Lesefeil fra UT_ReadLine.
CD
CD Bruk:
CD lesefeil = LU_LesULinje(pFil,sMaxTxLen,pszTx,&sNiv);
CD =============================================================================
*/
short CFyba::LU_LesULinje(FILE *pKomFil,short sMaxTxLen,wchar_t *pszTx,
                          short *psNiv)
{
   wchar_t szLinje[100],*cp;
   short lesefeil;

   if (!(lesefeil = UT_ReadLineNoComm(pKomFil,100,szLinje))) {

      cp = szLinje;

      /* Hopp over ledende blanke */
      while (UT_IsSpace(*cp)) {
         cp++;
      }

      /* Tell opp antall prikker */
      *psNiv = 0;
      while (*cp == '.') {
         (*psNiv)++;
         cp++;
      }
         
      /* Kopier ut linjen */
      UT_StrCopy(pszTx,cp,sMaxTxLen);
   }

   return lesefeil;
}


/*
AR-920524
CH LU_AppUtvalg                                      Legg til et nytt utvalg
CD ==========================================================================
CD Formål:
CD Legg til et nytt utvalg i en av kjedene av utvalg.
CD
CD Parametre:
CD Type              Navn     I/O  Forklaring
CD --------------------------------------------------------------------------
CD LC_UTVALG_BLOKK  *pUtBlokk  i   Toppblokk for aktuell utvalgstype.
CD wchar_t             *pszNavn   i   Utvalgsnavn
CD
CD Bruk:
CD LU_AppUtvalg(pUtAdm->pGruppe,szNavn);
CD =============================================================================
*/
void CFyba::LU_AppUtvalg (LC_UTVALG_BLOKK *pUB,wchar_t *pszNavn)
{
   LC_UTVALG * pU;

	/* Alloker minne og initier */
	pU = (LC_UTVALG *)malloc(sizeof(LC_UTVALG));
	memset(pU, 0, sizeof(LC_UTVALG));

	pU->sPrioritet = 0;
	pU->sOriginalPrioritet = 0;
   pU->sStatus = LC_UFORANDRET;
   pU->sTegnes = 1;
   pU->pszNavn = _wcsdup(pszNavn);  
	//pU->pszNavn = (wchar_t*)malloc((wcslen(pszNavn)+1) * sizeof(wchar_t));
	//UT_StrCopy(pU->pszNavn, pszNavn, wcslen(pszNavn)+1);
	pU->pszRegel = NULL;

	pU->pForsteUE = NULL;
   pU->pSisteUE = NULL;
   pU->pForrigeU = NULL;
   pU->pNesteU = NULL;

   /* Første */
   if (pUB->pForsteU == NULL) {
      pUB->pForsteU = pU;
      pUB->pSisteU = pU;

   } else {
      pU->pForrigeU = pUB->pSisteU;
      pUB->pSisteU->pNesteU = pU;
      pUB->pSisteU = pU;
   }

   return;
}


/*
AR-920402
CH LC_InqMaxPrioritet                                Hent største prioritet
CD ==========================================================================
CD Formål:
CD Henter største prioritet for gitt utvalgstype.
CD
CD Parametre:
CD Type         Navn          I/U   Forklaring
CD ----------------------------------------------------------------------
CD LC_UT_ADM *   pUA            i    Peker til administrasjonsblokk for utvalg.
CD short        sMaxPrioritet  r    Max prioritet
CD
CD Bruk:
CD sMaxPrioritet = LC_InqMaxPrioritet(pUA);
   ==========================================================================
*/
short CFyba::LC_InqMaxPrioritet(LC_UT_ADM * pUA)
{
   if (pUA != NULL)
   {
      return pUA->sMaxPrior;
   }

   return 0;
}


/*
AR-920814
CH LC_TestPrioritetBrukt                     Tester om en prioritet er brukt
CD ==========================================================================
CD Formål:
CD Tester om en prioritet er brukt.
CD
CD Parametre:
CD Type        Navn      I/U   Forklaring
CD --------------------------------------------------------------------------
CD LC_UT_ADM *  pUtAdm     i    Peker til administrasjonsblokk for utvalg.
CD short       sPrioritet i    Prioritet som skal testes.
CD short       sBrukt     r    Status: UT_TRUE=brukt, UT_FALSE=ikke brukt.
CD
CD Bruk:
CD sBrukt = LC_TestPrioritetBrukt(pUtAdm,sPrioritet);
   ==========================================================================
*/
short CFyba::LC_TestPrioritetBrukt(LC_UT_ADM * pUtAdm,short sPrioritet)
{
   LC_UTVALG * pU;


   if (pUtAdm != NULL)
   {
      /*
       * Test GRUPPE-UTVALG.
       */
      for (pU=pUtAdm->Gruppe.pForsteU; pU != NULL;  pU = pU->pNesteU) {
         if (sPrioritet == pU->sPrioritet)  return UT_TRUE;   /* ==> Funnet */
      }

      /*
       * Test PUNKT-UTVALG.
       */
      for (pU=pUtAdm->Punkt.pForsteU; pU != NULL;  pU = pU->pNesteU) {
         if (sPrioritet == pU->sPrioritet)  return UT_TRUE;   /* ==> Funnet */
      }

      /*
       * Test PINFO-UTVALG.
       */
      for (pU=pUtAdm->Pinfo.pForsteU; pU != NULL;  pU = pU->pNesteU) {
         if (sPrioritet == pU->sPrioritet)  return UT_TRUE;   /* ==> Funnet */
      }
   }

   return UT_FALSE;   /* ==> Ikke funnet */
}
 

/*
AR-940110
CH LU_PakkPrioritet                                    Pakker prioriteten
CD ==========================================================================
CD Formål:
CD Pakker prioriteten.
CD
CD Parametre:
CD Type        Navn      I/U   Forklaring
CD --------------------------------------------------------------------------
CD LC_UT_ADM *  pUtAdm     i    Peker til administrasjonsblokk for utvalg.
CD
CD Bruk:
CD LU_PakkPrioritet(pUtAdm);
   ==========================================================================
*/
void CFyba::LU_PakkPrioritet(LC_UT_ADM * pUtAdm)
{
   LC_UTVALG * pU;
   short s,sFerdig;
   short NyPrior[LC_MAX_ANT_PRIOR];
   short sAntNyPrior = 0;


   if (pUtAdm != NULL)
   {
      /*
       * Bygger opp tabell over hvilke prioriteter som er brukt.
       */

      /* GRUPPE-UTVALG. */
      for (pU=pUtAdm->Gruppe.pForsteU; pU != NULL;  pU = pU->pNesteU) {
         LU_HuskPrior(NyPrior,&sAntNyPrior,pU->sPrioritet);
      }
      /* PUNKT-UTVALG. */
      for (pU=pUtAdm->Punkt.pForsteU; pU != NULL;  pU = pU->pNesteU) {
         LU_HuskPrior(NyPrior,&sAntNyPrior,pU->sPrioritet);
      }
      /* PINFO-UTVALG. */
      for (pU=pUtAdm->Pinfo.pForsteU; pU != NULL;  pU = pU->pNesteU) {
         LU_HuskPrior(NyPrior,&sAntNyPrior,pU->sPrioritet);
      }

      /* Sorter prioritetene */
      qsort(NyPrior,sAntNyPrior,sizeof(short),LU_compare);


      /*
       * Legg inn nye prioriteter.
       */

      /* GRUPPE-UTVALG. */
      for (pU=pUtAdm->Gruppe.pForsteU; pU != NULL;  pU = pU->pNesteU) {
         sFerdig = UT_FALSE;
         for (s=0; sFerdig==UT_FALSE && s<sAntNyPrior; s++) {
            if (NyPrior[s] == pU->sPrioritet) {
               pU->sPrioritet = s;
               sFerdig = UT_TRUE;
            }
         }
      }
      /* PUNKT-UTVALG. */
      for (pU=pUtAdm->Punkt.pForsteU; pU != NULL;  pU = pU->pNesteU) {
         sFerdig = UT_FALSE;
         for (s=0; sFerdig==UT_FALSE && s<sAntNyPrior; s++) {
            if (NyPrior[s] == pU->sPrioritet) {
               pU->sPrioritet = s;
               sFerdig = UT_TRUE;
            }
         }
      }
      /* PINFO-UTVALG. */
      for (pU=pUtAdm->Pinfo.pForsteU; pU != NULL;  pU = pU->pNesteU) {
         sFerdig = UT_FALSE;
         for (s=0; sFerdig==UT_FALSE && s<sAntNyPrior; s++) {
            if (NyPrior[s] == pU->sPrioritet) {
               pU->sPrioritet = s;
               sFerdig = UT_TRUE;
            }
         }
      }

      /* Oppdaterer max prioritet */
      pUtAdm->sMaxPrior = sAntNyPrior-1;
   }

   return;
}
 

/* ===================================================== */
int LU_compare (const void *arg1, const void *arg2)
 {
    if (*(short*)arg1 == *(short*)arg2)  return  0;
    if (*(short*)arg1 < *(short*)arg2)   return -1;
    return 1;
 }


/*
AR-940110
CH LU_HuskPrior                          Legg inn prioritet i tabellen
CD ==========================================================================
CD Formål:
CD Legg inn prioritet i tabellen
CD
CD Parametre:
CD Type        Navn   I/U   Forklaring
CD --------------------------------------------------------------------------
CD short *NyPrior     iu   Tabell over nye prioriteter
CD short *sAntPrior   iu   Antall brukt i NyPrior
CD short  sPrior      i    Prioritet som skal legges inn.
CD
CD
CD Bruk:
CD LU_HuskPrior(NyPrior,&sAntPrior,pU->sPrioritet);
   ==========================================================================
*/
void CFyba::LU_HuskPrior(short *NyPrior,short *sAntPrior,short sPrior)
{
   short s;
   wchar_t szTx[10];

   for (s=0; s<*sAntPrior; s++) {
      if (sPrior == NyPrior[s]) {
         return;        /* ==> Prioriteten er brukt før, returner */
      }
   }

   if (*sAntPrior >= LC_MAX_ANT_PRIOR) {
      UT_SNPRINTF(szTx,10,L"%hd",sPrior);
      LC_Error(127,L"(LU_HuskPrior)",szTx);

   } else {
      /* Ny prioritet, husk denne */
      NyPrior[*sAntPrior] = sPrior;
      (*sAntPrior)++;
   }

   return;
}


/*
AR: 1999-11-20
CH LC_LoggPrioritetUtvalg                        Skriver prioritetsoversikt 
CD ==========================================================================
CD Formål:
CD Skriver oversikt over prioriteter og utvalg til log-filen.
CD
CD Parametre:
CD Type        Navn      I/U   Forklaring
CD --------------------------------------------------------------------------
CD LC_UT_ADM *  pUtAdm     i    Peker til administrasjonsblokk for utvalg.
CD
CD Bruk:
CD LC_LoggPrioritetUtvalg(pUtAdm);
   ==========================================================================
*/
void CFyba::LC_LoggPrioritetUtvalg(LC_UT_ADM * pUtAdm)
{
   if (pUtAdm != NULL)
   {
      short sMaxPrioritet = LC_InqMaxPrioritet(pUtAdm);
      short sPrioritet;
      LC_UTVALG * pU;
      short sG = 0;
      short sP = 0;

      UT_FPRINTF(stderr,L"\n\n***** Prioriteter og utvalg *****\n");

      for (sPrioritet=0; sPrioritet<sMaxPrioritet; sPrioritet++) {
    
         UT_FPRINTF(stderr,L"\nPrioritet: %hd\n",sPrioritet);

         /* GRUPPE-UTVALG. */
         for (pU=pUtAdm->Gruppe.pForsteU; pU != NULL;  pU = pU->pNesteU) {
            if (sPrioritet == pU->sPrioritet) {
               UT_FPRINTF(stderr,L"   Gruppeutvalg: %s (%hd) \n",pU->pszNavn ,pU->sOriginalPrioritet);
               sG++;
            }
         }

         /* PUNKT-UTVALG. */
         for (pU=pUtAdm->Punkt.pForsteU; pU != NULL;  pU = pU->pNesteU) {
            if (sPrioritet == pU->sPrioritet) {
               UT_FPRINTF(stderr,L"   Punktutvalg: %s (%hd) \n",pU->pszNavn ,pU->sOriginalPrioritet);
               sP++;
            }
         }

         /* PINFO-UTVALG. */
   #ifdef TEST
         for (pU=pUtAdm->Pinfo.pForsteU; pU != NULL;  pU = pU->pNesteU) {
            if (sPrioritet == pU->sPrioritet) {
               UT_FPRINTF(stderr,L"   Pinfoutvalg: %s (%hd) \n",pU->pszNavn ,pU->sOriginalPrioritet);
            }
         }
   #endif
      }

      UT_FPRINTF(stderr,L"\nTotalt  %hd gruppeutvalg, og  %hd punktutvalg.\n",sG,sP);
   }
}


/*
AR-940110
CH LC_UtvalgPrioritet                                   Finn brukt prioritet
CD ==========================================================================
CD Formål:
CD Sjekker GINFO og PINFO for å finne hvilke prioriteter som "berører" aktuell
CD gruppe. Resultatet markeres i Gruppetabellen ulPrior.
CD
CD Parametre:
CD Type           Navn       I/U Forklaring
CD ---------------------------------------------------------------------------
CD LC_UT_ADM     *pUtAdm      i  Peker til administrasjonsblokk for utvalg.
CD
CD Bruk:
CD LC_UtvalgPrioritet(pUtAdm);
   ===========================================================================
*/
void CFyba::LC_UtvalgPrioritet(LC_UT_ADM *pUtAdm)
{
   long lPnr;
   short sPrior,sKolonne;
   LC_UTVALG * pU;
   unsigned long ulPrioritet[4];


   if (pUtAdm != NULL)
   {
      ulPrioritet[0] = 0x00000000UL;
      ulPrioritet[1] = 0x00000000UL;
      ulPrioritet[2] = 0x00000000UL;
      ulPrioritet[3] = 0x80000000UL; /* Info er bygd opp */

      if (Sys.GrId.lNr != INGEN_GRUPPE) {

         /*
          * GINFO
          */
        
         /* Initier søk */
         pU = pUtAdm->Gruppe.pForsteU;
         pUtAdm->sGruppeValgt = UT_FALSE;
        
         /* Søk */
         while (pU != NULL) {
        
            sKolonne = pU->sPrioritet / 32;
            sPrior = pU->sPrioritet % 32;

            /* Hvis vi har tilslag på denne prioriteten trenger vi ikke å sjekke dette utvalget */
            if ((ulPrioritet[sKolonne] & (0x1UL << sPrior)) == 0UL) {
        
               if (LU_GiTestUtvalg(pUtAdm,pU)) {
                  /* Tilslag */
                  pUtAdm->sGruppeValgt = UT_TRUE;
            
                  /* Husk at prioriteten er brukt */
                  ulPrioritet[sKolonne]  |=  (0x1UL << sPrior);
               }
            }
        
            /* Fortsett med neste utvalg */
            pU = pU->pNesteU;
         }
        
        
         /*
          * PINFO
          *
          * Sjekker bare hvis :
          *   - utvalgsmetode "!" (ikke) er brukt,
          *   - gruppen har PINFO
          *   - gruppen har KP
          *   - gruppen har høyde
          */
    
         if (pUtAdm->Punkt.sTestAllePi == UT_TRUE  ||
             Sys.pGrInfo->info & GI_PINFO  ||
             Sys.pGrInfo->info & GI_KP  ||
             Sys.pGrInfo->info & GI_NAH) {

           for (lPnr=1; lPnr<=Sys.pGrInfo->nko; ++lPnr) {
               /* Initier søk */
               /* Utvalgsmetode "!" (ikke) er brukt, eller
                  punktet har PINFO mm. */
               if (pUtAdm->Punkt.sTestAllePi == UT_TRUE  ||
                   LC_TestPi(lPnr,pUtAdm->Punkt.sHoydeBrukt)) {
                  pU = pUtAdm->Punkt.pForsteU;
               } else {
                  pU = NULL;
               }
           
               /* Søk */
               while (pU != NULL) {

                  sKolonne = pU->sPrioritet / 32;
                  sPrior = pU->sPrioritet % 32;
           
                  /* Hvis vi har tilslag på denne prioriteten trenger vi ikke å sjekke dette utvalget */
                  if ((ulPrioritet[sKolonne] & (0x1UL << sPrior)) == 0UL) {
           
                     if (LC_PiTestUtvalg(pUtAdm,pU,lPnr)) {
                        /* Tilslag */
                        /* Husk at prioriteten er brukt */
                        ulPrioritet[sKolonne]  |=  (0x1UL << sPrior);
                     }
                  }
           
                  /* Fortsett med neste utvalg */
                  pU = pU->pNesteU;
               }
            }
         }

         Sys.pGrInfo->ulPrior[0] = ulPrioritet[0];
         Sys.pGrInfo->ulPrior[1] = ulPrioritet[1];
         Sys.pGrInfo->ulPrior[2] = ulPrioritet[2];
         Sys.pGrInfo->ulPrior[3] = ulPrioritet[3];
      }
   }
}



/*
AR:2007-08-23
CH LC_ErReferert                                 Sjekk om gruppe er referert
CD ==========================================================================
CD Formål:
CD Sjekker om aktuell gruppe er referert fra andre grupper.
CD
CD Parametre:
CD Type  Navn      I/U Forklaring
CD ---------------------------------------------------------------------------
CD bool  bReferert  r  Det finnes referanser til gruppen
CD
CD Bruk:
CD bReferert = LC_ErReferert();
===========================================================================
*/
bool CFyba::LC_ErReferert(void)
{
   LC_BGR FlateBgr;
   double a,n;
   LC_GEO_STATUS GeoStat;
   short ngi;
   long nko;
   unsigned short info;
   long lAntRef;
   short sGiLin,sRefPos;
   long *plRefArray,*plRef;
   long l;

   bool bReferert = false;

   /* Husk gruppen */
   LC_FILADM *pFil = Sys.GrId.pFil;
   LC_BGR Bgr = Sys.GrId;
   long lGmlSnr = LC_GetSn();

   LC_GetTK(1,&a,&n);
   LC_SBFlate(&GeoStat,LC_FRAMGR,a-0.1,n-0.1,a+0.1,n+0.1);
   if (LC_FFFlate(&GeoStat,&FlateBgr)) {
      do {
         if (FlateBgr.pFil == pFil) {
            /* Funnet flate i rett fil, sjekk referansene */
            LC_RxGr(&FlateBgr,LES_OPTIMALT,&ngi,&nko,&info);
            lAntRef = LC_InqAntRef();
            plRefArray = (long *) malloc(lAntRef * sizeof(long));
            sGiLin = 2;
            sRefPos = 0;
            LC_GetRef(plRefArray,lAntRef,&sGiLin,&sRefPos);

            plRef = plRefArray;
            for (l=0; (!bReferert) && l<lAntRef; l++) {
               if (labs(*plRef) == lGmlSnr) {
                  bReferert = true;
               } 
               ++plRef;
            }

            free(plRefArray);
         }
      } while ((!bReferert)  &&  LC_FNFlate(&GeoStat,&FlateBgr));
   }
   LC_AvsluttSok(&GeoStat);

   // Leser inn opprinnelig gruppe igjen
   LC_RxGr(&Bgr,LES_OPTIMALT,&ngi,&nko,&info);

   return bReferert;
}


/*
AR:2009-04-28
CH LC_ErReferertFraAntall                  Tell antall referanser til gruppe
CD ==========================================================================
CD Formål:
CD Tell antall referanser til aktuell gruppe.
CD
CD Parametre:
CD Type  Navn    I/U Forklaring
CD ---------------------------------------------------------------------------
CD long  lAntall  r  Antall grupper som refererer gruppen
CD
CD Bruk:
CD lAntall = LC_ErReferertFraAntall();
===========================================================================
*/
long CFyba::LC_ErReferertFraAntall(void)
{
   LC_BGR FlateBgr;
   double a,n;
   LC_GEO_STATUS GeoStat;
   short ngi;
   long nko;
   unsigned short info;
   long lAntRef;
   short sGiLin,sRefPos;
   long *plRefArray,*plRef;
   long l;

   long lAntall = 0;

   /* Husk gruppen */
   LC_FILADM *pFil = Sys.GrId.pFil;
   LC_BGR Bgr = Sys.GrId;
   long lGmlSnr = LC_GetSn();

   LC_GetTK(1,&a,&n);
   LC_SBFlate(&GeoStat,LC_FRAMGR,a-0.1,n-0.1,a+0.1,n+0.1);
   if (LC_FFFlate(&GeoStat,&FlateBgr)) {
      do {
         if (FlateBgr.pFil == pFil) {
            /* Funnet flate i rett fil, sjekk referansene */
            LC_RxGr(&FlateBgr,LES_OPTIMALT,&ngi,&nko,&info);
            lAntRef = LC_InqAntRef();
            plRefArray = (long *) malloc(lAntRef * sizeof(long));
            sGiLin = 2;
            sRefPos = 0;
            LC_GetRef(plRefArray,lAntRef,&sGiLin,&sRefPos);

            plRef = plRefArray;
            for (l=0; l<lAntRef; l++) {
               if (labs(*plRef) == lGmlSnr) {
                  ++lAntall;
               } 
               ++plRef;
            }

            free(plRefArray);
         }
      } while (LC_FNFlate(&GeoStat,&FlateBgr));
   }
   LC_AvsluttSok(&GeoStat);

   // Leser inn opprinnelig gruppe igjen
   LC_RxGr(&Bgr,LES_OPTIMALT,&ngi,&nko,&info);

   return lAntall;
}


/*
AR:2011-11-30
CH LC_ErReferertFraReadOnly         Sjekk om gruppe er referert fra ReadOnly
CD ==========================================================================
CD Formål:
CD Sjekker om aktuell gruppe er referert fra andre grupper (FLATE eller TRASE)
CD som er ReadOnly. Dette er vanligvis NGIS-grupper som er låst av andre.
CD
CD Parametre:
CD Type  Navn      I/U Forklaring
CD ---------------------------------------------------------------------------
CD bool  bReferert  r  Aktuell gruppe er referert fra gruppe som er ReadOnly
CD
CD Bruk:
CD bReferert = LC_ErReferertFraReadOnly();
===========================================================================
*/
bool CFyba::LC_ErReferertFraReadOnly(void)
{
   LC_BGR FlateBgr;
   double a,n;
   LC_GEO_STATUS GeoStat;
   short ngi;
   long nko;
   unsigned short info;
   long lAntRef;
   short sGiLin,sRefPos;
   long *plRefArray,*plRef;
   long l;

   bool bReferert = false;

   /* Husk gruppen */
   LC_FILADM *pFil = Sys.GrId.pFil;
   LC_BGR Bgr = Sys.GrId;
   long lGmlSnr = LC_GetSn();

   LC_GetTK(1,&a,&n);
   LC_SBFlate(&GeoStat,LC_FRAMGR,a-0.1,n-0.1,a+0.1,n+0.1);
   if (LC_FFFlate(&GeoStat,&FlateBgr))
   {
      do 
      {
         // Funnet flate i rett fil
         if (FlateBgr.pFil == pFil)
         {
            // Er flaten ReadOnly?
            LC_GetGrParaBgr(&FlateBgr,&ngi,&nko,&info);
            if ( info & GI_READ_ONLY)
            {
               // Sjekk referansene om den aktuelle gruppen er referert 
               LC_RxGr(&FlateBgr,LES_OPTIMALT,&ngi,&nko,&info);
               lAntRef = LC_InqAntRef();
               plRefArray = (long *) malloc(lAntRef * sizeof(long));
               sGiLin = 2;
               sRefPos = 0;
               LC_GetRef(plRefArray,lAntRef,&sGiLin,&sRefPos);

               plRef = plRefArray;
               for (l=0; (!bReferert) && l<lAntRef; l++)
               {
                  // Er 
                  if (labs(*plRef) == lGmlSnr)
                  {
                     bReferert = true;
                  } 
                  ++plRef;
               }

               free(plRefArray);
            }
         }
      } while ((!bReferert)  &&  LC_FNFlate(&GeoStat,&FlateBgr));
   }

   LC_AvsluttSok(&GeoStat);

   // Leser inn opprinnelig gruppe igjen
   LC_RxGr(&Bgr,LES_OPTIMALT,&ngi,&nko,&info);

   return bReferert;
}
