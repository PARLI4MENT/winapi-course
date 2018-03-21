#pragma once

#ifndef PALINDROMELIBRARYAPI
#define PALINDROMELIBRARYAPI extern "C" __declspec(dllimport)
#endif

PALINDROMELIBRARYAPI int PalindromeCounter( const wchar_t* text );
