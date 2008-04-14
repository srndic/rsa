/* ****************************************************************************
 *
 * 				BigInt.cpp
 * 
 * Author: Nedim Srndic
 * Release date: 14th of March 2008
 * 
 * This file contains the implementation for the BigInt class.
 * 
 * There are a few static constants defined in this file: 
 * 
 * - ULongMax		: ULONG_MAX (defined in climits) of type BigInt
 * 		Mainly used for speedup in the multiply() private member function. 
 * 		Represents the largest unsigned long integer a particular platform can
 * 		handle. If a BigInt is <= ULongMax, it can be converted to unsigned
 * 		long int. This is platform-specific. 
 * 	- SqrtUlongMax	: sqrt(ULONG_MAX) of type BigInt
 * 		Mainly used for speedup in the multiply() private member function. 
 * 		Represents the square root of the largest unsigned long integer a 
 * 		particular platform can	handle. If two BigInts are <= SqrtULongMax, 
 * 		they can be converted to unsigned long int and safely multiplied 
 * 		by the CPU. This is platform-specific. 
 * 
 * 	TODO:	comment some uncommented code
 * 
 * ****************************************************************************
 */

#include "BigInt.h"
#include <cstring>	//strlen()
#include <climits>	//ULONG_MAX
#include <algorithm>    //reverse_copy(), copy(), copy_backward(), 
						//fill(), fill_n()

using std::cout;
using std::endl;

//define and initialize BigInt::factor
const double BigInt::factor = 1.6;

//A BigInt number with the value of ULONG_MAX
static const BigInt ULongMax(ULONG_MAX);
//A BigInt number with the value of sqrt(ULONG_MAX)
static const BigInt SqrtULongMax
				(static_cast<unsigned long int>(sqrt(ULONG_MAX)));

//transforms the number from unsigned long int to unsigned char[]
//and pads the result with zeroes
unsigned long int BigInt::intToUChar(	unsigned long int number, 
										unsigned char *digits, 
										unsigned long int padding = 0)
{
	int i(0);
	do
	{
		//the number is stored in reverse
		//(i.e. long int 456 is stored as unsigned char[] {[6][5][4]})
		digits[i++] = number % 10;
		number /= 10;
	} while (number > 0);
	
	std::fill_n(digits + i, padding, 0);
	return i;
}

//converts ascii digits to equivalent unsigned char numbers
void BigInt::char2uchar(unsigned char *array, 
						unsigned long int length)
{
	for (unsigned long int i(0); i < length; i++)
		array[i] -= '0';
}

//check if all ascii values are digits '0' to '9'
bool BigInt::allCharsAreDigits(	const char *array, 
								unsigned long int length)
{
	for (unsigned long int i(0); i < length; i++)
		if (array[i] < '0' || array[i] > '9')
			return false;
			
	return true;
}

//compares two BigInts
int BigInt::compareNumbers(	unsigned char *a, unsigned long int na,      
		                    unsigned char *b, unsigned long int nb)
{
	if (na < nb)
	//a < b
	    return 2;
	else if (na > nb)
	//a > b
	    return 1;

	//check the digits one by one starting from the most significant one
	for (long int i = na - 1; i >= 0; i--)
	    //compare the digits
	    if (a[i] != b[i])
		{
		    if (a[i] < b[i])
		    //a < b
		        return 2;
		    else if (a[i] > b[i])
		    //a > b
		        return 1;
		}

	//a == b
	return 0;
}

//multiplies two unsigned char []
//we use the Divide and Conquer a.k.a. Karatsuba algorithm
void BigInt::multiply(	unsigned char *a, unsigned char *b,
						unsigned long int n,   
						unsigned char *buf1, unsigned char *result)
{
	//if *a <= SqrtULongMax && *b <= SqrtULongMax, 
	//the CPU can do the multiplication
	if (compareNumbers(a, n, SqrtULongMax.digits, SqrtULongMax.digitCount) != 1 &&
		compareNumbers(b, n, SqrtULongMax.digits, SqrtULongMax.digitCount) != 1)
	{
		intToUChar(toInt(a, n) * toInt(b, n), buf1, n << 1);
		return;
	}

	//nh <= nl
	unsigned long int 	nh(n >> 1), nl(n - nh), nt(nl + 1);
	unsigned char *t1(buf1 + (n << 1));
	
	BigInt::add(a + nl, nh, a, nl, buf1, nt);
	BigInt::add(b + nl, nh, b, nl, buf1 + nt, nt);
	BigInt::multiply(a + nl, b + nl, nh, t1, t1);	//p1
	BigInt::multiply(a, b, nl, t1 + (nh << 1), t1 + (nh << 1));		//p2
	BigInt::multiply(buf1, buf1 + nt, nt, t1 + (n << 1), t1 + (n << 1));//p3
	
	unsigned long int power(n);
	if (power & 1)
		power++;
	//copy and shift left p3 by power / 2 and pad right to n * 2 with zeroes
	a = buf1 + (power >> 1);
	std::fill(buf1, a, 0);
	std::copy(t1 + (n << 1), t1 + ((n + nl) << 1) + 1, a);
	std::fill(a + (nl << 1) + 1, t1, 0);
	
	//shifted p3 -= p2
	//a == shifted p3, b == p2
	b = t1 + (nh << 1);
	unsigned char carry(0), sum(0);
	for (unsigned long int i(0); i < (nl << 1); i++)
	{
		sum = 10 + a[i] - (b[i] + carry);
		if (sum < 10)	//carry
		{
			a[i] = sum;
			carry = 1;
		}
		else
		{
			a[i] = sum % 10;
			carry = 0;
		}
	}
	a = &a[nl << 1];
	for (; carry && a < t1; a++)
		if (*a)
		{
			(*a)--;
			break;
		}
		else
			*a = 9;
	
	//shifted p3 -= p1
	//a = shifted p3, b = p1
	a = buf1 + (power >> 1);
	b = t1;
	carry = 0;
	for (unsigned long int i(0); i < (nh << 1); i++)
	{
		sum = 10 + a[i] - (b[i] + carry);
		if (sum < 10)	//carry
		{
			a[i] = sum;
			carry = 1;
		}
		else
		{
			a[i] = sum % 10;
			carry = 0;
		}
	}
	a = &a[nh << 1];
	for (; carry && a < t1; a++)
		if (*a)
		{
			(*a)--;
			break;
		}
		else
			*a = 9;
	
	//shifted p3 += shifted p1
	//a = p3[power], b = p1
	a = buf1 + power;
	carry = 0;
	for (unsigned long int i(0); i < (nh << 1); i++)
	{
		a[i] = a[i] + b[i] + carry;
		carry = a[i] / 10;
		a[i] %= 10;
	}
	
	//p3 += p2
	//a = p3, b = p2
	a = buf1;
	b += (nh << 1);
	carry = 0;
	for (unsigned long int i(0); i < (nl << 1); i++)
	{
		a[i] = a[i] + b[i] + carry;
		carry = a[i] / 10;
		a[i] %= 10;
	}
	a += (nl << 1);
	for (unsigned long int i(0); carry; i++)
	{
		a[i] += 1;
		carry = a[i] / 10;
		a[i] %= 10; 
	}
}

//divides two BigInt numbers
void BigInt::divide(const BigInt &dividend, const BigInt &divisor, 
					BigInt &quotient, BigInt &remainder)
{
	BigInt Z1, R, X(dividend);
	
	while (X >= divisor)
	{
		unsigned long int O(X.Length() - divisor.Length());
		if (O <= ULongMax.Length() - 2)
		{
			unsigned long int i;
			if (X.Length() > ULongMax.Length() - 1)
				i = ULongMax.Length() - 1;
			else
				i = X.Length();
			unsigned long int j(i - O);
			Z1 = 	toInt(X.digits + X.Length() - i, i) / 
					toInt(divisor.digits + divisor.Length() - j, j);
		}
		else
		{
			unsigned long int i(ULongMax.Length() - 1);
			unsigned long int j;
			if (divisor.Length() > ULongMax.Length() - 2)
				j = ULongMax.Length() - 2;
			else
				j = divisor.Length();
			Z1 = 	toInt(X.digits + X.Length() - i, i) / 
					toInt(divisor.digits + divisor.Length() - j, j);
			Z1.shiftLeft(O - Z1.Length());		
		}
		
		predictZ1:
		R = Z1 * divisor;
	
		if (X >= R)
		{
			X = X - R;
			quotient += Z1;
		}
		else
		{
			if (Z1.Length() > 1)
				Z1.shiftRight(1);
			else
				--Z1;
			goto predictZ1;
		}
	}
	
	remainder = X;
}

/*returns the value of unsigned char * as long int*/
unsigned long int BigInt::toInt(unsigned char *digits, int n)
{
	unsigned long int newInt(0);
	unsigned long int powerOf10(1);
	for (int i(0); i < n; i++)
	{
		newInt += digits[i] * powerOf10;
		powerOf10 *= 10;
	}
	return newInt;
}

/*returns the sum of two unsigned char* */
void BigInt::add(	unsigned char *a, unsigned long int na, //shorter
					unsigned char *b, unsigned long int nb, 
					unsigned char *result, int nResult)
{
	//single digitwise sum
	unsigned char subSum(0);

	//single digitwise carry
	unsigned char subCarry(0);

	//count the digits
	unsigned long int i(0);
	
	//add the digits
 	for (; i < na; i++)
 	{
 	    subSum = b[i] + a[i] + subCarry;
		subCarry = subSum / 10;
		result[i] = subSum % 10;
    }
    
    for (; i < nb; i++)
	{
	    subSum = b[i] + subCarry;
	    subCarry = subSum / 10;
	    result[i] = subSum % 10;
	}
	
	if (subCarry)
		result[i++] = 1;
	
	std::fill_n(result + i, nResult - i, 0);
}

/*shifts the digits left by n places*/
BigInt &BigInt::shiftLeft(unsigned long int n)
{
	//if the number is 0, we won't shift it
	if (EqualsZero())
		return *this;
	if (length <= digitCount + n + 2)
		expandTo(digitCount + n + 2);
	
	std::copy_backward(digits, digits + digitCount, digits + n + digitCount);
	std::fill_n(digits, n, 0);
	digitCount += n;
	return *this;
}

/* shifts the digits n places to the right */
BigInt &BigInt::shiftRight(unsigned long int n)
{
	if (n >= digitCount)
		throw "Error 13: Overflow on shift right.";
	
	std::copy_backward(	digits + n, digits + digitCount, 
						digits + digitCount - n);
	digitCount -= n;
	return *this;
}

/*expands the digits to n places*/
void BigInt::expandTo(unsigned long int n)
{
	unsigned long int oldLength(length);
    length = n;
    unsigned char *oldDigits(digits);
	try
	{
		digits = new unsigned char[length];
	}
	catch (...)
	{
		delete[] digits;
		digits = oldDigits;
		length = oldLength;
		throw "Error 11: BigInt creation error (out of memory?).";
	}

	std::copy(oldDigits, oldDigits + digitCount, digits);
	delete[] oldDigits;
}

/*Zero params constructor - creates a zero*/
BigInt::BigInt() : digits(0), length(10), digitCount(1)
{
    try
    {
        digits = new unsigned char[length];
    }
    catch (...)
    {
    	delete[] digits;
        throw "Error 00: BigInt creation error (out of memory?).";
    }

    //initialize to 0
    digits[0] = 0;
}

/*const char * param constructor - converts a const char * into a BigInt*/
BigInt::BigInt(const char * charNum) : digits(0)
{			
	digitCount = strlen(charNum);

	if (digitCount == 0)
	    throw "Error 01: Input string empty.";

	//get rid of the leading zeroes
	while (charNum[0] == '0')
	{
		charNum++;
		digitCount --;
	}

	//check if the string contains only decimal digits
	if (! BigInt::allCharsAreDigits(charNum, digitCount))
	    throw "Error 02: Input string contains characters other than digits.";
		
	//the input string was like "00...00\0"
	if (charNum[0] == 0)
	{
		digitCount = 1;
		charNum--;
	}
		
	length = (int)(digitCount * factor + 1);
		
	try
	{
		digits = new unsigned char[length];
	}
	catch (...)
	{
		delete[] digits;
		throw "Error 03: BigInt creation error (out of memory?).";
	}

	//copy the digits backwards to the new BigInt
	std::reverse_copy(charNum, charNum + digitCount, digits);
	//convert them to unsigned char
	BigInt::char2uchar(digits, digitCount);
}

/*long int param constructor - converts a long int into a BigInt*/
BigInt::BigInt(unsigned long int intNum) : digits(0)
{		
	//negative numbers not supported
	if (intNum < 0)
	    throw "Error 04: Negative input integer.";

	//we don't know how many digits there are in intNum since
	//sizeof(long int) is platform dependent (2^128 ~ 39 digits), so we'll
	//first save them in a temporary unsigned char[], and later copy them
	unsigned char tempDigits[40] = {0};

	unsigned long int numLength = intToUChar(intNum, tempDigits);

	length = (unsigned long int)(numLength * factor + 1);
	digitCount = numLength;

	try
	{
		digits = new unsigned char[length];
	}
	catch (...)
	{
		delete [] digits;
		throw "Error 05: BigInt creation error (out of memory?).";
	}

	std::copy(tempDigits, tempDigits + digitCount, digits);
}

/*copy constructor*/
BigInt::BigInt(const BigInt &rightNumber) : length(rightNumber.length),
digitCount(rightNumber.digitCount)
{
	//TODO: is this good practice?
	//make sure we have enough space to expand in the future
    if (length <= digitCount + 2)
        length = int(length * factor + 1);
	try
	{
		digits = new unsigned char[length];
	}
	catch (...)
	{
		delete[] digits;
		throw "Error 06: BigInt creation error (out of memory?).";
	}

	std::copy(rightNumber.digits, rightNumber.digits + digitCount, digits);
}

/*overloaded assignment operator*/
BigInt &BigInt::operator =(const BigInt &rightNumber)
{
	//if the right-hand operand is longer than the left-hand one or
	//twice as small
	if (rightNumber.length > length || (rightNumber.length << 1) < length) 
	{
		length = (int) (rightNumber.length + 2);
		//keep a pointer to the current digits, in case
		//there is not enough memory to allocate for the new digits
		unsigned char *tempDigits(digits);
		
		try
		{
			digits = new unsigned char[length];
		}
		catch (...)
		{
			//clean up the mess
			delete[] digits;
			//restore the digits
			digits = tempDigits;
			throw "Error 07: BigInt assignment error (out of memory?).";
		}
		//it turns out we don't need this any more
		delete[] tempDigits;
	}
	//destructive self-assignment protection
	else if (this == &rightNumber)
	    return *this;

	//copy the values
	digitCount = rightNumber.digitCount;
	std::copy(rightNumber.digits, rightNumber.digits + digitCount, digits);
	return *this;
}

/*overloaded left shift operator used for console output*/
std::ostream &operator <<(std::ostream &cout, const BigInt &number)
{
    for (int i = number.digitCount - 1; i >= 0; i--)
        cout << (int(number.digits[i]));

	return cout;
}

/*overloaded < operator*/
bool operator <(const BigInt &leftNum,
		        const BigInt &rightNum)
{
	if (BigInt::compareNumbers(	leftNum.digits, leftNum.digitCount,
								rightNum.digits, rightNum.digitCount) == 2)
	    return true;
	return false;
}

/*overloaded <= operator*/
bool operator <=(	const BigInt &leftNum,
		        	const BigInt &rightNum)
{
	if (BigInt::compareNumbers(	leftNum.digits, leftNum.digitCount,
								rightNum.digits, rightNum.digitCount) == 1)
	    return false;
	return true;
}

/*overloaded > operator*/
bool operator >(const BigInt &leftNum,
		        const BigInt &rightNum)
{
	if (BigInt::compareNumbers(	leftNum.digits, leftNum.digitCount,
								rightNum.digits, rightNum.digitCount) == 1)
	    return true;
	return false;
}

/*overloaded >= operator*/
bool operator >=(	const BigInt &leftNum,
		        	const BigInt &rightNum)
{
	if (BigInt::compareNumbers(	leftNum.digits, leftNum.digitCount,
								rightNum.digits, rightNum.digitCount) == 2)
	    return false;
	return true;
}

/*overloaded == operator*/
bool operator ==(	const BigInt &leftNum,
		        	const BigInt &rightNum)
{
	if (BigInt::compareNumbers(	leftNum.digits, leftNum.digitCount,
								rightNum.digits, rightNum.digitCount))
	    return false;
	return true;
}

/*overloaded != operator*/
bool operator !=(	const BigInt &leftNum,
		        	const BigInt &rightNum)
{
	if (BigInt::compareNumbers(	leftNum.digits, leftNum.digitCount,
								rightNum.digits, rightNum.digitCount))
	    return true;
	return false;
}

/*overloaded addition operator*/
BigInt operator + (	const BigInt &leftNum,
					const BigInt &rightNum)
{
	//find the bigger of the operands
	const BigInt *smaller, *bigger;
	if (leftNum > rightNum)
	{
	    smaller = &rightNum;
	    bigger = &leftNum;
	}
	else
	{
		smaller = &leftNum;
		bigger = &rightNum;
	}

	BigInt sum(*bigger);

	//single digitwise sum
	unsigned char subSum(0);

	//single digitwise carry
	unsigned char subCarry(0);

	//add the first smaller->digitCount digits
 	for (int unsigned long i(0); i < smaller->digitCount; i++)
 	{
 	    subSum = sum.digits[i] + smaller->digits[i] + subCarry;
		subCarry = subSum / 10;
		sum.digits[i] = subSum % 10;
    }

    //add a leading zero to the sum, in case there is a carry after all
    //the digits have been added
	sum.digits[sum.digitCount] = 0;

    //if there is a carry from the previous digitwise sum then
    //continue with the process
	for (	unsigned long int i(smaller->digitCount); 
			i <= bigger->digitCount || subCarry; i++)
	{
	    subSum = sum.digits[i] + subCarry;
	    subCarry = subSum / 10;
	    sum.digits[i] = subSum % 10;
	}

    //if there has been a carry after all the digits have been added
    //increase the number of digits
	if (sum.digits[sum.digitCount] > 0)			
	    sum.digitCount++;

	return sum;
}

/*overloaded ++ operator, prefix version*/							
BigInt &BigInt::operator++()
{
	//count how many digits we have changed
	unsigned long int i(0);
	//increment *this by one
	for (; i < digitCount; i++)
	{
		if (digits[i] == 9)
			digits[i] = 0;
		else
		{
			digits[i]++;
			break;
		}
	}
	
	//if the number was like 99...99
	if (i == digitCount)
	{
		//check if length is too small for a new digit.
		if (digitCount + 2 >= length)
			//let's make sure there is enough space for new digits
			expandTo(digitCount + 2);
		
		digitCount++;
		digits[i] = 1;
	}
	
	return *this;
}

/*overloaded ++ operator, postfix version*/							
BigInt BigInt::operator++(int)
{
	BigInt temp(*this);
	++(*this);
	return temp;
}

/*overloaded += operator*/
BigInt &BigInt::operator+=(const BigInt &rightNum)
{
	//TODO: optimize
	*this = *this + rightNum;
	return *this;
}

/*overloaded subtraction operator*/
BigInt operator - (	const BigInt &leftNum,
					const BigInt &rightNum)
{
    //the left operand must be greater or equal to the right one
    if (leftNum < rightNum)
        throw "Error 08: Invalid subtraction operands.";

    //trivial case
    if (leftNum == rightNum)
    {
        return BigIntZero;
    }

    BigInt result(leftNum);
    //temporary variable
    BigInt rightNumCopy(rightNum);

    //often used temporary variable
    int rDigits(rightNumCopy.digitCount);

    //in case of a digitwise carry, overflow = true
    bool overflow(false);

    for (int i(0); i < rDigits; i++)
    {
        overflow = (leftNum.digits[i] - rightNumCopy.digits[i]) < 0;

        if (overflow)
        {
            result.digits[i] = leftNum.digits[i] + 10 - rightNumCopy.digits[i];
            //transfer carry
            rightNumCopy.digits[i+1]++;
        }
        else
            //make the digitwise subtraction
            result.digits[i] = leftNum.digits[i] - rightNumCopy.digits[i];
    }

    //if there is a carry and the following digit is 0 => there will
    //be a carry again...
    if (overflow && result.digits[rDigits] == 0)
    {
        result.digits[rDigits] = 9;
        
        int i(rDigits + 1);
        for (; result.digits[i] == 0; i++)
            result.digits[i] = 9;

        result.digits[i] -= 1;
    }
    //there is a carry but there will be no more carries
    else if (overflow)
        result.digits[rDigits]--;

	//TODO: optimize this
    //get rid of the leading zeroes
    for (int i(result.digitCount - 1); i > 0; i--)
        if (result.digits[i] == 0)
            result.digitCount--;
        else
            break;

    return result;
}

/*overloaded -- operator, prefix version*/							
BigInt &BigInt::operator--()
{
	//TODO: optimize
	*this = *this - BigIntOne;
	return *this;
}

/*overloaded -- operator, postfix version*/							
BigInt BigInt::operator--(int)
{
	//TODO: optimize
	BigInt temp(*this);
	*this = *this - BigIntOne;
	return temp;
}

/*overloaded -= operator*/
BigInt &BigInt::operator-=(const BigInt &rightNum)
{
	//TODO: optimize
	*this = *this - rightNum;
	return *this;	
}

/*overloaded multiplication operator*/
BigInt operator*(	const BigInt &leftNum,
					const BigInt &rightNum)
{
	if (leftNum.EqualsZero() || rightNum.EqualsZero())
		return BigIntZero;
		
	int n(	(leftNum.digitCount < rightNum.digitCount ? 
			rightNum.digitCount : leftNum.digitCount));
			
	//we will use a temporary buffer for multiplication
	unsigned char *a(0);
	
	try
	{
		a = new unsigned char[100 * n];
	}
	catch (...)
	{
		delete[] a;
		throw "Error 09: Not enough memory?";
	}
	
	unsigned char *b(a + n), *buf1(b + n);
	
	std::copy(leftNum.digits, leftNum.digits + leftNum.digitCount, a);
	std::fill(a + leftNum.digitCount, a + n, 0);	
	std::copy(rightNum.digits, rightNum.digits + rightNum.digitCount, b);
	std::fill(b + rightNum.digitCount, b + n, 0);
	
	BigInt::multiply(a, b, n, buf1, buf1);
	
	BigInt bigIntResult;
	bigIntResult.expandTo((n << 1) + 10);
	std::copy(buf1, buf1 + (n << 1), bigIntResult.digits);
	for (unsigned long int i = (n << 1) - 1; i > 0; i--)
	{
		if (bigIntResult.digits[i])
		{
			bigIntResult.digitCount = i + 1;
			break;
		}
	}
	delete[] a;
	
	return bigIntResult;
}

/*overloaded *= operator*/
BigInt &BigInt::operator*=(const BigInt &rightNum)
{
	*this = *this * rightNum;
	return *this;
}

/* overloaded division operator
 * works according to the formula leftNum == Z * rightNum + L*/
BigInt operator /(	const BigInt &leftNum, 
					const BigInt &rightNum)
{
	if (rightNum.EqualsZero())
		throw "Error 12: Attempt to divide by zero.";
		
	//we don't want to call this function twice
	int comparison(BigInt::compareNumbers(	leftNum.digits, 
											leftNum.digitCount, 
											rightNum.digits, 
											rightNum.digitCount));
	
	//if leftNum == 0 or leftNum < rightNum 
	if (leftNum.EqualsZero() || comparison == 2)
		return BigIntZero;

	//if leftNum == rightNum
	if (comparison == 0)
		return BigIntOne;
		
	BigInt quotient, remainder;
	BigInt::divide(leftNum, rightNum, quotient, remainder);
	return quotient;
}

/*overloaded /= operator*/
BigInt &BigInt::operator /=(const BigInt &rightNum)
{
	*this = *this / rightNum;
	return *this;
}

/*overloaded remainder operator*/
BigInt operator%(	const BigInt &leftNum, 
					const BigInt &rightNum)
{
	if (rightNum.EqualsZero())
		throw "Error 14: Attempt to divide by zero.";
		
	//we don't want to call this function twice
	int comparison(BigInt::compareNumbers(	leftNum.digits, 
											leftNum.digitCount, 
											rightNum.digits, 
											rightNum.digitCount));
	
	//leftNum == rightNum 
	if (comparison == 0)
		return BigIntZero;

	//if leftNum < rightNum
	if (comparison == 2)
		return leftNum;
		
	BigInt quotient, remainder;
	BigInt::divide(leftNum, rightNum, quotient, remainder);
	return remainder;
}
							
/*overloaded %= operator*/
BigInt &BigInt::operator%=(const BigInt &rightNum)
{
	*this = *this % rightNum;
	return *this;
}

/* returns *this to the power of n 
 * using the fast Square and Multiply algorithm*/
BigInt BigInt::Power(unsigned long int n) const
{
	BigInt result(BigIntOne);
	BigInt base(*this);
	
	while (n)
	{
		//if n is odd
		if (n & 1)
		{
			result = result * base;
			n -= 1;
		}
		n /= 2;
		base = base * base;
	}
	
	return result;
}

/*overloaded [] operator, returns the nth digit*/
unsigned char BigInt::operator [](unsigned long int n) const
{
	if (n >= digitCount)
		throw "Error 10: Index out of range.";
		
	return digits[digitCount - n - 1];
}

/*returns the value of BigInt as string*/
std::string BigInt::ToString() const
{
	std::string bigIntStr(digits, digits + digitCount);
	
	for (unsigned long int i(0); i < digitCount; i++)
		bigIntStr[i] = digits[i] + '0';
		
	std::reverse(bigIntStr.begin(), bigIntStr.end());
		
	return bigIntStr;
}
