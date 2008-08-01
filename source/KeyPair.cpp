/* ****************************************************************************
 * 
 * 				KeyPair.cpp
 * 
 * Author: Nedim Srndic
 * Release date: 22th of July 2008
 * 
 * This file contains the implementation for the KeyPair class. 
 * 
 * ****************************************************************************
 */

#include "KeyPair.h"

std::ostream &operator <<(std::ostream &cout, const KeyPair &k)
{
	return std::cout 
	<< "Private key:\n\tModulus: " << k.GetPrivateKey().GetModulus() 
	<< "\n\tExponent: " << k.GetPrivateKey().GetExponent() 
	<< "\n\nPublic key:\n\tModulus: " << k.GetPublicKey().GetModulus() 
	<< "\n\tExponent: " << k.GetPublicKey().GetExponent();
}
