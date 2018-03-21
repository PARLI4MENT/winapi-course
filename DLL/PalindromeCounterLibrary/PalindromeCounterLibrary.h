#pragma once

#ifdef PALINDROMECOUNTERLIBRARYAPI_EXPORTS
#define PALINDROMECOUNTERLIBRARYAPI extern "C"  __declspec(dllexport)
#else
#define PALINDROMECOUNTERLIBRARYAPI extern "C" __declspec(dllimport)
#endif

PALINDROMECOUNTERLIBRARYAPI int PalindromeCounter( const wchar_t* text );
