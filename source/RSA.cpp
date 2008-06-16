/* ****************************************************************************
 *
 * 				RSA.cpp
 * 
 * Author: Nedim Srndic
 * Release date: 16th of June 2008
 * 
 * This file contains the implementation for the RSA class.
 * 
 * ****************************************************************************
 */

#include <cstdlib>	//srand(), time()
#include "RSA.h"
#include "PrimeGenerator.h"	//Generate()

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
	
	//select a small odd integer e that is coprime with phi
	//usually 65537 is used, and we will use it too if it is coprime
	BigInt e("65537");
	
//	while (e >= "65537" && )
}
