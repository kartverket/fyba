/* == AR-991012 =========================================================== */
/*   KARTVERKET  -  FYSAK-PC                                          */
/*   Fil: fybax.h                                                           */
/*   Innhold: Interne prototyper for fysak-pc                               */
/*            Def. av typer og bufferstørrelser                             */
/*            Definisjon av filtabellen, ringbuffer mm.                     */
/* ======================================================================== */

#pragma once

#include "fyba.h"


/* ======================================================= */
/*  Definerer konstanter                                   */
/* ======================================================= */
#ifndef _FYBAX_DEFINED

#   define  FYBA_IDENT  L"FYBA - H1.1  2013-02-20"
#   define  FYBA_INDEKS_VERSJON  L"H002"
#   define  FYBA_SOSI_VERSJON  400

 /* Definerer PI */
//#   ifndef PI
////#      define PI  (3.14159265358979324)
//#      define PI  (3.14159265358979323846)
//#   endif

/* --- Konstant for testing av likhet på desimaltall -- */
#define LC_ACCY  1.0E-9


 /* Basetype */
 /* #define LC_START_KLADD       2  */  /* Åpning av kladdebase */
 /* (LC_BASE og LC_KLADD er definert i FYBA.H) */

 /* Filtabell */
 /* #define NO_OPEN_SOS   -1 */  /* Ingen åpen SOSI-fil */

 /* Kommando til LB_Wgru */
#   define KONTROLLER_PLASS    1   /* Bare kontroller plass, (ikke skriv SOSI) */
#   define SKRIV_VANLIG        2   /* Skriv til SOSI, vanlig */
#   define SKRIV_SISTE         3   /* Skriv til SOSI, med .SLUTT og bytepointer */

  /* Diverse */
#   define NYTT_SNR              -1L  /* Startverdi for akt.snr i filtabellen */

#   define NY_SOSI_ST            -1L  /* Start.sosi når gruppe ikke er skrevet */
#   define NY_RB_ST              -1L  /* Start.rb når gruppe ikke er skrevet */
#   define NY_UI_ST              -1L  /* Start.UI når gruppe ikke er skrevet */


#   define LI_LESEFEIL           -1  // Filen er åpen i annet program 
#   define LI_OPPTATT            -2  // Filen er åpen i annet program 
#   define LI_FEIL_INDEKSVERSJON -3  // Feil indeksversjon 
#   define LI_FEIL_STORRELSE     -4  // Feil størrelse på SOSI-filen 
#   define LI_FEIL_OPPDATTID     -5  // Feil oppdateringstid på SOSI-filen 


#   define _FYBAX_DEFINED
#endif


/* ================================================ */
/*  Definerer data-strukturer                       */
/* ================================================ */

/* Lesebuffer */
/* Konstanter for set_brukt */
#define SET_UBRUKT 0
#define SET_BRUKT  1

/* Konstanter for sStatus */
#define LESEBUFFER_TOM   0          /* (Tom eller ikke ajour) */
#define LESEBUFFER_OK    1

 /* Verdier for cur_type */
                          /*  >= 0 : Gruppenavn,linjenummer i navnetab. */
#define  LEST_KOORD  -1   /* Koordinatlinje */
#define  LEST_BLANK  -2   /* "blank linje" (!!!, mm) */
#define  LEST_GINFO  -3   /* GINFO */
#define  LEST_KOM    -4   /* Kommentar-linje */

/* UTVALG */
/*
 // Utvalgskommandoer 
#define U_OG          1     // ..OG 
#define U_ELLER       2     // ..VELG og ..ELLER


 // Utvalgsmetoder
#define U_IKKE         0     //  !     Ikke (Tilslag når SOSI-navnet     
                             //             ikke finnes. Bare GINFO.)    
#define U_ALLE         1     //  AL    Alle                              
#define U_FRATIL       2     //  <>    Fra-til                           
#define U_UTENFOR      3     //  ><    Utenfor                           
#define U_MINDRE       4     //  <     Mindre-enn                        
#define U_STORRE       5     //  >     Større-enn                        
#define U_DELELIG      6     //  /     Delelig-med, eventuellt med       
                             //        sjekk på om restverdi er 2. verdi 
#define U_UDELELIG     7     //  !/    Ikke-delelig-med                  
#define U_CONTEIN      8     //  ()    Inneholder                        
#define U_IKKECONTEIN  9     //  !()   Inneholder ikke (Tilslag når      
                             //        denne navn og verdi kombinasjonen 
                             //        ikke finnes)                      
#define U_LIK         10     //  =     Lik                               
#define U_IKKEVALGT   11     //  IV    Ikke valgt (Tilslag når gruppen   
                             //        ikke er tegnet ennå. Kombineres   
                             //        med SOSI-navnet "..*")            
#define U_IKKELIK     12     //  !=    Ikke lik (Tilslag når denne       
                             //        navn og verdi kombinasjonen ikke  
                             //        finnes)                           
#define U_FLERE       13     //  FL    Flere (Tilslag når SOSI-navnet    
                             //        forekommer flere ganger.)         
#define U_IKKEFLERE   14     //  !FL   Ikke flere enn (Tilslag når       
                             //        SOSI-navnet IKKE forekommer       
                             //        flere ganger enn gitt antall.)    



 // Parametertyper 
#define U_TALL          1     // Heltall     
#define U_FLYT          2     // Flyttall    
#define U_ALFA          9     // Tekststreng 
*/
