/* ****************************************************************************
 * 
 * 				BigInt.h
 * 
 * Author: Nedim Srndic
 * Release date: 14th of March 2008
 * 
 * A class representing a big positive integer that cannot be saved in
 * standard C++ integer types (i. e. 2^128 is "just" 39 digits long). 
 * Negative numbers are not supported at this time. 
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
 * When assigning a BigInt a that is twice as small or bigger than *this, 
 * the length is set to (a.length + 2). 
 * There are no memory leaks. 
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
 * 		For multiplication, the Square and multiply or Karatsuba algorithm is
 * 		used. It multiplies integers in O(n^log2(3)) complexity. 
 * 		log2(3) is approximately 1.585, so this is significantly faster
 * 		than classical multiplication. I have spent	a lot of time optimizing 
 * 		this. 
 * 
 * 	- C-style integer division 	(/, /=)
 * 		Division is painfully slow. I hope to speed this up in the future. 
 * 
 * 	- C-style integer division remainder (%, %=)
 * 		When calculating the remainder, the number is first divided. I hope to
 * 		speed this up in the future. 
 * 
 * 	- comparison 				(==, !=, <, <=, >, >=)
 * 		All of the <, <=, >, >= operators are equally fast. 
 * 
 * 	- exponentiation 			(Power())
 * 		For exponentiation, the Exponantiation by squaring 
 * 		(or Square and multiply or Binary exponentiation) algorithm is used. 
 * 		It uses O(log(n)) multiplications and therefore is significantly faster
 * 		than multiplying x with itself n-1 times. 
 * 
 * In addition to mathematical operations, BigInt supports: 
 * 
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
#include <string> //ToString()

class BigInt
{
	private:
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
		/*returns the sum of two unsigned char[]*/
		static void add(unsigned char *a, unsigned long int na, 
						unsigned char *b, unsigned long int nb, 
						unsigned char *result, int nResult);
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
		BigInt Power(unsigned long int n) const;
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
