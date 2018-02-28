#pragma once

// Theory: default stack size in MVS projects is 1 Mb. Run one of these functions with F5,
// and show the call stack pushing Ctrl+Alt+C. Select the last successful call and check
// the value of corresponding (to the called function) variable. You need to divide 1 Mb
// to the number given.

int EmptyFunctionCallsCount = 0; // Will be nearly 4881, i. e. 215 b for call.
int NotEmptyFunctuonCallsCount = 0; // Will be neary 4621, i. e. 226 b for call.

void DoNothing()
{
	++EmptyFunctionCallsCount;
	DoNothing();
}

void DoNotingWithIntVariable()
{
	int unusedVariable = 0;
	++NotEmptyFunctuonCallsCount;
	DoNotingWithIntVariable();
}
