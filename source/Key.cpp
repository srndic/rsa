/* ****************************************************************************
 * 
 * 				Key.cpp
 * 
 * Author: Nedim Srndic
 * Release date: 5th of September 2008
 * 
 * This file contains the implementation for the Key class. 
 * 
 * ****************************************************************************
 */

#include "Key.h"

std::ostream &operator<<(std::ostream &cout, const Key &key)
{
	return std::cout 
	<< "Modulus: " << key.GetModulus() << std::endl 
	<< "Exponent: " << key.GetExponent();
}
