#include "stdafx.h"

#include "PalindromeCounterLibraryStatic.h"

#include <iterator>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::wstring> FindWords( const wchar_t* text )
{
	std::wistringstream stream{ text };
	return std::vector<std::wstring>{std::istream_iterator<std::wstring, wchar_t>{stream}, std::istream_iterator<std::wstring, wchar_t>{}};
}

bool IsPalindrome( const std::wstring& word )
{
	const int First = 0;
	const int Last = static_cast<int>( word.length() ) - 1;

	for ( int i = 0; i < word.length() / 2; ++i ) {
		if ( word[First + i] != word[Last - i] ) {
			return false;
		}
	}

	return true;
}

int PalindromeCounter( const wchar_t* text )
{
	int palindromeCount = 0;

	auto words = FindWords( text );
	for ( auto word : words ) {
		palindromeCount += IsPalindrome( word ) ? 1 : 0;
	}

	return palindromeCount;
}
