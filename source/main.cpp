/* ****************************************************************************
 * 				main.cpp
 * 
 * A sample program used to demonstrate the functionality of the library. 
 * 
 * Author: Nedim Srndic
 * Release date: 14th of March 2008
 * 
 * TODO: add code to demonstrate basic functionality 
 * of BigInt and PrimeGenerator. 
 */

#include <cstdlib>
#include <iostream>
#include "BigInt.h"
#include "PrimeGenerator.h"
#include "test.h"

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
	try
	{
		/*CONSTRUCTOR, DESTRUCTOR, ASSIGNMENT AND STREAM OUTPUT OPERATOR TEST*/
		TestBigIntFunctions();
		/*				ARITHMETIC AND COMPARISON OPERATOR TEST	*/
		TestBigIntOperators();
		/*				FACTORIAL COMPUTATION BENCHMARK			*/
		FactorialBenchmark();
		/*				climits INFORMATION						*/
//		LimitsInfo();
		/*				RANDOM TEST GENERATOR					*/
		RandomBigIntTest(100);
		/*				RANDOM DIVISION TEST GENERATOR			*/
		RandomBigIntDivisionTest(550);
/*				PRIME GENERATOR TEST					*/
		TestPrimeGenerator(100);

		pauseScreen();
	}
	catch (const char errorMessage[])
	{
		cout << errorMessage;
		pauseScreen();
		return EXIT_FAILURE;
	}
	catch (...)
	{
		cout << "Unknown error." << endl;
		pauseScreen();
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}
