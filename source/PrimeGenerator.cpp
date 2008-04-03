/* ****************************************************************************
 *
 * 				PrimeGenerator.cpp
 * 
 * Author: Nedim Srndic
 * Release date: 14th of March 2008
 * 
 * This file contains the implementation for the PrimeGenerator class.
 * 
 * There is a static constant defined in this file: 
 * 
 * - RandMax		: RAND_MAX (defined in cstdlib) of type BigInt
 * 		Mainly used for speedup in the Generate member function. 
 * 		Represents the largest random unsigned long integer that a particular 
 * 		platform can generate. This is platform-specific. 
 * 	 
 * 	TODO:	comment some uncommented code
 * 
 * ****************************************************************************
 */

#include "PrimeGenerator.h"
#include <cstdlib>	//rand()

//A BigInt number with the value of ULONG_MAX
static const BigInt RandMax(RAND_MAX);

/*Generates a prime number "minDigits" digits long, 
* with the given seed for pseudo-random number generator*/
BigInt PrimeGenerator::Generate(unsigned long int minDigits, unsigned int seed)
{
	BigInt a;
	a.expandTo(minDigits + RandMax.digitCount + 10);
	
	if (seed)
		std::srand(seed);
	else
		std::srand(time(NULL));
		
	unsigned long int digitCount(0);
	
	while(digitCount <= minDigits)
	{
		unsigned long int newRand(std::rand());
		
		//10 is chosen to skip the first digit, because it might be 
		//statistically <= n, where n is the first digit of RandMax
		while (newRand >= 10)
		{
			a.digits[digitCount] = newRand % 10;
			newRand /= 10;
			digitCount++;
		}
	}
	
	a.digitCount = digitCount;
	return a;
}
