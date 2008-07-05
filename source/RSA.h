/* ****************************************************************************
 * 
 * 				RSA.h
 * 
 * Author: Nedim Srndic
 * Release date: 16th of June 2008
 * 
 * An implementation of the RSA public-key cryptography algorithm. 
 * 
 * RSA supports: 
 * 
 * 	- Message encryption 
 * 	- Message decryption 
 * 	- Public/private key-pair generation 
 * 
 * NOTE: it is highly recommended to call 
 * 		std::srand(time(NULL));
 * 	once when the program starts and before any use of methods provided by the 
 * 	RSA class. Calling the srand() function randomizes the standard C++ 
 * 	pseudorandom number generator, so that it provides different series of 
 * 	pseudorandom numbers every time the program is run. This greatly improves 
 * 	security. 
 * 
 * ****************************************************************************
 */

#ifndef RSA_H_
#define RSA_H_

#include <string>
#include "KeyPair.h"
#include "BigInt.h"

class RSA
{
	private:
		/* Returns the greatest common divisor of the two arguments 
		 * "a" and "b", using the Euclidean algorithm. */
		static BigInt GCD(const BigInt &a, const BigInt &b);
		/* Solves the equation 
		 * 			d = ax + by 
		 * given a and b, and returns d, x and y by reference. 
		 * It uses the Extended Euclidean Algorithm */
		static void extendedEuclideanAlgorithm(	const BigInt &a, 
												const BigInt &b, 
												BigInt &d, 
												BigInt &x, 
												BigInt &y);
		/* Solves the equation 
		 * 			ax is congruent to b (mod n), 
		 * given a, b and n finds x. */
		static BigInt solveModularLinearEquation(	const BigInt &a, 
													const BigInt &b, 
													const BigInt &n);
	public:
		/* Encrypts "message" using "key". */
		static const std::string &Encrypt(	const std::string message, 
											const Key &key);
		/* Decrypts "cyphertext" using "key". */
		static const std::string &Decrypt(	const std::string cyphertext, 
											const Key &key);
		/* Generates a public/private key-pair. The keys are retured in a 
		 * KeyPair. The generated keys are 
		 * 2 * "digitCount" or 2 * "digitCount - 1 digits long, 
		 * and have the probability of at least 1 - 4^(-k) of being prime. 
		 * For k = 3, that probability is 98.4375%, 
		 * and for k = 4 it is 99.609375%. 
		 * 
		 * k = 3 is recommended by Introduction to Algorithms, Second Edition;
		 * by Thomas H. Cormen, Charles E. Leiserson, Ronald L. Rivest and 
		 * Clifford Stein for prime number generation. 
		 * */
		static KeyPair GenerateKeyPair(	unsigned long int digitCount, 
										unsigned long int k = 3);
};

#endif /*RSA_H_*/
