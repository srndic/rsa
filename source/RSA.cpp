/* ****************************************************************************
 *
 * 				RSA.cpp
 * 
 * Author: Nedim Srndic
 * Release date: 16th of June 2008
 * 
 * This file contains the implementation for the RSA class.
 * 
 * TODO: finish this. Implement the extended Euclid algorithm
 * 
 * ****************************************************************************
 */

#include <cstdlib>	//srand(), time()
#include "RSA.h"
#include "PrimeGenerator.h"	//Generate()

/* Returns the greatest common divisor of the two arguments 
 * "a" and "b", using the Euclidean algorithm. */
BigInt RSA::GCD(const BigInt &a, const BigInt &b)
{
	if (b.EqualsZero())
		return a;
	else
		return RSA::GCD(b, a % b);
}

/* Solves the equation 
 * 			d = ax + by 
 * given a and b, and returns d, x and y by reference. 
 * It uses the Extended Euclidean Algorithm */
void RSA::extendedEuclideanAlgorithm(	const BigInt &a, const BigInt &b, 
										BigInt &d, BigInt &x, BigInt &y)
{
	if (b.EqualsZero())
	{
		d = a;
		x = BigIntOne;
		y = BigIntZero;
		return;
	}
	RSA::extendedEuclideanAlgorithm(a, b, d, x, y);
	BigInt temp(x);
	x = y;
	y = temp - a / b * y;
}

/* Generates a public/private key-pair. The keys are retured by 
 * reference, in the respective arguments. The generated keys are 
 * 2 * "digitCount" or 2 * "digitCount - 1 digits long, 
 * and have the probability of at least 1 - 4^(-k) of being prime. 
 * For k = 3, that probability is 98.4375%, 
 * and for k = 4 it is 99.609375%. 
 * 
 * k = 3 is recommended by Introduction to Algorithms, Second Edition;
 * by Thomas H. Cormen, Charles E. Leiserson, Ronald L. Rivest and 
 * Clifford Stein for prime number generation. 
 * */
void RSA::GenerateKeyPair(	Key &privateKey, 
							Key &publicKey, 
							unsigned long int digitCount, 
							unsigned long int k)
{
	//randomize the random number generator
	std::srand(time(NULL));
	
	//generate two random numbers p and q, each "digitCount" digits long
	BigInt p(PrimeGenerator::Generate(digitCount, k));
	BigInt q(PrimeGenerator::Generate(digitCount, k));
	
	//make sure they are different
	while (p == q)
	{
		p = PrimeGenerator::Generate(digitCount, k);
	}
	
	//calculate the modulus of both the public and private keys, n
	BigInt n(p * q);
	privateKey.modulus = n;
	publicKey.modulus = n;
	
	//calculate the totient phi
	BigInt phi((p - 1) * (q - 1));
	
	//we don't want the keys to be less than 20 bits long
	if (phi < "1048576")
		throw "Insufficient key strength!";
	
	//select a small odd integer e that is coprime with phi and e < phi
	//usually 65537 is used, and we will use it too if it fits
	//it is recommended that this be the least possible value for e
	BigInt e("65537");
	
	//make sure the requirements are met
	while (RSA::GCD(phi, e) != BigIntOne || e < "65537")
	{
		PrimeGenerator::makeRandom(e, 5);
	}
	
	
}
