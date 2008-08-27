/* ****************************************************************************
 * 				main.cpp
 * 
 * A sample program used to demonstrate the functionality of the library. 
 * 
 * Author: Nedim Srndic
 * Release date: 14th of March 2008
 * 
 * ****************************************************************************
 */

#include <cstdlib>	//srand()
#include <iostream>	//cout
#include <ctime>	//time()
#include <string>
#include "test.h"	//testing functions
#include "RSA.h"

using std::cout;
using std::endl;
using std::cin;

int main(int argc, char *argv[])
{
	//randomize the random number generator
	std::srand(time(NULL));
	
	try
	{
		/*CONSTRUCTOR, DESTRUCTOR, ASSIGNMENT AND STREAM OUTPUT OPERATOR TEST*/
		TestBigIntFunctions();
//		/*				ARITHMETIC AND COMPARISON OPERATOR TEST	*/
		TestBigIntOperators();
//		/*				FACTORIAL COMPUTATION BENCHMARK			*/
		FactorialBenchmark();
//		/*				RANDOM TEST GENERATOR					*/
		RandomBigIntTest(100);
//		/*				RANDOM DIVISION TEST GENERATOR			*/
		RandomBigIntDivisionTest(100);
//		/*				PRIME GENERATOR TEST					*/
		TestPrimeGenerator(5);
//		/*				KEY GENERATOR TEST						*/
		TestKeyGeneration(1);
		/*				ENCRYPTION/DECRYPTION TEST				*/
		TestEncryptionDecryption(1);
		/*				FILE ENCRYPTION/DECRYPTION TEST			*/
		TestFileEncryptionDecryption(1);
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
