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

/* Generates a random number with digitCount digits.
 * Returns it in the number parameter. */
void PrimeGenerator::makeRandom(BigInt &number, 
								unsigned long int digitCount)
{
	//make sure there is enough space
	if (number.length < digitCount + RandMax.digitCount + 10)
		number.expandTo(digitCount + RandMax.digitCount + 10);
	
	unsigned long int tempDigitCount(0);
	
	//generate random digits
	while(tempDigitCount <= digitCount)
	{
		unsigned long int newRand(std::rand());
		
		//10 is chosen to skip the first digit, because it might be 
		//statistically <= n, where n is the first digit of RandMax
		while (newRand >= 10)
		{
			number.digits[tempDigitCount] = newRand % 10;
			newRand /= 10;
			tempDigitCount++;
		}
	}
	
	//adjust the digitCount property of a to the required number of digits
	number.digitCount = digitCount;
}

/* Generates a random number with a value less than top.
 * Returns it in the number parameter. */
void PrimeGenerator::makeRandom(BigInt &number, 
								const BigInt &top)
{
	//randomly select the number of digits for the random number
	unsigned long int newDigitCount = (rand() % top.digitCount) + 1;
	makeRandom(number, newDigitCount);
	//make sure the number is <= top
	while (number >= top)
		makeRandom(number, newDigitCount);
}

/* Creates an odd BigInt with the specified number of digits. 
 * Returns it in the number parameter. */
void PrimeGenerator::makePrimeCandidate(BigInt &number, 
										unsigned long int digitCount)
{
	PrimeGenerator::makeRandom(number, digitCount);
	//make the number odd
	if (!(number.digits[0] & 1))
		number.digits[0]++;
	//make sure the leading digit is not a zero
	while (number.digits[number.digitCount - 1] == 0)
		number.digits[number.digitCount - 1] = std::rand() % 10;
}

/*Tests the primality of the given _odd_ number using the 
 * Miller-Rabin primality test*/
bool PrimeGenerator::isProbablePrime(const BigInt &number)
{
	//number - 1 = 2^a * b, a and b are integers, b is odd
	BigInt numberMinusOne(number - 1);
	//a ~~ log2(numberMinusOne) == numberMinusOne / log10(2)
	unsigned long int a(
			static_cast<unsigned long int>((numberMinusOne.digitCount - 1) /
					0.301029995663981195213738894724));
		
	//temp = 2^a
	BigInt temp((BigIntOne + BigIntOne).GetPower(a));
	while ((temp = temp * 2) < numberMinusOne)
		a++;
	
	BigInt b(numberMinusOne / (temp / 2));
	
	//reuse temp to generate a random number, temp < number - 1
	PrimeGenerator::makeRandom(temp, numberMinusOne);
	temp.SetPower(BigInt::toInt(b.digits, b.digitCount));
	
	if (congruenceEquation(temp, BigIntOne, number) || 
		congruenceEquation(temp, numberMinusOne, number))
		return true;
	
	for (unsigned long int i(0); i < a; i++)
	{
		temp.SetPower(2);
		temp = temp % number;
		if (congruenceEquation(temp, numberMinusOne, number))
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
	BigInt primeCandidate;
	PrimeGenerator::makePrimeCandidate(primeCandidate, digitCount);
	while (!isProbablePrime(primeCandidate))
	{
		primeCandidate++;
		primeCandidate++;
		if (primeCandidate.digitCount != digitCount)
			PrimeGenerator::makePrimeCandidate(primeCandidate, digitCount);
	}
	return primeCandidate;
}
