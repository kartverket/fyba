/* == AR 910819 ========================================== */
/*  KARTVERKET  -  FYSAK-PC                          */
/*  Fil: fyln.c                                            */
/*  Innhold: Navnesystem for fysak-pc                      */
/* ======================================================= */

#include "stdafx.h"

#include <fcntl.h>
#include <ctype.h>
#include <limits.h>


/*
AR-890616
CH LN_InitTab                                           Klargjør navnetabell
CD ==========================================================================
CD Formål:
CD Initierer navnetabellen med kjente navn.
CD
CD Parametre:
CD Type             Navn I/U  Forklaring
CD --------------------------------------------------------------------------
CD LC_NAVNETABELL *  pLn   i   Peker til navnetabell
CD
CD Bruk:
CD LN_InitTab(pLn);
   ==========================================================================
*/
void CFyba::LN_InitTab(LC_NAVNETABELL * pLn)
{
   UT_StrCopy(pLn->sosi[L_SLUTT].szNavn, L".SLUTT", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_SLUTT].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_SLUTT].ucNivo = 1;
   pLn->sosi[L_SLUTT].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_PUNKT].szNavn, L".PUNKT", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_PUNKT].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_PUNKT].ucNivo = 1;
   pLn->sosi[L_PUNKT].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_LINJE].szNavn, L".LINJE", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_LINJE].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_LINJE].ucNivo = 1;
   pLn->sosi[L_LINJE].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_KURVE].szNavn, L".KURVE", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_KURVE].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_KURVE].ucNivo = 1;
   pLn->sosi[L_KURVE].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_BUE].szNavn, L".BUE", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_BUE].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_BUE].ucNivo = 1;
   pLn->sosi[L_BUE].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_BUEP].szNavn, L".BUEP", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_BUEP].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_BUEP].ucNivo = 1;
   pLn->sosi[L_BUEP].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_SIRKEL].szNavn, L".SIRKEL", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_SIRKEL].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_SIRKEL].ucNivo = 1;
   pLn->sosi[L_SIRKEL].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_SIRKELP].szNavn, L".SIRKELP", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_SIRKELP].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_SIRKELP].ucNivo = 1;
   pLn->sosi[L_SIRKELP].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_KLOTOIDE].szNavn, L".KLOTOIDE", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_KLOTOIDE].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_KLOTOIDE].ucNivo = 1;
   pLn->sosi[L_KLOTOIDE].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_BEZIER].szNavn, L".BEZIER", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_BEZIER].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_BEZIER].ucNivo = 1;
   pLn->sosi[L_BEZIER].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_RASTER].szNavn, L".RASTER", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_RASTER].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_RASTER].ucNivo = 1;
   pLn->sosi[L_RASTER].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_TEKST].szNavn, L".TEKST", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_TEKST].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_TEKST].ucNivo = 1;
   pLn->sosi[L_TEKST].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_TRASE].szNavn, L".TRASE", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_TRASE].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_TRASE].ucNivo = 1;
   pLn->sosi[L_TRASE].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_FLATE].szNavn, L".FLATE", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_FLATE].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_FLATE].ucNivo = 1;
   pLn->sosi[L_FLATE].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_SVERM].szNavn, L".SVERM", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_SVERM].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_SVERM].ucNivo = 1;
   pLn->sosi[L_SVERM].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_DEF].szNavn, L".DEF", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_DEF].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_DEF].ucNivo = 1;
   pLn->sosi[L_DEF].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_OBJDEF].szNavn, L".OBJDEF", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_OBJDEF].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_OBJDEF].ucNivo = 1;
   pLn->sosi[L_OBJDEF].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_MLINJE].szNavn, L".MLINJE", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_MLINJE].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_MLINJE].ucNivo = 1;
   pLn->sosi[L_MLINJE].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_STRUKTUR].szNavn, L".STRUKTUR", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_STRUKTUR].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_STRUKTUR].ucNivo = 1;
   pLn->sosi[L_STRUKTUR].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_OBJEKT].szNavn, L".OBJEKT", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_OBJEKT].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_OBJEKT].ucNivo = 1;
   pLn->sosi[L_OBJEKT].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_SYMBOL].szNavn, L".SYMBOL", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_SYMBOL].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_SYMBOL].ucNivo = 1;
   pLn->sosi[L_SYMBOL].bBrukt = false;
   
   UT_StrCopy(pLn->sosi[L_HODE].szNavn, L".HODE", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_HODE].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_HODE].ucNivo = 1;
   pLn->sosi[L_HODE].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_NA].szNavn, L"..NØ", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_NA].ucAntPar = 2;
   pLn->sosi[L_NA].ucNivo = 2;
   pLn->sosi[L_NA].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_NAH].szNavn, L"..NØH", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_NAH].ucAntPar = 3;
   pLn->sosi[L_NAH].ucNivo = 2;
   pLn->sosi[L_NAH].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_NAD].szNavn, L"..NØD", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_NAD].ucAntPar = 3;
   pLn->sosi[L_NAD].ucNivo = 2;
   pLn->sosi[L_NAD].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_REF1].szNavn, L"..", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_REF1].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_REF1].ucNivo = 2;
   pLn->sosi[L_REF1].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_REF2].szNavn, L"..REF", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_REF2].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_REF2].ucNivo = 2;
   pLn->sosi[L_REF2].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_RADIUS].szNavn, L"..RADIUS", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_RADIUS].ucAntPar = 1;
   pLn->sosi[L_RADIUS].ucNivo = 2;
   pLn->sosi[L_RADIUS].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_ENHET2].szNavn, L"..ENHET", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_ENHET2].ucAntPar = 1;
   pLn->sosi[L_ENHET2].ucNivo = 2;
   pLn->sosi[L_ENHET2].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_ENHET2H].szNavn, L"..ENHET-H", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_ENHET2H].ucAntPar = 1;
   pLn->sosi[L_ENHET2H].ucNivo = 2;
   pLn->sosi[L_ENHET2H].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_ENHET2D].szNavn, L"..ENHET-D", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_ENHET2D].ucAntPar = 1;
   pLn->sosi[L_ENHET2D].ucNivo = 2;
   pLn->sosi[L_ENHET2D].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_ENHET3].szNavn, L"...ENHET", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_ENHET3].ucAntPar = 1;
   pLn->sosi[L_ENHET3].ucNivo = 3;
   pLn->sosi[L_ENHET3].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_ENHET3H].szNavn, L"...ENHET-H", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_ENHET3H].ucAntPar = 1;
   pLn->sosi[L_ENHET3H].ucNivo = 3;
   pLn->sosi[L_ENHET3H].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_ENHET3D].szNavn, L"...ENHET-D", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_ENHET3D].ucAntPar = 1;
   pLn->sosi[L_ENHET3D].ucNivo = 3;
   pLn->sosi[L_ENHET3D].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_ORIGONO].szNavn, L"...ORIGO-NØ", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_ORIGONO].ucAntPar = 2;
   pLn->sosi[L_ORIGONO].ucNivo = 3;
   pLn->sosi[L_ORIGONO].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_HOYDE].szNavn, L"..HØYDE", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_HOYDE].ucAntPar = 1;
   pLn->sosi[L_HOYDE].ucNivo = 2;
   pLn->sosi[L_HOYDE].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_DYBDE].szNavn, L"..DYBDE", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_DYBDE].ucAntPar = 1;                                   
   pLn->sosi[L_DYBDE].ucNivo = 2;
   pLn->sosi[L_DYBDE].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_NGISFLAGG].szNavn, L"..NGIS-FLAGG", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_NGISFLAGG].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_NGISFLAGG].ucNivo = 2;
   pLn->sosi[L_NGISFLAGG].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_NGISLAG].szNavn, L"..NGIS-LAG", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_NGISLAG].ucAntPar = LC_ANT_PAR_UKJENT;
   pLn->sosi[L_NGISLAG].ucNivo = 2;
   pLn->sosi[L_NGISLAG].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_OBJTYPE].szNavn, L"..OBJTYPE", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_OBJTYPE].ucAntPar = 1;
   pLn->sosi[L_OBJTYPE].ucNivo = 2;
   pLn->sosi[L_OBJTYPE].bBrukt = false;

   UT_StrCopy(pLn->sosi[L_KP].szNavn, L"...KP", LC_MAX_SOSINAVN_LEN);
   pLn->sosi[L_KP].ucAntPar = 1;
   pLn->sosi[L_KP].ucNivo = 3;
   pLn->sosi[L_KP].bBrukt = false;

   pLn->sAntNavn = L_KP + 1;           /* Antall navn i navnetabellen */
}


/*
AR-910919
CH LN_Enhet                                          Sjekk om det er ..ENHET
CD ==========================================================================
CD Formål:
CD Sjekk om denne ginfo-linjen er ..ENHET.
CD
CD Parametre:
CD Type             Navn        I/U  Forklaring
CD --------------------------------------------------------------------------
CD LC_NAVNETABELL *  pLn          i   Peker til navnetabell
CD wchar_t            *ginfo_linje  i   Første pos i linjen
CD short            ok           r   1=linjen er ..ENHET, 0=ikke ..ENHET
CD
CD Bruk:
CD ok = LN_Enhet(pLn,ginfo_linje);
  ===========================================================================
*/
short CFyba::LN_Enhet(LC_NAVNETABELL * pLn,wchar_t *ginfo_linje)
{
   wchar_t ord[LC_MAX_SOSINAVN_LEN];

   //JAØ-20000313
   //Leter etter "..ENHET " istedet for "..ENHET" for ikke å få tilslag på ..ENHET-H eller ..ENHET-D
   UT_StrCopy(ord,pLn->sosi[L_ENHET2].szNavn,LC_MAX_SOSINAVN_LEN);
   UT_StrCat(ord, L" ", LC_MAX_SOSINAVN_LEN);

   return(wcsstr(ginfo_linje,ord) != NULL);
   //return(wcsstr(ginfo_linje,pLn->sosi[L_ENHET2].szNavn) != NULL);
}


/*
AR-940704
CH LN_EnhetHoyde                                   Sjekk om det er ..ENHET-H
CD ==========================================================================
CD Formål:
CD Sjekk om denne ginfo-linjen er ..ENHET-H.
CD
CD Parametre:
CD Type             Navn        I/U  Forklaring
CD --------------------------------------------------------------------------
CD LC_NAVNETABELL *  pLn          i   Peker til navnetabell
CD wchar_t            *ginfo_linje  i   Første pos i linjen
CD short            ok           r   1=linjen er ..ENHET-H, 0=ikke ..ENHET-H
CD
CD Bruk:
CD ok = LN_EnhetHoyde(pLn,ginfo_linje);
  ===========================================================================
*/
short CFyba::LN_EnhetHoyde(LC_NAVNETABELL * pLn,wchar_t *ginfo_linje)
{
   wchar_t ord[LC_MAX_SOSINAVN_LEN];

   // Leter etter "..ENHET-H " istedet for "..ENHET-H" for ikke å få tilslag på andre navn
   UT_StrCopy(ord,pLn->sosi[L_ENHET2H].szNavn,LC_MAX_SOSINAVN_LEN);
   UT_StrCat(ord, L" ", LC_MAX_SOSINAVN_LEN);
                                         
   //return(wcsstr(ginfo_linje,pLn->sosi[L_ENHET2H].szNavn) != NULL);
   return(wcsstr(ginfo_linje,ord) != NULL);
}


/*
AR-940704
CH LN_EnhetDybde                                    Sjekk om det er ..ENHET-D
CD ==========================================================================
CD Formål:
CD Sjekk om denne ginfo-linjen er ..ENHET-D.
CD
CD Parametre:
CD Type             Navn        I/U  Forklaring
CD --------------------------------------------------------------------------
CD LC_NAVNETABELL *  pLn          i   Peker til navnetabell
CD wchar_t            *ginfo_linje  i   Første pos i linjen
CD short            ok           r   1=linjen er ..ENHET-D, 0=ikke ..ENHET-D
CD
CD Bruk:
CD ok = LN_EnhetDybde(pLn,ginfo_linje);
  ===========================================================================
*/
short CFyba::LN_EnhetDybde(LC_NAVNETABELL * pLn,wchar_t *ginfo_linje)
{
   wchar_t ord[LC_MAX_SOSINAVN_LEN];

   // Leter etter "..ENHET-D " istedet for "..ENHET-D" for ikke å få tilslag på andre navn
   UT_StrCopy(ord,pLn->sosi[L_ENHET2D].szNavn,LC_MAX_SOSINAVN_LEN);
   UT_StrCat(ord, L" ",LC_MAX_SOSINAVN_LEN);

   //return(wcsstr(ginfo_linje,pLn->sosi[L_ENHET2D].szNavn) != NULL);
   return(wcsstr(ginfo_linje,ord) != NULL);
}


/*
AR-910315
CH LN_TestOy                               Sjekk om referansen inneholder øy
CD ==========================================================================
CD Formål:
CD Sjekk om denne ginfo-linjen inneholder referanse med ØY.
CD Forutsetter at aktuell linje inneholder referanser. 
CD
CD Parametre:
CD Type     Navn        I/U  Forklaring
CD -----------------------------------------------------------------------
CD wchar_t    *ginfo_linje  i   Første pos i linjen
CD short    ok           r   1=linjen har øy-flate, 0=ikke øy
CD
CD Bruk:
CD ok = LN_TestOy(wchar_t *ginfo_linje);
  ===========================================================================
*/
short CFyba::LN_TestOy(wchar_t *ginfo_linje)
{
   if (wcschr(ginfo_linje,L'(') != NULL)  return 1;

   return 0;
}


/*
AR-910918
CH LN_FinnNavn                          Søk etter et SOSI-navn i navnetabelen
CD =============================================================================
CD Formål:
CD Søker etter navnet i navnetabellen.
CD (Ukjent navn blir ikke lagt inn i navnetabellen.)
CD
CD Parametre:
CD Type             Navn    I/U   Forklaring
CD -----------------------------------------------------------------------------
CD LC_NAVNETABELL *  pLn      i    Peker til navnetabell
CD wchar_t            *streng   i    Peker til SOSI-navn. Avslutta av '\0'.
CD short           *navn_nr  u    Navnets linjenummer i navnetabellen
CD short            nivo     r    Antall prikker (0=ukjent navn, 1=gruppenavn, osv.)
CD
CD Bruk:
CD nivo = LN_FinnNavn(pLn,streng,&neste,&navn_nr);
   =============================================================================
*/
short CFyba::LN_FinnNavn(LC_NAVNETABELL * pLn,wchar_t *navn,short *navn_nr)
{
   short nr = 0;

                                    /* Utfør søket */
   for (; nr < pLn->sAntNavn; nr++)
   {
      if (wcsncmp(navn,pLn->sosi[nr].szNavn,LC_MAX_SOSINAVN_LEN-1) == 0)
      {
         // Legger inn merke om at navnet er brukt
         pLn->sosi[nr].bBrukt = true;

         // Navnet er funnet, ==> returner
         *navn_nr = nr;
         return  (pLn->sosi[nr].ucNivo);     /* Nivå */
      }
   }
   
   return 0;                         /* Ukjent navn */
}


/*
AR-910710
CH LN_PakkNavn                          Søk etter navn, legg inn ukjent navn
CD ==========================================================================
CD Formål:
CD Finner et SOSI-navn i navnetabellen.  (Max LC_MAX_SOSINAVN_LEN tegn.)
CD Hvis navnet er ukjent, blir det lagt inn i tabellen.
CD
CD Parametre:
CD Type            Navn    I/U Forklaring
CD --------------------------------------------------------------------------
CD LC_NAVNETABELL *  pLn     i   Peker til navnetabell
CD wchar_t            *navn    i  SOSI-navn.
CD short           *ant_par u  Antall parametre til dette navnet.
CD                             LC_ANT_PAR_UKJENT (-1) = Ant. param. er ukjent.
CD short           *navn_nr u  Navnenummer.
CD short            nivo    r  Antall prikker i navnet (1=gruppenavn, osv.)
CD
CD Bruk:
CD type = LN_PakkNavn(pLn,navn,&navn_nr,&ant_par);
   =============================================================================
*/
short CFyba::LN_PakkNavn (LC_NAVNETABELL * pLn,wchar_t *navn,short *navn_nr,short *ant_par)
{
   unsigned char nivo;
   wchar_t *cp;
   short nr;
                                    /* Utfør søket */
   for (nr=0; nr < pLn->sAntNavn; nr++) 
   {
      if (wcscmp(navn,pLn->sosi[nr].szNavn) == 0)
      {
         // Legger inn merke om at navnet er brukt
         pLn->sosi[nr].bBrukt = true;

         // Navnet er funnet, ==> returner
         *ant_par = pLn->sosi[nr].ucAntPar;
         *navn_nr = nr;
         return  (pLn->sosi[nr].ucNivo);     /* Nivå */
      }
   }

   /* Er det plass i tabellen for et nytt navn? */
   if (pLn->sAntNavn >= LC_MAX_NAVN) {
      UT_FPRINTF(stderr,L"Utskrift av navnetabellen:\n");
      for (nr=0; nr < pLn->sAntNavn; nr++) {
         UT_FPRINTF(stderr,L"%s\n",LN_VisNavn(pLn,nr));
      }
      LC_Error(21,L"(LN_PakkNavn)",L"");
      exit (2);
   }   

   /* ----- Nytt navn */
   UT_StrCopy(pLn->sosi[pLn->sAntNavn].szNavn,navn,LC_MAX_SOSINAVN_LEN);
   pLn->sosi[pLn->sAntNavn].ucAntPar = (unsigned char) LC_ANT_PAR_UKJENT;
   *ant_par = LC_ANT_PAR_UKJENT;
   
   /* Finn antall prikker */
   nivo = 0;
   cp = navn;
   while (*cp == L'.') {
      nivo++;
      cp++;
   }
   pLn->sosi[pLn->sAntNavn].ucNivo = nivo;

   // Legger inn merke om at navnet er brukt
   pLn->sosi[pLn->sAntNavn].bBrukt = true;

   *navn_nr = pLn->sAntNavn;

   pLn->sAntNavn++;

   /* Melding om ulovlig gruppestart */
   if (nivo <= 1) {
      LC_Error(22,L"(LN_PakkNavn)",navn);
   }

   return (nivo);     /* Antall prikker */
}


/*
AR-910819
CH LN_GetNavn                               Hent en linje fra  navnetabellen
CD ==========================================================================
CD Formål:
CD Henter et navn fra navnetabellen.
CD
CD Parametre:
CD Type             Navn  I/U   Forklaring
CD --------------------------------------------------------------------------
CD LC_NAVNETABELL *  pLn    i    Peker til navnetabell
CD long             navn   i    Linjenummer i navnetabellen
CD wchar_t            *tx     r    Peker til SOSI-navn, NULL=ukjent linje
CD
CD Bruk:
CD cp = LN_GetNavn(pLn,navn_nr);
   =============================================================================
*/
wchar_t * CFyba::LN_GetNavn(LC_NAVNETABELL * pLn,short navn)
{
   SOSINAVN *ip;

   if (navn >= 0  &&  navn < pLn->sAntNavn){
       ip = pLn->sosi + navn;
       return ip->szNavn;
   }

   return NULL;
}


/*
AR:2009-05-05
CH LC_GetElementNavn                                        Hent elementnavn
CD ==========================================================================
CD Formål:
CD Hent et elementnavn fra den interne navnetabellen i FYBA.
CD Denne tabellen inneholder både gruppenavn (.LINJE, .KURVE, ...) og
CD egenskapsnavn (..OBJTYPE, ..LTEMA, ...)
CD
CD Tabellen har tre logiske deler:
CD  - (Linje 0 - L_HODE): Forhåndsdefinerte gruppenavn.
CD  - (Linje L_HODE+1 - L_KP): Forhåndsdefinerte egenskapsnavn.
CD  - (Linje L_KP+1 - n): Andre elementnavn brukt i SOSI-filen etter
CD                        indeksoppbygging.
CD
CD Selv om egenskapen blir fjernet fra SOSI-filen blir navnet fortsatt
CD liggende i navnetabellen
CD
CD Parametre:
CD Type        Navn    I/U  Forklaring
CD --------------------------------------------------------------------------
CD LC_FILADM  *pFil     i   Peker til FilAdm
CD short       sNavnNr  i   Linjenummer i navnetabellen (0 - n)
CD bool       *bBrukt       Viser om navnet har/er brukt i filen
CD                          Hvis det har vært en gruppe som har brukt navnet blir
CD                          denne stående "true" selv om gruppen er slettet.
CD const wchar_t *pszNavn  r   Peker til elementnavn, 
CD                          NULL = ukjent fil eller ulovlig linjenummer
CD
CD
CD Bruk:
CD // Går gjennom alle navnene ut over de forhåndsdefinerte navnene.
CD short sNavnNr = L_KP+1; 
CD while ((pszNavn = LC_GetElementNavn(pFil,sNavnNr)) != NULL)
CD {
CD    // Gjør noe med navnet
CD    ...
CD    ++sNavnNr;
CD }
=============================================================================
*/
const wchar_t * CFyba::LC_GetElementNavn(LC_FILADM *pFil,short sNavnNr,bool *bBrukt)
{
   LO_TestFilpeker(pFil,L"GetNavn");

   LC_NAVNETABELL *pLn = &(pFil->SosiNavn);    //Peker til filens navnetabell

   if (sNavnNr >= 0  &&  sNavnNr < pLn->sAntNavn)
   {
      SOSINAVN *ip = pLn->sosi + sNavnNr;

      *bBrukt = ip->bBrukt;

      return ip->szNavn;
   }

   return NULL;
}


/*
AR-910819
CH LN_VisNavn                                Hent en linje fra  navnetabellen
CD =============================================================================
CD Formål:
CD Henter en linje fra navnetabellen som formatert streng.
CD
CD Parametre:
CD Type             Navn  I/U   Forklaring
CD -----------------------------------------------------------------------------
CD LC_NAVNETABELL *  pLn    i    Peker til navnetabell
CD long             navn   i    Linjenummer i navnetabellen som skall vises
CD wchar_t            *tx     r    Peker til streng med formatert linje
CD
CD Bruk:
CD for (navn=0,linje=10; navn<10; navn++,linje++){
CD     SH_OutTx(linje,1,LN_VisNavn(pLn,navn));
CD }
   =============================================================================
*/
wchar_t * CFyba::LN_VisNavn(LC_NAVNETABELL * pLn,short navn)
{
   SOSINAVN *ip;

   if (navn < pLn->sAntNavn){
       ip = pLn->sosi + navn;
       UT_SNPRINTF(err.tx,LC_ERR_LEN,L"%2d %16s", navn,ip->szNavn);
   } else{
       *err.tx = '\0';
   }

   return err.tx;
}


/*
AR-940413
CH LN_TolkKvalitet                                             Tolk KVALITET
CD ==========================================================================
CD Formål:
CD Tolk parameterstrengen for KVALITET.
CD
CD Parametre:
CD Type    Navn         I/U   Forklaring
CD -------------------------------------------------------------------------
CD wchar_t   *pszParameter       i  Peker til '\0'-avslutta streng, eller
CD                                 NULL hvis KVALITET mangler.
CD short  *psMetode           u  Hvordan data er registrert.
CD                                 KVAL_MET_UNDEF  metode er udefinert.
CD                                 KVAL_MET_STD    standard metode fra nivå over.
CD long   *plNoyaktighet      u  Registreringsnøyaktighet
CD                                 KVAL_NOY_UKJENT nøyaktighet er ukjent.
CD                                 KVAL_NOY_STD    standard nøyaktighet fra nivå over.
CD short  *psSynbarhet        u  Synbarhet i bilde
CD                                 KVAL_SYN_GOD    godt synlig.
CD                                 KVAL_SYN_UNDEF  synbarhet er udefinert.
CD                                 KVAL_SYN_STD    standard metode fra nivå over.
CD short  *psHoydeMetode      u  Hvordan høyden er registrert.
CD                                 KVAL_MET_UNDEF  metode er udefinert.
CD                                 KVAL_MET_STD    standard metode fra nivå over.
CD long   *plHoydeNoyaktighet u  Registreringsnøyaktighet
CD                                 KVAL_NOY_UKJENT nøyaktighet er ukjent.
CD                                 KVAL_NOY_STD    standard nøyaktighet fra nivå over.
CD
CD Bruk:
CD   ist = LN_TolkKvalitet(pszParameter,&sMetode,&lNoyaktighet,&sSynbarhet,
CD                         &sHoydeMetode,&lHoydeNoyaktighet);
CD =============================================================================
*/
void CFyba::LN_TolkKvalitet(wchar_t *pszParameter,short *psMetode,long *plNoyaktighet,
                     short *psSynbarhet,short *psHoydeMetode,long *plHoydeNoyaktighet)
{
   wchar_t ord[32];
   short i;
   wchar_t szMetode[6] = {L"*"};
   wchar_t szNoyaktighet[11] = {L"*"};
   wchar_t szSynbarhet[6] = {L"0"};
   wchar_t szHoydeMetode[6] = {L" "};
   wchar_t szHoydeNoyaktighet[11] = {L" "};

   /* Er det noen parameterstreng? */
   if (pszParameter) {
                              /* Hent strengene */
      if (UT_StrToken(pszParameter,0,&i,32,ord)) {
         UT_StrCopy(szMetode,ord,6);

         if (UT_StrToken(pszParameter,i,&i,32,ord)) {
            UT_StrCopy(szNoyaktighet,ord,10);

            if (UT_StrToken(pszParameter,i,&i,32,ord)) {
               UT_StrCopy(szSynbarhet,ord,6);

               if (UT_StrToken(pszParameter,i,&i,32,ord)) {
                  UT_StrCopy(szHoydeMetode,ord,6);

                  if (UT_StrToken(pszParameter,i,&i,32,ord)) {
                     UT_StrCopy(szHoydeNoyaktighet,ord,10);
                  }
               }
            }
         }
      }
   }

   /* Tolk strengene til tallverdier */
   if (*szMetode == '*') {
      *psMetode = KVAL_MET_UNDEF;
   } else if (*szMetode == '@') {
      *psMetode = KVAL_MET_STD;
   } else {
      UT_StrShort(szMetode,0,&i,psMetode);
   }

   if (*szNoyaktighet == '*') {
      *plNoyaktighet = KVAL_NOY_UKJENT;
   } else if (*szNoyaktighet == '@') {
      *plNoyaktighet = KVAL_NOY_STD;
   } else {
      UT_StrLong(szNoyaktighet,0,&i,plNoyaktighet);
   }

   if (*szSynbarhet == '*') {
      *psSynbarhet = KVAL_SYN_UNDEF;
   } else if (*szSynbarhet == '@') {
      *psSynbarhet = KVAL_SYN_STD;
   } else {
      UT_StrShort(szSynbarhet,0,&i,psSynbarhet);
   }

   if (*szHoydeMetode == '*') {
      *psHoydeMetode = KVAL_MET_UNDEF;
   } else if (*szHoydeMetode == '@') {
      *psHoydeMetode = KVAL_MET_STD;
   } else if (*szHoydeMetode == ' ') {
      *psHoydeMetode = *psMetode;
   } else {
      UT_StrShort(szHoydeMetode,0,&i,psHoydeMetode);
   }

   if (*szHoydeNoyaktighet == '*') {
      *plHoydeNoyaktighet = KVAL_NOY_UKJENT;
   } else if (*szHoydeNoyaktighet == '@') {
      *plHoydeNoyaktighet = KVAL_NOY_STD;
   } else if (*szHoydeNoyaktighet == ' ') {
      *plHoydeNoyaktighet = *plNoyaktighet;
   } else {
      UT_StrLong(szHoydeNoyaktighet,0,&i,plHoydeNoyaktighet);
   }

   return;
}


/*
AR-940413
CH LC_FormatterKvalitet                                   Formatter KVALITET
CD ==========================================================================
CD Formål:
CD Formater parameterstrengen for KVALITET.
CD Resultatet legges i en intern streng, og må kopieres over til andre
CD variabler før endring.
CD
CD Parametre:
CD Type    Navn         I/U   Forklaring
CD -------------------------------------------------------------------------
CD short   sMetode           i Hvordan data er registrert.
CD                               KVAL_MET_UNDEF  metode er udefinert.
CD                               KVAL_MET_STD    standard metode fra nivå over.
CD long    lNoyaktighet      i Registreringsnøyaktighet
CD                               KVAL_NOY_UKJENT nøyaktighet er ukjent.
CD                               KVAL_NOY_STD    standard nøyaktighet fra nivå over 
CD short   sSynbarhet        i Synbarhet i bilde
CD                               KVAL_SYN_GOD    godt synlig.
CD                               KVAL_SYN_UNDEF  synbarhet er udefinert.
CD                               KVAL_SYN_STD    standard metode fra nivå over.
CD short   sHoydeMetode      i Hvordan data er registrert.
CD                               KVAL_MET_UNDEF  metode er udefinert.
CD                               KVAL_MET_STD    standard metode fra nivå over.
CD long    lHoydeNoyaktighet i Registreringsnøyaktighet
CD                               KVAL_NOY_UKJENT nøyaktighet er ukjent.
CD                               KVAL_NOY_STD    standard nøyaktighet fra nivå over 
CD wchar_t   *pszParameter      r Peker til '\0'-avslutta streng.
CD
CD Bruk:
CD  pszParameter = LC_FormatterKvalitet(sMetode,lNoyaktighet,sSynbarhet,
CD                                      sHoydeMetode,lHoydeNoyaktighet);
CD =============================================================================
*/
wchar_t * CFyba::LC_FormatterKvalitet(short sMetode,long lNoyaktighet,short sSynbarhet,
                           short sHoydeMetode,long lHoydeNoyaktighet)
{
   static wchar_t szParameter[60];
   wchar_t szMetode[8] = {L"*"};
   wchar_t szNoyaktighet[13] = {L" *"};
   wchar_t szSynbarhet[8] = {L" *"};
   wchar_t szHoydeMetode[8] = {L" *"};
   wchar_t szHoydeNoyaktighet[13] = {L" *"};


   /* Metode */
   if (sMetode == KVAL_MET_STD) {
      *szMetode = '@';
   } else if (sMetode != KVAL_MET_UNDEF) {
      UT_SNPRINTF(szMetode,8,L"%hd",sMetode);
   }

   /* Nøyaktighet */
   if (lNoyaktighet == KVAL_NOY_STD) {
      szNoyaktighet[1] = '@';
   } else if (lNoyaktighet != KVAL_NOY_UKJENT  && lNoyaktighet != KVAL_NOY_UNDEF) {
      UT_SNPRINTF(szNoyaktighet,13,L" %ld",lNoyaktighet);
   }

   /* Synbarhet */
   if (sSynbarhet == KVAL_SYN_STD) {
      szSynbarhet[1] = '@';
   } else if (sSynbarhet != KVAL_SYN_UNDEF) {
      UT_SNPRINTF(szSynbarhet,8,L" %hd",sSynbarhet);
   }

   /* Høyde-metode */
   if (sHoydeMetode == KVAL_MET_STD) {
      szHoydeMetode[1] = '@';
   } else if (sHoydeMetode != KVAL_MET_UNDEF) {
      UT_SNPRINTF(szHoydeMetode,8,L" %hd",sHoydeMetode);
   }

   /* Høyde-nøyaktighet */
   if (lHoydeNoyaktighet == KVAL_NOY_STD) {
      szHoydeNoyaktighet[1] = '@';
   } else if (lHoydeNoyaktighet != KVAL_NOY_UKJENT  &&  lHoydeNoyaktighet != KVAL_NOY_UNDEF) {
      UT_SNPRINTF(szHoydeNoyaktighet,13,L" %ld",lHoydeNoyaktighet);
   }

   
   /* Bygg opp parameterstrengen */

   UT_StrCopy(szParameter,szMetode,60);

   if (szNoyaktighet[1] != '*'         ||
       sSynbarhet != KVAL_SYN_GOD      ||
       sHoydeMetode != sMetode         ||
       lHoydeNoyaktighet != lNoyaktighet) {

      UT_StrCat(szParameter,szNoyaktighet,60);

      if (sSynbarhet != KVAL_SYN_GOD      ||
          sHoydeMetode != sMetode         ||
          lHoydeNoyaktighet != lNoyaktighet) {

         UT_StrCat(szParameter,szSynbarhet,60);

         if (sHoydeMetode != sMetode         ||
             lHoydeNoyaktighet != lNoyaktighet) {

            UT_StrCat(szParameter,szHoydeMetode,60);

            if (lHoydeNoyaktighet != lNoyaktighet) {
               UT_StrCat(szParameter,szHoydeNoyaktighet,60);
            }
         }
      }
   }

   return szParameter;
}


/*
AR: 2000-01-19
CH LC_FinnNivo                                     Beregn nivå
CD ==============================================================
CD Formål:
CD Teller antall prikker i starten på egenskapsnavn.
CD
CD PARAMETERLISTE:
CD Type     Navn      I/U   Merknad
CD -------------------------------------------------------------
CD wchar_t    *pszGinfo   i    Streng med egenskapsnavn i starten
CD short    sNivo      r    Antall prikker 
CD
CD Bruk:
CD sNivo = LC_FinnNivo(pszGinfo);
  ================================================================
*/
short CFyba::LC_FinnNivo(const wchar_t * pszNavn)
{
   short sNivo = 0;

   while (*pszNavn != L'\0'  &&  *pszNavn == L'.') {
      ++pszNavn;
      ++sNivo;
   }

   return sNivo;
}
