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
		 * reference, in the respective arguments. */
		void GenerateKeyPair(Key &privateKey, Key &publicKey);
};

#endif /*RSA_H_*/
