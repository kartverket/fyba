/* ------------------------------
 * Fil: StrtPros.c
 * ------------------------------ */

#include "stdafx.h"

#ifdef UNIX
#  ifndef _INCLUDE_POSIX_SOURCE
#    define _INCLUDE_POSIX_SOURCE
#  endif
#  ifndef _HPUX_SOURCE
#    define _HPUX_SOURCE
#  endif
#endif

#ifdef OS2
#endif

#ifdef WIN32
#  include<windows.h>
#endif

#include "fyut.h"


/*
AR-961118
CH  UT_StartProsess      
CD  ==============================================================
CD  Formål:
CD  Starter en ny prosess.
CD
CD  PARAMETERLISTE:
CD  Type     Navn           I/U Merknad
CD  ------------------------------------------------------------- 
CD  wchar_t *pszCommandLine  i  Kommandolinje
CD  short    sVent           i  UT_VENT = Vent til prosessen avsluttes
CD                              UT_FORTSETT = Ikke vent
CD  int      nCmdShow        i  Styrer visning av vinduet. SW_SHOWNORMAL er default.
CD                              (Tilsvarer ShowWindow)
CD                              Bruk SW_SHOWMINIMIZED eller SW_SHOWMINNOACTIVE for minimert kjøring.

CD  short    sStatus         r  Status:
CD                              UT_TRUE = OK
CD                              UT_FALSE = Feil.
CD
CD  Bruk:
CD  sStatus = UT_StartProsess(szKommandolinje,UT_VENT);
CD
CD  ==============================================================
*/
SK_EntPnt_UT short UT_StartProsess(const wchar_t *pszCommandLine, short sVent, int nCmdShow,
                                   HANDLE hStdInput, HANDLE hStdOutput, HANDLE hStdError)
{

#ifdef WIN32
   PROCESS_INFORMATION ProcessInfo;
   STARTUPINFO StartupInfo;
   DWORD ExitCode = 0;
   BOOL bInheritHandles = FALSE;
   //wchar_t szCurrentDir[_MAX_PATH];
   //wchar_t szShortPath[100];


   // Setter opp oppstartinformasjon
   StartupInfo.cb = sizeof(STARTUPINFO); 
   StartupInfo.lpDesktop = NULL; 
   StartupInfo.lpTitle = NULL; 
   StartupInfo.cbReserved2 = 0;
   StartupInfo.lpReserved = NULL; 
   StartupInfo.lpReserved2 = NULL;
   StartupInfo.dwFlags = 0;

   if ( hStdInput != NULL  ||  hStdOutput != NULL  ||  hStdError != NULL)
   {
      bInheritHandles = TRUE;
      StartupInfo.dwFlags |= STARTF_USESTDHANDLES;

      StartupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE); 
      StartupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE); 
      StartupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE); 

      if ( hStdInput != NULL )   StartupInfo.hStdInput  = hStdInput;
      if ( hStdOutput != NULL )  StartupInfo.hStdOutput = hStdOutput;
      if ( hStdError != NULL )   StartupInfo.hStdError  = hStdError;
   }

   //   STARTF_USESTDHANDLES	If this value is specified, sets the standard input of the process, standard output,
   //                        and standard error handles to the handles specified in the hStdInput, hStdOutput, 
   //                        and hStdError members of the STARTUPINFO structure. The CreateProcess function's fInheritHandles
   //                        parameter must be set to TRUE for this to work properly.
	//                        If this value is not specified, the hStdInput, hStdOutput, and hStdError members of the STARTUPINFO
   //                        structure are ignored.

   if (nCmdShow != SW_SHOWNORMAL)
   { 
      StartupInfo.dwFlags |= STARTF_USESHOWWINDOW;  // The wShowWindow member contains additional information. 
      StartupInfo.wShowWindow = nCmdShow;
   }

   // wShowWindow
   // If dwFlags specifies STARTF_USESHOWWINDOW, this member can be any of the values that can be specified 
   // in the nCmdShow parameter for the ShowWindow function, except for SW_SHOWDEFAULT. Otherwise, this member is ignored. 
   // For GUI processes, the first time ShowWindow is called, its nCmdShow parameter is ignored wShowWindow specifies the default value. 
   // In subsequent calls to ShowWindow, the wShowWindow member is used if the nCmdShow parameter of ShowWindow is set to SW_SHOWDEFAULT. 

   //StartupInfo.dwX; 
   //StartupInfo.dwY; 
   //StartupInfo.dwXSize; 
   //StartupInfo.dwYSize; 
   //StartupInfo.dwXCountChars; 
   //StartupInfo.dwYCountChars; 
   //StartupInfo.dwFillAttribute; 

   //GetCurrentDirectory(_MAX_PATH,szCurrentDir);
   //GetShortPathName(szCurrentDir,szShortPath,100);

   // Utfører system-kommando  
   if ( ! CreateProcessW(NULL, (LPTSTR)pszCommandLine, NULL, NULL, bInheritHandles,
                        NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE, 
                        NULL, NULL, &StartupInfo, &ProcessInfo))
   {
      return UT_FALSE;
   }   

   // Vent til prosessen er ferdig
   if (sVent == UT_VENT) {
      WaitForSingleObject( ProcessInfo.hProcess, INFINITE);

      //Retrieves the termination status of the specified process.
      GetExitCodeProcess(ProcessInfo.hProcess, &ExitCode);
   }

   CloseHandle(ProcessInfo.hProcess);
   CloseHandle(ProcessInfo.hThread);
   
   return (ExitCode == 0)? UT_TRUE:UT_FALSE;

#else

   return UT_FALSE;
#endif
}
