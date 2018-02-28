#pragma once

#include <Windows.h>

#include "ErrorLogger.h"

// All GDI Objets: https://msdn.microsoft.com/ru-ru/library/windows/desktop/ms724291%28v=vs.85%29.aspx. 
void PrintMaxPensCount()
{
	int pensCount = 0;

	// See: https://msdn.microsoft.com/ru-ru/library/windows/desktop/dd183509(v=vs.85).aspx (CreatePen),
	// https://msdn.microsoft.com/ru-ru/library/windows/desktop/dd162937(v=vs.85).aspx (RGB).
	while ( CreatePen( PS_DASH, 1, RGB( 127, 127, 127 ) ) != NULL ) {
		++pensCount;
	}

	std::cout << pensCount << " pens were created." << std::endl;

	Log();
}