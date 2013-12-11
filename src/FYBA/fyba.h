/******************************************************************************
*
* KARTVERKET  -  FYSAK
*
* Filename: fyba.h
*
* Content: Prototyper for rutiner for les/skriv av SOSI-fil.
*
* Copyright (c) 1990-2012 Kartverket
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

//
// Når FYBA brukes som DLL, defineres FYBA_DLL_IMPORTS
// Når FYBA brukes som lib er det ikke nødvendig med noen spesiell definisjon.
// (Når FYBA skal kompileres/lages som DLL defineres FYBA_DLL_EXPORTS)
//


#ifdef WIN32
#ifdef FYBA_DLL_IMPORTS /* FYBA brukes som DLL */
#  pragma comment (lib, "FYBA_DLL.lib")
//#  ifdef _DEBUG
//#     pragma comment (lib, "CFYBA_DLLD.lib")
//#  else
//#     pragma comment (lib, "CFYBA_DLL.lib")
//#  endif
#else
#   ifndef FYBA_DLL_EXPORTS  /* FYBA brukes som LIB */
#     ifdef _DEBUG
#        pragma comment (lib, "FYBAD.lib")
#     else
#        pragma comment (lib, "FYBA.lib")
#     endif
#   endif
#endif


#  ifndef DllExport
#    define DllExport __declspec(dllexport)
#    define DllImport __declspec(dllimport)
#  endif /* !DllExport */
#else
#  define DllExport
#  define DLLImport
#  define __cdecl
#endif


#ifndef SK_EntPnt_FYBA

#   ifdef FYBA_DLL_EXPORTS /* FYBA kompileres til DLL ==> FYBA_DLL.DLL */
#      define DllExport

#   else 
#      ifdef FYBA_DLL_IMPORTS /* FYBA brukes som DLL */
#         define DllImport

#      else /* FYBA kompileres eller brukes som LIB */
#         define SK_EntPnt_FYBA
#      endif
#   endif
#endif


#include <stdio.h>
#include <stdlib.h>

#include <fygm.h>
#include <fyut.h>

#define BRUK_MAP_SNR /* Brukes hvis serienummer organiseres i map. Fjernes hvis serienummer organiseres i array. */
#include <map>

/* ======================================================= */
/*  Definerer konstanter                                   */
/* ======================================================= */
   /* Max-verdier for basen */
#define LC_MAX_GRU        10000000  /* Max grupper i en base */
#define LC_MAX_GINFO          6000  /* Max linjer GINFO i en gruppe */
#define LC_MAX_GINFO_BUFFER 250000  /* GINFO-buffer */
#define LC_MAX_KOORD        150000  /* Max antall koordinater i en gruppe */
#define LC_MAX_PINFO_BUFFER 250000  /* PINFO-buffer */
#define LC_MAX_ANT_PRIOR       128  /* Max antall prioriteter */
#define LC_BASEVER_LEN          50  /* Max lengde av szBaseVer (inkl. '\0') */
#define LC_MAX_SOSINAVN_LEN     50  /* Max lengde for SOSI-navn (inkl. '\0') */
#define LC_MAX_NAVN            500  /* Max linjer i navnetabellen (pr. fil) */
#define LC_NGISLAG_LEN          50  /* Max lengde av NGIS-LAG fra filhodet */
#define LC_MAX_SOSI_LINJE_LEN 1024  /* Max linjelengde for lesing fra SOSI-filen */
#define LC_TX8_BUFFER_LEN   (LC_MAX_SOSI_LINJE_LEN*8)  /* Størrelse av lavnivå lesebuffer fra SOSI-filen */
#define LC_MAX_OBJTYPE_LEN      33  // Max lengde av objettypenavn (inkl. '\0')
#define LC_MAX_GINFO_NIVAA      15  // Max antall (prikk)nivåer i GINFO

#define  LC_INTERNT_TEGNSETT TS_ISO8859
        

   /* Konstanter for basetype */
enum LcBasetype { LC_BASE  = 1,    /* Vanlig base */
                  LC_KLADD = 3 };  /* Kladdebase */

   /* Konstanter for åpning av eksisterende base */
#define  UKJENT_BASE     -1
#define  FEIL_VERSJON    -2
#define  IKKE_STENGT     -3
#define  IDX_MANGLER     -4
#define  IDX_FEIL_DATO   -5
#define  SOS_MANGLER     -6
#define  SOS_FEIL_DATO   -7
#define  SOS_FEIL_AKSESS -8

	/* Konstanter for LC_CloseBase */
#define RESET_IDX   0   /* Fjern indeksfilene */
#define SAVE_IDX    1   /* Lagrer indeksfilene */

   /* Konstanter for LC_OpenSos */
#define LC_BASE_FRAMGR 1  /* Framgrunnsfil i basen */
#define LC_BASE_BAKGR  2  /* Bakgrunnsfil i basen (Bare les) */
#define LC_SEKV_LES    3  /* Sekvensiell, les */
#define LC_SEKV_SKRIV  4  /* Sekvensiell, skriv */
#define LC_SEKV_UTVID  5  /* Sekvensiell, utvid gammel fil */

#define LC_NY_IDX      1  /* Tvungen nygenerering */
#define LC_GML_IDX     0 /* Bruk gammel .idx hvis den er OK */

#define LC_VIS_STATUS   1  /* Vis status under indeksoppbygging */
#define LC_INGEN_STATUS 0  /* Ikke vis status */

   /* Lag i basen */
#define LC_SEKV     0x0001   /* Sekvensiell fil */
#define LC_BAKGR    0x0002   /* Bakgrunnsfil i base */
#define LC_FRAMGR   0x0004   /* Framgrunnsfil i base */

   /* Status fra LC_OpenSos */
#define LC_CANCEL   -3  // Avbrutt med [Esc], eller lesefeil
#define LC_DUBLIKAT -4  // Filen er i basen fra før
#define LC_OPPTATT  -5  // Filen er åpen i annet program 

	/* Konstanter for LC_SetNgisModus */
#define NGIS_NORMAL   0   /* Vanlig modus */
#define NGIS_SPESIAL  1   /* Spesialmodus der det er mulig å finne og
                                lese grupper som er merka som sletta */

	/* Konstanter for LC_SetUtvidModus */
#define LC_UTVID_SIKKER 0 /* SOSI-filen stenges og filstørrelsen oppdateres */
                          /* etter hver gruppe som er skrevet på slutten av filen */
#define LC_UTVID_RASK   1 /* SOSI-filen stenges IKKE etter hver oppdatering*/

   /* Konstanter til QueryGP mfl. */
#define LC_HODE   1
#define LC_GINFO  2
#define LC_PINFO  4

/* Konstanter til bruk ved kombinert flatesøk. JAØ-20000516 */
#define LC_INGEN 0
#define LC_NOEN  1
#define LC_ALLE  2

   /* Gruppenummer når det ikke er noen aktuell gruppe */
#define INGEN_GRUPPE  -1L    /* bgr når det ikke er noen akt. gruppe */

   /* Lesemetode for LC_RxGr */
#define LES_OPTIMALT   0     /* Les mest effektivt base/SOSI */
#define LES_SOSI       1     /* Les alltid fra SOSI-filen */

   /* Skrivemetode for LC_WxGr */
#define SKRIV_OPTIMALT 0     /* Skriv mest effektivt kø/SOSI */
#define SKRIV_SOSI     1     /* Skriv direkte til SOSI-filen */

   /* Handteringmetode for NGIS-nøkkel for LC_CopyGr */
#define OPPDATER_NGIS 0  /* NGIS-nøkkel oppdateres i henhold til hodet i filen det kopieres til */
#define BEVAR_NGIS    1  /* NGIS-nøkkel bevares uforandret i kopien */

   /* Manglende kvalitetsopplysninger */
#define KVAL_MET_UNDEF  -1   /* Udefinert metode */
#define KVAL_MET_STD    -2   /* Standard metode fra nivå over */

#define KVAL_NOY_UNDEF  -1L  // Udefinert nøyaktighet. -  OBS! Denne skal utgå
#define KVAL_NOY_STD    -2L  /* Standard nøyaktighet fra nivå over */
#define KVAL_NOY_UKJENT  999999L  // Ukjent nøyaktighet 


#define KVAL_SYN_GOD     0   /* Godt synlig */
#define KVAL_SYN_UNDEF  -1   /* Udefinert synbarhet */
#define KVAL_SYN_STD    -2   /* Standard synbarhet fra nivå over */

   /* For LC_GetTH og LC_PutTH */
   /* Konstant for å si at punktet ikke har høyde */
   /* OBS! Denne må stemme med definisjonen i filen port.h */
#ifndef HOYDE_MANGLER
#   define HOYDE_MANGLER        -9999.999   /* Høyde mangler i punktet */
#endif

#define KOORD_MANGLER           -9999.999   /* Koordinat mangler i punktet */

   /* Gruppeinformasjon */
#define GI_PINFO     ((unsigned short)0x0001) /* Har PINFO */
#define GI_NAH       ((unsigned short)0x0002) /* Gruppen har høyde informasjon (..NØH) */
#define GI_KP        ((unsigned short)0x0004) /* Har knutepunkt (...KP n) */
#define GI_REF       ((unsigned short)0x0008) /* Har referanser (.. :n) */
#define GI_OY_REF    ((unsigned short)0x0010) /* Har referanser med øy */
#define GI_SLETTA    ((unsigned short)0x0020) /* Er sletta (merka som sletta) */
#define GI_NGIS      ((unsigned short)0x0040) /* Er tatt ut fra NGIS */
#define GI_NAD       ((unsigned short)0x0080) /* Har dybde informasjon (..NØD) */
#define GI_READ_ONLY ((unsigned short)0x0100) /* Bare leseaksess (kan ikke endres) */

   /* Avgrensing av brukttabellen */
#define  BT_MIN_BT    0       /* Første kolonne i brukttabellen */
#define  BT_MAX_BT   31       /* Siste kolonne i brukttabellen */
#define  BT_MIN_USER  1       /* Første tilgjengelig for brukerprogram */
#define  BT_MAX_USER 26       /* Siste tilgjengelig for brukerprogram */
   /* Spesielle posisjoner i brukttabellen */
#define  BT_SKRKO     0       /* I skrivekø */
#define  BT_REFBOX   27       /* Referanser er brukt i omskrevet boks */
#define  BT_X        28       /* Reserve systemposisjon */
#define  BT_SNRSOK   29       /* Funnet ved SNR-søk */
#define  BT_GISOK    30       /* Funnet ved GINFO-søk */
#define  BT_GEOSOK   31       /* Funnet ved geografisk søk */

   /* Logiske operasjoner mellom kolonner i brukttabellen */
#define BC_AND      0
#define BC_OR       1
#define BC_COPY     2
#define BC_INVERT   3
#define BC_EXCHANGE 4


   /* Konstanter for get og put referansenummer */
#define START_OY   9999999L      /* Start øy */
#define SLUTT_OY  -9999999L      /* Slutt øy */

   /* Konstanter for GetArrayTK() */
#define HENT_FORRFRA  1          /* Vanlig */
#define HENT_BAKFRA  -1          /* Snu buferet */

/* UTVALG */

// Utvalgskommandoer 
#define LC_U_OG          1     // ..OG 
#define LC_U_ELLER       2     // ..VELG og ..ELLER


 // Utvalgsmetoder
#define LC_U_IKKE         0     //  !     Ikke (Tilslag når SOSI-navnet     
                             //             ikke finnes. Bare GINFO.)    
#define LC_U_ALLE         1     //  AL    Alle                              
#define LC_U_FRATIL       2     //  <>    Fra-til                           
#define LC_U_UTENFOR      3     //  ><    Utenfor                           
#define LC_U_MINDRE       4     //  <     Mindre-enn                        
#define LC_U_STORRE       5     //  >     Større-enn                        
#define LC_U_DELELIG      6     //  /     Delelig-med, eventuellt med       
                                //        sjekk på om restverdi er 2. verdi 
#define LC_U_UDELELIG     7     //  !/    Ikke-delelig-med                  
#define LC_U_CONTEIN      8     //  ()    Inneholder                        
#define LC_U_IKKECONTEIN  9     //  !()   Inneholder ikke (Tilslag når      
                                //        denne navn og verdi kombinasjonen 
                                //        ikke finnes)                      
#define LC_U_LIK         10     //  =     Lik                               
#define LC_U_IKKEVALGT   11     //  IV    Ikke valgt (Tilslag når gruppen   
                                //        ikke er tegnet ennå. Kombineres   
                                //        med SOSI-navnet "..*")            
#define LC_U_IKKELIK     12     //  !=    Ikke lik (Tilslag når denne       
                                //        navn og verdi kombinasjonen ikke  
                                //        finnes)                           
#define LC_U_FLERE       13     //  FL    Flere (Tilslag når SOSI-navnet    
                                //        forekommer flere ganger.)         
#define LC_U_IKKEFLERE   14     //  !FL   Ikke flere enn (Tilslag når       
                                //        SOSI-navnet IKKE forekommer       
                                //        flere ganger enn gitt antall.)    



 // Parametertyper 
#define LC_U_TALL          1     // Heltall     
#define LC_U_FLYT          2     // Flyttall    
#define LC_U_ALFA          4     // Tekststreng 
#define LC_U_DEFINERT      8     // Type definert i utvalgsregel


/*
 *!--------------------------------------------------------------!
 *! Kvalitet                                                     !
 *!--------------------------------------------------------------!
 */
typedef struct dLC_KVALITET {  /* Standard Kvalitet */
   short sMetode;
   long  lNoyaktighet;
   short sSynbarhet;
   short sHoydeMetode;
   long  lHoydeNoyaktighet;
} LC_KVALITET;

/*
 *!--------------------------------------------------------------!
 *! Buffer for lesing av SOSI-fil                                !
 *!--------------------------------------------------------------!
 */

typedef struct dLB_LESEBUFFER {
   wchar_t tx[LC_MAX_SOSI_LINJE_LEN];  /* Lesebuffer */
   UT_INT64 filpos;                 /* Filposisjon for starten av bufret */
   UT_INT64 startpos;               /* Startposisjon i filen for aktuellt SOSI-navn */
   wchar_t *cp;                        /* Peker til aktuell posisjon i bufret */
   wchar_t *pp;                        /* Peker til start parameter */
   wchar_t *ep;                        /* Peker til posisjon etter aktuellt ord */
	wchar_t *np;                        /* Peker til posisjon etter parameter (neste cp) */
   short cur_navn[LC_MAX_GINFO_NIVAA]; /* Aktuellt SOSI-navn på dette nivå */
   short cur_niv;                   /* Aktuellt nivå (der cp peker) (Ant. prikker) */
   short cur_ant_par;               /* Antall parametre på aktuellt nivå */
   short cur_type;                  /* Viser hva aktuellt set inneholder */
   short set_brukt;                 /* Status som viser om aktuellt "sett" er brukt */
   short sTegnsett;                 /* SOSI-filens tegnsett */
   short sStatus;                   /* Status, viser om buffer har brukbart innhold */
} LB_LESEBUFFER;


/*
 *!----------------------------------------------------------!
 *! BUFFER for koordinater og GINFO i minne                  !
 *!----------------------------------------------------------!
*/

/*
CH SOSI-buffer                 Binær kopi av SOSI-filen
CD For å øke hastigheten holder FYBA en binær kopi av SOSI-filen.
CD Denne kopien ligger delevis i minne og delevis på disk.
CD
CD Denne kopien består av gruppeinformasjon, koordinater og punktinformasjon.
CD
CD Gruppeinformasjonen for en gruppe handteres som en lang streng med pekere
CD til startposisjon for hver GINFO-linje.
CD
CD !-----------:-----------:----------:----------------------------------!
CD !  GINFO 1  !  GINFO 2  ! GINFO 3  ! .....                            !
CD !-----------:-----------:----------:----------------------------------!
CD  !            !           !
CD  !---------!  !           !
CD            !  !           !
CD !--------! !  !           !
CD ! GINFO  ! !  !           !
CD !--------! !  !           !
CD ! ofset  !-!  !           !
CD !        !-----           !
CD !        !-----------------
CD
CD Koordinatene lagres i egne array. Punktinformasjonen lagres som
CD en lang streng for hele gruppen med peker for hvert punkt til startposisjon
CD for PINFO for hvert punkt på samme måte som GINFO.
CD
CD !--------! !--------! !--------:-------:--------!
CD ! Øst    ! ! Nord   ! ! Høyde  !  KP   ! PINFO  !
CD !--------! !--------! !--------!-------!--------!
CD ! double ! ! double ! ! double ! short ! ushort !
CD !   m    ! !   m    ! !   m    !       !  ofset !
*/
typedef struct d_LB_INFO {
   double dHoyde;            // Høyde
   short sKp;                // Knutepunkt (0=ikke knutepunkt)
   unsigned long ulPiOfset;  // PINFO ofset i eget buffer
} LB_INFO;


#   define LC_INGEN_PINFO ULONG_MAX     /* Offset ved tom PINFO */


/*
 *!--------------------------------------------------------------!
 *! Navnetabell                                                  !
 *!--------------------------------------------------------------!
 */
typedef struct dSOSINAVN       // Navnetabellen
{           
   wchar_t szNavn[LC_MAX_SOSINAVN_LEN]; // Sosi-navn
   unsigned char ucAntPar;            // Antall parametre til dette navnet
   unsigned char ucNivo;              // "Prikk-nivå"  (1 = Gruppenavn)
   bool bBrukt;                 // Viser om navnet er vært brukt
} SOSINAVN;

typedef struct dLC_NAVNETABELL 
{
   short sAntNavn;              /* Antall navn totalt i navnetabellen */
   SOSINAVN sosi[LC_MAX_NAVN];  /* Navnetabellen */
} LC_NAVNETABELL;


#  define LC_ANT_PAR_UKJENT  255  /* Antall parametre til dette navnet er
                                     ukjent, ta med fram til neste " ."  */

  /* Definerte navn */
#define L_SLUTT       0
#define L_PUNKT       1
#define L_LINJE       2
#define L_KURVE       3
#define L_BUE         4
#define L_BUEP        5
#define L_SIRKEL      6
#define L_SIRKELP     7
#define L_KLOTOIDE    8
#define L_SVERM       9
#define L_TEKST      10
#define L_TRASE      11
#define L_FLATE      12
#define L_BEZIER     13
#define L_RASTER     14
#define L_DEF        15 
#define L_OBJDEF     16
#define L_MLINJE     17
#define L_STRUKTUR   18
#define L_OBJEKT     19
#define L_SYMBOL     20
#define L_HODE       21  /* L_HODE må alltid ligge sist av gruppenavnene */

#define L_NA         22  /* Andre definerte navn */
#define L_NAH        23
#define L_REF1       24
#define L_REF2       25
#define L_RADIUS     26
#define L_ENHET2     27
#define L_ENHET2H    28
#define L_ENHET2D    29
#define L_ENHET3     30
#define L_ENHET3H    31
#define L_ENHET3D    32
#define L_ORIGONO    33
#define L_HOYDE      34
#define L_DYBDE      35
#define L_NAD        36
#define L_NGISFLAGG  37
#define L_NGISLAG    38
#define L_OBJTYPE    39
#define L_KP         40   /* L_KP må alltid ligge sist av de forhåndsdefierte navnene */



/* ======= INDEKS-TABELLER =================== */

/*
CH Serienummer-tabell
CD !-------------*
CD ! Gruppenummer!
CD !-------------!       Linjenummer i tabellen er serienummer.
CD !  lGrNr      !
CD !  (long)     !
CD *-------------*
*/



/*
CH Geografisk-søketabell                               Geografisk søketabell
CD
CD Geografisk søk er basert på omskrevet boks.
CD Boksene organiseres i et R-tre. 
CD
CD !-----------------------------------*
CD !      Omskreven boks               !
CD !                                   !
CD ! min-N  ! min-Ø  ! max-N  ! max-Ø  !
CD !        !        !        !        !
CD !--------!--------!--------!--------!
CD !dMinNord!dMinAust!dMaxNord!dMaxAust!
CD !  d     !   d    !   d    !   d    !
CD !        !        !        !        !
CD *-----------------------------------*
*/

/*
 *!--------------------------------------------------------------!
 *! Boks for geografisk søk                                      !
 *!--------------------------------------------------------------!
 */
typedef struct dLC_BOKS{
   double dMinAust;
	double dMinNord;
   double dMaxAust;      
   double dMaxNord;      
} LC_BOKS;

#define LC_R_MAX_SON  3      /* Max antall sønner for hver node i R-treet */

typedef struct dLC_R_NODE{
   struct dLC_R_NODE *pFar;  /* Far i treet (node) */
   LC_BOKS Boks;             /* Sum av omskreven boks for sønnene */
   short sSonType;  /* Hvilken type sønner har denne noden LC_NODE / LC_LEAF */
   short sSonAnt;   /* Antall sønner */
   union {
      struct dLC_R_NODE *pNode[LC_R_MAX_SON];  /* Sønner i treet (node) */
      struct dLC_R_LEAF *pLeaf[LC_R_MAX_SON];  /* Sønner i treet (løv) */
   } Son;
} LC_R_NODE;

#define LC_NODE 0
#define LC_LEAF 1


typedef struct dLC_R_LEAF{
   LC_R_NODE *pFar;  /* Far i treet (node) */
   LC_BOKS Boks;     /* Omskreven boks for gruppen */
   long lNr;         /* Gruppenummer i filen  */
} LC_R_LEAF;


/*
 *!--------------------------------------------------------------!
 *! Gruppetabel                                                  !
 *!--------------------------------------------------------------!
 */
/*
CH Gruppetabell                                     Gruppetabell
CD Dette er hovedtabellen med informasjon om hver enkelt gruppe på SOSI-filen. 
CD
CD !------------------------------:-----------------------------------------------*
CD !Start  !Start !Ant.   !Ant.   !            Gruppeinnhold                      !
CD ! på    ! i    !tegn   !tegn   !-----------------------------------------------!
CD !SOSI-  ! RB   !GINFO- !PINFO- !Gruppe!ant. !ant. !Kvali-  !Enhet ! Div. info  !
CD !fil    !      !buffer !buffer !navn  !GINFO! NØ  ! tet    !      ! (bit def.) !
CD !-------!------!-------!-------!------!-----!-----!--------!------!------------!
CD !sosi_st!rb_st !ulGiLen!ulPiLen!gnavn !ngi  !nko  !Kvalitet!dEnhet! info       !
CD !  n64  ! n64  !  ul   !  ul   ! s    !  s  ! l   ! struct !  d   ! us         !
CD !       !      !       !       !      !     !     !        !      !0=i         !
CD !       !      !       !       !      !     !     !        !      !1=H         !
CD !       !      !       !       !      !     !     !        !      !2=KP        !
CD !       !      !       !       !      !     !     !        !      !3=REF       !
CD !       !      !       !       !      !     !     !        !      !4=REF med ØY!
CD !       !      !       !       !      !     !     !        !      !5=sletta    !
CD !       !      !       !       !      !     !     !        !      !6=NGIS oppd !
CD *------------------------------------------------------------------------------*
*/
typedef struct dLC_GRTAB_LINJE{
   unsigned long ulPrior[4]; /* Bitfelt for prioriteter. */
									  /* Siste bit viser om feltet er bygd opp. */
   UT_INT64 sosi_st;     /* Startposisjon i SOSI-filen */
   UT_INT64 rb_st;       /* Peker til start av gruppen i buffer-fil */
   long  rb_forrige_gr;  /* Forrige gruppe i buffer-filen */
   long  rb_neste_gr;    /* Neste gruppe i buffer-filen */
   short gnavn;          /* Gruppenavn. Eks. .HODE, .PUNKT, mm */
   short ngi;            /* Antall GINFO-linjer */
   long nko;             /* Antall koordinater */
   unsigned short info;  /* Info, se under gruppetabellen */
	LC_KVALITET Kvalitet; /* Aktuell kvalitet fra GINFO */
   wchar_t szObjtype[LC_MAX_OBJTYPE_LEN]; /* ..OBJTYPE fra GINFO */
   double dEnhet;        /* Aktuell enhet for gruppen i bufferet */
   double dEnhetHoyde;   /* Aktuell enhet-H for gruppen i bufferet */
   double dEnhetDybde;   /* Aktuell enhet-D for gruppen i bufferet */
   unsigned long ulGiLen; /* Antall tegn i GINFO-buffer (inkl. \0) */
   unsigned long ulPiLen; /* Antall tegn i PINFO-buffer (inkl. \0) */
   LC_R_LEAF *pRL;        /* Peker inn i geografisk søketre */

} LC_GRTAB_LINJE;


/*
CH Brukttabell                                            Merking av grupper
CD Denne tabellen brukes delevis internt av FYBA, og delevis av brukerprogrammet.
CD
CD Følgende bit er definert:
CD -------------------------------------------------------------------------
CD  31  Gruppen er funnet ved geografisk søk   -----! Brukere kan lese/bruke
CD  30  Gruppen er funnet ved GINFO-utvalg          ! disse, men det er ikke
CD  29  Gruppen er funnet ved serienummer-søk       ! lov til å endre dem.
CD  28  Gruppen er i ringbufferet                   !
CD  27  Referanser er brukt i omskrevet blokk ------!
CD  26                              -----!
CD   .                                   ! Brukere har
CD   .                                   ! full tilgang
CD   2                                   ! til disse.
CD   1                              -----!
CD   0  Gruppen ligger i kø for skriving til SOSI-filen
*/


   /*
    *!--------------------------------------------------------------!
    *! Overordnet blokk med pekere til de ulike indekstabellene     !
    *!--------------------------------------------------------------!
    */
#define LC_IDX_LIN_BLOKK 3000
#define LC_ANT_IDX_BLOKK  (LC_MAX_GRU / LC_IDX_LIN_BLOKK)

#ifdef BRUK_MAP_SNR
   /* Map for serienummersøk */
   typedef std::map<long, long> MapSerieNr;
#endif

typedef struct dLC_IDX_TABELL{
	/* Array med pekere til starten av blokk med Gruppetabell-linjer */
   LC_GRTAB_LINJE * GtAdm[LC_MAX_GRU / LC_IDX_LIN_BLOKK];
#ifndef BRUK_MAP_SNR
   /* Array med pekere til starten av blokk med Gruppenummer for gitt SNR */
   long             *SnrAdm[LC_MAX_GRU / LC_IDX_LIN_BLOKK];
#else
   /* Peker til map med serienummer */
   MapSerieNr *SerieNrAdm;
#endif

   /* Array med pekere til starten av blokk med Brukttabell-linjer */
   unsigned long    *BtAdm[LC_MAX_GRU / LC_IDX_LIN_BLOKK];

	/* Array med pekere til starten av blokk med Geo-tabell-linjer */
   /* LC_GEOSOK_BOKS *GeoAdm[LC_MAX_GRU / LC_IDX_LIN_BLOKK]; */
} LC_IDX_TABELL;


/*
 *!--------------------------------------------------------------!
 *! Koordinat 2D                                                 !
 *!--------------------------------------------------------------!
 */
typedef struct dLC_KOORD_2D {  /* (ø,n) Koordinatpar */
   double dAust;     /* øst-koordinat */
   double dNord;     /* nord-koordinat */
} LC_KOORD_2D;

/*
 *!--------------------------------------------------------------!
 *! Koordinat 3D                                                 !
 *!--------------------------------------------------------------!
 */
typedef struct dLC_KOORD_3D {  /* (ø,n,h) Koordinatpar */
   double dAust;     /* øst-koordinat */
   double dNord;     /* nord-koordinat */
   double dHoyde;    /* Høyde */
} LC_KOORD_3D;

/*
 *!--------------------------------------------------------------!
 *! Rektangel                                                    !
 *!--------------------------------------------------------------!
 */
typedef struct dLC_REKT {
   double dMinAust;
	double dMinNord;
   double dMaxAust;      
   double dMaxNord;      
} LC_REKT;


/*
 *!--------------------------------------------------------------!
 *! Kontanter og struktur for handtering av TRANSPAR i filhodet  !
 *!--------------------------------------------------------------!
 */
// Maske som styrer hvilke elementer i LC_TRANSPAR som benyttes
#define LC_TR_KOORDSYS  ((unsigned short)0x0001)  /* Koordsys */  
#define LC_TR_TRANSSYS  ((unsigned short)0x0002)  /* Transsys */
#define LC_TR_GEOSYS    ((unsigned short)0x0004)  /* Geosys   */  
#define LC_TR_GEOKOORD  ((unsigned short)0x0008)  /* Geokoord */  
#define LC_TR_ORIGO     ((unsigned short)0x0010)  /* Origo-nø */  
#define LC_TR_ENHET     ((unsigned short)0x0020)  /* Enhet    */  
#define LC_TR_ENHETH    ((unsigned short)0x0040)  /* Enhet-h  */  
#define LC_TR_ENHETD    ((unsigned short)0x0080)  /* Enhet-d  */  
#define LC_TR_VERTDATUM ((unsigned short)0x0100)  /* Vert-datum */
#define LC_TR_VERTINT   ((unsigned short)0x0200)  /* Vert-int   */
#define LC_TR_VERTDELTA ((unsigned short)0x0400)  /* Vert-delta */

#define LC_TR_ALLT      ((unsigned short)0xFFFF)  /* Alle deler av ..TRANSPAR */  


// Definerer struktur for ..TRANSPAR
typedef struct dLC_TRANSPAR {
   /* ...KOORDSYS */
   short   sKoordsys;
   wchar_t    szKoordsysDatum[36];
   wchar_t    szKoordsysProjek[36];
   /* ...TRANSSYS */
   short sTranssysTilsys;
   double dTranssysKonstA1;
   double dTranssysKonstB1;
   double dTranssysKonstA2;
   double dTranssysKonstB2;
   double dTranssysKonstC1;
   double dTranssysKonstC2;
   /* ...GEOSYS */
   short sGeosysDatum;
   short sGeosysProj;
   short sGeosysSone;
   /*...GEOKOORD */
   short sGeoKoord;
   /* ...ORIGO-NØ */
   LC_KOORD_2D Origo;
   //double dOrigoAust;
   //double dOrigoNord;
   /* ...ENHET */
   double  dEnhet;
   /* ...ENHET-H */
   double  dEnhet_h;
   /* ...ENHET-D */
   double  dEnhet_d;      
   /* ...VERT-DATUM */ 
   wchar_t szVertdatHref[7];
   wchar_t szVertdatDref[6];
   wchar_t szVertdatFref[6];
   wchar_t szVertdatHtyp[2];
   /* ...VERT-INT */ 
   short sVertintHref;
   short sVertintDref;
   short sVertintFref;
   /* ...VERT-DELTA */ 
   short sVdeltaMin;
   short sVdeltaMax;
} LC_TRANSPAR;


#define LC_TR_GEOSYS_INGEN_VERDI  -9999  // Brukes  for å angi at projeksjon og sone under geosys ikke er gitt


/*
 *!--------------------------------------------------------------!
 *! Filadministrasjon                                            !
 *!--------------------------------------------------------------!
 */
typedef struct dLC_FILADM{
   wchar_t    szBaseVer[LC_BASEVER_LEN]; // Versjon og dato for aktuell versjon av FYBA
   wchar_t    szIdxVer[5];    // Indeksfil-versjon
   short    sIdxOpen;       // UT_FALSE/UT_TRUE - Flagg som viser at indeks er åpnet
   unsigned long ulPid;     // Prosess ID for programmet som har åpnet filen
   unsigned short  usLag;   // Lag: (LC_SEKV,LC_FRAMGR,LC_BAKGR)
   wchar_t    szNgisLag[LC_NGISLAG_LEN]; // Ngislag i filhodet
   short    sAccess;        // Aksess: (READ / UPDATE)
	/* short    sDisk; */          // Disk for SOSI-filen (1=A, 2=B, osv.)
   wchar_t   *pszNavn;        // Filnavn (SOSI-filens navn, inkl. sti
   UT_INT64 SosiBytes;      // Antall byte i SOSI-filen
   FTID     SosiTid;        // Oppdateringstidspunkt for SOSI-filen
   LC_TRANSPAR TransPar;       // Transformasjonsparametre fra filhodet
   unsigned short TransMaske;  // Maske som viser hvilke deler av TransPar som inneholder data
   LC_REKT Omr;                // ..OMRÅDE fra filhodet
   short   sTegnsett;          // Tegnsett fra filhodet eller standardverdi
   short   sSosiVer;            // ..SOSI-VERSJON fra fil-hodet * 100
   wchar_t   SosiNiv[2];          // ..SOSI-NIVÅ fra fil-hodet
                                 // SosiNiv[0] = nivå fra filåpningen
                                 // SosiNiv[1] = nivå fra senere handtering
                                 // Filhodet oppdateres når filen stenges
   // short usUlovligRef;  // Bryter som viser om det er ulovlige
   //                      // referanser i filen (Indeksoppbygging)
   unsigned short usDataFeil;  // Flagg som viser om det er datafeil
                               // i filen (Indeksoppbygging)
   LC_KVALITET Kvalitet;   // Kvalitet fra filhodet

   UT_INT64 n64AktPos;        // Aktuell posisjon for sekv. skriv / les
	UT_INT64 n64NesteLedigRbPos; // Neste ledige posisjon i buffer-filen
   long lSisteGrRb;       // Siste gruppe i buffer-filen

   long lMaxSnr;        // Største serienummer brukt i filen
   long    lAntGr;      // Antall grupper i filen

   struct dLC_NAVNETABELL  SosiNavn;   // Navnetabell

	LC_IDX_TABELL *pIdx; // Starten av indekstabellen
   LC_R_NODE *pGeoRN;   // Peker til starten av trestruktur for geografisk søk
   LC_BOKS Omraade;     // Område angitt i filhodet ved åpning

   struct dLC_FILADM *pNesteFil;  // Peker til neste fil-adm.
   struct dLC_BASEADM *pBase;     // Peker til base-adm. for denne filen
} LC_FILADM;


/* Konstanter for flagg som viser om det er datafeil i filen (Indeksoppbygging) */
#define LC_DATAFEIL_REF       1   // Ulovlig referanse (referanse til gruppe som ikke finnes)
#define LC_DATAFEIL_BUE       2   // Ulovlige buer i filen
#define LC_DATAFEIL_PRIKKNIVO 4   // Ulovlig sprang i prikknivå mellom egenskaper i GINFO

/*
 *!--------------------------------------------------------------!
 *! Baseadministrasjon                                           !
 *!--------------------------------------------------------------!
 */
typedef struct dLC_BASEADM{
   LcBasetype type;        // Basetype: LC_BASE / LC_KLADD

   long   lAntGr;       // Antall grupper i basen
   LC_BOKS Omraade;     // Summert område fra filhodene
   short sAntFramgrFil; // Antall filer i framgrunn
   short sAntBakgrFil;  // Antall filer i bakgrunn

   LB_LESEBUFFER BufAdm;  // Vanlig lesebuffer mot SOSI-fil

   LC_FILADM *pCurSos;    // Fil-adm for åpen SOSI-fil
   FILE      *pfSos;      // Filhandel for åpen SOSI-fil

   LC_FILADM *pCurRb;    // Fil-adm for åpen Rb-fil
	FILE      *pfRb;      // Filhandel for åpen Rb-fil
   short      sModusRb;  // LES eller SKRIV til buffer-filen
   UT_INT64    n64FilPosRb; // Aktuell posisjon i buffer-filen 

   LC_FILADM *pForsteFil;    // Peker til første fil-adm.
   LC_FILADM *pSisteFil;     // Peker til siste fil-adm.

   struct dLC_BASEADM *pNesteBase;      // Peker til neste base-adm
} LC_BASEADM;


/*
 *!--------------------------------------------------------------!
 *! Gruppenummer                                                 !
 *!--------------------------------------------------------------!
 */
typedef struct dLC_BGR {
   LC_FILADM *pFil;   /* Peker til FilAdm for SOSI-fil */
   long lNr;          /* Gruppenummer innen filen */
} LC_BGR;

/*
 *!--------------------------------------------------------------!
 *! Hent flate-referanser, status                                !
 *!--------------------------------------------------------------!
 */
typedef struct dLC_GR_STAT {  
   short sGiLinNr;  /* Aktuell GINFO-linjenummer referanselesing */
   short sRefPos;   /* Aktuell posisjon i GINFO-linja */
   short sRefLin;   /* Viser om aktuel linje inneholder referanser */
} LC_GR_STATUS;

typedef struct dLC_GRF_STAT {
   LC_GR_STATUS Omkr;
   unsigned short usOmkretsFerdig;
   LC_GR_STATUS Oy;
   LC_BGR Bgr;      /* Aktuell øy-gruppe */
} LC_GRF_STATUS;


/*
 *!--------------------------------------------------------------!
 *! Geografisk søk, status mm.                                   !
 *!--------------------------------------------------------------!
 */

/* Kjede med resultatet av søket */
typedef struct dLC_KJEDE_BGR {
   LC_BGR Bgr;
   struct dLC_KJEDE_BGR *pNesteKB;
} LC_KJEDE_BGR;

/* Søkemetode i basen */
#define LC_GEO_SEKV  0x0000   /* Søker sekvensiellt gjennom gruppene */
#define LC_GEO_RTRE  0x0001   /* Bruker R-tre-srukturene i søket */

/* Status */
typedef struct dLC_GEO_STAT {
   unsigned short usMetode; /* Søkemetode (LC_GEO_SEKV eller LC_GEO_RTRE) */ 
   double nvn,nva,ohn,oha; /* Rektangel for søkeområde. */
   unsigned short usLag; /* Lag det skal søkes i. (LC_FRAMGR | LC_BAKGR) */
   LC_BGR Bgr;           /* Aktuell gruppe */
   /* Kjede med resultatet av søket */
   LC_KJEDE_BGR *pForsteKB;
   LC_KJEDE_BGR *pSisteKB;
   LC_KJEDE_BGR *pAktuellKB;
} LC_GEO_STATUS;

   
/*
CH Utvalgstabell
CD .===============================================================================.
CD !Ginfo/!Utvalgs-!Prio-!SOSI-!Utvalgs-!Para-!Ledd-!Start !Slutt !Min !Max !Regel-!
CD !Pinfo !kommando!ritet!navn !metode  !type !nr   !i str !i str !    !    !navn  !
CD !------!--------!-----!-----!--------!-----!-----!------!------!----!----!------!
CD !gi_pi !kommando!sPri-!sosi !metode  !type !ledd !start !slutt !min !max !regel !
CD !      !        !orit-!     !        !     !     !      !      !    !    !      !
CD !  c   !  c     !et   !c[]  ! c      !  c  !  c  !  c   !  c   !c[] !c[] ! c[]  !
CD !      !        !     !     !        !     !     !      !      !    !    !      !
CD !      !        ! s   !     !        !     !     !      !      !    !    !      !
CD !Def. se under  !     !     !Def. se !se   !     !      !      !Def. se nedenfor!
CD !      !        !     !     !nedenfor!nedf.!     !      !      !    !    !      !
CD *-------------------------------------------------------------------------------*
*/

   /* Gruppe, punkt eller pinfo-uvalg */
#define U_GRUPPE   1   /* GRUPPE-UTVALG */
#define U_PUNKT    2   /* PUNKT-UTVALG */
#define U_PINFO    3   /* PINFO-UTVALG */

/* Struktur for statusopplysninger for LC_InitPP / LC_GetPP */
typedef struct {
   short type;         /* LC_GETPP_KP, LC_GETPP_HOYDE, LC_GETPP_KVALITET, LC_GETPP_VANLIG */
   wchar_t pinfo_navn[LC_MAX_SOSINAVN_LEN]; /* Sosi-navn det skal finnes verdi til */
   long curr_punkt;        /* Aktuellt punkt */
   long slutt_punkt;       /* Første punkt etter søkeområdet */
	short neste_tegn;       /* Neste tegn (Ved flere PINFO i punktet) */
} LC_GETPP_STATUS;
/* Bruk:  LC_GETPP_STATUS  pp_stat; */

#define LC_GETPP_VANLIG   0
#define LC_GETPP_KP       1
#define LC_GETPP_HOYDE    2
#define LC_GETPP_KVALITET 3

   /* Konstant for sAktuellPrioritet */
#define LC_OVERSE_PRIORITET  -1

/* Utvalgslinje */
typedef struct sLC_UTVALG_ELEMENT {
   wchar_t kommando;
   wchar_t sosi[LC_MAX_SOSINAVN_LEN];
   wchar_t metode;
   //wchar_t type;
   short type;
   char  ledd;         /* Ledd-nr for flerleddet parameter */
	char  start;        /* Startposisjon i tegnstreng (0=hele) */
   char  slutt;        /* Sluttposisjon i tegnstreng (0=resten) */
   wchar_t *min;
   wchar_t *max;
   struct sLC_UTVALG_ELEMENT *pNesteUE;       /* Neste på dette nivå */
   struct sLC_UTVALG_ELEMENT *pForsteUE; /* Første på nivået under */
   struct sLC_UTVALG_ELEMENT *pSisteUE;  /* Siste på nivået under */
} LC_UTVALG_ELEMENT;

typedef struct sLC_LAG {
   wchar_t  *pszLagNavn;           /* Lagnavn */
   short  sLagAktiv;            /* Lag aktiv for tegning */
   struct sLC_LAG *pNesteLag;   /* Neste lag */
} LC_LAG;

#define LC_UFORANDRET 0
#define LC_ENDRET     1
#define LC_NY         2
#define LC_SLETTET    3

typedef struct sLC_UTVALG {
   wchar_t  *pszNavn;
   short  sPrioritet;
   short  sOriginalPrioritet;
   short sStatus;
   short sTegnes;                 // Flagg for å styre om utvalgsregelen skal brukes ved tegning
   LC_UTVALG_ELEMENT *pForsteUE; /* Første utvalgslinje på øverste nivå */
   LC_UTVALG_ELEMENT *pSisteUE;  /* Siste utvalgslinje på øverste nivå */

   struct sLC_UTVALG *pForrigeU;  /* Forrige utvalg */
   struct sLC_UTVALG *pNesteU;    /* Neste utvalg */
   struct sLC_LAG *pLag;          /* Lag */
   wchar_t *pszRegel;                /* Regel */
} LC_UTVALG;

/* Toppblokk for GRUPPE-, PUNKT- og PINFO-utvalg */
typedef struct dLC_UTVALG_BLOKK {
   short sHoydeBrukt;
   short sTestAllePi;    /* "!" er brukt, må sjekke alle punkt */
   LC_UTVALG *pForsteU;
   LC_UTVALG *pSisteU;
   LC_UTVALG *pAktU;
} LC_UTVALG_BLOKK;

/* Administrasjonsblokk for utvalg */
typedef struct dLC_UT_ADM {
   short            sMaxPrior;       /* Største prioritet */
   LC_UTVALG_BLOKK  Gruppe;
   LC_UTVALG_BLOKK  Punkt;
   LC_UTVALG_BLOKK  Pinfo;
   LC_UTVALG_BLOKK *pAktUB;
	short            sGruppeValgt;
   LC_LAG *pForsteLag;
   LC_LAG *pSisteLag;
} LC_UT_ADM;

/* Administrasjonsblokk for serienummersøk */
typedef struct dLC_SNR_ADM {
   LC_FILADM *pFil;
	long lMinSnr;
   long lMaxSnr;
   long lAktSnr;
} LC_SNR_ADM;

/*
CH Polygonbeskrivelse                        Strukturer for polygonbeskrivelse.
CD 
CD Dette er et sett med strukturer som er kjedet sammen til en komplett
CD beskrielse av en flate. Eksempel på bruk er gitt under $LENKE<LC_POL_GetRef>.
CD
CD
CD   !-----------------!
CD   ! LC_POLYGON      !
CD   !                 !       !-------------------------!
CD   ! !- Omkrets --!  !       !   !-----------------!   !---------------!
CD   ! !LC_POL_OMKR !  !       ! !-!LC_POL_ELEMENT   ! !-!LC_POL_ELEMENT !
CD   ! !            !  !       ! ! ! - Bgr           ! ! ! - Bgr         !
CD   ! !- Siste     !--!-------! ! ! - Snr           ! ! ! - Snr         !
CD   ! !- Første    !--!---------! ! - Retning       ! ! ! - Retning     !
CD   ! !------------!  !           ! - Forrige (NULL)! ! ! - Forrige     !
CD   ! !- Hull ------! !           ! - Neste         !-! ! - Neste (NULL)!
CD   ! !LC_OY_ADM    ! !           !-----------------!   !---------------!
CD   ! !             ! !
CD   ! !- Første øy  !-!-!
CD !-!-!- Siste øy   ! ! !
CD ! ! !-------------! ! !
CD ! !-----------------! !
CD !   !-----------------!
CD !   !                                 !------------------------!
CD !   !                  !------------! !   !-----------------! !---------------!
CD !  !--------------!  !-!LC_POL_OMKR ! ! !-!LC_POL_ELEMENT   !!!LC_POL_ELEMENT !
CD !  !LC_OY_ELEMENT !  ! !            ! ! ! ! - Bgr           !!! - Bgr         !
CD !  !- Omkrets     !--! !- Siste     !-! ! ! - Snr           !!! - Snr         !
CD !  !- Neste       !-!  !- Første    !---! ! - Retning       !!! - Retning     !
CD !  !--------------! !  !------------!     ! - Forrige (NULL)!!! - Forrige     !
CD !                   !                     ! - Neste         !!! - Neste (NULL)!
CD !                   !                     !-----------------! !---------------!
CD !                   !
CD !   !---------------!                 !--------------------------!
CD !   !                  !------------! !   !-----------------!   !---------------!
CD !  !--------------!  !-!LC_POL_OMKR ! ! !-!LC_POL_ELEMENT   ! !-!LC_POL_ELEMENT !
CD !--!LC_OY_ELEMENT !  ! !            ! ! ! ! - Bgr           ! ! ! - Bgr         !
CD    !- Omkrets     !--! !- Siste     !-! ! ! - Snr           ! ! ! - Snr         !
CD    !- Neste (NULL)!    !- Første    !---! ! - Retning       ! ! ! - Retning     !
CD    !--------------!    !------------!     ! - Forrige (NULL)! ! ! - Forrige     !
CD                                           ! - Neste         !-! ! - Neste (NULL)!
CD                                           !-----------------!   !---------------!
*/


/*
 *!--------------------------------------------------------------!
 *! Polygon element (ett for hver gruppe i polygonbeskrivelsen)  !
 *!--------------------------------------------------------------!
 */
typedef struct dLC_POL_ELEMENT {
   LC_BGR Bgr;       // Gruppenummer
   short  sRetning;  // LC_MED_DIG eller LC_MOT_DIG
   long   lSnr;      // Serienummer
   LC_KOORD_2D PktStart;   // Koordinat i starten av elementet (Brukes ikke av FYBA. Til disp.)
   LC_KOORD_2D PktSlutt;   // Koordinat i slutten av elementet (Brukes ikke av FYBA. Til disp.)
   struct dLC_POL_ELEMENT *pNestePE;   // Peker til neste element i polygonet
   struct dLC_POL_ELEMENT *pForrigePE; // Peker til forrige element i polygonet
} LC_POL_ELEMENT;

/*
 *!----------------------------------------------------------------------!
 *! Adm. blokk for polygon (en for hver lukket "del", omkrets eller øy)  !
 *!----------------------------------------------------------------------!
 */
typedef struct dLC_POL_OMKR {
   LC_POL_ELEMENT *pForstePE;
   LC_POL_ELEMENT *pSistePE;
} LC_POL_OMKR;

/*
 *!--------------------------------------------------------------!
 *! Øy (i polygon) element  (en for hver øy)                    !
 *!--------------------------------------------------------------!
 */
typedef struct dLC_OY_ELEMENT{
	LC_POL_OMKR PO;  /* Administrasjonsblokk til kjede som beskriv øyavgrensinga */
   struct dLC_OY_ELEMENT *pNesteOE;  /* Peker til neste øyelement i polygonet */
   struct dLC_OY_ELEMENT *pForrigeOE;  /* Peker til forrige øyelement i polygonet */
} LC_OY_ELEMENT;

/*
 *!------------------------------------------------------------------------!
 *! Adm. blokk for øy (i polygon) element (en for alle øyene i en flate)  !
 *!------------------------------------------------------------------------!
 */
typedef struct dLC_OY_ADM{
   LC_OY_ELEMENT *pForsteOE;   /* Første øy (hull) */
   LC_OY_ELEMENT *pSisteOE;    /* Siste øy  (hull) */
} LC_OY_ADM;

/*
 *!--------------------------------------------------------------!
 *! Adm blokk for Polygon  (En for hver flate, inkl øyer)        !
 *!--------------------------------------------------------------!
 */

typedef struct dLC_POLYGON {
   LC_POL_OMKR HovedPO;      /* Adm blokk for kjede som beskriver ytre avgrensing */
   LC_OY_ADM  OyOA;        /* Kjede som beskriver hull i flata */
} LC_POLYGON;


/* GINFO i minne */
typedef struct dLC_GINFO_BUFFER {
   wchar_t *pszTx;         /* GINFO buffer */
   unsigned long ulOfset[LC_MAX_GINFO]; /* Peker til starten av hver GINFO-linje i antall tegn fra starten */
} LC_GINFO_BUFFER;


// --- Diverse for intern mellomlagring av hele PINFO

// Statusverdier
#define LC_PIBUF_TOM   0
#define LC_PIBUF_OK    1
#define LC_PIBUF_FULL  2
// Ant. tegn i buffer
#define LC_MAX_PIBUF_TEGN  LC_MAX_SOSI_LINJE_LEN
// Ant linjer i tabell med pekere til startposisjoner i strengen
#define LC_MAX_PIBUF_LIN 50


/*
 *!--------------------------------------------------------------!
 *! Systemadministrasjon                                         !
 *!--------------------------------------------------------------!
 */
typedef struct dLC_SYSTEMADM {
   wchar_t   szBaseVer[LC_BASEVER_LEN];  // Versjon og dato for aktuell versjon av FYBA
   wchar_t   szIdxVer[5];                // Indeksfil-versjon

   short  sResPlass;  // Ant. tegn reserve plass bak ny gruppe
   long   lMaxSkriv;  // Max antall skriv uten lagring til SOSI-filen
   long   lAntSkriv;  // Antall skriv siden siste lagring til SOSI
   short  sNGISmodus; // Behandlingsmåte for grupper med NGIS-oppdateringsflagg
   short  sUtvidModus; // Handteringsmåte for utvidelse av SOSI-filer.
   wchar_t   szIdxPath[_MAX_PATH]; //Katalognavn for ny indeks.
   LB_LESEBUFFER BufAdm;         // Lesebuffer mot SOSI-fil-hode

   char *pszTx8Buffer;   // Lavnivå lesebuffer fra SOSI-fil

   LC_NAVNETABELL  SosiNavn;     // Navnetabell (brukes av HO-rutinene)

   /* Div opplysninger om aktuell gruppe */
   short sGrEndra;         // Er aktuell gruppe endra?
   LC_BGR GrId;            // Gruppens identifikasjon
   LC_GRTAB_LINJE *pGrInfo;  // Peker til gruppetabell-linje
   
   /* Aktuell gruppe i minne */
   /* GINFO i minne */
   LC_GINFO_BUFFER Hode;     // Hodebuffer
   unsigned short usHoLen;   // Ant tegn i hodebuffer
   LC_GINFO_BUFFER Ginfo;    // GINFO-buffer
   double *pdAust;    // Øst koordinat
   double *pdNord;    // Nord koordinat
   LB_INFO * pInfo;    // Høyde,KP og PINFO-peker
   wchar_t *pszPinfo;    // PINFO buffer

   /* Søkebuffer for PINFO */
   long lPibufPnr;                 // Punktnummer for data i buffer
   short sPibufAntPi;               // Antall elementer brukt i PIbuf
   wchar_t cPibuf[LC_MAX_PIBUF_TEGN];  // Kopi av PINFO, (for raskere søking)
   short sPibufStatus;              // Status for bruken av cPibuf
   /* Peker til startposisjoner i cPibuf for hver PINFO */
   wchar_t *pcPibufNavn[LC_MAX_PIBUF_LIN];   // Peker til SOSI-navn
   wchar_t *pcPibufVerdi[LC_MAX_PIBUF_LIN];  // Peker til verdi

   unsigned short usMerkRefGr;  // Flagg for å vise om refererte grupper skal merkes ved utvalg

   LC_BASEADM * pForsteBase;   // Peker til første base-adm.
   LC_BASEADM * pAktBase;      // Peker til aktuell base-adm.
} LC_SYSTEMADM;


// Felles streng for feilmeldinger
#define LC_ERR_LEN 300
typedef struct dLC_FEILMELDING {
   short nr;
   short strategi;
   wchar_t tx[LC_ERR_LEN];
} LC_FEILMELDING;


//////////////////////////////////////////////////////////////////////////
class CFyba
{
public:
   CFyba(LcBasetype basetype = LC_BASE);
   ~CFyba(void);


   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fyho.c                       */
   /* ======================================================= */
   short HO_New(const wchar_t *pszFil,short koosys,double origo_a,double origo_n,
      double enhet,double enhet_h,double enhet_d,
      double nv_a,double nv_n,double oh_a,double oh_n);
   short HO_TestSOSI(const wchar_t *pszFil,UT_INT64 *sluttpos);
   wchar_t *HO_GetVal(const wchar_t *pszFil,wchar_t *sosi_navn,short *sett_nr);
   short HO_GetKvalitet(const wchar_t *pszFil,short *psMetode,long *plNoyaktighet,
      short *psSynbarhet,short *psHoydeMetode,long *plHoydeNoyaktighet);
   short HO_GetTrans(const wchar_t *pszFil,short *koosys,double *origo_a,
      double *origo_n,double *enhet,double *enhet_h,double *enhet_d);
   short HO_GetTransEx(const wchar_t *pszFil,unsigned short *pusMaske, LC_TRANSPAR *pTrans);
   short HO_GetOmr(const wchar_t *pszFil,double *nv_a,double *nv_n,double *oh_a,double *oh_n);
   short HO_GetTegnsett(const wchar_t *pszFil,short *psTegnsett);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fylh.c                       */
   /* ======================================================= */
   short LC_GetTrans(short *koosys,double *origo_a,double *origo_n,double *enhet,
      double *enhet_h,double *enhet_d);
   short LC_GetTransEx(unsigned short *pusMaske, LC_TRANSPAR *pTrans);
   short LC_PutTrans(short koosys,double origo_a,double origo_n,
      double enhet,double enhet_h,double enhet_d);
   short LC_PutTransEx(unsigned short usMaske, LC_TRANSPAR *pTrans);
   short LC_GetTegnsett(short *psTegnsett);
   short LC_GetOmr(double *nv_a,double *nv_n,double *oh_a,double *oh_n);
   short LC_PutOmr(double nv_a,double nv_n,double oh_a,double oh_n);
   void  LC_NyttHode(void);
   short LC_TestHode(void);
   short LC_GetBaEnhet(double *enhet);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fylo.c                       */
   /* ======================================================= */
   LC_BASEADM* LC_OpenBase(LcBasetype basetype);
   void			LC_CloseBase(LC_BASEADM *pBase,short s_stat);
   short			LC_OpenSos(const wchar_t *fil,short sModus,short sNyIdx,short sVisStatus,
      LC_FILADM **ppFil, short *o_stat);
   void			LC_CloseSos(LC_FILADM *pFil,short s_stat);
   void        LC_FcloseSos(LC_FILADM *pFil);
   void    LC_SetDefLpfi(short ant_tegn);
   short   LC_InqDefLpfi(void);
   short   LC_InqLag(unsigned short *usLag);
   unsigned short LC_InqFilLag(LC_FILADM *pFil);
   void    LC_SetFilLag(LC_FILADM *pFil,unsigned short usLag);
   short   LC_Backup(LC_FILADM *pFil,const wchar_t *pszBackupPath);
   void    LC_MaxSkriv(long);
   long    LC_InqMaxSkriv(void);
   void    LC_SetNgisModus(short modus);
   short   LC_GetNgisModus(void);
   void    LC_SetUtvidModus(short modus);
   short   LC_GetUtvidModus(void);
   LC_BASEADM* LC_InqCurBase(void);
   void    LC_SelectBase(LC_BASEADM *pBase);
   short   LC_GetBaOm(unsigned short usLag,double *nva,double *nvn,double *oha,double *ohn);
   short   LC_GetFiOm(LC_FILADM *pFil,double *nva,double *nvn,double *oha,double *ohn);
   LC_FILADM * LC_GetFiNr(const wchar_t *fil_navn);
   wchar_t   *LC_GetFiNa(LC_FILADM *pFil);
   short   LC_ErFilBase(const wchar_t *fil);
   short   LC_ErKoordsysLik(void);
   short   LC_ErVertdatumLik(void);
   wchar_t*   LC_GetNgisLag(LC_FILADM *pFil);
   void    LC_SetEndringsstatus(short sStatus);
   short   LC_SetIdxPath(const wchar_t *pszIdxPath);
   const wchar_t* LC_GetIdxPath(void);
   /* Konstanter for intgt.sEndra (aktuell gruppe er endra) */
#define END_UENDRA  0   /* Ikke endra */
#define END_KOPI    1   /* Endra ved totalkopi fra annen gruppe */
#define END_ENDRA   2   /* Endra ved normal Put fra program */

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fylx.c                       */
   /* ======================================================= */
#define GRF_YTRE   0x01  /* Ytre avgrensing */
#define GRF_INDRE   0x02  /* Indre avgrensing, øyer */
#define LC_MED_DIG 0x01  /* Brukes MED dig retning */
#define LC_MOT_DIG 0x02  /* Brukes MOT dig retning */
#define GRF_START_OY    0x04  /* Første gruppe i øy */
#define GRF_SLUTT_OY    0x08  /* Siste gruppe i øy */

   long   LC_InqAntRef(void);
   void   LC_InitGetRefFlate(LC_GRF_STATUS *pGrfStat);
   long   LC_GetRefFlate(LC_GRF_STATUS *RefStat,unsigned short sHent,long *ref_array,
      unsigned char *ref_status,long max_ref);
   short  LC_PutRef(long *ref_array,long ant_ref);
   long   LC_GetRef(long *ref_array,long max_ref,short *gilin,short *refpos);

   short  LC_GetBuePar(short buff_retning, double *as, double *ns, double *radius,
      double *fi, double *dfi, short *sfeil);
   short  LC_GetBue(short retning,double *a1,double *n1,double *a2,double *n2,
      double *radius,short *storbue);
   short  LC_GetBuep(short retning,double *a1,double *n1,double *a2,double *n2,
      double *a3,double *n3);
   short  LC_GetSirkel(double *as,double *ns,double *radius);
   short  LC_GetSirkelp(double *a1,double *n1,double *a2,double *n2,
      double *a3,double *n3);

   void   LC_GetCurEnhet(LC_FILADM * pFil,short *nivaa, double *enhet,
      double *enhet_h, double *enhet_d);
   short  LC_UpdateGiEnhet(LC_FILADM *pFil,double enhet,double enhet_h,double enhet_d);

   short  LC_GetKvalitet(short *psMetode,long *plNoyaktighet,short *psSynbarhet,
      short *psHoydeMetode,long *plHoydeNoyaktighet);
   short  LC_GetCurKvalitet(LC_FILADM *pFil,short *nivaa,long pnr,
      short *psMetode,long *plNoyaktighet,short *psSynbarhet,
      short *psHoydeMetode,long *plHoydeNoyaktighet);
   short  LC_UpdateGiKvalitet(LC_FILADM *pFil,short sMetode,long lNoyaktighet,
      short sSynbarhet,short sHoydeMetode,long lHoydeNoyaktighet);
   short  LC_UpdatePiKvalitet(LC_FILADM *pFil,long pnr,short sMetode,long lNoyaktighet,
      short sSynbarhet,short sHoydeMetode,long lHoydeNoyaktighet);

   wchar_t * LC_GetGP(const wchar_t *,short *,short);
   short LC_PutGP(const wchar_t *sosi_navn,const wchar_t *verdi,short *linje_nr);
   short LC_AppGP(const wchar_t *sosi_navn,const wchar_t *verdi,short *linje_nr);
   short  LC_UpdateGP(short linje_nr,const wchar_t *sosi_navn,const wchar_t *verdi);
   void   LC_InitPP(wchar_t *sosi_navn,long forste_punkt,long siste_punkt,
      LC_GETPP_STATUS *pp_status);
   wchar_t * LC_GetPP(long *punkt,LC_GETPP_STATUS *pp_stat);
   wchar_t * LC_GetPiVerdi(const wchar_t *pszSosiNavn,long sPnr,short *sSettNr);
   short  LC_TestPi(long punkt_nr,short sTestHoyde);
   short  LC_FinnKp(long *forste_punkt,long siste_punkt,short *kp);
   long   LC_GetSn(void);
   void   LC_PutSn(long snr);
   wchar_t * LC_GetGi(short gilin);
   void   LC_PutGi(short gilin,const wchar_t *ginfo);
   void   LC_GetTK(long pkt,double *aust,double *nord);
   void   LC_GetArrayTK(short retning,long max_antall,long fra_punkt,
      double *aust,double *nord,long *antall);
   void   LC_GetArrayTH(short retning,long max_antall,long fra_punkt,
      double *hoyde,long *antall);
   void   LC_PutTK(long punkt_nr,double aust,double nord);
   double LC_GetTH(long pkt);
   void   LC_PutTH(long pkt,double hoyde);
   double LC_GetHoyde(long punkt_nr);
   void   LC_PutTD(long punkt_nr, double dybde);
   double LC_GetTD(long punkt_nr);
   double LC_GetDybde(long punkt_nr);
   wchar_t * LC_GetPi(long pkt);
   short  LC_PutPi(long pkt,const wchar_t *pi);
   short  LC_GetKp(long pkt);
   void   LC_PutKp(long pkt,short kp);
   double LC_BerAreal(void);
   double LC_BerLengde(void);
   bool   LC_BerLengde3D(double *skraa_lengde);
   double LC_BerAvgrensLengde(void);
   double LC_BerIndreAvgrensLengde(void);
   double LC_BerYtreAvgrensLengde(void);
   void   LC_DumpTab(void);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fylb.c                       */
   /* ======================================================= */
   short  LC_GetGrFi(LC_FILADM **ppFil);
   void   LC_InitNextFil(LC_FILADM **ppFil);
   short  LC_NextFil(LC_FILADM **ppFil,unsigned short usLag);
   void   LC_InitNextBgr(LC_BGR * pBgr);
   short  LC_NextBgr(LC_BGR * pBgr,unsigned short usLag);
   short  LC_InqAntFiler(unsigned short usLag);
   short  LC_GetGrNr(LC_BGR * pBgr);
   const wchar_t *LC_GetObjtypeBgr(LC_BGR * pBgr);
   short  LC_GetGrPara(short *ngi,long *nko,unsigned short *info);
   short  LC_GetGrParaBgr(LC_BGR * pBgr,short *ngi,long *nko,unsigned short *info);
   short  LC_RsGr(short *rstat,LC_FILADM *pFil,short *ngi,long *nko,
      unsigned short *info,long *gml_snr);
   short  LC_RsHode(LC_FILADM *pFil,short *ngi,long *nko,unsigned short *info);
   void   LC_WsGr(LC_FILADM *pFil);
   void   LC_WsGrPart(LC_FILADM *pFil,long fra_punkt,long antall);
   short  LC_EndreHode(LC_FILADM *pFil);
   short  LC_RxGr(LC_BGR * pBgr,short les_sosi,short *ngi,long *nko,unsigned short *info);
   short  LC_WxGr(short k_stat);
   void   LC_RoundKoord(void);
   long   LC_FiLastGr(LC_FILADM *pFil);
   short  LC_NyGr (LC_FILADM *pFil,wchar_t *sosi,LC_BGR * pBgr,long *snr);
   short  LC_CopyGr (LC_BGR * pBgr,short ngis,short *ngi,long *nko,unsigned short *info);
   short  LC_CopyCoord(LC_BGR * pBgr,short retning,long til_linje,short *ngi,
      long *nko,unsigned short *info);
   short  LC_DelGr(void);
   short  LC_SnuGr(void);
   short  LC_SplittGr (long p1,long p2,LC_BGR * pBgr2);
   short  LC_SammenfoyGr(LC_BGR * pFraBgr,short retning,short plassering,short metode,
                         short *ngi,long *nko,unsigned short *info);
   void   LC_ErstattReferanse (LC_FILADM *pFil,long lGmlSnr,long lNyttSnr, bool bSammeRetning);
   bool   LC_ErSammeGr(LC_BGR *pBgr1, LC_BGR *pBgr2);


   /* Konstanter for SammenfoyGr() */
#define LC_SG_FORRAN 1   /* Heng den andre gruppen inn forran første koordinat */
#define LC_SG_BAK    2   /* Heng den andre gruppen inn etter siste koordinat */
#define LC_SG_BEHOLD 3   /* Begge sammenføyings-punktene beholdes */
#define LC_SG_FJERN  4   /* Bare det ene av sammenføyings-punktene beholdes */


   short  LC_InsGiL (short linje, short antall);
   short  LC_AppGiL (void);
   short  LC_DelGiL (short linje, short antall);
   short  LC_DelGiNavn(wchar_t *pszEgenskapNavn);
   long  LC_InsKoL (long linje, long antall);
   long  LC_AppKoL (void);
   long  LC_DelKoL (long linje, long antall);
   void   LC_Save (void);
   void   LC_OppdaterEndret(short endring);
#define O_GINFO   0  /* Oppdater tabeller i forhold til GINFO */
#define O_ENDRET  1  /* Merk for endret og oppdat. tab. */
#define O_SLETTET 2  /* Merk for slettet og oppdat. tab. */

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fyli.c                       */
   /* ======================================================= */
   /* Brukttabellen */
   void  LC_SetBt(LC_BGR * pGr,short kolonne);
   void  LC_ClrBt(LC_BGR * pGr,short kolonne);
   short LC_GetBt(LC_BGR * pGr,short kolonne);
   void  LC_EraseBt(short fra_kol,short til_kol);
   void  LC_CopyBt(short fra_kol,short til_kol,short operasjon);
   void  LC_SetModusMerk(unsigned short usModus);
   long  LC_MerkGr(short sKolonne,short sBryter);
   void  LC_ClrPrioritet(LC_BGR * pGr,short kolonne);
   void  LC_SetPrioritet(LC_BGR * pGr,short kolonne);
   short LC_InqPrioritet(LC_BGR * pGr,short kolonne);
   void  LC_ErasePrioritet(LC_BGR * pGr);
   void  LC_EraseAllPrioritet(LC_FILADM *pFil);
   void  LC_DumpBt(const wchar_t *pszMelding);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fyld.c                       */
   /* ======================================================= */
   void  LC_DelIdx(const wchar_t *szSosFil);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fyta.c                       */
   /* ======================================================= */
   /* Tabellsystemet */
   short LC_InitTabel(const wchar_t *pszKatal, long n_rec,short rec_len,void *buffer);
   short LC_PutTabel(long linje,void *buffer);
   short LC_GetTabel(long linje,void *buffer);
   void  LC_CloseTabel(void);
   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fyln.c                       */
   /* ======================================================= */
   wchar_t *LC_FormatterKvalitet(short sMetode,long lNoyaktighet,short sSynbarhet,
                                 short sHoydeMetode,long lHoydeNoyaktighet);
   short LC_FinnNivo(const wchar_t *pszNavn);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fyls.c                       */
   /* ======================================================= */
   void   LC_SBSn(LC_SNR_ADM *pSnrAdm,LC_FILADM *pFil,long lMinSnr,long lMaxSnr);
   short  LC_MoveSn(LC_SNR_ADM *pSnrAdm,long lSnr,LC_BGR * pBgr);
   short  LC_FiSn(LC_FILADM *pFil,long lSnr,LC_BGR * pBgr);
   void   LC_FiArraySn(LC_FILADM *pFil,short antall,long *snr,long *bgr);
   long   LC_FASn(LC_SNR_ADM *pSnrAdm);
   short  LC_FFSn(LC_SNR_ADM *pSnrAdm,LC_BGR *pBgr);
   short  LC_FNSn(LC_SNR_ADM *pSnrAdm,LC_BGR *pBgr);
   short  LC_FPSn(LC_SNR_ADM *pSnrAdm,LC_BGR *pBgr);
   short  LC_FLSn(LC_SNR_ADM *pSnrAdm,LC_BGR *pBgr);
   short  LC_FFSnBt(LC_SNR_ADM *pSnrAdm,short kolonne,LC_BGR *pBgr);
   short  LC_FNSnBt(LC_SNR_ADM *pSnrAdm,short kolonne,LC_BGR *pBgr);
   short  LC_FPSnBt(LC_SNR_ADM *pSnrAdm,short kolonne,LC_BGR *pBgr);
   short  LC_FLSnBt(LC_SNR_ADM *pSnrAdm,short kolonne,LC_BGR *pBgr);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fylr.c                       */
   /* ======================================================= */
   short  LC_GetGrWin(LC_BGR *pBgr,double *nva,double *nvn,double *oha,double *ohn);
   /* Flate-søk */
   void   LC_SBFlate(LC_GEO_STATUS *pGeoStat,unsigned short usLag,
      double nv_a,double nv_n,double oh_a,double oh_n);
   short  LC_FFFlate(LC_GEO_STATUS *pGeoStat,LC_BGR * pBgr);
   short  LC_FNFlate(LC_GEO_STATUS *pGeoStat,LC_BGR * pBgr);
   /* Geografisk søk primær gruppe */
   void   LC_SBGeo(LC_GEO_STATUS *pGeoStat,unsigned short usLag,
      double nv_a,double nv_n,double oh_a,double oh_n);
   short  LC_FFGeo(LC_GEO_STATUS *pGeoStat,LC_BGR *pBgr);
   short  LC_FNGeo(LC_GEO_STATUS *pGeoStat,LC_BGR *pBgr);
   long   LC_FAGeo(LC_GEO_STATUS *pGeoStat);
   short LC_FFGeoFil(LC_GEO_STATUS *pGeoStat,LC_FILADM *pFil,LC_BGR *pBgr);
   short LC_FNGeoFil(LC_GEO_STATUS *pGeoStat,LC_FILADM *pFil,LC_BGR *pBgr);
   void LC_AvsluttSok(LC_GEO_STATUS *pGeoStat);

   /* Vindustest mm */
   short  LC_WTst(double nva,double nvn,double oha,double ohn);
   short  LC_PTst(double a,double n);
   short  LC_PTstOmkrets(double a,double n);

   /* Debugformål */
   void LC_DumpGeoRtre(LC_FILADM *pFil);


   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fylu.c                       */
   /* ======================================================= */
   LC_UT_ADM *  LC_OpenQuery(void);
   void    LC_CloseQuery(LC_UT_ADM *pUtAdm);
   //short   LC_PutQueryLine(LC_UT_ADM *pUtAdm,wchar_t *qulin);
   short   LC_PutQueryLine(LC_UT_ADM *pUtAdm,const wchar_t *qulin,short sType);
   void    LC_PutQueryRegel(LC_UTVALG *pU,const wchar_t *navn);
   short   LC_LesUtvalg(LC_UT_ADM *pUtAdm,const wchar_t *pszKomFil);
   wchar_t   *LC_GetUtRegelNavn(LC_UT_ADM *pUtAdm,short *ist);
   wchar_t   *LC_GruppeUtvalg(LC_UT_ADM *pUtAdm,short sPrior,short *sstat,wchar_t **regelnavn);
   short   LU_GiTestUtvalg(LC_UT_ADM *pUtAdm,LC_UTVALG * pU);
   void    LC_PunktUtvalg(LC_UT_ADM *pUtAdm,short sPrior,short *psStat,long lPnr,wchar_t **ppszRegel);
   LC_UTVALG *LC_FinnPinfoUtvalg(LC_UT_ADM *pUtAdm,const wchar_t *pszNavn);
   short   LC_PiTestUtvalg(LC_UT_ADM *pUtAdm,LC_UTVALG * pU,long lPnr);
   short   LC_GiQuery(LC_UT_ADM  *pUtAdm);
   long    LC_FAGiQuery(LC_UT_ADM *pUtAdm,unsigned short usLag);
   long    LC_FAPiQuery(LC_UT_ADM *pUtAdm,unsigned short usLag);
   long    LC_FAGiKombinertFlateQuery(LC_UT_ADM *pUtAdmFlate,LC_UT_ADM *pUtAdmOmkrets,
      unsigned short usLag,short sMetode);
   short   LC_QueryGP(wchar_t *qulin,unsigned short iniv,unsigned short *univ,short *ulin,wchar_t **para);
   short   LC_InqMaxPrioritet(LC_UT_ADM *pUA);
   short   LC_TestPrioritetBrukt(LC_UT_ADM *pUtAdm,short sPrioritet);
   void    LC_UtvalgPrioritet(LC_UT_ADM *pUtAdm);
   void    LC_LoggPrioritetUtvalg(LC_UT_ADM *pUtAdm);
   void    LU_FrigiUE(LC_UTVALG_ELEMENT *pUE);
   short   LU_AppUE (LC_UTVALG *pU,short sNiv,const wchar_t *pszTx);
   void    LC_PutLag(LC_UT_ADM *pUtAdm,LC_UTVALG *pU,const wchar_t *navn);
   bool    LC_ErReferert(void);
   long    LC_ErReferertFraAntall(void);
   bool    LC_ErReferertFraReadOnly(void);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fylp.c                       */
   /* ======================================================= */
   void LC_POL_InitPolygon(LC_POLYGON *pPolygon);
   void LC_POL_FrigiPolygon(LC_POLYGON *pPolygon);

   void LC_POL_InitOmkrets(LC_POL_OMKR *pPO);
   LC_POL_ELEMENT * LC_POL_LeggTilGruppeOmkrets(LC_POL_OMKR *pPO,LC_BGR *pBgr,short sRetning,long lSnr);
   void LC_POL_FjernSisteGruppeOmkrets(LC_POL_OMKR *pPO);
   void LC_POL_FjernGruppeOmkrets(LC_POL_OMKR *pPO, LC_POL_ELEMENT *pPE);
   void LC_POL_FrigiOmkrets(LC_POL_OMKR *pPO);

   void LC_POL_InitOy(LC_OY_ADM *pOA);
   void LC_POL_FrigiAlleOyer(LC_OY_ADM *pOA);
   void LC_POL_FjernOy(LC_OY_ADM *pOA,LC_OY_ELEMENT *pOE);
   void LC_POL_LeggTilOy(LC_OY_ADM *pOA,LC_POL_OMKR *pPO);

   short LC_POL_TestBrukt(LC_POLYGON *pPolygon, LC_BGR *pBgr,  bool bSjekkIndreGrense);
   short LC_POL_PutRef(LC_POLYGON *pPolygon);
   void  LC_POL_GetRef(LC_POLYGON *pPolygon);
   void  LC_POL_GetRefOmkrets(LC_POL_OMKR *pPO);
   short LC_POL_PTst(LC_POLYGON *pPolygon,double a,double n);
   short LC_POL_PTstOmkrets(LC_POL_OMKR *pPO,double a,double n);
   short LC_POL_OmkretsSkjaering(LC_POL_OMKR *pPO,double a,double n);
   void  LC_POL_Box(LC_POL_OMKR *pPO,double *nva,double *nvn, double *oha,double*ohn);
   short LC_ErLinjeRefLin(wchar_t *pszSosiLin, short sRefLin);
   short LC_POL_OmkretsRetning(LC_POL_OMKR *pPO);
   void  LC_POL_FjernLoop(LC_POL_OMKR *pPO);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fyln.c                       */
   /* ======================================================= */
   const wchar_t *LC_GetElementNavn(LC_FILADM *pFil,short sNavnNr,bool *bBrukt);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fyba.c                       */
   /* ======================================================= */
   wchar_t *LC_InqVer(void);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fyle.c                       */
   /* ======================================================= */
   void LC_SetErrorHandler(void (*f) (short,const wchar_t*,const wchar_t*));
   void LC_SetStartMessageHandler(void (*f)(const wchar_t*));
   void LC_SetShowMessageHandler(void (*f)(double));
   void LC_SetEndMessageHandler(void (*f)(void));
   void LC_SetCancelHandler(short (*f)(void));




private:
   // Systemadministrasjon
   LC_SYSTEMADM Sys;

   LC_FEILMELDING err;

   // Returstreng
   wchar_t retur_str[LC_MAX_SOSI_LINJE_LEN];

   /* Tabellsystem */
   short fytab_open;

   struct {
      wchar_t szPath[_MAX_PATH];
      FILE   *fpek;
      size_t recl;
      short  modus;
      long   cur_lin;
   } fytab;


   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fyho.c                       */
   /* ======================================================= */
   short ho_TestFyllKommentar(const char *pszTx);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fylo.c                       */
   /* ======================================================= */
   void   LO_CloseSos(LC_BASEADM *pBase);
   void   LO_ReopenSos(LC_FILADM *pFil);
   void   LO_BeFt(LC_FILADM *pFil);
   void   LO_TestFilpeker(LC_FILADM *pFil,wchar_t *pszRutineNavn);
   LC_BASEADM * LO_AppBaseAdm(void);
   short        LO_DelBaseAdm(LC_BASEADM * pBase);
   LC_FILADM  * LO_AppFilAdm(LC_BASEADM * pBase);
   void         LO_DelFilAdm(LC_FILADM *pFil);
   short        LO_OpenKladd(LC_BASEADM * pBase);
   short        LO_InklSos(LC_FILADM *pFil,short vising);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fylb.c                       */
   /* ======================================================= */
   void    LB_Save(LC_FILADM *pFil);
   void    LB_NyRp(void);
   short   LB_RGru(LC_FILADM *pFil,UT_INT64 start,UT_INT64 *slutt);
   void    LB_Swap(void);
   wchar_t *LB_FormaterEnhet(wchar_t *streng,short sStrengMaxLen,wchar_t *SosiNavn,double enhet);
   void    LB_ClGr (void);
   short   LB_WGru (short strategi,long fra_punkt,long antall,
      LC_FILADM *pFil,UT_INT64 ffipos,UT_INT64 *lfipos);
   short   LB_Plass(LC_FILADM *pFil, UT_INT64 start, UT_INT64 *neste);
   short   LB_WriteLine (FILE *fil,short sTegnsett,wchar_t *tx);
   short   LB_WriteLine (FILE *fil, char *tx);
   short   LB_GetSet(FILE *fil,LB_LESEBUFFER *pLb,LC_NAVNETABELL * pNavn);
   void    LB_WriteBlank(FILE *fil,short sTegnsett,UT_INT64 ltilpos);
   wchar_t *LB_GetParameter(LB_LESEBUFFER *plb);
   short    LB_TestFyll(const wchar_t *pszTx);
   void     LB_WriteRb(void);
   short    LB_FlyttGrTilSlutt(LC_FILADM *pFil, UT_INT64 start, UT_INT64 *neste);
   short    LB_RensOmkrets(LC_POL_OMKR * pPO,long lAktSnr,long lFraSnr);
   void     LR_TestEndreBuepTilKurve(double dDeltaFi);
   size_t   LB_EffektivStrenglengde(short sTegnsett, wchar_t *tx);
   void     LB_FyllBuffer (FILE *fil,LB_LESEBUFFER *pLb);


   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fyli.c                       */
   /* ======================================================= */
   short   LI_TestIdx(wchar_t *szSosFil);
   short   LI_OpenInit(LC_FILADM *pFil);
   short   LI_OpenRead(LC_FILADM *pFil);
   void    LI_Close(LC_FILADM *pFil,short s_stat);
   void    LI_SaveAdm(LC_FILADM *pFil);

   LC_R_LEAF * LI_GetGeo(LC_FILADM *pFil,long linje);
   /*  void   LI_PutGeo(LC_FILADM *pFil,long linje,LC_GEOSOK_BOKS *geop); */

   long   LI_GetSnr(LC_FILADM *pFil,long lSnr);
   void   LI_PutSnr(LC_FILADM *pFil,long lSnr,long lGrNr);

   LC_GRTAB_LINJE * LI_GetGrt(LC_FILADM *pFil,long linje);
   LC_GRTAB_LINJE * LI_AppGrt(LC_FILADM *pFil,long linje);

   unsigned long   LI_GetBt(LC_FILADM *pFil,long linje);
   void   LI_PutBt(LC_FILADM *pFil,long linje,unsigned long bt_val);

   void    LI_SetBt(LC_FILADM *pFil,long lGrNr,short kolonne);
   void    LI_ClrBt(LC_FILADM *pFil,long lGrNr,short kolonne);
   short   LI_InqBt(LC_FILADM *pFil,long lGrNr,short kolonne);
   void    LI_EraseBt(short fra_kol,short til_kol);

   void LI_WriteRb(LC_FILADM *pFil, UT_INT64 n64FilPos,
      wchar_t *pszGi, unsigned long ulGiLen,
      double *pdAust, double *pdNord,
      LB_INFO * pInfo, long lNko,
      wchar_t *pszPi, unsigned long ulPiLen);
   void LI_ReadRb(LC_FILADM *pFil, UT_INT64 n64FilPos,
      wchar_t *pszGi, unsigned long ulGiLen,
      double *pdAust, double *pdNord,
      LB_INFO * pInfo, long lNko,
      wchar_t *pszPi, unsigned long ulPiLen);
   void LI_ReadCoordRb(LC_FILADM *pFil, UT_INT64 n64FilPos, unsigned long ulGiLen,
      double *pdAust, double *pdNord,
      LB_INFO * pInfo, long lNko,
      wchar_t *pszPi, unsigned long ulPiLen);
   long LI_BerBufferLen(unsigned long ulGiLen,long lNko,unsigned long ulPiLen);

   FILE *LI_OpenIdxFil(LC_FILADM *pFil, const wchar_t *pszNavn, const wchar_t *pszType);
   FILE *LI_OpenAdm(LC_FILADM *pFil);
   FILE *LI_OpenGrt(LC_FILADM *pFil);
   FILE *LI_OpenGeo(LC_FILADM *pFil);
   FILE *LI_OpenSnr(LC_FILADM *pFil);
   FILE *LI_OpenBt(LC_FILADM *pFil);
   void  LI_OpenRb(LC_FILADM *pFil,UT_INT64 n64FilPos,short sModus);
   short LI_ReadAdm(LC_FILADM *pFil);
   void  LI_CreateIdx(LC_FILADM *pFil);
   void  LI_FrigiIdx(LC_FILADM *pFil);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fyln.c                       */
   /* ======================================================= */
   void  LN_InitTab(LC_NAVNETABELL *pLn);
   short LN_Enhet(LC_NAVNETABELL *pLn,wchar_t *ginfo_linje);
   short LN_EnhetHoyde(LC_NAVNETABELL *pLn,wchar_t *ginfo_linje);
   short LN_EnhetDybde(LC_NAVNETABELL *pLn,wchar_t *ginfo_linje);
   short LN_TestOy(wchar_t *ginfo_linje);
   short LN_PakkNavn(LC_NAVNETABELL *pLn,wchar_t *navn,short *navn_nr,
      short *ant_par);
   short LN_FinnNavn(LC_NAVNETABELL *pLn,wchar_t *navn,short *navn_nr);
   wchar_t *LN_GetNavn(LC_NAVNETABELL *pLn,short navn);
   wchar_t *LN_VisNavn(LC_NAVNETABELL *pLn,short navn);
   void  LN_TolkKvalitet(wchar_t *pszParameter,short *psMetode,long *plNoyaktighet,
      short *psSynbarhet,short *psHoydeMetode,long *plHoydeNoyaktighet);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fyls.c                       */
   /* ======================================================= */
   void    LS_Indx(void);
   void    LS_PutSn(LC_FILADM *pFil,long lGrNr,long lSnr);
   wchar_t *  LS_VisSn(LC_FILADM *pFil,long lin);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fylu.c                       */
   /* ======================================================= */
   void  LU_FrigiUtvalg(LC_UTVALG *pU);
   void  LU_DelLastQuery(LC_UTVALG_BLOKK *pUB);
   short LU_TolkUtvalgslinje(LC_UTVALG_ELEMENT * pUE,const wchar_t *pszTx);
   short LU_PiTestDelutvalg(LC_UT_ADM * pUtAdm,LC_UTVALG_ELEMENT * pUE,long lPnr);
   short LU_PiTestLinje(LC_UTVALG_ELEMENT * pUE,long lPnr);
   //short LU_GiTestUtvalg(LC_UT_ADM * pUtAdm,LC_UTVALG *pU);
   short LU_GiTestDelutvalg(LC_UT_ADM * pUtAdm,LC_UTVALG_ELEMENT * pUE);
   short LU_GiTestLinje(LC_UT_ADM * pUtAdm,LC_UTVALG_ELEMENT * pUE,
                        short *gilin,wchar_t **apara);
   short LU_ParaTest(LC_UTVALG_ELEMENT * pUE,wchar_t *para,wchar_t *pszAktPara,short sMaxLen);
   void  LU_JustPara(wchar_t *para,short ledd,short start,short slutt,
                     wchar_t *akt_para,short max_len);
   short LU_LesULinje(FILE *pKomFil,short sMaxTxLen,wchar_t *pszTx,
                      short *psNiv);
   void LU_AppUtvalg (LC_UTVALG_BLOKK *pUtBlokk,wchar_t *pszNavn);
   void LU_PakkPrioritet(LC_UT_ADM * pUtAdm);
   void LU_HuskPrior(short *NyPrior,short *sAntPrior,short sPrior);
   void LU_SjekkDatatype(wchar_t *pszVerdi,wchar_t szMetode,short *sType);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fylr.c                       */
   /* ======================================================= */
   /* Indeksoppbygging */
   void  LR_Indx(void);
   void  LR_IndxFlate(void);
   short LR_PTstGruppe(LC_BGR * pBgr,double a,double n);
   void LR_R_Delete(LC_R_LEAF * pCL);
   LC_R_LEAF * LR_InsertGeo(LC_FILADM *pFil,long lNr,LC_BOKS * pB);
   void LR_R_FrigiGren(LC_R_NODE *pRN);
   LC_R_LEAF * LR_R_Insert(long lGrNr,LC_BOKS * pB,LC_R_NODE * pFar,LC_R_NODE * pRN,LC_R_NODE * *ppNyRN);
   LC_R_NODE * LR_R_CreateRNode( LC_R_NODE * pFar,short sSonType);
   LC_R_LEAF * LR_R_CreateRLeaf(long lGrNr, LC_BOKS * pB,LC_R_NODE * pFar);
   void LR_R_BoksSum(LC_BOKS * pB1,LC_BOKS * pB2);
   double LR_BoksDeltaArealSum(LC_BOKS * pB1,LC_BOKS * pB2);
   void LR_LeggTilKB(LC_GEO_STATUS * pGeoStat,LC_FILADM *pFil,long lNr);
   short LR_R_BoksTestIntersect(LC_BOKS * pB1,LC_BOKS * pB2);
   void LR_R_SjekkNode(LC_GEO_STATUS * pGeoStat,LC_BOKS * pB,LC_FILADM *pFil,LC_R_NODE * pRN);
   void LR_R_SjekkNodeFlate(LC_GEO_STATUS * pGeoStat,LC_BOKS * pB,LC_FILADM *pFil,LC_R_NODE * pRN);
   void LR_VelgMetode(LC_GEO_STATUS * pGeoStat);

   //#ifdef TEST
   //#include <string.h>
   // void LR_R_DumpNode(LC_R_NODE * pRN, int iNivo);
   void LR_R_DumpLov(LC_R_LEAF * pRL, LC_FILADM *pDumpFil, int iNivo, double dA, double dN,long *plAntBarn);
   void LR_R_DumpNode(LC_R_NODE * pRN, LC_FILADM *pDumpFil, int iNivo, double dA, double dN, double dLengde,long *plAntBarn);
   //#endif

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fylx.c                       */
   /* ======================================================= */
   void   LX_PutSn(long snr);
   void   LX_CreGiPeker(LC_GINFO_BUFFER *pGinfo,short ngi);
   wchar_t *LX_GetGi(short lin_nr);
   long LX_GetRefOmkrets(LC_GR_STATUS *pGRS,long *ref_array,
                         unsigned char *ref_status,long max_ref);
   long LX_GetRefOy(LC_GRF_STATUS * pGS,long *ref_array,
                    unsigned char *ref_status,long max_ref);
   void LX_CreatePibuf(long lPnr);
   double LX_ArealGruppe(LC_BGR * pBgr,short retning);
   void LX_PutGi(short lin_nr, const wchar_t *szGinfo);
   void LX_VisUlovligPnr(wchar_t *pszRutine, long lPnr);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fylp.c                       */
   /* ======================================================= */

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fylh.c                       */
   /* ======================================================= */
   wchar_t* LH_GetNgisLag(void);

   /* ======================================================= */
   /*  Funksjonsdefinisjoner for fyho.c                       */
   /* ======================================================= */
   void  ho_New(FILE *fil,short koosys,double origo_a,double origo_n,
                double enhet,double enhet_h,double enhet_d,
                double nv_a,double nv_n,double oh_a,double oh_n);
   short ho_TestSOSI(FILE *fil,UT_INT64 *sluttpos);
   wchar_t *ho_GetVal(FILE *fil,wchar_t *sosi_navn,short *sett_nr);
   short ho_GetKvalitet(FILE *fil,short *psMetode,long *plNoyaktighet,
                        short *psSynbarhet,short *psHoydeMetode,long *plHoydeNoyaktighet);
   short ho_GetTrans(FILE *fil,short *koosys,double *origo_a,
                     double *origo_n,double *enhet,double *enhet_h,double *enhet_d);
   short ho_GetTransEx(FILE *pFil,unsigned short *pusMaske, LC_TRANSPAR * pTrans);
   short ho_GetOmr(FILE *fil,double *nv_a,double *nv_n,double *oh_a,double *oh_n);
   short ho_GetTegnsett(FILE *pfFil,short *psTegnsett);
   short ho_FinnHode(FILE *pFil, UT_INT64 *lHodepos);

};


/* ======================================================= */
/*  Funksjonsdefinisjoner for fyle.cpp                     */
/* ======================================================= */
SK_EntPnt_FYBA short LC_StrError(short feil_nr,wchar_t **feilmelding);


//////////////////////////////////////////////////////////////////////////
//
// Når FYBA brukes som LIB må følgende funksjoner finnes definert
//
//////////////////////////////////////////////////////////////////////////
void  LC_Error (short ifeilnr, const wchar_t logtx[], const wchar_t vartx[]);
void  LC_StartMessage(const wchar_t *cfil);
void  LC_ShowMessage(double prosent);
void  LC_EndMessage(void);
short LC_Cancel(void);

//////////////////////////////////////////////////////////////////////////
//
// Når FYBA brukes som DLL må følgende funksjoner finnes definert.
// Eksempel finnes i filen Fyba_Callback.cpp
// Funksjonene aktiveres med LC_SetXxxxxHandler rutinene.
// Hvis disse ikke blir aktivert brukes enkle rutiner som ligger i DLL-en.
//
//////////////////////////////////////////////////////////////////////////
void   LC_ErrorHandler (short ifeilnr, const wchar_t* logtx, const wchar_t* vartx);
void   LC_StartMessageHandler(const wchar_t *cfil);
void   LC_ShowMessageHandler(double prosent);
void   LC_EndMessageHandler(void);
short  LC_CancelHandler(void);


