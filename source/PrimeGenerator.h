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
	private: 
		/* Creates an odd BigInt with the specified number of digits*/
		static BigInt createPrimeCandidate(unsigned long int digitCount);
		/* Creates an odd BigInt with a value less than top*/
		static BigInt createPrimeCandidate(BigInt top);
		/* Tests the primality of the given number using the 
		 * Miller-Rabin primality test*/
		static bool isProbablePrime(const BigInt &number);
		/* Returns true if a is congruent to b with the modulus n, otherwise 
		 * returns false*/
		static bool congruenceEquation(	const BigInt &a, 
										const BigInt &b, const BigInt &n);
	public:
		/*Returns a probable prime number "digitCount" digits long*/
		static BigInt Generate(	unsigned long int digitCount);
};

#endif /*PRIMEGENERATOR_H_*/
