/* ****************************************************************************
 * 				PrimeGenerator.h
 * 
 * A class used to generate large prime numbers. 
 * 
 * Author: Nedim Srndic
 * Release date: 14th of March 2008
 * 
 * TODO: add basic functionality.
 * 
 * ****************************************************************************
 */

#ifndef PRIMEGENERATOR_H_
#define PRIMEGENERATOR_H_

#include "BigInt.h"

class PrimeGenerator
{
	public:
		/*Generates a prime number "minDigits" digits long, 
		 * with the given seed for pseudo-random number generator*/
		static BigInt Generate(	unsigned long int minDigits, 
								unsigned int seed = 0);
};

#endif /*PRIMEGENERATOR_H_*/
