/******************************************************************^$date;
*                                                                           *
*       Hjelpebiblioteket   U T  (Utilities)                                *
*       Lars Staurset, KARTVERKET / FYSAK-prosjektet, januar 1990     *
*       Fil: UT2.C versjon C22: Streng- og teikn-operasjonar                *
*                                                                           *
****************************************************************************/

#include "stdafx.h"

#include	<math.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "fyut.h"

#ifdef WIN32
#  include <windows.h>
#  include <memory.h>
#  include <stdlib.h>
#endif

/* Interne prototyper */
static short VT_JustStr (wchar_t ztr[], short mstr, wchar_t str[]);
static wchar_t  VT_StrWord (const wchar_t *str, short ii, short *iu, wchar_t *sep, short mw,
								 wchar_t word[]);

/*******************************************************************************
*                                                                              *
*       FRAMHENTING AV TOKEN M.M.                                              *
*                                                                              *
*******************************************************************************/

/*LS-890927*********************************************************************
*       Get Token                                                              *
*******************************************************************************/

SK_EntPnt_UT wchar_t *UT_GetToken (wchar_t *str, short *tl)
{
	 wchar_t *ptr;
                                /* Hopp fram til første ikkje-blanke */
	 while (UT_IsSpace(*str)) ++str;
                /* Hopp evt. forbi eitt komma el. punktum og blanke etter det */
    if (*str == L',' || *str == L'.') {
        ++str;
        while (UT_IsSpace(*str)) ++str;
    }
                                /* Sjekk om strengen er ferdigbehandla */
    if (*str == L'\0') {
      *tl=0;
      return NULL;
    }
                                /* Token avgrensa av hermeteikn */
    if (*str == L'"') {
		  ptr = str + 1;
        while (*ptr != L'"' && *ptr != L'\0' && *ptr != L'\n') ptr++;
        *tl = (short)(ptr - str + 1);
        return str;
    }
                                /* Tekststreng fram til skilleteikn */
	 else {
		  ptr = str;
        while (!UT_IsSpace(*ptr)  &&  *ptr != L',' &&  *ptr != L'\0')  ptr++;
		  *tl = (short)(ptr - str);
        return str;
    }
}

/*LS-890927****AR-920317********************************************************
*       Get Token From String                                                  *
*******************************************************************************/

SK_EntPnt_UT wchar_t UT_StrToken (const wchar_t str[], short ii, short *iu, short mt, wchar_t token[])
{
    wchar_t ch;
    register short it;

    
    /* Korrigerer max strenglengde (AR) */
    mt--;

    /* Hopp fram til første ikkje-blanke */
    while (UT_IsSpace(str[ii]))  ++ii;
	
    /* Hopp evt. forbi eitt komma eller semikolon og blanke etter det */
	 if (str[ii] == L','  ||  str[ii] == ';') {
        ii++;
		  while (UT_IsSpace(str[ii]))  ++ii;
    }
    
    /* Sjekk om strengen er ferdigbehandla */
    if (str[ii] == L'\0') {
        token[0] = L'\0';
        ch = L'\0';

    /* Token avgrensa av doble hermeteikn */
    } else if (str[ii] == L'"') {
        for (ii++, it = 0; !wcschr(L"\"",str[ii]); ++ii) {
            if (it < mt) token[it++] = str[ii];
        }
        token[it] = L'\0';
		  ch = L'"';
        if (str[ii]) ii++;
    
    /* Token avgrensa av enkle hermeteikn */
    } else if (str[ii] == L'\'') {
        for (ii++, it = 0; !wcschr(L"'",str[ii]); ++ii) {
            if (it < mt) token[it++] = str[ii];
        }
        token[it] = L'\0';
		  ch = L'\'';
        if (str[ii]) ii++;
    

    /* Token går fram til ',' , ';' , 'Space' el '\0' */
    } else {
        for (it = 0; (!UT_IsSpace(str[ii]) && str[ii] != L',' && str[ii] != L';' && str[ii] != L'\0'); ii++) {
				if (it < mt) token[it++] = str[ii];
		  }
        token[it] = L'\0';
										  /* Returkode avh. av slutt-teikn */
        if (str[ii] == L'\0' || UT_IsSpace(str[ii]))
            ch = ' ';
        else
            ch = str[ii++];
    }
    *iu = ii;

    return ch;
}


/*LS-880929*****AR-890713*******************************************************
*       Get Word From String                                                   *
*******************************************************************************/

SK_EntPnt_UT wchar_t UT_StrWord (const wchar_t *str, short ii, short *iu, short mw, wchar_t *word)
{
    return VT_StrWord (str, ii, iu, L" ,\x9\xA\xB\xC\xD", mw, word);
}

/*LS-881005******AR-890713******************************************************
*       Get Integer From String                                                *
*******************************************************************************/

SK_EntPnt_UT short UT_StrInt (const wchar_t str[], short ii, short *iu, int *_int)
{
    short ok;
    long l;

    ok = UT_StrLong (str,ii,iu,&l);
    *_int = (int)l;
    return ok;
}

/*LS-881005****AR-890713********************************************************
*       Get Short Integer From String                                          *
*******************************************************************************/

SK_EntPnt_UT short UT_StrShort (const wchar_t *str, short ii, short *iu, short *s)
{
   wchar_t tal[7];

	if (VT_StrWord (str,ii,iu,L" ,\x9\xA\xB\xC\xD",7,tal))
   {
      *s = (short)_wtoi(tal);
      return UT_TRUE;
	}

   *s = (short)0;
   return UT_FALSE;
}


/*LS-880929*****AR-890713*******************************************************
*       Get Long Integer From String                                           *
*******************************************************************************/

SK_EntPnt_UT short UT_StrLong (const wchar_t str[], short ii, short *iu, long *_long)
{
   wchar_t tal[12];

   if (VT_StrWord (str,ii,iu,L" ,\x9\xA\xB\xC\xD",12,tal))
   {
      *_long = _wtol(tal);
      return UT_TRUE;
   }
      
	*_long = 0;
	return UT_FALSE;
}


/*LS-880929*****AR:2007-08-09***************************************************
*       Get Integer64 From String                                              *
*******************************************************************************/

SK_EntPnt_UT short UT_StrInt64 (const wchar_t str[], short ii, short *iu, UT_INT64 *i64)
{
   wchar_t tal[20];

   if (VT_StrWord (str,ii,iu,L" ,\x9\xA\xB\xC\xD",20,tal))
   {
#ifdef LINUX
      *i64 = atoll(tal);
#else
      *i64 = _wtoi64(tal);
#endif
      return UT_TRUE;
   }
      
	*i64 = 0;
	return UT_FALSE;
}


/*LS-880929*****AR-890713*******************************************************
*       Get Unsigned Long Integer From String                                  *
*******************************************************************************/

SK_EntPnt_UT short UT_StrUlong (const wchar_t str[], short ii, short *iu, unsigned long *_ulong)
{
   wchar_t tal[12],*endptr;

   if (VT_StrWord (str,ii,iu,L" ,\x9\xA\xB\xC\xD",12,tal))
   {

      *_ulong = wcstoul( tal, &endptr, 10 );

		return UT_TRUE;
   }
      
   *_ulong = 0;
   return UT_FALSE;
}

/*LS-890909*********************************************************************
*       Get Double From String                                                 *
*******************************************************************************/

SK_EntPnt_UT short UT_StrDbl (const wchar_t *str, short ii, short *iu, wchar_t decpt, double *dbl)
{
    wchar_t tal[31], *sep;
    static wchar_t sep1[] = L" ,\x9\xA\xB\xC\xD", sep2[] = L" \x9\xA\xB\xC\xD";

    sep = (decpt == ',') ? sep2 : sep1;
    if (VT_StrWord (str,ii,iu,sep,31,tal)) {
        UT_AtoD (tal,decpt,dbl);
        return UT_TRUE;
    }

    *dbl = 0.0;
    return UT_FALSE;
}

/*LS-890927****AR-890713********************************************************
*       Get Word From String                                                   *
*******************************************************************************/

static wchar_t VT_StrWord (const wchar_t *str, short ii, short *iu, wchar_t *sep, short mw, wchar_t word[])
{
    wchar_t ch;
    int iw;


                            /* Korrigerer max strenglengde (AR) */
    mw--;
                            /* Hopp fram til første ikkje-blanke */
    while (UT_IsSpace(str[ii]))  ii++;
                            /* Hopp evt. forbi eitt komma og blanke etter det */
    if (wcschr (sep,L',')) {
		  if (str[ii] == L',')  ii++;
        while (UT_IsSpace(str[ii]))  ii++;
    }

                            /* Sjekk om strengen er ferdigbehandla */
    if (str[ii] == L'\0') {
		  word[0] = L'\0';
		  ch = L'\0';

									 /* Ordet går fram til separator-teikn */
    } else {
        for (iw = 0; !wcschr (sep,str[ii]); ii++) {
            if (iw < mw) word[iw++] = str[ii];
        }
        word[iw] = L'\0';
                            /* Returkode avh. av slutt-teikn */
        if (str[ii] == L'\0' || UT_IsSpace(str[ii]))
            ch = L' ';
        else
            ch = str[ii++];
    }
    *iu = ii;
	 return ch;
}


/*
AR:2011-08-04
CH UT_memcpy                                      Kopiere buffer
CD ==============================================================
CD For beskrivelse, se dokumentasjonen av memcpy_s.
CD
CD PARAMETERLISTE:
CD Type    Navn             I/U  Merknad
CD -------------------------------------------------------------
CD void   *dest              iu  Buffer det skal kopieres til.
CD size_t  numberOfElements  i   Size of the destination buffer.
CD void   *src               i   Buffer det kopieres fra.
CD size_t  count             i   Number of characters to copy.
CD int     status            r   Zero if successful; an error code on failure.
CD  ==============================================================
*/
SK_EntPnt_UT int UT_memcpy(void *dest,size_t numberOfElements,const void *src,size_t count)
{
#ifdef WIN32
   return memcpy_s(dest,numberOfElements,src,count);
#else
   if (memcpy(dest,src,count)==NULL) return 1;
   return 0;
#endif
}


/*
AR:2011-08-08
CH UT_strerror                          Henter systemfeilmelding
CD ==============================================================
CD For beskrivelse, se dokumentasjonen av strerror_s.
CD
CD PARAMETERLISTE:
CD Type    Navn             I/U  Merknad
CD -------------------------------------------------------------
CD wchar_t   *buffer            iu  Buffer to hold error string.
CD size_t  numberOfElements  i   Size of buffer.
CD int     errnum            i   Error number.
CD int     status            r   Zero if successful; an error code on failure.
CD  ==============================================================
*/
SK_EntPnt_UT int UT_strerror(wchar_t *buffer,size_t numberOfElements,int errnum)
{
#ifdef WIN32
   return _wcserror_s(buffer,numberOfElements,errnum);
#else
   UT_StrCopy(buffer,strerror(errnum),numberOfElements);
   return 0;
#endif
}


/*
AR:2011-08-08
CH UT_strtok                                        Henter token
CD ==============================================================
CD Overbygning over strtok i standardbiblioteket.
CD For beskrivelse, se dokumentasjonen av strtok_s.
CD
CD PARAMETERLISTE:
CD Type    Navn             I/U  Merknad
CD -------------------------------------------------------------
CD wchar_t       *strToken      iu  String containing token or tokens.
CD const wchar_t *strDelimit    i   Set of delimiter characters.
CD wchar_t      **context       iu  Used to store position information between calls to UT_strtok
CD wchar_t       *token         r   Returns a pointer to the next token found in strToken. 
CD                               They return NULL when no more tokens are found. 
CD
CD  ==============================================================
*/
SK_EntPnt_UT wchar_t *UT_strtok(wchar_t *strToken,const wchar_t *strDelimit,wchar_t **context)
{
#ifdef WIN32
   return wcstok_s(strToken,strDelimit,context);
#else
   return strtok(strToken,strDelimit);
#endif
}


/*
GL.06.03.89
CH  UT_StrCopy                    Kopiere streng med overflyttest
CD  ==============================================================
CD  Kopierer en streng med max ant tegn. Legger på \0 på slutten.
CD
CD  PARAMETERLISTE:
CD  Navn     Type       I/U  Merknad
CD  -------------------------------------------------------------
CD  dst     *wchar_t       u   streng det skal kopieres til.
CD  src     const *wchar_t i   streng det skal kopieres fra.
CD  maxlen  int         i   max antall tegn som skal kopieres
CD                          (Inkludert null-terminator.)
CD  ==============================================================
*/
SK_EntPnt_UT void UT_StrCopy (wchar_t *dst, const wchar_t *src, int maxlen)
{
#ifdef WIN32
   wcsncpy_s(dst,maxlen,src,_TRUNCATE);
#else
   strncpy(dst,src,maxlen);
#endif
}


/*
T.H.10.08.96
CH  UT_StrCat                               Konkatinerer streng
CD  ==============================================================
CD  Koncatinerer en streng med max ant tegn. Legger på \0 på slutten.
CD
CD  PARAMETERLISTE:
CD  Navn     Type    I/U  Merknad
CD  -------------------------------------------------------------
CD  dst     *wchar_t     u   streng det skal kopieres til.
CD  src     *wchar_t     i   streng det skal kopieres fra.
CD  maxlen   int      i   Max lengde av dst, inkl. null-terminator.
CD  retur    short    r   1 = OK, 0 = feil.
CD  ==============================================================
*/
SK_EntPnt_UT short UT_StrCat (wchar_t *dst,const wchar_t *src, const int maxlen)
{
   //return (strcat_s(dst,maxlen,src))?  0 : 1; 
#ifdef WIN32
   int err = wcsncat_s(dst,maxlen,src,_TRUNCATE);  //Returns 0 if successful, an error code on failure.
   return (err == 0)?  1 : 0; 
#else
   strncat(dst,src,maxlen); 
   return 1;
#endif

}


/* (AR) */
/*GL-880907*LS-880829***********************************************************
*       Get Substring                                                          *
*******************************************************************************/
SK_EntPnt_UT void UT_GetSubstr (wchar_t ctx[],short fra,short til,wchar_t sub[],short maxlen)
{
	short i;
	short j = 0;
	short txlen = (short)wcslen(ctx);


	sub[0] = L'\0';

	if (fra >= txlen)  return;  /* Starter etter slutten av strengen ==> retur */

   if (til > txlen) til=txlen;

   if (fra == -1) {
		fra++;
      while (UT_IsSpace(ctx[fra])  &&  fra < txlen) {
         fra++;
      }
   }

   if (til == -1) {
      til=txlen;
      while (UT_IsSpace(ctx[til])  &&  til > 0) {
         til--;
      }
   }

   for (i=fra; i<= til && ctx[i] != '\0' && maxlen>1 ; i++, maxlen--) {
      sub[j++]=ctx[i];
   }

   sub[j]=L'\0';
}


/* (AR) */
/*LS-880929*********************************************************************
*       Search String for Substring (INDEX)                                    *
*******************************************************************************/

SK_EntPnt_UT short UT_Index (wchar_t str[], wchar_t substr[])
{                              /* Wortman & Sidebottom, p. 101 */
    short i;
    short j,k;

    for (i = 0; str[i] != L'\0'; i++) {
        for (j=i, k=0;  substr[k] != L'\0' && substr[k] == str[j];  j++, k++) {
           ;
        }
        if (substr[k] == L'\0') return (i);
    }
    return (-1);
}

/*******************************************************************************
*                                                                              *
*       ANDRE STRENG- OG TEIKN-RUTINER                                         *
*                                                                              *
*******************************************************************************/

/*LS-880929**AR-890713**********************************************************
*       Clear After CR/LF                                                      *
*******************************************************************************/

SK_EntPnt_UT wchar_t *UT_ClrCrlf (wchar_t *str)
{
   wchar_t *cp;

   if ((cp = wcspbrk(str,L"\r\n")) != NULL)   *cp = L'\0';
   
   return str;
}

/*AR-900111********************************************************************
*       Clear Trailing Space                                                     *
*******************************************************************************/
SK_EntPnt_UT wchar_t *UT_ClrTrailsp(wchar_t *str)
{
   wchar_t *cp;

   if (*str != L'\0')
   {
      // Fjern blanke på slutten
      cp = wcschr(str,L'\0');
      --cp;
      while ( cp >= str && UT_IsSpace(*cp) )
      {
         --cp;
      }
      *(cp+1) = L'\0';
   }

   return str;
}


/*AR-900111********************************************************************
*       Clear Trailing Space                                                     *
*******************************************************************************/
SK_EntPnt_UT char *UT_ClrTrailsp(char *str)
{
   char *cp;

   if (*str != L'\0')
   {
      // Fjern blanke på slutten
      cp = strchr(str,L'\0');
      --cp;
      while ( cp >= str && UT_IsSpace(*cp) )
      {
         --cp;
      }
      *(cp+1) = L'\0';
   }

   return str;
}


/*AR-900111********************************************************************
*    Clear leading and trailing Space, and compress multiple space to one.    *
*******************************************************************************/
SK_EntPnt_UT wchar_t *UT_ClrExtrasp(wchar_t *str)
{
   wchar_t *cp,*tp;
   short sSp = UT_TRUE;

   /* Fjern blanke på slutten */
   UT_ClrTrailsp(str);

   /* Fjern ledende blanke og overflødige blanke inni strengen */
   cp = tp = str;

   while (*cp != L'\0') {
      if (! UT_IsSpace(*cp)) {
         *tp++ = *cp;
         sSp = UT_FALSE;
      } else {
         if (sSp == UT_FALSE)  *tp++ = L' ';
         sSp = UT_TRUE;
      }
      cp++;
   }
   *tp = L'\0';

   return str;
}


/*
AR-890312
CH UT_IsSpace                                           Test om mellomromstegn
CD =============================================================================
CD Formål:
CD Sjekker om et tegn er mellomromstegn (ascii 0x09-0x0d eller 0x20)
CD
CD Parametre:
CD Type     Navn   I/U   Forklaring
CD -----------------------------------------------------------------------------
CD wchar_t  c       i    Tegn som skall testes.
CD int      ist     r    1=mellomromstegn, 0=ikke mellomromstegn.
CD
CD Bruk:
CD ist = UT_IsSpace(*cp);
   =============================================================================
*/
SK_EntPnt_UT int UT_IsSpace(wchar_t c)
{
   //setlocale(LC_ALL, "Norwegian_Norway.1252");

   //setlocale(LC_ALL, "Norwegian_Norway.UTF-8");


   //no_utf8	Norwegian	no_NO.UTF-8	Norwegian_Norway.1252	WINDOWS-1252
   //   no_gr_utf8	Norwegian (Primary)	no_NO.UTF-8	-parent no_utf8 used-


   return iswspace(c);
}


SK_EntPnt_UT int UT_IsSpace(char c)
{
#ifdef WIN32
   static unsigned char atab[256] =
   { 0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

   return atab[(unsigned char)c];

#else
   switch (c){
   case 0x20:
   case 0x09:
   case 0x0A:
   case 0x0B:
   case 0x0C:
   case 0x0D:
      return 1;
      break;
   default:
      return 0;
      break;
   }
#endif
}





/*LS-890430*********************************************************************
*       Check for Printable Character                                          *
*******************************************************************************/

SK_EntPnt_UT int UT_IsPrint (wchar_t ch)
{
    if (isprint (ch))                   return UT_TRUE;
    else if (ch == 0)                   return UT_FALSE;
    else if (wcschr(L"ÆØÅæøå",ch)) return UT_TRUE;
    else                                return UT_FALSE;
}

/*LS-890517*********************************************************************
*       Check for Lower-Case Character                                         *
*******************************************************************************/

SK_EntPnt_UT int UT_IsLower (wchar_t ch)
{
    if (islower (ch))                      return UT_TRUE;
    else if (ch == 0)                      return UT_FALSE;
    else if (wcschr (L"æøåáäèïñö",ch)) return UT_TRUE;
    else                                   return UT_FALSE;
}

/*LS-890913*********************************************************************
*       Convert Character to Lower-Case                                        *
*******************************************************************************/

SK_EntPnt_UT wchar_t UT_ToLower (wchar_t ch)
{
   return (wchar_t)tolower((int)ch);


   //if (ch >= 'A'  &&  ch <= 'Z') {
   //   ch += 0x20;

   //} else {
   //   switch (ch) {
   //      case 'Æ' : ch = 'æ'; break;
   //      case 'Ø' : ch = 'ø'; break;
   //      case 'Å' : ch = 'å'; break;
   //
   //      case 'Á' : ch = 'á'; break; // á - SK-fonter + standardfonter
   //      case 'Ä' : ch = 'ä'; break; // ä - SK-fonter + standardfonter
   //      case 'È' : ch = 'è'; break; // Samisk c - SK-fonter og è - standardfonter
   //      case 'Ï' : ch = 'ï'; break; // ï - SK-fonter + standardfonter
   //      case 'Ñ' : ch = 'ñ'; break; // Samisk n(aksent) - SK-fonter og ñ - standardfonter
   //      case 'Ö' : ch = 'ö'; break; // ö - SK-fonter + standardfonter

   //      case 0x010C : ch = 0x010D; break; // Samisk c - utvidede standardfonter
   //      case 0x0110 : ch = 0x0111; break; // Samisk d - utvidede standardfonter
   //      case 0x014A : ch = 0x014B; break; // Samisk n(lav) - utvidede standardfonter
   //      case 0x0143 : ch = 0x0144; break; // Samisk n(aksent) - utvidede standardfonter
   //      case 0x0160 : ch = 0x0161; break; // Samisk s - utvidede standardfonter
   //      case 0x0166 : ch = 0x0167; break; // Samisk t - utvidede standardfonter
   //      case 0x017D : ch = 0x017E; break; // Samisk z - utvidede standardfonter
   //   }
   //}

   //return ch;
}

/*AR-881020*LS-890517***********************************************************
*       Convert String to Lower-Case                                           *
*******************************************************************************/

SK_EntPnt_UT wchar_t *UT_StrLower (wchar_t *ti)
{
   wchar_t *t = ti;

   while ((*t = UT_ToLower(*t)) != L'\0')
       t++;
   return (ti);
}

/*LS-890517*********************************************************************
*       Check for Upper-Case Character                                         *
*******************************************************************************/

SK_EntPnt_UT int UT_IsUpper (wchar_t ch)
{
    if (isupper (ch))           return UT_TRUE;
    else if (ch == 0)           return UT_FALSE;
    else if (wcschr (L"ÆØÅÁÄÈÏÑÖ",ch)) return UT_TRUE;
    else                        return UT_FALSE;
}

/*LS-890913*********************************************************************
*       Convert Character to Upper-Case                                        *
*******************************************************************************/

SK_EntPnt_UT wchar_t UT_ToUpper (wchar_t ch)
{
    if (ch >= L'a'  &&  ch <= L'z') {
       ch -= 0x20;

    } else {
        switch (ch) {
            case L'æ' : ch = L'Æ'; break;
            case L'ø' : ch = L'Ø'; break;
            case L'å' : ch = L'Å'; break;
         
            case L'á' : ch = L'Á'; break; // á - SK-fonter + standardfonter
            case L'ä' : ch = L'Ä'; break; // ä - SK-fonter + standardfonter
            case L'è' : ch = L'È'; break; // Samisk c - SK-fonter og è - standardfonter
            case L'ï' : ch = L'Ï'; break; // ï - SK-fonter + standardfonter
            case L'ñ' : ch = L'Ñ'; break; // Samisk n(aksent) - SK-fonter og ñ - standardfonter
            case L'ö' : ch = L'Ö'; break; // ö - SK-fonter + standardfonter

            case 0x010D : ch = 0x010C; break; // Samisk c - utvidede standardfonter
            case 0x0111 : ch = 0x0110; break; // Samisk d - utvidede standardfonter
            case 0x014B : ch = 0x014A; break; // Samisk n(lav) - utvidede standardfonter
            case 0x0144 : ch = 0x0143; break; // Samisk n(aksent) - utvidede standardfonter
            case 0x0161 : ch = 0x0160; break; // Samisk s - utvidede standardfonter
            case 0x0167 : ch = 0x0166; break; // Samisk t - utvidede standardfonter
            case 0x017E : ch = 0x017D; break; // Samisk z - utvidede standardfonter
        }
    }

    return ch;
}

/*LS-890517*********************************************************************
*       Convert String to Upper-Case                                           *
*******************************************************************************/

SK_EntPnt_UT wchar_t *UT_StrUpper (wchar_t *ti)
{
   wchar_t *t = ti;

   while ((*t = (wchar_t)UT_ToUpper(*t)) != L'\0')
       t++;
   return (ti);
}

/*******************************************************************************
*                                                                              *
*       KONVERTERINGSRUTINER                                                   *
*                                                                              *
*******************************************************************************/

#ifdef OS216

/*LS-880928*********************************************************************
*       Convert String to Integer If Possible                                  *
*******************************************************************************/

SK_EntPnt_UT int UT_AtoI (wchar_t *str, int *_int)
{
    short s,ok;

    ok = UT_AtoS (str,&s);
    *_int = (int)s;
    return ok;
}

/*LS-880929*********************************************************************
*       Convert String to Short Integer If Possible                            *
*******************************************************************************/

SK_EntPnt_UT short UT_AtoS (wchar_t str[], short *s)
{
    short ok,slen;
    short len,sif;

    ok = UT_FALSE;
                                    /* Finn første siffer != 0 */
    /*; for (sif=0; str[sif]<'1' || str[sif]>'9'; sif++); */ 
    /* Rettet AR-940623 for å handtere streng uten siffer 1-9 */
    for (sif=0; (str[sif]<L'1' || str[sif]>L'9') && str[sif]!=L'\0' ; sif++);
                                    /* Lengde på sifferstrengen */
    slen = (short)strlen(str);
    len = slen - sif;
                                    /* Viss <5 siffer: ok */
    if (len < 5) ok = UT_TRUE;
                                    /* Viss 5 siffer: u.s. dei 4 første */
    else if (len == 5) {
        sif = (short)str[--slen];
        str[slen] = L'\0';
        *s = (short)abs(atoi(str));
        if (*s < 3276) ok = UT_TRUE;
                                    /* På grensa for lovleg verdi, sjekk det 5. */
        else if (*s == 3276) {
            if (sif <= L'7') ok = UT_TRUE;
            else if (sif == L'8') {
                                    /* Neg. tal kan ha 1 større abs.verdi */
                if (atoi(str) < 0) ok = UT_TRUE;
            }
        }
                                    /* Sett tilbake det lånte sifferet */
        str[slen++] = (wchar_t)sif;
    }
    if (ok) *s = (short)atoi(str);
    else *s = 0;
    return ok;
}

/*LS-880929*********************************************************************
*       Convert String to Long Integer If Possible                             *
*******************************************************************************/

SK_EntPnt_UT short UT_AtoL (wchar_t str[], long *_long)
{
    short ok,slen;
    register short len,sif;

    ok = UT_FALSE;
                                    /* Finn første siffer != 0 */
    /*; for (sif=0; str[sif]<'1' || str[sif]>'9'; sif++); */ 
    /* Rettet AR-940623 for å handtere streng uten siffer 1-9 */
    for (sif=0; (str[sif]<L'1' || str[sif]>L'9') && str[sif]!=L'\0' ; sif++);

                                    /* Lengde på sifferstrengen */
    slen = (short)strlen(str);
    len = slen - sif;
                                    /* Viss <10 siffer: ok */
    if (len < 10) ok = UT_TRUE;
                                    /* Viss 10 siffer: u.s. dei 9 første */
    else if (len == 10) {
        sif = (short)str[--slen];
        str[slen] = L'\0';
        *_long = labs (_tstol (str));
        if (*_long < 214748364) ok = UT_TRUE;
                                    /* På grensa for lovleg verdi, sjekk det 10. */
        else if (*_long == 214748364) {
            if (sif <= L'7') ok = UT_TRUE;
            else if (sif == L'8') {
                                    /* Neg. tal kan ha 1 større abs.verdi */
                if (_tstol(str) < 0) ok = UT_TRUE;
            }
        }
                                    /* Sett tilbake det lånte sifferet */
        str[slen++] = (wchar_t)sif;
    }
    if (ok) *_long = _tstol (str);
    else *_long = 0L;
    return ok;
}

#endif


/*LS-890519*********************************************************************
*       Convert String to Double                                               *
*******************************************************************************/

SK_EntPnt_UT short UT_AtoD (wchar_t *str, wchar_t decpt, double *dbl)
{
    wchar_t *kompos;
                                    /* Pass på desimalkomma */
    if (decpt == L',') {
        kompos = wcschr (str,L',');
        if (kompos) *kompos = L'.';
    }
    else
        kompos = NULL;
    *dbl = _wtof (str);
    if (kompos) *kompos = L',';
    return UT_TRUE;
}

/*LS-889297*********************************************************************
*       Convert Integer to String                                              *
*******************************************************************************/

SK_EntPnt_UT short UT_ItoA (int _int, short mstr, wchar_t *str)
{
   wchar_t ztr[12];
   
   UT_SNPRINTF(ztr,12,L"%d",_int);
   return VT_JustStr (ztr,mstr,str);
}

/*LS-880928*********************************************************************
*       Convert Short Integer to String                                        *
*******************************************************************************/

SK_EntPnt_UT short UT_StoA (short s, short mstr, wchar_t *str)
{
   wchar_t ztr[7];

   UT_SNPRINTF(ztr,7,L"%hd",s);
   return VT_JustStr (ztr,mstr,str);
}

/*LS-880929*********************************************************************
*       Convert Long Integer to String                                         *
*******************************************************************************/

SK_EntPnt_UT short UT_LtoA (long _long, short mstr, wchar_t *str)
{
   wchar_t ztr[12];

   UT_SNPRINTF(ztr,12,L"%ld",_long);
   return VT_JustStr (ztr,mstr,str);
}

/*LS-890912*********************************************************************
*       Convert Double To String                                               *
*******************************************************************************/

SK_EntPnt_UT short UT_DtoA (double dbl, short dec, wchar_t decpt, short mstr, wchar_t *str)
{  
   /* size_t len; */
   wchar_t *cp,ztr[41];
   
   /* Sjekk mot for lang streng  */ 
   if (dbl!=0.0   &&  (log10(fabs(dbl)) + dec  > 36.0)) {
      wmemset( ztr, L'*', 40);
      ztr[40] = L'\0';

   } else {

      /* Konverter til streng */
      UT_SNPRINTF(ztr,41,L"%.*f",dec,dbl);

      /* Skift til rett desimaltegn */
      if ((cp = wcschr(ztr,L'.')) != NULL) {
         *cp = decpt;
      }

      /* Kontroller strenglengda */
      /*if ((len = strlen (ztr)) > 40) {                  */
      /*   UT_FPRINTF (stderr,"\nFeil i UT_DtoA: %d",len);   */
      /*   exit (1);                                      */
      /*}                                                 */
   }

   /* Juster etter behov */
   return VT_JustStr(ztr,mstr,str);
}

/*LS-890912*********************************************************************
*       Check and Justify String                                               *
*******************************************************************************/

static short VT_JustStr (wchar_t ztr[], short mstr, wchar_t str[])
{
    short ok,i,lstr;

    lstr = (short)wcslen (ztr);
    mstr--;
                                        /* For kort streng? */
    if (mstr > 0 && mstr < lstr) {
        for (i=0; i<mstr; i++) str[i] = ztr[i+lstr-mstr];
        str[mstr] = L'\0';
        ok = UT_FALSE;
    }
                                        /* Lang nok streng */
    else {
        //strcpy_s (str,mstr,ztr);
#ifdef LINUX
        strncpy (str,ztr,mstr+1);
#else
        UT_StrCopy(str,ztr,mstr+1); // Må bruke mstr+1 fordi det er trukket fra en i den verdien som kommer inn
#endif
        ok = UT_TRUE;
    }
                                        /* For lang streng? */
    if (mstr > lstr) UT_JustStr (L'>',L' ',(short)(mstr+1),str);
                                        /* Avslutt */
    return ok;
}

/*LS-891011*********************************************************************
*       Justify and Fill String                                                *
*******************************************************************************/

SK_EntPnt_UT void UT_JustStr (wchar_t justmode, wchar_t fill, short lstr, wchar_t *str)
{
   wchar_t *pc1, *pc2, *pc, *qc;

   if (lstr > 0) {
                                           /* Terminer strengen */
      lstr--;
      *(str+lstr) = L'\0';
                                           /* Finn endane av strengen */
      pc1 = str;
      while (UT_IsSpace(*pc1) || *pc1 == fill) {
         pc1++;
      }

      pc2 = wcschr(pc1,L'\0')-1;
      while (UT_IsSpace(*pc2) || *pc2 == fill) {
         pc2--;
      }

      switch (justmode) {
         case L'>' :                      /* Høgrepakking */
            for (pc = pc2, qc = str+lstr-1; pc >= pc1; pc--) {
               if (!UT_IsSpace(*pc) && *pc != fill) {
                  *qc-- = *pc;
               }
            }
            while (qc >= str) *qc-- = fill;
            break;
         case L'/' :                      /* Høgrejustering */
            for (pc = pc2, qc = str+lstr-1; pc >= pc1; pc--)
               *qc-- = *pc;
            while (qc >= str) *qc-- = fill;
            break;
         case L'<' :                      /* Venstrepakking */
            for (pc = pc1, qc = str; pc <= pc2; pc++) {
               if (!UT_IsSpace(*pc) && *pc != fill) *qc++ = *pc;
            }
            while (qc < str+lstr) *qc++ = fill;
            break;
         case L'`' :                      /* Venstrejustering */
            for (pc = pc1, qc = str; pc <= pc2; pc++)
               *qc++ = *pc;
            while (qc < str+lstr) *qc++ = fill;
            break;
      }
   }
}


/*
AR-890113 LS-890915
CH UT_Ascii7ToAscii8                                          Konverter til PC-ASCII
CD =============================================================================
CD Formål:
CD Konverterer en streng fra ND-7-bits norsk ASCII til PC-8-bits norsk ASCII.
CD
CD Parametre:
CD  Type    Navn     I/U   Forklaring
CD -----------------------------------------------------------------------------
CD  char   *tx        i    Peker til streng som skal konverteres
CD                         (avsluttet med '\0').
CD  char   *tp        r    Peker til konvertert streng.
CD
CD Bruk:
CD tp = UT_Ascii7ToAscii8(tx);
   =============================================================================
*/
SK_EntPnt_UT char *UT_Ascii7ToAscii8(char *tx)
{
   static unsigned char atab[256] =
   {   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
      20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
      40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
      60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
      80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,146,157,143, 94, 95, 96, 97, 98, 99,
     100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,
     120,121,122,145,155,134,126,127,128,129,130,131,132,133,134,135,136,137,138,139,
     140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
     160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,
     180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,
     200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,
     220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
     240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255};

   if (tx != NULL)
   {
      for (char *tp = tx;  *tp != '\0';  ++tp)
      {
         *tp = (char)atab[(unsigned char)*tp];
      } 
   }

   return (tx);
}


/*
AR-890113 LS-890915
CH UT_Ascii8ToAscii7                                          Konverter til ND-ASCII
CD =============================================================================
CD Formål:
CD Konverterer en streng fra PC-8-bits norsk ASCII til ND-7-bits norsk ASCII.
CD
CD Parametre:
CD  Type    Navn     I/U   Forklaring
CD -----------------------------------------------------------------------------
CD  char   *tx        i    Peker til streng som skal konverteres
CD                         (avsluttet med '\0').
CD  char   *tp        r    Peker til konvertert streng.
CD
CD Bruk:
CD tp = UT_Ascii8ToAscii7(tx)
   =============================================================================
*/
SK_EntPnt_UT char *UT_Ascii8ToAscii7(char *tx)
{
   static unsigned char atab[256] =
   {   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
      20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
      40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
      60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
      80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
     100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,
     120,121,122,123,124,125,126,127,128,129,130,131,132,133,125,135,136,137,138,139,
     140,141,142, 93,144,123, 91,147,148,149,150,151,152,153,154,124,156, 92,158,159,
     160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,
     180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,
     200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,
     220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
     240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255};

   if (tx != NULL)
   {
      for (char *tp = tx;  *tp != '\0';  ++tp)
      {
         *tp = (char)atab[(unsigned char)*tp];
      }
   }

   return (tx);
}


/*
AR-920929
CH UT_Ascii8ToISO8859                                 Konverter til ISO8859-10
CD =============================================================================
CD Formål:
CD Konverterer en streng fra PC-8-bits norsk ASCII til ISO8859-10.
CD
CD Parametre:
CD  Type    Navn     I/U   Forklaring
CD -----------------------------------------------------------------------------
CD  char   *tx        i    Peker til streng som skal konverteres
CD                         (avsluttet med '\0').
CD  char   *tp        r    Peker til konvertert streng.
CD
CD Bruk:
CD tp = UT_Ascii8ToISO8859(tx)
   =============================================================================
*/
SK_EntPnt_UT char *UT_Ascii8ToISO8859(char *tx)
{
   static unsigned char atab[256] =
   {   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
      20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
      40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
      60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
      80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
     100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,
     120,121,122,123,124,125,126,127,128,129,130,131,132,133,229,135,136,137,138,139,
     140,141,142,197,144,230,198,147,148,149,150,151,152,153,154,248,156,216,158,159,
     160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,
     180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,
     200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,
     220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
     240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255};

   if (tx != NULL)
   {
      for (char *tp = tx;  *tp != '\0';  ++tp)
      {
         *tp = (char)atab[(unsigned char)*tp];
      }
   }

   return (tx);
}


/*
AR-920929
CH UT_ISO8859ToAscii8                                    Konverter til PC-ASCII
CD =============================================================================
CD Formål:
CD Konverterer en streng fra ISO8859-10 til PC-8-bits norsk ASCII.
CD
CD Parametre:
CD  Type    Navn     I/U   Forklaring
CD -----------------------------------------------------------------------------
CD  char   *tx        i    Peker til streng som skal konverteres
CD                         (avsluttet med '\0').
CD  char   *tp        r    Peker til konvertert streng.
CD
CD Bruk:
CD tp = UT_ISO8859ToAscii8(tx);
   =============================================================================
*/
SK_EntPnt_UT char *UT_ISO8859ToAscii8(char *tx)
{
   static unsigned char atab[256] =
   {   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
      20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
      40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
      60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
      80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
     100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,
     120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,
     140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
     160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,
     180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,143,146,199,
     200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,157,217,218,219,
     220,221,222,223,224,225,226,227,228,134,145,231,232,233,234,235,236,237,238,239,
     240,241,242,243,244,245,246,247,155,249,250,251,252,253,254,255};

   if (tx != NULL)
   {
      for (char *tp = tx;  *tp != '\0';  ++tp)
      {
         *tp = (char)atab[(unsigned char)*tp];
      }
   }

   return (tx);
}


/*
AR-920929
CH UT_Ascii7ToISO8859                                 Konverter til ISO8859-10
CD =============================================================================
CD Formål:
CD Konverterer en streng fra norsk 7-bits ASCII til ISO8859-10.
CD
CD Parametre:
CD  Type    Navn     I/U   Forklaring
CD -----------------------------------------------------------------------------
CD  char   *tx        i    Peker til streng som skal konverteres
CD                         (avsluttet med '\0').
CD  char   *tp        r    Peker til konvertert streng.
CD
CD Bruk:
CD tp = UT_Ascii7ToISO8859(tx)
   =============================================================================
*/
SK_EntPnt_UT char *UT_Ascii7ToISO8859(char *tx)
{
   static unsigned char atab[256] =
   {   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
      20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
      40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
      60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
      80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,198,216,197, 94, 95, 96, 97, 98, 99,
     100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,
     120,121,122,230,248,229,126,127,128,129,130,131,132,133,134,135,136,137,138,139,
     140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
     160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,
     180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,
     200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,
     220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
     240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255};

   if (tx != NULL)
   {
      for (char *tp = tx;  *tp != '\0';  ++tp)
      {
         *tp = (char)atab[(unsigned char)*tp];
      }
   }

   return (tx);
}


/*
AR-920929
CH UT_ISO8859ToAscii7                                    Konverter til ND-ASCII
CD =============================================================================
CD Formål:
CD Konverterer en streng fra ISO8859-10 til norsk 7-bits ASCII.
CD
CD Parametre:
CD  Type    Navn     I/U   Forklaring
CD -----------------------------------------------------------------------------
CD  char   *tx        i    Peker til streng som skal konverteres
CD                         (avsluttet med '\0').
CD  char   *tp        r    Peker til konvertert streng.
CD
CD Bruk:
CD tp = UT_ISO8859ToAscii7(tx);
   =============================================================================
*/
SK_EntPnt_UT char *UT_ISO8859ToAscii7(char *tx)
{
   static unsigned char atab[256] =
   {   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
      20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
      40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
      60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
      80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
     100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,
     120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,
     140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
     160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,
     180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196, 93, 91,199,
     200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215, 92,217,218,219,
     220,221,222,223,224,225,226,227,228,125,123,231,232,233,234,235,236,237,238,239,
     240,241,242,243,244,245,246,247,124,249,250,251,252,253,254,255};

   if (tx != NULL)
   {
      for (char *tp = tx;  *tp != '\0';  ++tp)
      {
         *tp = (char)atab[(unsigned char)*tp];
      }
   }

   return (tx);
}


/*
AR:2011-10-10
CH UT_ISO8859ToWChar                                       Konverter til wchar_t
CD =============================================================================
CD Formål:
CD Konverterer en streng fra 8 bits ISO8859-10 til 16 bits wchar_t (UNICODE).
CD
CD OBS! Avviker fra standard konvertering fra ISO 8859-10 til UNICODE, for å
CD handtere norske tilpassinger i ISO 8859-10.
CD 
CD Parametre:
CD Type      Navn    I/U   Forklaring
CD -----------------------------------------------------------------------------
CD char     *tx       i    Peker til streng som skal konverteres
CD                         (avsluttet med '\0').
CD wchar_t  *wtx      iu   Peker til buffer som motar konvertert streng.
CD                         wtx må ha plass til minst det antall tegn som er i tx,
CD                         inkludert null-terminator.  
CD wchar_t  *pwtx     r    Peker til konvertert streng.
CD
CD Bruk:
CD pwtx = UT_ISO8859ToWChar(tx,wtx);
   =============================================================================
*/
SK_EntPnt_UT wchar_t *UT_ISO8859ToWChar(const char *tx, wchar_t *wtx)
{
   // Konverteringstabell
   static wchar_t ISO885910_to_Unicode[0x100] = 
   {
      0x0000,   // 0x00	NULL
      0x0001,   // 0x01	START OF HEADING
      0x0002,   // 0x02	START OF TEXT
      0x0003,   // 0x03	END OF TEXT
      0x0004,   // 0x04	END OF TRANSMISSION
      0x0005,   // 0x05	ENQUIRY
      0x0006,   // 0x06	ACKNOWLEDGE
      0x0007,   // 0x07	BELL
      0x0008,   // 0x08	BACKSPACE
      0x0009,   // 0x09	HORIZONTAL TABULATION
      0x000A,   // 0x0A	LINE FEED
      0x000B,   // 0x0B	VERTICAL TABULATION
      0x000C,   // 0x0C	FORM FEED
      0x000D,   // 0x0D	CARRIAGE RETURN
      0x000E,   // 0x0E	SHIFT OUT
      0x000F,   // 0x0F	SHIFT IN
      0x0010,   // 0x10	DATA LINK ESCAPE
      0x0011,   // 0x11	DEVICE CONTROL ONE
      0x0012,   // 0x12	DEVICE CONTROL TWO
      0x0013,   // 0x13	DEVICE CONTROL THREE
      0x0014,   // 0x14	DEVICE CONTROL FOUR
      0x0015,   // 0x15	NEGATIVE ACKNOWLEDGE
      0x0016,   // 0x16	SYNCHRONOUS IDLE
      0x0017,   // 0x17	END OF TRANSMISSION BLOCK
      0x0018,   // 0x18	CANCEL
      0x0019,   // 0x19	END OF MEDIUM
      0x001A,   // 0x1A	SUBSTITUTE
      0x001B,   // 0x1B	ESCAPE
      0x001C,   // 0x1C	FILE SEPARATOR
      0x001D,   // 0x1D	GROUP SEPARATOR
      0x001E,   // 0x1E	RECORD SEPARATOR
      0x001F,   // 0x1F	UNIT SEPARATOR
      0x0020,   // 0x20	SPACE
      0x0021,   // 0x21	EXCLAMATION MARK
      0x0022,   // 0x22	QUOTATION MARK
      0x0023,   // 0x23	NUMBER SIGN
      0x0024,   // 0x24	DOLLAR SIGN
      0x0025,   // 0x25	PERCENT SIGN
      0x0026,   // 0x26	AMPERSAND
      0x0027,   // 0x27	APOSTROPHE
      0x0028,   // 0x28	LEFT PARENTHESIS
      0x0029,   // 0x29	RIGHT PARENTHESIS
      0x002A,   // 0x2A	ASTERISK
      0x002B,   // 0x2B	PLUS SIGN
      0x002C,   // 0x2C	COMMA
      0x002D,   // 0x2D	HYPHEN-MINUS
      0x002E,   // 0x2E	FULL STOP
      0x002F,   // 0x2F	SOLIDUS
      0x0030,   // 0x30	DIGIT ZERO
      0x0031,   // 0x31	DIGIT ONE
      0x0032,   // 0x32	DIGIT TWO
      0x0033,   // 0x33	DIGIT THREE
      0x0034,   // 0x34	DIGIT FOUR
      0x0035,   // 0x35	DIGIT FIVE
      0x0036,   // 0x36	DIGIT SIX
      0x0037,   // 0x37	DIGIT SEVEN
      0x0038,   // 0x38	DIGIT EIGHT
      0x0039,   // 0x39	DIGIT NINE
      0x003A,   // 0x3A	COLON
      0x003B,   // 0x3B	SEMICOLON
      0x003C,   // 0x3C	LESS-THAN SIGN
      0x003D,   // 0x3D	EQUALS SIGN
      0x003E,   // 0x3E	GREATER-THAN SIGN
      0x003F,   // 0x3F	QUESTION MARK
      0x0040,   // 0x40	COMMERCIAL AT
      0x0041,   // 0x41	LATIN CAPITAL LETTER A
      0x0042,   // 0x42	LATIN CAPITAL LETTER B
      0x0043,   // 0x43	LATIN CAPITAL LETTER C
      0x0044,   // 0x44	LATIN CAPITAL LETTER D
      0x0045,   // 0x45	LATIN CAPITAL LETTER E
      0x0046,   // 0x46	LATIN CAPITAL LETTER F
      0x0047,   // 0x47	LATIN CAPITAL LETTER G
      0x0048,   // 0x48	LATIN CAPITAL LETTER H
      0x0049,   // 0x49	LATIN CAPITAL LETTER I
      0x004A,   // 0x4A	LATIN CAPITAL LETTER J
      0x004B,   // 0x4B	LATIN CAPITAL LETTER K
      0x004C,   // 0x4C	LATIN CAPITAL LETTER L
      0x004D,   // 0x4D	LATIN CAPITAL LETTER M
      0x004E,   // 0x4E	LATIN CAPITAL LETTER N
      0x004F,   // 0x4F	LATIN CAPITAL LETTER O
      0x0050,   // 0x50	LATIN CAPITAL LETTER P
      0x0051,   // 0x51	LATIN CAPITAL LETTER Q
      0x0052,   // 0x52	LATIN CAPITAL LETTER R
      0x0053,   // 0x53	LATIN CAPITAL LETTER S
      0x0054,   // 0x54	LATIN CAPITAL LETTER T
      0x0055,   // 0x55	LATIN CAPITAL LETTER U
      0x0056,   // 0x56	LATIN CAPITAL LETTER V
      0x0057,   // 0x57	LATIN CAPITAL LETTER W
      0x0058,   // 0x58	LATIN CAPITAL LETTER X
      0x0059,   // 0x59	LATIN CAPITAL LETTER Y
      0x005A,   // 0x5A	LATIN CAPITAL LETTER Z
      0x005B,   // 0x5B	LEFT SQUARE BRACKET
      0x005C,   // 0x5C	REVERSE SOLIDUS
      0x005D,   // 0x5D	RIGHT SQUARE BRACKET
      0x005E,   // 0x5E	CIRCUMFLEX ACCENT
      0x005F,   // 0x5F	LOW LINE
      0x0060,   // 0x60	GRAVE ACCENT
      0x0061,   // 0x61	LATIN SMALL LETTER A
      0x0062,   // 0x62	LATIN SMALL LETTER B
      0x0063,   // 0x63	LATIN SMALL LETTER C
      0x0064,   // 0x64	LATIN SMALL LETTER D
      0x0065,   // 0x65	LATIN SMALL LETTER E
      0x0066,   // 0x66	LATIN SMALL LETTER F
      0x0067,   // 0x67	LATIN SMALL LETTER G
      0x0068,   // 0x68	LATIN SMALL LETTER H
      0x0069,   // 0x69	LATIN SMALL LETTER I
      0x006A,   // 0x6A	LATIN SMALL LETTER J
      0x006B,   // 0x6B	LATIN SMALL LETTER K
      0x006C,   // 0x6C	LATIN SMALL LETTER L
      0x006D,   // 0x6D	LATIN SMALL LETTER M
      0x006E,   // 0x6E	LATIN SMALL LETTER N
      0x006F,   // 0x6F	LATIN SMALL LETTER O
      0x0070,   // 0x70	LATIN SMALL LETTER P
      0x0071,   // 0x71	LATIN SMALL LETTER Q
      0x0072,   // 0x72	LATIN SMALL LETTER R
      0x0073,   // 0x73	LATIN SMALL LETTER S
      0x0074,   // 0x74	LATIN SMALL LETTER T
      0x0075,   // 0x75	LATIN SMALL LETTER U
      0x0076,   // 0x76	LATIN SMALL LETTER V
      0x0077,   // 0x77	LATIN SMALL LETTER W
      0x0078,   // 0x78	LATIN SMALL LETTER X
      0x0079,   // 0x79	LATIN SMALL LETTER Y
      0x007A,   // 0x7A	LATIN SMALL LETTER Z
      0x007B,   // 0x7B	LEFT CURLY BRACKET
      0x007C,   // 0x7C	VERTICAL LINE
      0x007D,   // 0x7D	RIGHT CURLY BRACKET
      0x007E,   // 0x7E	TILDE
      0x007F,   // 0x7F	DELETE
      0x0080,   // 0x80	<control>
      0x0081,   // 0x81	<control>
      0x0082,   // 0x82	<control>
      0x0083,   // 0x83	<control>
      0x0084,   // 0x84	<control>
      0x0085,   // 0x85	<control>
      0x0086,   // 0x86	<control>
      0x0087,   // 0x87	<control>
      0x0088,   // 0x88	<control>
      0x0089,   // 0x89	<control>
      0x008A,   // 0x8A	<control>
      0x008B,   // 0x8B	<control>
      0x008C,   // 0x8C	<control>
      0x008D,   // 0x8D	<control>
      0x008E,   // 0x8E	<control>
      0x008F,   // 0x8F	<control>
      0x0090,   // 0x90	<control>
      0x0091,   // 0x91	<control>
      0x0092,   // 0x92	<control>
      0x0093,   // 0x93	<control>
      0x0094,   // 0x94	<control>
      0x0095,   // 0x95	<control>
      0x0096,   // 0x96	<control>
      0x0097,   // 0x97	<control>
      0x0098,   // 0x98	<control>
      0x0099,   // 0x99	<control>
      0x009A,   // 0x9A	<control>
      0x009B,   // 0x9B	<control>
      0x009C,   // 0x9C	<control>
      0x009D,   // 0x9D	<control>
      0x009E,   // 0x9E	<control>

      //0x009F,   // 0x9F	<control>
      0x0178,   // 0x9F	LATIN CAPITAL LETTER Y WITH DIAERESIS

      0x00A0,   // 0xA0	NO-BREAK SPACE
      0x0104,   // 0xA1	LATIN CAPITAL LETTER A WITH OGONEK
      0x0112,   // 0xA2	LATIN CAPITAL LETTER E WITH MACRON
      0x0122,   // 0xA3	LATIN CAPITAL LETTER G WITH CEDILLA
      0x012A,   // 0xA4	LATIN CAPITAL LETTER I WITH MACRON
      0x0128,   // 0xA5	LATIN CAPITAL LETTER I WITH TILDE
      0x0136,   // 0xA6	LATIN CAPITAL LETTER K WITH CEDILLA
      0x00A7,   // 0xA7	SECTION SIGN
      0x013B,   // 0xA8	LATIN CAPITAL LETTER L WITH CEDILLA
      0x0110,   // 0xA9	LATIN CAPITAL LETTER D WITH STROKE
      0x0160,   // 0xAA	LATIN CAPITAL LETTER S WITH CARON
      0x0166,   // 0xAB	LATIN CAPITAL LETTER T WITH STROKE
      0x017D,   // 0xAC	LATIN CAPITAL LETTER Z WITH CARON
      0x00AD,   // 0xAD	SOFT HYPHEN
      0x016A,   // 0xAE	LATIN CAPITAL LETTER U WITH MACRON
      0x014A,   // 0xAF	LATIN CAPITAL LETTER ENG
      0x00B0,   // 0xB0	DEGREE SIGN
      0x0105,   // 0xB1	LATIN SMALL LETTER A WITH OGONEK
      0x0113,   // 0xB2	LATIN SMALL LETTER E WITH MACRON
      0x0123,   // 0xB3	LATIN SMALL LETTER G WITH CEDILLA
      0x012B,   // 0xB4	LATIN SMALL LETTER I WITH MACRON
      0x0129,   // 0xB5	LATIN SMALL LETTER I WITH TILDE
      0x0137,   // 0xB6	LATIN SMALL LETTER K WITH CEDILLA
      0x00B7,   // 0xB7	MIDDLE DOT
      0x013C,   // 0xB8	LATIN SMALL LETTER L WITH CEDILLA
      0x0111,   // 0xB9	LATIN SMALL LETTER D WITH STROKE
      0x0161,   // 0xBA	LATIN SMALL LETTER S WITH CARON
      0x0167,   // 0xBB	LATIN SMALL LETTER T WITH STROKE
      0x017E,   // 0xBC	LATIN SMALL LETTER Z WITH CARON
      0x2015,   // 0xBD	HORIZONTAL BAR
      0x016B,   // 0xBE	LATIN SMALL LETTER U WITH MACRON
      0x014B,   // 0xBF	LATIN SMALL LETTER ENG
      0x0100,   // 0xC0	LATIN CAPITAL LETTER A WITH MACRON
      0x00C1,   // 0xC1	LATIN CAPITAL LETTER A WITH ACUTE
      0x00C2,   // 0xC2	LATIN CAPITAL LETTER A WITH CIRCUMFLEX
      0x00C3,   // 0xC3	LATIN CAPITAL LETTER A WITH TILDE
      0x00C4,   // 0xC4	LATIN CAPITAL LETTER A WITH DIAERESIS
      0x00C5,   // 0xC5	LATIN CAPITAL LETTER A WITH RING ABOVE
      0x00C6,   // 0xC6	LATIN CAPITAL LETTER AE
      0x012E,   // 0xC7	LATIN CAPITAL LETTER I WITH OGONEK
      0x010C,   // 0xC8	LATIN CAPITAL LETTER C WITH CARON
      0x00C9,   // 0xC9	LATIN CAPITAL LETTER E WITH ACUTE
      0x0118,   // 0xCA	LATIN CAPITAL LETTER E WITH OGONEK
      0x00CB,   // 0xCB	LATIN CAPITAL LETTER E WITH DIAERESIS
      0x0116,   // 0xCC	LATIN CAPITAL LETTER E WITH DOT ABOVE
      0x00CD,   // 0xCD	LATIN CAPITAL LETTER I WITH ACUTE
      0x00CE,   // 0xCE	LATIN CAPITAL LETTER I WITH CIRCUMFLEX
      0x00CF,   // 0xCF	LATIN CAPITAL LETTER I WITH DIAERESIS
      0x00D0,   // 0xD0	LATIN CAPITAL LETTER ETH (Icelandic)

      //0x0145,   // 0xD1	LATIN CAPITAL LETTER N WITH CEDILLA
      0x0143,   // 0xD1	LATIN CAPITAL LETTER N WITH ACUTE

      0x014C,   // 0xD2	LATIN CAPITAL LETTER O WITH MACRON
      0x00D3,   // 0xD3	LATIN CAPITAL LETTER O WITH ACUTE
      0x00D4,   // 0xD4	LATIN CAPITAL LETTER O WITH CIRCUMFLEX
      0x00D5,   // 0xD5	LATIN CAPITAL LETTER O WITH TILDE
      0x00D6,   // 0xD6	LATIN CAPITAL LETTER O WITH DIAERESIS
      0x0168,   // 0xD7	LATIN CAPITAL LETTER U WITH TILDE
      0x00D8,   // 0xD8	LATIN CAPITAL LETTER O WITH STROKE
      0x0172,   // 0xD9	LATIN CAPITAL LETTER U WITH OGONEK
      0x00DA,   // 0xDA	LATIN CAPITAL LETTER U WITH ACUTE
      0x00DB,   // 0xDB	LATIN CAPITAL LETTER U WITH CIRCUMFLEX
      0x00DC,   // 0xDC	LATIN CAPITAL LETTER U WITH DIAERESIS
      0x00DD,   // 0xDD	LATIN CAPITAL LETTER Y WITH ACUTE
      0x00DE,   // 0xDE	LATIN CAPITAL LETTER THORN (Icelandic)
      0x00DF,   // 0xDF	LATIN SMALL LETTER SHARP S (German)
      0x0101,   // 0xE0	LATIN SMALL LETTER A WITH MACRON
      0x00E1,   // 0xE1	LATIN SMALL LETTER A WITH ACUTE
      0x00E2,   // 0xE2	LATIN SMALL LETTER A WITH CIRCUMFLEX
      0x00E3,   // 0xE3	LATIN SMALL LETTER A WITH TILDE
      0x00E4,   // 0xE4	LATIN SMALL LETTER A WITH DIAERESIS
      0x00E5,   // 0xE5	LATIN SMALL LETTER A WITH RING ABOVE
      0x00E6,   // 0xE6	LATIN SMALL LETTER AE
      0x012F,   // 0xE7	LATIN SMALL LETTER I WITH OGONEK
      0x010D,   // 0xE8	LATIN SMALL LETTER C WITH CARON
      0x00E9,   // 0xE9	LATIN SMALL LETTER E WITH ACUTE
      0x0119,   // 0xEA	LATIN SMALL LETTER E WITH OGONEK
      0x00EB,   // 0xEB	LATIN SMALL LETTER E WITH DIAERESIS
      0x0117,   // 0xEC	LATIN SMALL LETTER E WITH DOT ABOVE
      0x00ED,   // 0xED	LATIN SMALL LETTER I WITH ACUTE
      0x00EE,   // 0xEE	LATIN SMALL LETTER I WITH CIRCUMFLEX
      0x00EF,   // 0xEF	LATIN SMALL LETTER I WITH DIAERESIS
      0x00F0,   // 0xF0	LATIN SMALL LETTER ETH (Icelandic)

      //0x0146,   // 0xF1	LATIN SMALL LETTER N WITH CEDILLA
      0x0144,   // 0xF1	LATIN SMALL LETTER N WITH ACUTE

      0x014D,   // 0xF2	LATIN SMALL LETTER O WITH MACRON
      0x00F3,   // 0xF3	LATIN SMALL LETTER O WITH ACUTE
      0x00F4,   // 0xF4	LATIN SMALL LETTER O WITH CIRCUMFLEX
      0x00F5,   // 0xF5	LATIN SMALL LETTER O WITH TILDE
      0x00F6,   // 0xF6	LATIN SMALL LETTER O WITH DIAERESIS
      0x0169,   // 0xF7	LATIN SMALL LETTER U WITH TILDE
      0x00F8,   // 0xF8	LATIN SMALL LETTER O WITH STROKE
      0x0173,   // 0xF9	LATIN SMALL LETTER U WITH OGONEK
      0x00FA,   // 0xFA	LATIN SMALL LETTER U WITH ACUTE
      0x00FB,   // 0xFB	LATIN SMALL LETTER U WITH CIRCUMFLEX
      0x00FC,   // 0xFC	LATIN SMALL LETTER U WITH DIAERESIS
      0x00FD,   // 0xFD	LATIN SMALL LETTER Y WITH ACUTE
      0x00FE,   // 0xFE	LATIN SMALL LETTER THORN (Icelandic)

      //0x0138,   // 0xFF	LATIN SMALL LETTER KRA
      0x00FF,   // 0xFF LATIN SMALL LETTER Y WITH DIAERESIS
   };

   // Utfør konverteringen
   if (tx != NULL)
   {
      for ( ;  *tx != '\0';  ++tx,++wtx)
      {
         *wtx = ISO885910_to_Unicode[(unsigned char)*tx];
      }

      // Null-terminatoren
      *wtx = ISO885910_to_Unicode[(unsigned char)*tx];
   }
   
   return (wtx);
}


/*
AR:2011-10-10
CH UT_WCharToISO8859                                   Konverter til ISO8859-10
CD =============================================================================
CD Formål:
CD Konverterer en streng fra 16 bits wchar_t (UNICODE) til 8 bits ISO8859-10.
CD 
CD OBS! Avviker fra standard konvertering fra UNICODE til ISO 8859-10, for å
CD handtere norske tilpassinger i ISO 8859-10.
CD
CD Parametre:
CD Type      Navn    I/U   Forklaring
CD -----------------------------------------------------------------------------
CD wchar_t  *wtx      i    Peker til streng som skal konverteres
CD                         (avsluttet med '\0').
CD char     *tx       iu   Peker til buffer som motar konvertert streng.
CD                         tx må ha plass til minst det antall tegn som er i wtx,
CD                         inkludert null-terminator.  
CD char     *ptx      r    Peker til konvertert streng.
CD
CD Bruk:
CD ptx = UT_WCharToISO8859(wtx,tx);
   =============================================================================
*/
SK_EntPnt_UT char *UT_WCharToISO8859(const wchar_t *wtx, char *tx)
{
   // Utfør konverteringen
   if (wtx != NULL)
   {
      for ( ;  *wtx != L'\0';  ++wtx,++tx)
      {

         switch (*wtx)
         {
            case 0x0000 : *tx = '\x00'; break;  //	NULL
            case 0x0001 : *tx = '\x01'; break;  //	START OF HEADING
            case 0x0002 : *tx = '\x02'; break;  //	START OF TEXT
            case 0x0003 : *tx = '\x03'; break;  //	END OF TEXT
            case 0x0004 : *tx = '\x04'; break;  //	END OF TRANSMISSION
            case 0x0005 : *tx = '\x05'; break;  //	ENQUIRY
            case 0x0006 : *tx = '\x06'; break;  //	ACKNOWLEDGE
            case 0x0007 : *tx = '\x07'; break;  //	BELL
            case 0x0008 : *tx = '\x08'; break;  //	BACKSPACE
            case 0x0009 : *tx = '\x09'; break;  //	HORIZONTAL TABULATION
            case 0x000A : *tx = '\x0A'; break;  //	LINE FEED
            case 0x000B : *tx = '\x0B'; break;  //	VERTICAL TABULATION
            case 0x000C : *tx = '\x0C'; break;  //	FORM FEED
            case 0x000D : *tx = '\x0D'; break;  //	CARRIAGE RETURN
            case 0x000E : *tx = '\x0E'; break;  //	SHIFT OUT
            case 0x000F : *tx = '\x0F'; break;  //	SHIFT IN
            case 0x0010 : *tx = '\x10'; break;  //	DATA LINK ESCAPE
            case 0x0011 : *tx = '\x11'; break;  //	DEVICE CONTROL ONE
            case 0x0012 : *tx = '\x12'; break;  //	DEVICE CONTROL TWO
            case 0x0013 : *tx = '\x13'; break;  //	DEVICE CONTROL THREE
            case 0x0014 : *tx = '\x14'; break;  //	DEVICE CONTROL FOUR
            case 0x0015 : *tx = '\x15'; break;  //	NEGATIVE ACKNOWLEDGE
            case 0x0016 : *tx = '\x16'; break;  //	SYNCHRONOUS IDLE
            case 0x0017 : *tx = '\x17'; break;  //	END OF TRANSMISSION BLOCK
            case 0x0018 : *tx = '\x18'; break;  //	CANCEL
            case 0x0019 : *tx = '\x19'; break;  //	END OF MEDIUM
            case 0x001A : *tx = '\x1A'; break;  //	SUBSTITUTE
            case 0x001B : *tx = '\x1B'; break;  //	ESCAPE
            case 0x001C : *tx = '\x1C'; break;  //	FILE SEPARATOR
            case 0x001D : *tx = '\x1D'; break;  //	GROUP SEPARATOR
            case 0x001E : *tx = '\x1E'; break;  //	RECORD SEPARATOR
            case 0x001F : *tx = '\x1F'; break;  //	UNIT SEPARATOR
            case 0x0020 : *tx = '\x20'; break;  //	SPACE
            case 0x0021 : *tx = '\x21'; break;  //	EXCLAMATION MARK
            case 0x0022 : *tx = '\x22'; break;  //	QUOTATION MARK
            case 0x0023 : *tx = '\x23'; break;  //	NUMBER SIGN
            case 0x0024 : *tx = '\x24'; break;  //	DOLLAR SIGN
            case 0x0025 : *tx = '\x25'; break;  //	PERCENT SIGN
            case 0x0026 : *tx = '\x26'; break;  //	AMPERSAND
            case 0x0027 : *tx = '\x27'; break;  //	APOSTROPHE
            case 0x0028 : *tx = '\x28'; break;  //	LEFT PARENTHESIS
            case 0x0029 : *tx = '\x29'; break;  //	RIGHT PARENTHESIS
            case 0x002A : *tx = '\x2A'; break;  //	ASTERISK
            case 0x002B : *tx = '\x2B'; break;  //	PLUS SIGN
            case 0x002C : *tx = '\x2C'; break;  //	COMMA
            case 0x002D : *tx = '\x2D'; break;  //	HYPHEN-MINUS
            case 0x002E : *tx = '\x2E'; break;  //	FULL STOP
            case 0x002F : *tx = '\x2F'; break;  //	SOLIDUS
            case 0x0030 : *tx = '\x30'; break;  //	DIGIT ZERO
            case 0x0031 : *tx = '\x31'; break;  //	DIGIT ONE
            case 0x0032 : *tx = '\x32'; break;  //	DIGIT TWO
            case 0x0033 : *tx = '\x33'; break;  //	DIGIT THREE
            case 0x0034 : *tx = '\x34'; break;  //	DIGIT FOUR
            case 0x0035 : *tx = '\x35'; break;  //	DIGIT FIVE
            case 0x0036 : *tx = '\x36'; break;  //	DIGIT SIX
            case 0x0037 : *tx = '\x37'; break;  //	DIGIT SEVEN
            case 0x0038 : *tx = '\x38'; break;  //	DIGIT EIGHT
            case 0x0039 : *tx = '\x39'; break;  //	DIGIT NINE
            case 0x003A : *tx = '\x3A'; break;  //	COLON
            case 0x003B : *tx = '\x3B'; break;  //	SEMICOLON
            case 0x003C : *tx = '\x3C'; break;  //	LESS-THAN SIGN
            case 0x003D : *tx = '\x3D'; break;  //	EQUALS SIGN
            case 0x003E : *tx = '\x3E'; break;  //	GREATER-THAN SIGN
            case 0x003F : *tx = '\x3F'; break;  //	QUESTION MARK
            case 0x0040 : *tx = '\x40'; break;  //	COMMERCIAL AT
            case 0x0041 : *tx = '\x41'; break;  //	LATIN CAPITAL LETTER A
            case 0x0042 : *tx = '\x42'; break;  //	LATIN CAPITAL LETTER B
            case 0x0043 : *tx = '\x43'; break;  //	LATIN CAPITAL LETTER C
            case 0x0044 : *tx = '\x44'; break;  //	LATIN CAPITAL LETTER D
            case 0x0045 : *tx = '\x45'; break;  //	LATIN CAPITAL LETTER E
            case 0x0046 : *tx = '\x46'; break;  //	LATIN CAPITAL LETTER F
            case 0x0047 : *tx = '\x47'; break;  //	LATIN CAPITAL LETTER G
            case 0x0048 : *tx = '\x48'; break;  //	LATIN CAPITAL LETTER H
            case 0x0049 : *tx = '\x49'; break;  //	LATIN CAPITAL LETTER I
            case 0x004A : *tx = '\x4A'; break;  //	LATIN CAPITAL LETTER J
            case 0x004B : *tx = '\x4B'; break;  //	LATIN CAPITAL LETTER K
            case 0x004C : *tx = '\x4C'; break;  //	LATIN CAPITAL LETTER L
            case 0x004D : *tx = '\x4D'; break;  //	LATIN CAPITAL LETTER M
            case 0x004E : *tx = '\x4E'; break;  //	LATIN CAPITAL LETTER N
            case 0x004F : *tx = '\x4F'; break;  //	LATIN CAPITAL LETTER O
            case 0x0050 : *tx = '\x50'; break;  //	LATIN CAPITAL LETTER P
            case 0x0051 : *tx = '\x51'; break;  //	LATIN CAPITAL LETTER Q
            case 0x0052 : *tx = '\x52'; break;  //	LATIN CAPITAL LETTER R
            case 0x0053 : *tx = '\x53'; break;  //	LATIN CAPITAL LETTER S
            case 0x0054 : *tx = '\x54'; break;  //	LATIN CAPITAL LETTER T
            case 0x0055 : *tx = '\x55'; break;  //	LATIN CAPITAL LETTER U
            case 0x0056 : *tx = '\x56'; break;  //	LATIN CAPITAL LETTER V
            case 0x0057 : *tx = '\x57'; break;  //	LATIN CAPITAL LETTER W
            case 0x0058 : *tx = '\x58'; break;  //	LATIN CAPITAL LETTER X
            case 0x0059 : *tx = '\x59'; break;  //	LATIN CAPITAL LETTER Y
            case 0x005A : *tx = '\x5A'; break;  //	LATIN CAPITAL LETTER Z
            case 0x005B : *tx = '\x5B'; break;  //	LEFT SQUARE BRACKET
            case 0x005C : *tx = '\x5C'; break;  //	REVERSE SOLIDUS
            case 0x005D : *tx = '\x5D'; break;  //	RIGHT SQUARE BRACKET
            case 0x005E : *tx = '\x5E'; break;  //	CIRCUMFLEX ACCENT
            case 0x005F : *tx = '\x5F'; break;  //	LOW LINE
            case 0x0060 : *tx = '\x60'; break;  //	GRAVE ACCENT
            case 0x0061 : *tx = '\x61'; break;  //	LATIN SMALL LETTER A
            case 0x0062 : *tx = '\x62'; break;  //	LATIN SMALL LETTER B
            case 0x0063 : *tx = '\x63'; break;  //	LATIN SMALL LETTER C
            case 0x0064 : *tx = '\x64'; break;  //	LATIN SMALL LETTER D
            case 0x0065 : *tx = '\x65'; break;  //	LATIN SMALL LETTER E
            case 0x0066 : *tx = '\x66'; break;  //	LATIN SMALL LETTER F
            case 0x0067 : *tx = '\x67'; break;  //	LATIN SMALL LETTER G
            case 0x0068 : *tx = '\x68'; break;  //	LATIN SMALL LETTER H
            case 0x0069 : *tx = '\x69'; break;  //	LATIN SMALL LETTER I
            case 0x006A : *tx = '\x6A'; break;  //	LATIN SMALL LETTER J
            case 0x006B : *tx = '\x6B'; break;  //	LATIN SMALL LETTER K
            case 0x006C : *tx = '\x6C'; break;  //	LATIN SMALL LETTER L
            case 0x006D : *tx = '\x6D'; break;  //	LATIN SMALL LETTER M
            case 0x006E : *tx = '\x6E'; break;  //	LATIN SMALL LETTER N
            case 0x006F : *tx = '\x6F'; break;  //	LATIN SMALL LETTER O
            case 0x0070 : *tx = '\x70'; break;  //	LATIN SMALL LETTER P
            case 0x0071 : *tx = '\x71'; break;  //	LATIN SMALL LETTER Q
            case 0x0072 : *tx = '\x72'; break;  //	LATIN SMALL LETTER R
            case 0x0073 : *tx = '\x73'; break;  //	LATIN SMALL LETTER S
            case 0x0074 : *tx = '\x74'; break;  //	LATIN SMALL LETTER T
            case 0x0075 : *tx = '\x75'; break;  //	LATIN SMALL LETTER U
            case 0x0076 : *tx = '\x76'; break;  //	LATIN SMALL LETTER V
            case 0x0077 : *tx = '\x77'; break;  //	LATIN SMALL LETTER W
            case 0x0078 : *tx = '\x78'; break;  //	LATIN SMALL LETTER X
            case 0x0079 : *tx = '\x79'; break;  //	LATIN SMALL LETTER Y
            case 0x007A : *tx = '\x7A'; break;  //	LATIN SMALL LETTER Z
            case 0x007B : *tx = '\x7B'; break;  //	LEFT CURLY BRACKET
            case 0x007C : *tx = '\x7C'; break;  //	VERTICAL LINE
            case 0x007D : *tx = '\x7D'; break;  //	RIGHT CURLY BRACKET
            case 0x007E : *tx = '\x7E'; break;  //	TILDE
            case 0x007F : *tx = '\x7F'; break;  //	DELETE
            case 0x0080 : *tx = '\x80'; break;  //	<control>
            case 0x0081 : *tx = '\x81'; break;  //	<control>
            case 0x0082 : *tx = '\x82'; break;  //	<control>
            case 0x0083 : *tx = '\x83'; break;  //	<control>
            case 0x0084 : *tx = '\x84'; break;  //	<control>
            case 0x0085 : *tx = '\x85'; break;  //	<control>
            case 0x0086 : *tx = '\x86'; break;  //	<control>
            case 0x0087 : *tx = '\x87'; break;  //	<control>
            case 0x0088 : *tx = '\x88'; break;  //	<control>
            case 0x0089 : *tx = '\x89'; break;  //	<control>
            case 0x008A : *tx = '\x8A'; break;  //	<control>
            case 0x008B : *tx = '\x8B'; break;  //	<control>
            case 0x008C : *tx = '\x8C'; break;  //	<control>
            case 0x008D : *tx = '\x8D'; break;  //	<control>
            case 0x008E : *tx = '\x8E'; break;  //	<control>
            case 0x008F : *tx = '\x8F'; break;  //	<control>
            case 0x0090 : *tx = '\x90'; break;  //	<control>
            case 0x0091 : *tx = '\x91'; break;  //	<control>
            case 0x0092 : *tx = '\x92'; break;  //	<control>
            case 0x0093 : *tx = '\x93'; break;  //	<control>
            case 0x0094 : *tx = '\x94'; break;  //	<control>
            case 0x0095 : *tx = '\x95'; break;  //	<control>
            case 0x0096 : *tx = '\x96'; break;  //	<control>
            case 0x0097 : *tx = '\x97'; break;  //	<control>
            case 0x0098 : *tx = '\x98'; break;  //	<control>
            case 0x0099 : *tx = '\x99'; break;  //	<control>
            case 0x009A : *tx = '\x9A'; break;  //	<control>
            case 0x009B : *tx = '\x9B'; break;  //	<control>
            case 0x009C : *tx = '\x9C'; break;  //	<control>
            case 0x009D : *tx = '\x9D'; break;  //	<control>
            case 0x009E : *tx = '\x9E'; break;  //	<control>
            
            //case 0x009F : *tx = '\x9F'; break;  //	<control>
            case 0x0178 : *tx = '\x9F'; break;  //	LATIN CAPITAL LETTER Y WITH DIAERESIS

            case 0x00A0 : *tx = '\xA0'; break;  //	NO-BREAK SPACE
            case 0x0104 : *tx = '\xA1'; break;  //	LATIN CAPITAL LETTER A WITH OGONEK
            case 0x0112 : *tx = '\xA2'; break;  //	LATIN CAPITAL LETTER E WITH MACRON
            case 0x0122 : *tx = '\xA3'; break;  //	LATIN CAPITAL LETTER G WITH CEDILLA
            case 0x012A : *tx = '\xA4'; break;  //	LATIN CAPITAL LETTER I WITH MACRON
            case 0x0128 : *tx = '\xA5'; break;  //	LATIN CAPITAL LETTER I WITH TILDE
            case 0x0136 : *tx = '\xA6'; break;  //	LATIN CAPITAL LETTER K WITH CEDILLA
            case 0x00A7 : *tx = '\xA7'; break;  //	SECTION SIGN
            case 0x013B : *tx = '\xA8'; break;  //	LATIN CAPITAL LETTER L WITH CEDILLA
            case 0x0110 : *tx = '\xA9'; break;  //	LATIN CAPITAL LETTER D WITH STROKE
            case 0x0160 : *tx = '\xAA'; break;  //	LATIN CAPITAL LETTER S WITH CARON
            case 0x0166 : *tx = '\xAB'; break;  //	LATIN CAPITAL LETTER T WITH STROKE
            case 0x017D : *tx = '\xAC'; break;  //	LATIN CAPITAL LETTER Z WITH CARON
            case 0x00AD : *tx = '\xAD'; break;  //	SOFT HYPHEN
            case 0x016A : *tx = '\xAE'; break;  //	LATIN CAPITAL LETTER U WITH MACRON
            case 0x014A : *tx = '\xAF'; break;  //	LATIN CAPITAL LETTER ENG
            case 0x00B0 : *tx = '\xB0'; break;  //	DEGREE SIGN
            case 0x0105 : *tx = '\xB1'; break;  //	LATIN SMALL LETTER A WITH OGONEK
            case 0x0113 : *tx = '\xB2'; break;  //	LATIN SMALL LETTER E WITH MACRON
            case 0x0123 : *tx = '\xB3'; break;  //	LATIN SMALL LETTER G WITH CEDILLA
            case 0x012B : *tx = '\xB4'; break;  //	LATIN SMALL LETTER I WITH MACRON
            case 0x0129 : *tx = '\xB5'; break;  //	LATIN SMALL LETTER I WITH TILDE
            case 0x0137 : *tx = '\xB6'; break;  //	LATIN SMALL LETTER K WITH CEDILLA
            case 0x00B7 : *tx = '\xB7'; break;  //	MIDDLE DOT
            case 0x013C : *tx = '\xB8'; break;  //	LATIN SMALL LETTER L WITH CEDILLA
            case 0x0111 : *tx = '\xB9'; break;  //	LATIN SMALL LETTER D WITH STROKE
            case 0x0161 : *tx = '\xBA'; break;  //	LATIN SMALL LETTER S WITH CARON
            case 0x0167 : *tx = '\xBB'; break;  //	LATIN SMALL LETTER T WITH STROKE
            case 0x017E : *tx = '\xBC'; break;  //	LATIN SMALL LETTER Z WITH CARON
            case 0x2015 : *tx = '\xBD'; break;  //	HORIZONTAL BAR
            case 0x016B : *tx = '\xBE'; break;  //	LATIN SMALL LETTER U WITH MACRON
            case 0x014B : *tx = '\xBF'; break;  //	LATIN SMALL LETTER ENG
            case 0x0100 : *tx = '\xC0'; break;  //	LATIN CAPITAL LETTER A WITH MACRON
            case 0x00C1 : *tx = '\xC1'; break;  //	LATIN CAPITAL LETTER A WITH ACUTE
            case 0x00C2 : *tx = '\xC2'; break;  //	LATIN CAPITAL LETTER A WITH CIRCUMFLEX
            case 0x00C3 : *tx = '\xC3'; break;  //	LATIN CAPITAL LETTER A WITH TILDE
            case 0x00C4 : *tx = '\xC4'; break;  //	LATIN CAPITAL LETTER A WITH DIAERESIS
            case 0x00C5 : *tx = '\xC5'; break;  //	LATIN CAPITAL LETTER A WITH RING ABOVE
            case 0x00C6 : *tx = '\xC6'; break;  //	LATIN CAPITAL LETTER AE
            case 0x012E : *tx = '\xC7'; break;  //	LATIN CAPITAL LETTER I WITH OGONEK
            case 0x010C : *tx = '\xC8'; break;  //	LATIN CAPITAL LETTER C WITH CARON
            case 0x00C9 : *tx = '\xC9'; break;  //	LATIN CAPITAL LETTER E WITH ACUTE
            case 0x0118 : *tx = '\xCA'; break;  //	LATIN CAPITAL LETTER E WITH OGONEK
            case 0x00CB : *tx = '\xCB'; break;  //	LATIN CAPITAL LETTER E WITH DIAERESIS
            case 0x0116 : *tx = '\xCC'; break;  //	LATIN CAPITAL LETTER E WITH DOT ABOVE
            case 0x00CD : *tx = '\xCD'; break;  //	LATIN CAPITAL LETTER I WITH ACUTE
            case 0x00CE : *tx = '\xCE'; break;  //	LATIN CAPITAL LETTER I WITH CIRCUMFLEX
            case 0x00CF : *tx = '\xCF'; break;  //	LATIN CAPITAL LETTER I WITH DIAERESIS
            case 0x00D0 : *tx = '\xD0'; break;  //	LATIN CAPITAL LETTER ETH (Icelandic)
            
            //case 0x0145 : *tx = '\xD1'; break;  //	LATIN CAPITAL LETTER N WITH CEDILLA
            //case 0x0209 : *tx = '\xD1'; break;  //	LATIN CAPITAL LETTER N WITH ACUTE // AR:2013-09-16 byttet til ny kode
            case 0x0143 : *tx = '\xD1'; break;  //	LATIN CAPITAL LETTER N WITH ACUTE

            case 0x014C : *tx = '\xD2'; break;  //	LATIN CAPITAL LETTER O WITH MACRON
            case 0x00D3 : *tx = '\xD3'; break;  //	LATIN CAPITAL LETTER O WITH ACUTE
            case 0x00D4 : *tx = '\xD4'; break;  //	LATIN CAPITAL LETTER O WITH CIRCUMFLEX
            case 0x00D5 : *tx = '\xD5'; break;  //	LATIN CAPITAL LETTER O WITH TILDE
            case 0x00D6 : *tx = '\xD6'; break;  //	LATIN CAPITAL LETTER O WITH DIAERESIS
            case 0x0168 : *tx = '\xD7'; break;  //	LATIN CAPITAL LETTER U WITH TILDE
            case 0x00D8 : *tx = '\xD8'; break;  //	LATIN CAPITAL LETTER O WITH STROKE
            case 0x0172 : *tx = '\xD9'; break;  //	LATIN CAPITAL LETTER U WITH OGONEK
            case 0x00DA : *tx = '\xDA'; break;  //	LATIN CAPITAL LETTER U WITH ACUTE
            case 0x00DB : *tx = '\xDB'; break;  //	LATIN CAPITAL LETTER U WITH CIRCUMFLEX
            case 0x00DC : *tx = '\xDC'; break;  //	LATIN CAPITAL LETTER U WITH DIAERESIS
            case 0x00DD : *tx = '\xDD'; break;  //	LATIN CAPITAL LETTER Y WITH ACUTE
            case 0x00DE : *tx = '\xDE'; break;  //	LATIN CAPITAL LETTER THORN (Icelandic)
            case 0x00DF : *tx = '\xDF'; break;  //	LATIN SMALL LETTER SHARP S (German)
            case 0x0101 : *tx = '\xE0'; break;  //	LATIN SMALL LETTER A WITH MACRON
            case 0x00E1 : *tx = '\xE1'; break;  //	LATIN SMALL LETTER A WITH ACUTE
            case 0x00E2 : *tx = '\xE2'; break;  //	LATIN SMALL LETTER A WITH CIRCUMFLEX
            case 0x00E3 : *tx = '\xE3'; break;  //	LATIN SMALL LETTER A WITH TILDE
            case 0x00E4 : *tx = '\xE4'; break;  //	LATIN SMALL LETTER A WITH DIAERESIS
            case 0x00E5 : *tx = '\xE5'; break;  //	LATIN SMALL LETTER A WITH RING ABOVE
            case 0x00E6 : *tx = '\xE6'; break;  //	LATIN SMALL LETTER AE
            case 0x012F : *tx = '\xE7'; break;  //	LATIN SMALL LETTER I WITH OGONEK
            case 0x010D : *tx = '\xE8'; break;  //	LATIN SMALL LETTER C WITH CARON
            case 0x00E9 : *tx = '\xE9'; break;  //	LATIN SMALL LETTER E WITH ACUTE
            case 0x0119 : *tx = '\xEA'; break;  //	LATIN SMALL LETTER E WITH OGONEK
            case 0x00EB : *tx = '\xEB'; break;  //	LATIN SMALL LETTER E WITH DIAERESIS
            case 0x0117 : *tx = '\xEC'; break;  //	LATIN SMALL LETTER E WITH DOT ABOVE
            case 0x00ED : *tx = '\xED'; break;  //	LATIN SMALL LETTER I WITH ACUTE
            case 0x00EE : *tx = '\xEE'; break;  //	LATIN SMALL LETTER I WITH CIRCUMFLEX
            case 0x00EF : *tx = '\xEF'; break;  //	LATIN SMALL LETTER I WITH DIAERESIS
            case 0x00F0 : *tx = '\xF0'; break;  //	LATIN SMALL LETTER ETH (Icelandic)

            //case 0x0146 : *tx = '\xF1'; break;  //	LATIN SMALL LETTER N WITH CEDILLA
            case 0x0144 : *tx = '\xF1'; break;  //	LATIN SMALL LETTER N WITH ACUTE

            case 0x014D : *tx = '\xF2'; break;  //	LATIN SMALL LETTER O WITH MACRON
            case 0x00F3 : *tx = '\xF3'; break;  //	LATIN SMALL LETTER O WITH ACUTE
            case 0x00F4 : *tx = '\xF4'; break;  //	LATIN SMALL LETTER O WITH CIRCUMFLEX
            case 0x00F5 : *tx = '\xF5'; break;  //	LATIN SMALL LETTER O WITH TILDE
            case 0x00F6 : *tx = '\xF6'; break;  //	LATIN SMALL LETTER O WITH DIAERESIS
            case 0x0169 : *tx = '\xF7'; break;  //	LATIN SMALL LETTER U WITH TILDE
            case 0x00F8 : *tx = '\xF8'; break;  //	LATIN SMALL LETTER O WITH STROKE
            case 0x0173 : *tx = '\xF9'; break;  //	LATIN SMALL LETTER U WITH OGONEK
            case 0x00FA : *tx = '\xFA'; break;  //	LATIN SMALL LETTER U WITH ACUTE
            case 0x00FB : *tx = '\xFB'; break;  //	LATIN SMALL LETTER U WITH CIRCUMFLEX
            case 0x00FC : *tx = '\xFC'; break;  //	LATIN SMALL LETTER U WITH DIAERESIS
            case 0x00FD : *tx = '\xFD'; break;  //	LATIN SMALL LETTER Y WITH ACUTE
            case 0x00FE : *tx = '\xFE'; break;  //	LATIN SMALL LETTER THORN (Icelandic)

            //case 0x0138 : *tx = '\xFF'; break;  //	LATIN SMALL LETTER KRA
            case 0x00FF : *tx = '\xFF'; break;  //	LATIN SMALL LETTER Y WITH DIAERESIS

            default: *tx = '?';               //   Ukjent tegn
         }
      }

      // Null-terminatoren
      *tx = '\0';
   }
   
   return (tx);
}


const byte MASKBITS = 0x3F;    // 0011 1111
const byte MASK1BIT = 0x80;    // 1000 0000
const byte MASK2BIT = 0xC0;    // 1100 0000
const byte MASK3BIT = 0xE0;    // 1110 0000
const byte MASK4BIT = 0xF0;    // 1111 0000


/*
AR:2011-10-07
CH UT_Utf8ToWChar                               Konverter fra UTF-8 til wchar_t
CD =============================================================================
CD Formål:
CD Konverterer et multibyte UTF-8 tegn til 16 bits UNICODE.
CD
CD Parametre:
CD  Type     Navn     I/U   Forklaring
CD -----------------------------------------------------------------------------
CD  char    *tx        i    Peker til UTF-8 tegn. (1, 2 eller 3 byte).
CD  size_t  *numBytes  u    Antall byte brukt fra tx.
CD  wchar_t  wc        r    16 bits tegn.
CD
CD Bruk:
CD wc = UT_Utf8ToWChar(tx, &numBytes);
   =============================================================================
*/
SK_EntPnt_UT wchar_t UT_Utf8ToWChar( const char *tx, int *numBytes ) 
{
   if ( tx[0] < MASK1BIT && tx[0] >= 0 ) 
   {
      *numBytes = 1;
      return tx[0];
   } 
   else if((tx[0] & MASK3BIT) == MASK2BIT)
   {
      *numBytes = 2;
      return ((tx[0] & 0x1F) << 6) | (tx[1] & MASKBITS);
   } 
   else if( (tx[0] & MASK4BIT) == MASK3BIT )
   {
      *numBytes = 3;
      return ((tx[0] & 0x0F) << 12) | ( (tx[1] & MASKBITS) << 6) | (tx[2] & MASKBITS);
   }

   *numBytes = 1;
   return L'?';
}


/*
AR:2011-10-07
CH UT_WCharToUtf8                               Konverter fra UTF-8 til wchar_t
CD =============================================================================
CD Formål:
CD Konverterer et multibyte UTF-8 tegn til 16 bits UNICODE.
CD
CD Parametre:
CD  Type     Navn     I/U   Forklaring
CD -----------------------------------------------------------------------------

CD  wchar_t  wc        i    16 bits tegn.
CD  char    *dest      i    Peker til buffer som skal motta UTF-8 tegnsekvens.
CD                          Det må være plass til minst 3 byte i dest.   
CD                          Det skrives 1, 2 eller 3 byte til dest. 
CD                          Null-terminator blir ikke skrevet til dest.
CD  char    *next      r    Peker til posisjonen etter det som er skrevet til dest.
CD                          Kallende program kan enten legge inn null-terminator,
CD                          eller fortsette videre på strengen.  
CD
CD Bruk:
CD wc = UT_WCharToUtf8(tx, &numBytes);
   =============================================================================
*/
SK_EntPnt_UT char *UT_WCharToUtf8( wchar_t wc, char *dest )
{
   //
   // Under Windows kunne denne ha vært erstattet med WideCharToMultiByte 
   //

   if( wc < 0x80 )
   {
      dest[0] = (char)wc;
      //dest[1] = '\0';
      return &dest[1];
   } 
   else if( wc < 0x800 ) 
   {
      dest[0] = (char)(MASK2BIT | wc >> 6);
      dest[1] = (char)(MASK1BIT | wc & MASKBITS);
      //dest[2] = '\0';
      return &dest[2];
   }
   else if( wc < 0x10000 ) 
   {
      dest[0] = (char)(MASK3BIT | wc >> 12);
      dest[1] = (char)(MASK1BIT | wc >> 6 & MASKBITS);
      dest[2] = (char)(MASK1BIT | wc & MASKBITS);
      //dest[3] = '\0';
      return &dest[3];
   }

   else
   {
      dest[0] = '?';
      //dest[1] = '\0';
      return &dest[1];
   }
}


/*
AR:2011-10-07
CH UT_GetUtf8Len                                   Finn konvertert strenglengde
CD =============================================================================
CD Formål:
CD Beregner strenglengde av en 16 bits UNICODE streng hvis 
CD den konverteres til UTF-8 tegnsett.
CD
CD Parametre:
CD  Type     Navn     I/U   Forklaring
CD -----------------------------------------------------------------------------
CD  wchar_t *wtx       i    16 bits UNICODE streng.
CD  size_t  *numBytes  r    Lengde uten null-terminator.
CD
CD Bruk:
CD wc = UT_GetUtf8Len(tx, &numBytes);
   =============================================================================
*/
SK_EntPnt_UT size_t UT_GetUtf8Len(const wchar_t *wtx)
{
   size_t numBytes = 0;

   for ( ; *wtx != L'\0'; ++wtx)
   {
      if (*wtx < 0x80)
         ++numBytes;
      else if (*wtx < 0x800)
         numBytes += 2;
      else
         numBytes += 3;
   }

   return numBytes;
}

/*
HT-980519
CH UT_ErUtf8                                                   Sjekker tegnsett
CD =============================================================================
CD Formål:
CD Sjekker om strengen inneholder tegnkombinasjoner som gjør at den
CD må handteres som UTF-8.
CD
CD Parametre:
CD Type    Navn   I/U   Forklaring
CD -----------------------------------------------------------------------------
CD char  *pszTx    i   NULL-terminert streng som skal testes.
CD bool   bUtf8    r   Status:  true  = Er UTF-8
CD                              false = Ikke funnet tegn som tilsier UTF-8
CD Bruk:
CD bUtf8 = UT_ErUtf8(pszTx);
CD =============================================================================
*/
SK_EntPnt_UT bool UT_ErUtf8(const char *pszTx)
{
   bool bUtf8 = false;
   bool bUlovligUtf8 = false;

   while ( *pszTx )
   {
      {
         if ( pszTx[0] < MASK1BIT && pszTx[0] >= 0 ) 
         {
            pszTx++;
         } 
         else if((pszTx[0] & MASK3BIT) == MASK2BIT)     // Har bit-sekvens 110x xxxx
         {
            if((pszTx[1] & MASK2BIT) == MASK1BIT)
            {
               bUtf8 = true;
               pszTx += 2;
            }
            else
            {
               // Ulovlig kombinasjon, byte 2 starter med ulovlig bit-kombinasjon
               bUlovligUtf8 = true;
               pszTx++;
            }
         } 
         else if( (pszTx[0] & MASK4BIT) == MASK3BIT )   // Har bit-sekvens 1110 xxxx
         {
            if((pszTx[1] & MASK2BIT) == MASK1BIT  &&
               (pszTx[2] & MASK2BIT) == MASK1BIT)
            {
               bUtf8 = true;
               pszTx += 3;
            }
            else
            {
               // Ulovlig kombinasjon, byte 2 eller 3 starter med ulovlig bit-kombinasjon
               bUlovligUtf8 = true;
               pszTx++;
            }
         }
         else
         {
            // Ulovlig kombinasjon
            bUlovligUtf8 = true;
            pszTx++;
         }
      }
   }

   return ( bUtf8 && ( ! bUlovligUtf8) );
}

SK_EntPnt_UT bool UT_TestNormalUtf8_2byte(const char *pszTx)
{
   if((unsigned char)pszTx[0] == 0xC3)
   {
      switch((unsigned char)pszTx[1]) {
         case 0x86: // Æ
         case 0xA6: // æ
         case 0x98: // Ø
         case 0xB8: // ø 
         case 0x85: // Å
         case 0xA5: // å      
         case 0x89: // É
         case 0xA9: // é
         case 0x84: // Ä
         case 0xA4: // ä
         case 0x96: // Ö
         case 0xB6: // ö
         case 0x9C: // Ü
         case 0xBC: // ü
         case 0x81: // Á
         case 0xA1: // á
         case 0x8F: // Ï
         case 0xAF: // ï
         case 0x82: // Â
         case 0xA2: // â
         case 0x95: // Õ
         case 0xB5: // õ
         case 0xBF: // ÿ
         case 0x80: // À
         case 0xA0: // à
         case 0xA8: // è
         case 0x9F: // ß
         return true;
      }
   }
   else if((unsigned char)pszTx[0] == 0xC4)
   {
      switch((unsigned char)pszTx[1]) {
         case 0x8C: // Č
         case 0x8D: // č
         case 0x90: // Đ
         case 0x91: // đ
         return true;
      }
   }
   else if((unsigned char)pszTx[0] == 0xC5)
   {
      switch((unsigned char)pszTx[1]) {
         case 0x8A: // Ŋ
         case 0x8B: // ŋ
         case 0xA0: // Š
         case 0xA1: // š
         case 0xA6: // Ŧ
         case 0xA7: // ŧ
         case 0xBD: // Ž
         case 0xBE: // ž
         case 0x83: // Ń
         case 0x84: // ń
         case 0xB8: // Ÿ
         return true;
      }
   }
   else if((unsigned char)pszTx[0] == 0xC6)
   {
      switch((unsigned char)pszTx[1]) {
         case 0xB7: // Ʒ
         return true;
      }
   }
   else if((unsigned char)pszTx[0] == 0xCA)
   {
      switch((unsigned char)pszTx[1]) {
         case 0x92: // ȝ
         return true;
      }
   }
   else if((unsigned char)pszTx[0] == 0xC7)
   {
      switch((unsigned char)pszTx[1]) {
         case 0xAE: // Ǯ
         case 0xAF: // ǯ
         case 0xA6: // Ǧ
         case 0xA7: // ǧ
         case 0xA4: // Ǥ
         case 0xA5: // ǥ
         case 0xA8: // Ǩ
         case 0xA9: // ǩ
         return true;
      }
   }
   else if((unsigned char)pszTx[0] == 0xCB)
   {
      switch((unsigned char)pszTx[1]) {
         case 0x8A: // ʹ
         return true;
      }
   }
   else if((unsigned char)pszTx[0] == 0xC2)
   {
      switch((unsigned char)pszTx[1]) {
         case 0xB4: // ´
         case 0xA9: // ©
         case 0xAF: // ¯
         case 0xB0: // °
         case 0xBC: // ¼
         case 0xBD: // ½
         case 0xBE: // ¾
         case 0xA4: // ¤
         return true;
      }
   }

   return false;
}

SK_EntPnt_UT bool UT_TestNormalUtf8_3byte(const char *pszTx)
{   
   if(((unsigned char)pszTx[0] == 0xEF) && ((unsigned char)pszTx[1] == 0xBB) && ((unsigned char)pszTx[2] == 0xBF)) // BOM
      return true;
   if(((unsigned char)pszTx[0] == 0xE2) && ((unsigned char)pszTx[1] == 0x80) && ((unsigned char)pszTx[2] == 0x93)) // –
      return true;
   
   return false;
}

/*
PG-130125
CH UT_ErNormalUtf8                                              Sjekker tegnsett
CD =============================================================================
CD Formål:
CD Sjekker om strengen inneholder normale tegnkombinasjoner som gjør at den
CD må handteres som UTF-8.
CD
CD Parametre:
CD Type    Navn   I/U   Forklaring
CD -----------------------------------------------------------------------------
CD char  *pszTx    i   NULL-terminert streng som skal testes.
CD short  sUtf8    r   Status:   UTF8_ULOVLIG // Ulovlig UTF-8
CD                               UTF8_IKKE    // Ikke UTF-8
CD                               UTF8_LOVLIG  // UTF-8
CD Bruk:
CD sUtf8 = UT_ErNormalUtf8(pszTx);
CD =============================================================================
*/
SK_EntPnt_UT short UT_ErNormalUtf8(const char *pszTx)
{
   bool bUtf8 = false;
   bool bUlovligUtf8 = false;

   while ( *pszTx )
   {
      {
         if ( pszTx[0] < MASK1BIT && pszTx[0] >= 0 ) 
         {
            ++pszTx;
         } 
         else if((pszTx[0] & MASK3BIT) == MASK2BIT)     // Har bit-sekvens 110x xxxx
         {
            if((pszTx[1] & MASK2BIT) == MASK1BIT)
            {
               if(UT_TestNormalUtf8_2byte(pszTx)) // Tester på normale norske tegn
               {
                  bUtf8 = true;
                  pszTx += 2;
               }
               else
               {
                  // Ikke normal norsk kombinasjon
                  return UTF8_ULOVLIG;
               }
            }
            else
            {
               // Ulovlig kombinasjon, byte 2 starter med ulovlig bit-kombinasjon
               return UTF8_ULOVLIG;
            }
         }
         // Forekommer sjelden i norske filer
         else if( (pszTx[0] & MASK4BIT) == MASK3BIT )   // Har bit-sekvens 1110 xxxx
         {
            if((pszTx[1] & MASK2BIT) == MASK1BIT  &&
               (pszTx[2] & MASK2BIT) == MASK1BIT)
            {
               if(UT_TestNormalUtf8_3byte(pszTx)) // Tester på normale norske tegn
               {
                  bUtf8 = true;
                  pszTx += 3;
               }
               else
               {
                  // Ikke normal norsk kombinasjon
                  return UTF8_ULOVLIG;
               }
            }
            else
            {
               // Ulovlig kombinasjon, byte 2 eller 3 starter med ulovlig bit-kombinasjon
               return UTF8_ULOVLIG;
            }
         }
         else
         {
            // Ulovlig kombinasjon
            return UTF8_ULOVLIG;
         }
      }
   }
   if(bUtf8)
   {
      return UTF8_LOVLIG;
   }

   return UTF8_IKKE;
}

/*
PG-130125
CH UT_ErUtf8Fil                                                 Sjekker tegnsett
CD =============================================================================
CD Formål:
CD Sjekker om fila har "Byte order mark", om fila inneholder vanlige UTF-8 tegn
CD og om den er fri for ulovlige UTF-8-kombinasjoner
CD
CD Parametre:
CD Type     Navn   I/U   Forklaring
CD -----------------------------------------------------------------------------
CD wchar_t *pszTx    i   NULL-terminert streng som skal testes.
CD short    sUtf8    r   Status:  FIL_UTF8_IKKE   // Ikke UTF-8 fil
CD                                FIL_UTF8_BOM    // UTF-8 fil med BOM
CD                                FIL_UTF8        // UTF-8 fil uten BOM
CD Bruk:
CD sUtf8 = UT_ErUtf8Fil(pszFilnavn);
CD =============================================================================
*/
SK_EntPnt_UT short UT_ErUtf8Fil(const wchar_t *pszFilnavn)
{
   bool bFunnetUtf8 = false;
   short sFilUtf8 = FIL_UTF8, ierr;

   FILE *pTestfil;

   // Åpner fil
   pTestfil = UT_OpenFile(pszFilnavn, L"sos", UT_READ, UT_OLD, &ierr);
   if (ierr != UT_OK)
      return -1; // Feil på fil

   short sLen = 2048;
   char *pszBuffer = (char *)malloc(sLen*sizeof(char));

   UT_INT64 n64FilPos;
   short sStatus = UT_OK;

   do {
      UT_GetPos_i64(pTestfil, &n64FilPos);

      sStatus = UT_ReadLine(pTestfil, sLen, pszBuffer);

      if (sStatus == UT_OK)
      {
         // Starten av filen, finn eventuelt byte order mark (BOM)
         if (n64FilPos == 0)
         {
            if (strlen(pszBuffer) > 2  &&
               (unsigned char)pszBuffer[0] == 0xEF  &&
               (unsigned char)pszBuffer[1] == 0xBB  &&
               (unsigned char)pszBuffer[2] == 0xBF )
            {
               // Funnet "byte order mark"
               sFilUtf8 = FIL_UTF8_BOM;
               break; // Hopper over resten
            }
         }
         short sUtf8 = UT_ErNormalUtf8(pszBuffer); // Tester linja for UTF-8
         if(sUtf8 == UTF8_ULOVLIG)
         {
            // Funnet ulovlig UTF-8
            sFilUtf8 = FIL_UTF8_IKKE;
            break; // Hopper over resten
         }
         else if(sUtf8 == UTF8_LOVLIG)
            bFunnetUtf8 = true;
      }
   }
   while (sStatus == UT_OK);

   free(pszBuffer); pszBuffer = NULL;
   fclose(pTestfil); pTestfil = NULL;

   if((sFilUtf8 == FIL_UTF8) && (bFunnetUtf8 == false)) // Ikke funnet noe. Mest normalt i 2013..
      sFilUtf8 = FIL_UTF8_IKKE;
  
   return sFilUtf8;
}

/*
For koder over 127 er kun følgende tillatt benyttet i ei SOSI-fil.
De særnorske tegnene ÆØÅ er plassert på følgende koder:
                   Æ     Ø     Å      æ     ø     å     é
DOSN8            146   157   143    145   155   134 
ND7,DECN7         91    92    93    123   124   125
ISO8859-1 og 10  198   216   197    230   248   229   233


Samiske tegn i ISO8859-10, og tegnets symbol ved bruk av
standard Windows font:

8-bits kode:  169 170 171 172 175 185 186 187 188 191 193
Symbol:         ©   ª   ½   ¬   ¯   ¹   º   ╗   ¼   ¿   Á

8-bits kode:  196 200 207 209 214 225 228 232 239 241 246
Symbol:         Ä   È   Ï   Ñ   Ö   á   ä   è   ï   ñ   ö

*/


/*
AR-890113
CH UT_KonverterTegnsett_8_8                                   Konverter tegnsett
CD ==========================================================================
CD Formål:
CD Konverterer en streng fra et 7/8 bits tegnsett til et annet 7/8 bits tegnsett.
CD
CD Parametre:
CD  Type            Navn         I/U  Forklaring
CD -----------------------------------------------------------------------------
CD  short           sFraTegnsett  i   Fra tegnsett:
CD                                      TS_DOSN8   = DOS norsk 8-bits
CD                                      TS_ND7     = Norsk Data 7-bits
CD                                      TS_DECM8   = DEC multinasjonal 8-bits 
CD                                                   (Handteres likt med TS_ISO8859) 
CD                                      TS_DECN7   = DEC norsk 7-bits
CD                                                   (Handteres likt med TS_ND7)
CD                                      TS_ISO8859 = ISO8859-1 og 10
CD  short           sTilTegnsett  i   Til tegnsett 
CD                                      TS_UKJENT  = Ukjent tegnsett
CD                                      TS_DOSN8   = DOS norsk 8-bits
CD                                      TS_ND7     = Norsk Data 7-bits
CD                                      TS_DECM8   = DEC multinasjonal 8-bits
CD                                                   (Handteres likt med TS_ISO8859)
CD                                      TS_DECN7   = DEC norsk 7-bits
CD                                                   (Handteres likt med TS_ND7)
CD                                      TS_ISO8859 = ISO8859-1 og 10
CD  unsigned char  *pszTx         iu  Peker til streng som skal konverteres
CD                                    (avsluttet med '\0').
CD
CD Bruk:
CD UT_KonverterTegnsett_8_8(sFraTegnsett,sTilTegnsett,pszTx);
   =============================================================================
*/
SK_EntPnt_UT void UT_KonverterTegnsett_8_8(short sFraTegnsett, short sTilTegnsett, char *pszTx)
{
   if (sFraTegnsett != sTilTegnsett)
   {
      switch (sFraTegnsett)
      {
         // Fra DOS 8-bits
         case TS_DOSN8:
            switch (sTilTegnsett)
            {
               // Til norsk 7-bits
               case TS_ND7:
               case TS_DECN7:
                  UT_Ascii8ToAscii7(pszTx);

               // Til ISO8859
               case TS_ISO8859:
               case TS_DECM8:
                  UT_Ascii8ToISO8859(pszTx);
            }
            break;

         // Fra norsk 7-bits
         case TS_ND7:
         case TS_DECN7:
            switch (sTilTegnsett)
            {
               // Til DOS 8-bits
               case TS_DOSN8:
                  UT_Ascii7ToAscii8(pszTx);

               // Til ISO8859
               case TS_ISO8859:
               case TS_DECM8:
                  UT_Ascii7ToISO8859(pszTx);
            }
            break;

         // Fra ISO8859
         case TS_ISO8859:
         case TS_DECM8:
            switch (sTilTegnsett)
            {
               // Til DOS 8-bits
               case TS_DOSN8:
                  UT_ISO8859ToAscii8(pszTx);
               
               // Til norsk 7-bits
               case TS_ND7:
               case TS_DECN7:
                  UT_ISO8859ToAscii7(pszTx);
            }
            break;
      }
   }
}


/*
AR:2012-02-09
CH UT_KonverterTegnsett_8_16                              Konverter tegnsett
CD ==========================================================================
CD Formål:
CD Konverterer en streng fra et 7/8 bits tegnsett til 16 bits wchar_t (UNICODE).
CD
CD Parametre:
CD  Type     Navn         I/U  Forklaring
CD -----------------------------------------------------------------------------
CD  short    sFraTegnsett  i   Fra tegnsett:
CD                                  TS_DOSN8   = DOS norsk 8-bits
CD                                  TS_ND7     = Norsk Data 7-bits
CD                                  TS_DECM8   = DEC multinasjonal 8-bits 
CD                                               (Handteres likt med TS_ISO8859) 
CD                                  TS_DECN7   = DEC norsk 7-bits
CD                                               (Handteres likt med TS_ND7)
CD                                  TS_ISO8859 = ISO8859-1 og 10
CD                                  TS_UTF8    = UTF-8 
CD  char    *pszTx         i  Peker til streng som skal konverteres
CD                            (avsluttet med '\0').
CD  wchar_t *pwszResultat  iu Peker til streng som mottar resultatet av konverteringen
CD                            Må være minst like lang som pszTx.
CD
CD Bruk:
CD UT_KonverterTegnsett_8_16(sFraTegnsett,pszTx,pwszTx);
   =============================================================================
*/
SK_EntPnt_UT void UT_KonverterTegnsett_8_16 (short sFraTegnsett, char *pszTx, wchar_t *pwszResultat)
{
   // Konverter til rett tegnsett
   if (sFraTegnsett == TS_UTF8)
   {
      // Konverterer fra UTF-8 til 16 bits UNICODE
      char *cp = pszTx;
      wchar_t *tp = pwszResultat;
      int numBytes;

      while (*cp!='\0')
      {
         *tp = UT_Utf8ToWChar(cp, &numBytes); 
         ++tp;
         cp += numBytes;
      }

      *tp = L'\0';
   }
   else
   {
      // Konverterer først til ISO8859-10
      UT_KonverterTegnsett_8_8(sFraTegnsett, TS_ISO8859, pszTx);   
      // Deretter videre til 16 bits UNICODE
      UT_ISO8859ToWChar(pszTx,pwszResultat);
   }
}

/*
PG:2012-11-01
*/
SK_EntPnt_UT void UT_KonverterTegnsett_8_16_UTF8 (const std::string strTx, std::wstring &wstrResultat) // Spesielt for GML
{
   // Konverterer fra UTF-8 til 16 bits UNICODE
   const char *cp = strTx.c_str();
   wchar_t *tp = (wchar_t*)malloc((strTx.length())*sizeof(wchar_t)); // Kan bli allokert litt større enn nødvendig - hvis UTF-8 tegn
   wchar_t *start = tp;
   int numBytes;

   while (*cp!='\0')
   {
      *tp = UT_Utf8ToWChar(cp, &numBytes); 
      ++tp;
      cp += numBytes;
   }
   *tp = L'\0';
   tp = start;
   wstrResultat = tp;

   free(tp);
   tp=NULL;
}

/*
AR:2012-02-09
CH UT_KonverterTegnsett_16_8                              Konverter tegnsett
CD ==========================================================================
CD Formål:
CD Konverterer en streng fra 16 bits wchar_t (UNICODE) til et 7/8 bits tegnsett.
CD
CD Parametre:
CD  Type     Navn         I/U  Forklaring
CD -----------------------------------------------------------------------------
CD  short    sTilTegnsett  i   Til tegnsett:
CD                                  TS_DOSN8   = DOS norsk 8-bits
CD                                  TS_ND7     = Norsk Data 7-bits
CD                                  TS_DECM8   = DEC multinasjonal 8-bits 
CD                                               (Handteres likt med TS_ISO8859) 
CD                                  TS_DECN7   = DEC norsk 7-bits
CD                                               (Handteres likt med TS_ND7)
CD                                  TS_ISO8859 = ISO8859-1 og 10
CD                                  TS_UTF8    = UTF-8 
CD  wchar_t *pwszTx        i  Peker til streng som skal konverteres
CD                            (avsluttet med '\0').
CD  char    *pszResultat   iu Peker til streng som mottar resultatet av konverteringen
CD  size_t   maxBytes      i  Max lengde av pszResultat, inkludert null-terminator
CD
CD Bruk:
CD UT_KonverterTegnsett_16_8(sTilTegnsett,pwszTx,pszTx,maxBytes);
   =============================================================================
*/
SK_EntPnt_UT void UT_KonverterTegnsett_16_8 (short sTilTegnsett, const wchar_t *pwszTx, char *pszResultat, size_t maxBytes)
{
   // Konverter til rett tegnsett
   if (sTilTegnsett == TS_UTF8)
   {
      // Sjekk at resultatstrengen er stor nok
      if (UT_GetUtf8Len(pwszTx)+1 > maxBytes)
      {
         pszResultat[0] = '?';
         pszResultat[1] = '\0';
      }
      else
      {
         // Konverterer fra 16 bits UNICODE til UTF-8 
         const wchar_t *cp = pwszTx;
         char *tp = pszResultat;

         while (*cp!=L'\0')
         {
            tp = UT_WCharToUtf8(*cp,tp); 
            ++cp;
         }
         // Må legge til null-terminator
         *tp = L'\0';
      }
   }
   else
   {
      // Sjekk at resultatstrengen er stor nok
      if (wcslen(pwszTx)+1 > maxBytes)
      {
         pszResultat[0] = '?';
         pszResultat[1] = '\0';
      }
      else
      {
         // Konverterer først til ISO8859-10
         UT_WCharToISO8859(pwszTx,pszResultat);
         // Deretter videre til riktig 8-bits tegnsett
         UT_KonverterTegnsett_8_8(TS_ISO8859,sTilTegnsett,pszResultat);   
      }
   }
}


/*
AR:2012-02-09
CH UT_StrTilWstr                                             Konverter tegnsett
CD ==========================================================================
CD Formål:
CD Konverterer std::string (ISO8859-10) til std::wstring (16 bits UNICODE).
CD
CD Parametre:
CD Type          Navn       I/U  Forklaring
CD -----------------------------------------------------------------------------
CD std::string  &rstrFraTx   i   Streng som skal konverteres
CD std::wstring &rwstrTilTx  iu  Streng som mottar resultatet.
CD
CD Bruk:
CD StrTilWcs(rstrFraTx, rwstrResultat);
   =============================================================================
*/
void UT_StrTilWstr(const std::string &rstrFraTx, std::wstring &rwstrTilTx)
{
   wchar_t *pwsz = (wchar_t*) malloc((rstrFraTx.length()+1) * sizeof(wchar_t)); 

   UT_ISO8859ToWChar(rstrFraTx.c_str(),pwsz);
   rwstrTilTx = pwsz;

   free(pwsz);
}


/*
AR:2012-02-09
CH UT_WstrTilStr                                             Konverter tegnsett
CD ==========================================================================
CD Formål:
CD Konverterer std::string (ISO8859-10) til std::wstring (16 bits UNICODE).
CD
CD Parametre:
CD Type           Navn        I/U  Forklaring
CD -----------------------------------------------------------------------------
CD std::wstring  &rwstrFraTx   i   Streng som skal konverteres
CD std::string   &rstrTilTx   iu   Streng som mottar resultatet.
CD
CD Bruk:
CD UT_WstrTilStr(rstrFraTx, rwstrResultat);
   =============================================================================
*/
void UT_WstrTilStr(const std::wstring &rwstrFraTx, std::string &rstrTilTx)
{
   char *psz = (char*) malloc((rwstrFraTx.length()+1) * sizeof(char)); 

   UT_WCharToISO8859(rwstrFraTx.c_str(),psz);
   rstrTilTx = psz;

   free(psz);
}


/*
AR:2003-10-13
CH UT_StrCmpi                                            Sammenlign strenger
CD ==========================================================================
CD Formål:
CD Sammenligner to strenger uavhengig av store og små bokstaver.
CD For sammenligning der det bare sjekkes på likhet fungerer den for norske bokstaver.
CD (Sammenligner bare karakterverdier.)
CD For sortering må UT_StrColli brukes for å handtere norske bokstaver rett. 
CD UT_StrCmpi er mye raskere enn UT_StrColli.
CD
CD Parametre:
CD  Type    Navn         I/U   Forklaring
CD -----------------------------------------------------------------------------
CD  wchar_t   *pszTx1        i    Peker til første streng (avsluttet med '\0').
CD  wchar_t   *pszTx2        i    Peker til andre streng (avsluttet med '\0').
CD  int     iStatus       r    Status, samme som strcmpi
CD
CD Bruk:
CD iStatus = UT_StrCmpi(szTx1,szTx2);
   =============================================================================
*/
SK_EntPnt_UT int UT_StrCmpi(const wchar_t *pszTx1, const wchar_t *pszTx2)
{
   wchar_t *ps1,*ps2;
   int iStatus;


   ps1 = (wchar_t*)malloc((wcslen(pszTx1)+1) * sizeof(wchar_t));
#ifdef LINUX
   strncpy(ps1, pszTx1, strlen(pszTx1)+1);
#else
   UT_StrCopy(ps1, pszTx1, wcslen(pszTx1)+1);
#endif
   UT_StrUpper(ps1);

   ps2 = (wchar_t*)malloc((wcslen(pszTx2)+1) * sizeof(wchar_t));
#ifdef LINUX
   strncpy(ps2, pszTx2, strlen(pszTx2)+1);
#else
   UT_StrCopy(ps2, pszTx2, wcslen(pszTx2)+1);
#endif
   UT_StrUpper(ps2);

   iStatus = wcscmp(ps1,ps2);

   free(ps1);
   free(ps2);

   return iStatus;
}

/*
PG:2010-10-21
CH UT_StrColli                                            Sammenlign strenger
CD ==========================================================================
CD Formål:
CD Sammenligner to strenger uavhengig av store og små bokstaver.
CD Fungerer for norske bokstaver. Justerer for landskode (LC_COLLATE)
CD Hvis det er bare er likhet det sjekkes på, bør heller den raskere UT_StrCmpi brukes.
CD
CD Parametre:
CD  Type    Navn         I/U   Forklaring
CD -----------------------------------------------------------------------------
CD  wchar_t   *pszTx1        i    Peker til første streng (avsluttet med '\0').
CD  wchar_t   *pszTx2        i    Peker til andre streng (avsluttet med '\0').
CD  int     iStatus       r    Status, samme som strcmpi
CD
CD Bruk:
CD iStatus = UT_StrColli(szTx1,szTx2);
   =============================================================================
*/
SK_EntPnt_UT int UT_StrColli(const wchar_t *pszTx1, const wchar_t *pszTx2)
{
   wchar_t *ps1,*ps2;
   int iStatus;

   ps1 = (wchar_t*)malloc((wcslen(pszTx1)+1) * sizeof(wchar_t));
   UT_StrCopy(ps1, pszTx1, wcslen(pszTx1)+1);
   UT_StrUpper(ps1);

   ps2 = (wchar_t*)malloc((wcslen(pszTx2)+1) * sizeof(wchar_t));
   UT_StrCopy(ps2, pszTx2, wcslen(pszTx2)+1);
   UT_StrUpper(ps2);

   iStatus = wcscoll(ps1,ps2);

   free(ps1);
   free(ps2);

   return iStatus;
}




// ==========================================================================
// Formaterer heltall med tre og tre siffer i grupper
// sBufferLen - Max antall tegn i pszBuffer inkl. null-terminator.
// ==========================================================================
SK_EntPnt_UT void UT_FormaterInt64(wchar_t *pszBuffer, size_t sBufferLen, __int64 value)
{
   wchar_t szTx[128];
   short sAntGruppe;
   wchar_t *pszFra,*pszTil;

   // ----- Konverter til tekst
   _i64tow_s(value,szTx,128,10);
   size_t uformatertLengde = wcslen(szTx);

   // ----- Formater med tre og tre siffer i grupper
   short sAntMellomrom = ((short)uformatertLengde-1)/3;

   if ( (uformatertLengde + sAntMellomrom + 1) > sBufferLen)
   {        
      // Har plass til tallet uten formatering
      if ( uformatertLengde < sBufferLen)  wcscpy_s(pszBuffer, sBufferLen, szTx);
      // Ikke plass
      else                                 wcscpy_s(pszBuffer, sBufferLen, L"***");
   }

   else
   {
      pszFra = szTx + uformatertLengde;
      pszTil = pszBuffer + uformatertLengde + sAntMellomrom;
      sAntGruppe = 0;   // Antall tegn i aktuell siffergruppe
      *pszTil-- = *pszFra--;     // Null-terminatoren
      while (pszFra >= szTx)
      {
         if(sAntGruppe == 3)
         {
            // Legg inn mellomrom
            *pszTil-- = L' ';
            sAntGruppe = 0;
         }
         ++sAntGruppe;
         *pszTil-- = *pszFra--;
      }
   }
}