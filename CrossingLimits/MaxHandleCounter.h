#pragma once

#include <Windows.h>

void PrintMaxHandlesCount()
{
	// See: https://msdn.microsoft.com/ru-ru/library/windows/desktop/ms724251(v=vs.85).aspx.
	const HANDLE event = CreateEvent( NULL, TRUE, TRUE, NULL );
	
	int handlesCount = 1;
	HANDLE duplicate;

	// See: https://msdn.microsoft.com/ru-ru/library/windows/desktop/ms724251(v=vs.85).aspx.
	while ( DuplicateHandle( GetCurrentProcess(), event, GetCurrentProcess(), &duplicate, 0, FALSE, DUPLICATE_SAME_ACCESS ) == TRUE ) {
		++handlesCount;
	}

	std::cout << handlesCount << " handles were created, it is about 2 ** " << log2( handlesCount ) << "." <<  std::endl;
}
