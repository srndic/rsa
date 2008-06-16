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
 * ****************************************************************************
 */

#ifndef RSA_H_
#define RSA_H_

#include <string>
#include "Key.h"

class RSA
{
	private:
		
	public:
		/* Encrypts "message" using "key". */
		const std::string &Encrypt(	const std::string message, 
									const Key &key);
		/* Decrypts "cyphertext" using "key". */
		const std::string &Decrypt(	const std::string cyphertext, 
									const Key &key);
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
		void GenerateKeyPair(	Key &privateKey, 
								Key &publicKey, 
								unsigned long int digitCount, 
								unsigned long int k = 3);
};

#endif /*RSA_H_*/
