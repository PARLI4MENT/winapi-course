// CrossingLimits.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include "MaxMemoryCounter.h"
#include "MemoryFragmenter.h"
#include "MaxHandleCounter.h"
#include "MaxPensCounter.h"
#include "FunctionCallMemoryCounter.h"

int main()
{
	//PrintMaxMemoryAndLastError();
	//FragmentMemory();
	//PrintMaxHandlesCount();
	//PrintMaxPensCount();
	//DoNothing();
	//DoNotingWithIntVariable();

	// This piece of crap is used to keep process running.
	int enterSomethingToCloseProcess = 0;
	std::cin >> enterSomethingToCloseProcess;

	return 0;
}
