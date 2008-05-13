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
 * TODO: comment some uncommented code
 * 
 * ****************************************************************************
 */

#include "PrimeGenerator.h"
#include <cstdlib>	//rand(), srand()

//A BigInt number with the value of ULONG_MAX
static const BigInt RandMax(RAND_MAX);

/*Creates an odd BigInt with the specified number of digits*/
BigInt PrimeGenerator::createPrimeCandidate(unsigned long int digitCount)
{
	BigInt a;
	a.expandTo(digitCount + RandMax.digitCount + 10);
	
	unsigned long int tempDigitCount(0);
	
	while(tempDigitCount <= digitCount)
	{
		unsigned long int newRand(std::rand());
		
		//10 is chosen to skip the first digit, because it might be 
		//statistically <= n, where n is the first digit of RandMax
		while (newRand >= 10)
		{
			a.digits[tempDigitCount] = newRand % 10;
			newRand /= 10;
			tempDigitCount++;
		}
	}
	
	//adjust the digitCount property of a to the required number of digits
	a.digitCount = digitCount;
	//make the number odd
	if (!(a.digits[0] & 1))
		a.digits[0]++;
	//make sure the leading digit is not a zero
	while (a.digits[a.digitCount - 1] == 0)
		a.digits[a.digitCount - 1] = std::rand() % 10;
	
	return a;
}

/*Creates an odd BigInt with a value less than "top"*/
BigInt PrimeGenerator::createPrimeCandidate(BigInt top)
{
	unsigned long int newDigitCount = (rand() % top.digitCount) + 1;
	BigInt primeCandidate = createPrimeCandidate(newDigitCount);
	while (primeCandidate >= top)
		primeCandidate = createPrimeCandidate(newDigitCount);
	
	return primeCandidate;
}

/*Tests the primality of the given _odd_ number using the 
 * Miller-Rabin primality test*/
bool PrimeGenerator::isProbablePrime(const BigInt &number)
{
	//number - 1 = 2^a * b, a and b are integers, b is odd
	BigInt numberMinusOne(number - 1);
	unsigned long int a(
			static_cast<unsigned long int>(numberMinusOne.digitCount /
			log(static_cast<double>(numberMinusOne.digitCount))));
		
	//temp = 2^a
	BigInt temp((BigIntOne + BigIntOne).GetPower(a));
	while ((temp = temp * 2) < numberMinusOne)
		a++;
	
	BigInt b(numberMinusOne / (temp / 2));
	
	//generate a prime number c, c < number - 1
	BigInt c(PrimeGenerator::createPrimeCandidate(number - 1));
	c.SetPower(BigInt::toInt(b.digits, b.digitCount));
	
	if (congruenceEquation(c, BigIntOne, number) || 
		congruenceEquation(c, numberMinusOne, number))
		return true;
	
	for (unsigned long int i(0); i < a; i++)
	{
		c.SetPower(2);
		c = c % number;
		if (congruenceEquation(c, numberMinusOne, number))
			return true;
	}
	
	return false;
}

/* Returns true if a is congruent to b with the modulus n, otherwise 
 * returns false*/
bool PrimeGenerator::congruenceEquation(const BigInt &a, 
										const BigInt &b, const BigInt &n)
{
	return a % n == b % n;
}

/*Returns a probable prime number "digitCount" digits long*/
BigInt PrimeGenerator::Generate(unsigned long int digitCount)
{
	BigInt primeCandidate = PrimeGenerator::createPrimeCandidate(digitCount);
	while (!isProbablePrime(primeCandidate))
	{
		primeCandidate++;
		primeCandidate++;
		if (primeCandidate.digitCount != digitCount)
			primeCandidate = PrimeGenerator::createPrimeCandidate(digitCount);
	}
	return primeCandidate;
}
