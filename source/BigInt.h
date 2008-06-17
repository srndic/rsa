/* ****************************************************************************
 * 
 * 				BigInt.h
 * 
 * Author: Nedim Srndic
 * Release date: 14th of March 2008
 * 
 * A class representing a positive or negative integer that may 
 * be too large to fit in any of the standard C++ integer types 
 * (i. e. 2^128 is "just" 39 digits long). 
 * The digits are stored in a dinamic array of tipe unsigned char*, 
 * with values from 0 to 9 (not '0' to '9'), so that the CPU can  
 * add/subtract individual digits. 
 * 
 * The array has "length" memory locations, one byte each (the size of 
 * unsigned char is probably one byte). There are "digitCount" digits actually
 * in use, the rest is spare space. 
 * The number of digits is constrained by available memory and the limit of the
 * unsigned long int type used for indexing (the "length" property). 
 * The individual digits are stored right-to-left, to speed up computing and 
 * allow for faster growth of numbers (no need to reallocate memory when 
 * the digitCount grows). 
 * 
 * The class handles its own memory management. There are no memory leaks
 * reported until this date. 
 * When creating a BigInt from const char* or unsigned long int, 
 * copying from an other BigInt with (digitCount + 2 <= length) 
 * (soon to be full), new memory is allocated and 
 * length is adjusted to (length * factor + 1). This is done to expand the 
 * capacity of the digits array to accomodate potential newcomer digits. 
 * When assigning a BigInt "bInt" that is twice as small or bigger than *this, 
 * the length is set to (bInt.length + 2). 
 * 
 * BigInt supports: 
 * 
 * 	- addition 					(unary +, binary +, +=, prefix ++, postfix ++)
 * 		If you can, use prefix ++ insted of postfix ++ because of much greater
 * 		performance. 
 * 
 * 	- subtraction 				(binary -, -=, prefix --, postfix --)
 * 		If you can, use prefix -- insted of postfix -- because of greater
 * 		performance. 
 * 
 * 	- multiplication 			(*, *=)
 * 		For multiplication, one can choose between the Square and multiply 
 * 		or Karatsuba algorithm, or long multiplication at compile time 
 * 		(this can be done by defining or undefining the macro "KARATSUBA").
 * 		The Karatsuba algorithm multiplies integers in O(n^log2(3)) 
 * 		complexity. log2(3) is approximately 1.585, so this should be 
 * 		significantly faster than long multiplication, if the numbers are 
 * 		big enough. I haven't tested this too much. Currently, the long 
 * 		multiplication is better implemented, and runs faster than the 
 * 		Karatsuba multiplication for numbers shorter than about 100 digits. 
 * 
 * 	- C-style integer division 	(/, /=)
 * 		Division is pretty slow. I hope to speed this up in the future. 
 * 
 * 	- C-style integer division remainder (%, %=)
 * 		When calculating the remainder, the number is first divided. I'm
 * 		not sure if this can be done any smarter. I hope to
 * 		speed this up in the future. 
 * 
 * 	- comparison 				(==, !=, <, <=, >, >=)
 * 		All of the <, <=, >, >= operators are equally fast. 
 * 
 * 	- exponentiation 	(GetPower(), SetPower(), GetPowerMod(), SetPowerMod())
 * 		For exponentiation, the Exponantiation by squaring 
 * 		(or Square and multiply or Binary exponentiation) algorithm is used. 
 * 		It uses O(log(n)) multiplications and therefore is significantly faster
 * 		than multiplying x with itself n-1 times. 
 * 
 * In addition to mathematical operations, BigInt supports: 
 * 
 * 	- automatic conversion from const char * and unsigned long int 
 * 	- safe construction, copying, assignment and destruction 
 * 	- writing to the standard output (operator <<(std::ostream, BigInt))
 * 	- returning nth digit (operator[])
 * 	- returning the number of digits (Length())
 * 	- returning a string of digits (ToString())
 * 		This can be useful for human-readable output. 
 * 	- returning a value indicating wether the BigInt equals zero (EqualsZero())
 * 		The fastest way to determine this.
 * 
 * There are a few static constants defined in this file: 
 * 
 * 	- BigIntZero 	: a zero of type BigInt
 * 		If you need a zero fast, use this. 
 * 	- BigIntOne		: a one of type BigInt 
 * 		If you need a one fast, use this. 
 * 
 * ****************************************************************************
*/

#ifndef BIGINT_H_
#define BIGINT_H_

#include <iostream>	//ostream
#include <cmath>	//sqrt()
#include <string>	//ToString()

class BigInt
{
	private:
		//for optimization purposes
		friend class PrimeGenerator;
		//an array of digits stored right to left,
		//i.e. int 345 = unsigned char {[5], [4], [3]}
		unsigned char *digits;
		//the total length of the allocated memory
		unsigned long int length;
		//number of digits
		unsigned long int digitCount;
		//multiplication factor for the length property
		//when creating or copying objects
		static const double factor;
		//transforms the number from unsigned long int to unsigned char[]
		//and pads the result with zeroes
		static unsigned long int intToUChar(unsigned long int number, 
											unsigned char *digits, 
											unsigned long int padding);
		//converts ascii digits to equivalent unsigned char numeric values
		static void char2uchar(	unsigned char *array, 
								unsigned long int length);
		//check if all ascii values are digits '0' to '9'
		static bool allCharsAreDigits(	const char *array, 
										unsigned long int length);
		//compares two unsigned char[]
		static int compareNumbers(	unsigned char *a, unsigned long int na,
		                            unsigned char *b, unsigned long int nb);
		//multiplies two unsigned char[]
		//we use the Divide and Conquer a.k.a. Karatsuba algorithm
		static void karatsubaMultiply(	unsigned char *a, unsigned char *b,
										unsigned long int n, 
										unsigned char *buffer);
		//multiplies two unsigned char[] the long way
		static void longMultiply(	unsigned char *a, unsigned long int na,
									unsigned char *b, unsigned long int nb,
									unsigned char *result);
		//simple addition, used by the multiply function
		//returns the remaining carry
		static unsigned char quickAdd(	unsigned char *a, unsigned char *b, 
										unsigned long int n);
		//simple subtraction, used by the multiply function
		static void quickSub(	unsigned char *a, unsigned char *b, 
								unsigned char *end, unsigned long int n);
		//divides two BigInt numbers
		static void divide(	const BigInt &dividend, const BigInt &divisor,
							BigInt &quotient, BigInt &remainder);
		/*returns the value of unsigned char[] as long int*/
		static unsigned long int toInt(unsigned char *digits, int n);
		/* Saves the sum of two unsigned char* shorter and longer into result. 
		 * It must be nShorter <= nLonger. If doFill == true, it fills the 
		 * remaining free places with zeroes (used in KaratsubaMultiply()). 
		 * Returns true if there was an overflow at the end (meaning that
		 * the result.digitCount was longer.digitCount + 1. */
		static bool add(unsigned char *shorter, unsigned long int nShorter, 
					unsigned char *longer, unsigned long int nLonger, 
					unsigned char *result, int nResult, 
					bool doFill = true);
		/*shifts the digits n places to the left*/
		BigInt &shiftLeft(unsigned long int n);
		/* shifts the digits n places to the right */
		BigInt &shiftRight(unsigned long int n);
		/*expands the digits* to n*/
		void expandTo(unsigned long int n);
	public:
		/*Zero params constructor - creates a new BigInt with the value of 0*/
		BigInt();
		/*const char * param constructor - converts a const char * 
		 * into a BigInt*/
		BigInt(const char *charNum);
		/*long int param constructor - converts a long int into a BigInt*/
		BigInt(unsigned long int intNum);
		/*copy constructor*/
		BigInt(const BigInt &rightNumber);
		/*overloaded assignment operator*/
		BigInt &operator =(const BigInt &rightNumber);
		/*destructor*/
		~BigInt();
		friend std::ostream &operator <<(	std::ostream &cout, 
											const BigInt &number);
		friend bool operator <(	const BigInt &leftNum,
		                        const BigInt &rightNum);
		friend bool operator <=(const BigInt &leftNum,
		                        const BigInt &rightNum);
		friend bool operator >(	const BigInt &leftNum,
		                        const BigInt &rightNum);
		friend bool operator >=(const BigInt &leftNum,
		                        const BigInt &rightNum);
		friend bool operator ==(const BigInt &leftNum,
		                        const BigInt &rightNum);
		friend bool operator !=(const BigInt &leftNum,
		                        const BigInt &rightNum);
		friend BigInt operator + (	const BigInt &leftNum,
									const BigInt &rightNum);
		BigInt &operator+();
		BigInt &operator++();
		BigInt operator++(int);
		BigInt &operator+=(const BigInt &rightNum);
		friend BigInt operator - (	const BigInt &leftNum,
									const BigInt &rightNum);
		BigInt &operator--();
		BigInt operator--(int);
		BigInt &operator-=(const BigInt &rightNum);
		friend BigInt operator*(	const BigInt &leftNum,
									const BigInt &rightNum);
		BigInt &operator*=(const BigInt &rightNum);
		friend BigInt operator/(	const BigInt &leftNum, 
									const BigInt &rightNum);
		BigInt &operator/=(const BigInt &rightNum);
		friend BigInt operator%(	const BigInt &leftNum, 
									const BigInt &rightNum);
		BigInt &operator%=(const BigInt &rightNum);
		/*returns *this to the power of n*/
		BigInt GetPower(unsigned long int n) const;
		/* *this = *this to the power of n*/
		void SetPower(unsigned long int n);
		/*returns *this to the power of n*/
		BigInt GetPower(BigInt n) const;
		/* *this = *this to the power of n*/
		void SetPower(BigInt n);
		/* returns (*this to the power of b) mod n */
		BigInt GetPowerMod(const BigInt &b, const BigInt &n);
		/* *this = (*this to the power of b) mod n */
		void SetPowerMod(const BigInt &b, const BigInt &n);
		/*returns the nth digit*/
		unsigned char operator [](unsigned long int n) const;
		/*returns the number of digits*/
		unsigned long int Length() const;
		/*returns the value of BigInt as string*/
		std::string ToString() const;
		/*returns a value indicating whether *this equals 0*/
		bool EqualsZero() const;
};

/*destructor*/
inline BigInt::~BigInt()
{
	delete[] digits;
}

inline BigInt &BigInt::operator+()
{
	return *this;
}

/*returns the number of digits*/
inline unsigned long int BigInt::Length() const
{
	return digitCount;
}

/*returns a value indicating whether *this equals 0*/
inline bool BigInt::EqualsZero() const
{
	return digitCount == 1 && digits[0] == 0;
}
		
//A BigInt number with the value of 0
static const BigInt BigIntZero;
//A BigInt number with the value of 1
static const BigInt BigIntOne(1);


#endif /*BIGINT_H_*/
