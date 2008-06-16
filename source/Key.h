#ifndef KEY_H_
#define KEY_H_

#include "BigInt.h"

class Key
{
private:
	const BigInt modulus;
	const BigInt exponent;
public:
	Key(const BigInt modulus, const BigInt exponent) :
		modulus(modulus), exponent(exponent)
	{

	}
	const BigInt &GetModulus() const
	{
		return modulus;
	}
	const BigInt &GetExponent() const
	{
		return exponent;
	}
};

#endif /*KEY_H_*/
