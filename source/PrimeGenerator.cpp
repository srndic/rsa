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
 * ****************************************************************************
 */

#include "PrimeGenerator.h"
#include <cstdlib>	//rand(), srand()
#include <ctime>	//time()

//A BigInt number with the value of RAND_MAX
static const BigInt RandMax(RAND_MAX);
//A BigInt number with the value of ULONG_MAX
static const BigInt ULongMax(ULONG_MAX);

/* Generates a random number with digitCount digits.
 * Returns it by reference in the "number" parameter. */
void PrimeGenerator::makeRandom(BigInt &number, unsigned long int digitCount)
{
	//make sure there is enough space
	if (number.length < digitCount + RandMax.digitCount + 10)
		number.expandTo(digitCount + RandMax.digitCount + 10);

	unsigned long int tempDigitCount(0);

	//generate random digits
	while (tempDigitCount <= digitCount)
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

/* Generates a random "number" such as 1 <= "number" < "top".
 * Returns it by reference in the "number" parameter. */
void PrimeGenerator::makeRandom(BigInt &number, const BigInt &top)
{
	//randomly select the number of digits for the random number
	unsigned long int newDigitCount = (rand() % top.digitCount) + 1;
	makeRandom(number, newDigitCount);
	//make sure the number is < top and not zero
	while (number >= top || number.EqualsZero())
		makeRandom(number, newDigitCount);
	//make sure the leading digit is not zero
	while (number.digits[number.digitCount - 1] == 0)
		number.digitCount--;
}

/* Creates an odd BigInt with the specified number of digits. 
* Returns it by reference in the "number" parameter. */
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

/* Tests the primality of the given _odd_ number using the 
 * Miller-Rabin probabilistic primality test. Returns true if 
 * the tested argument "number" is a probable prime with a 
 * probability of at least 1 - 4^(-k), otherwise false. */
bool PrimeGenerator::isProbablePrime(	const BigInt &number, 
										unsigned long int k)
{
	//first we need to calculate such a and b, that
	//number - 1 = 2^a * b, a and b are integers, b is odd
	BigInt numberMinusOne(number - 1);
	unsigned long int a(0);
	BigInt temp(numberMinusOne);
	BigInt b, quotient;
	static const BigInt two(BigIntOne + BigIntOne);

	while (b.EqualsZero())
	{
		//temp = quotient * 2 + remainder
		BigInt::divide(temp, two, quotient, b);
		temp = quotient;
		a++;
	}
	b = temp * two + b;
	a--;

	//test with k different possible witnesses to ensure that the probability
	//that "number" is prime is at least 1 - 4^(-k)
	for (unsigned long int i = 0; i < k; i++)
	{
		//reuse temp to generate a random number, 1 <= temp <= number - 1
		PrimeGenerator::makeRandom(temp, number);

		if (isWitness(temp, number, b, a, numberMinusOne))
			return false; //definitely a composite number
	}

	return true; //a probable prime
}

/* Returns true if "candidate" is a witness for the compositeness
 * of "number", false if "candidate" is a strong liar. "exponent" 
 * and "squareCount" are used for computation */
bool PrimeGenerator::isWitness(	BigInt candidate, 
								const BigInt &number, 
								const BigInt &exponent, 
								unsigned long int squareCount, 
								const BigInt &numberMinusOne)
{
	//calculate candidate = (candidate to the power of exponent) mod number
	candidate.SetPowerMod(exponent, number);
	//temporary variable, used to call the divide function
	BigInt quotient;

	for (unsigned long int i = 0; i < squareCount; i++)
	{
		bool maybeWitness(false);
		if (candidate != BigIntOne && candidate != numberMinusOne)
			maybeWitness = true;

		BigInt::divide(candidate * candidate, number, quotient, candidate);
		if (maybeWitness && candidate == BigIntOne)
			return true; //definitely a composite number
	}

	if (candidate != BigIntOne)
		return true; //definitely a composite number

	return false; //probable prime
}

/* Returns a probable prime number "digitCount" digits long, 
 * with a probability of at least 1 - 4^(-k) that it is prime. */
BigInt PrimeGenerator::Generate(unsigned long int digitCount, 
								unsigned long int k)
{
	BigInt primeCandidate;
	PrimeGenerator::makePrimeCandidate(primeCandidate, digitCount);
	while (!isProbablePrime(primeCandidate, k))
	{
		//select the next odd number and try again
		primeCandidate++;
		primeCandidate++;
		if (primeCandidate.digitCount != digitCount)
		PrimeGenerator::makePrimeCandidate(primeCandidate, digitCount);
	}
	return primeCandidate;
}
