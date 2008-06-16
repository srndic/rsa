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
		/* Generates a random number with digitCount digits.
		 * Returns it in the number parameter. */
		static void makeRandom(	BigInt &number, 
								unsigned long int digitCount);
		/* Generates a random number with a value less than top.
		 * Returns it in the number parameter. */
		static void makeRandom(	BigInt &number, 
								const BigInt &top);
		/* Creates an odd BigInt with the specified number of digits. 
		 * Returns it in the number parameter. */
		static void makePrimeCandidate(	BigInt &number, 
										unsigned long int digitCount);
		/* Tests the primality of the given number using the 
		 * Miller-Rabin primality test*/
		static bool isProbablePrime(const BigInt &number);
		/* Returns true if "candidate" is a witness for the compositeness
		 * of "number", false if "candidate" is a strong liar. "exponent" 
		 * and "squareCount" are used for computation */
		static bool isWitness(	BigInt candidate, 
								const BigInt &number, 
								const BigInt &exponent, 
								unsigned long int squareCount, 
								const BigInt &numberMinusOne);
	public:
		/*Returns a probable prime number "digitCount" digits long*/
		static BigInt Generate(unsigned long int digitCount);
};

#endif /*PRIMEGENERATOR_H_*/
