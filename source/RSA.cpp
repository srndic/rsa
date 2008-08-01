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

#include "RSA.h"
#include "Key.h"	//Key
#include "KeyPair.h"	//KeyPair
#include "PrimeGenerator.h"	//Generate()
#include <string>	//string
#include <fstream>	//ifstream, ofstream

using std::string;

/* Returns the greatest common divisor of the two arguments 
 * "a" and "b", using the Euclidean algorithm. */
BigInt RSA::GCD(const BigInt &a, const BigInt &b)
{
	if (b.EqualsZero())
		return a;
	else
		return RSA::GCD(b, a % b);
}

/* Solves the equation 
 * 			d = ax + by 
 * given a and b, and returns d, x and y by reference. 
 * It uses the Extended Euclidean Algorithm */
void RSA::extendedEuclideanAlgorithm(	const BigInt &a, const BigInt &b, 
										BigInt &d, BigInt &x, BigInt &y)
{
	if (b.EqualsZero())
	{
		d = a;
		x = BigIntOne;
		y = BigIntZero;
		return;
	}
	RSA::extendedEuclideanAlgorithm(b, a % b, d, x, y);
	BigInt temp(x);
	x = y;
	y = temp - a / b * y;
}

/* Solves the equation 
 * 			ax is congruent to b (mod n), 
 * given a, b and n finds x. */
BigInt RSA::solveModularLinearEquation(	const BigInt &a, 
										const BigInt &b, 
										const BigInt &n)
{
	BigInt p, q, r;
	RSA::extendedEuclideanAlgorithm(a, n, p, q, r);
	if ((b % p).EqualsZero())
		return (q * (b / p)) % n;
	else
		throw "Error RSA00: Error in key generation.";
}

/* Throws an exception if "key" is too short to be used. */
void RSA::checkKeyLength(const Key &key)
{
	if (key.GetModulus().Length() < 7)
			throw "Error RSA01: Insufficient key length.";
}

/* Transforms a std::string message into a BigInt message. 
 * Every ASCII character of the original message is replaced by it's 
 * ASCII value and appended to the end of the newly created BigInt object
 * 'decoded' as a three-digit number, from left to right. */
BigInt RSA::encode(const string &message)
{
	BigInt encoded;
	encoded.digitCount = message.length() * 3;
	encoded.expandTo(encoded.digitCount + 4);
	for (unsigned long int i(0); i < message.length(); i++)
	{
		unsigned char ASCII = message[i];
		encoded.digits[i * 3 + 2] = ASCII % 10;
		ASCII /= 10;
		encoded.digits[i * 3 + 1] = ASCII % 10;
		encoded.digits[i * 3] = ASCII / 10;
	}
	return encoded;
}

/* Transforms a BigInt cyphertext into a std::string cyphertext. */
string RSA::decode(const BigInt &message)
{
	string decoded;
	for (unsigned long int i(0); i < message.digitCount / 3; i++)
	{
		char ASCII = 100 * char(message.digits[i * 3]);
		ASCII += 10 * char(message.digits[i * 3 + 1]);
		decoded.push_back(ASCII + char(message.digits[i * 3 + 2]));
	}
	return decoded;
}

/* Encrypts a "chunk" (a small part of a message) using "key" */
string RSA::encryptChunk(const string &chunk, const Key &key)
{
	BigInt a = RSA::encode(chunk);
	a.SetPowerMod(key.GetExponent(), key.GetModulus());
	return a.ToString();
}

/* Decrypts a "chunk" (a small part of a message) using "key" */
string RSA::decryptChunk(const BigInt &chunk, const Key &key)
{
	BigInt a = chunk;
	a.SetPowerMod(key.GetExponent(), key.GetModulus());
	return RSA::decode(a);
}

/* Encrypts a string "message" using "key". */
std::string RSA::encryptString(const std::string &message, const Key &key)
{
	//partition the message into manageable chunks
	const unsigned long int chunkSize(((key.GetModulus().Length()-1)/3)-1);
	const unsigned long int chunkCount = message.length() / chunkSize;
	
	string cypherText;
	for (unsigned long int i(0); i < chunkCount; i++)
	{
		//extract and encrypt the chunks
		string chunk(message.substr(i * chunkSize, chunkSize));
		chunk = RSA::encryptChunk(chunk + "a", key);
		cypherText.append(chunk.append(" "));
	}
	if (chunkSize * chunkCount == message.length())
		return cypherText;
	
	//handle the last chunk. It is smaller than the others
	const unsigned long int lastChunkSize = message.length() % chunkSize;
	string lastChunk(message.substr(chunkCount * chunkSize, lastChunkSize));
	lastChunk = RSA::encryptChunk(lastChunk + "a", key);
	return cypherText.append(lastChunk.append(" "));
}

/* Decrypts a string "message" using "key". */
std::string RSA::decryptString(const std::string &cypherText, const Key &key)
{
	//partition the cypherText into manageable chunks
	string message;
	long int i(0), j(0);
	while ((j = cypherText.find(' ', i)) != -1)
	{
		//there are spaces between chunks
		BigInt chunk(cypherText.substr(i, j - i));
		if (chunk >= key.GetModulus())
			throw "Error RSA02: Chunk too large.";
		
		//decrypt the chunks
		string text = RSA::decryptChunk(chunk, key);
		message.append(text.substr(0, text.length() - 1));
		i = j + 1;
	}
	return message;
}

/* Tests the file for 'eof', 'bad ' errors and throws an exception. */
void RSA::fileError(bool eof, bool bad)
{
	if (eof)
		throw "Error RSA03: Unexpected end of file.";
	else if (bad)
		throw "Error RSA04: Bad file?";
	else
		throw "Error RSA05: File contains unexpected data.";
}

/* Returns the string "message" RSA-encrypted using the key "key". */
string RSA::Encrypt(const string &message, const Key &key)
{
	RSA::checkKeyLength(key);
	
	return RSA::encryptString(message, key);
}

/* Encrypts the file "sourceFile" using the key "key" and saves 
 * the result into the file "destFile". */
void RSA::Encrypt(	const char *sourceFile, const char *destFile, 
					const Key &key)
{
	RSA::checkKeyLength(key);
	
	//open the input and output files
	std::ifstream source(sourceFile, std::ios::in | std::ios::binary);
	if (!source)
		throw "Error RSA06: Opening file \"sourceFile\" failed.";
	std::ofstream dest(destFile, std::ios::out | std::ios::binary);
	if (!dest)
		throw "Error RSA07: Creating file \"destFile\" failed.";
	
	//find the source file length
	source.seekg(0, std::ios::end);
	const unsigned long int fileSize = source.tellg();
	source.seekg(0, std::ios::beg);
	
	//create an input buffer
	const unsigned long int bufferSize = 4096;
	char buffer[bufferSize];
	
	//encrypt file chunks
	const unsigned long int chunkCount = fileSize / bufferSize;
	for (unsigned long int i(0); i <= chunkCount; i++)
	{
		unsigned long int readLength; 
		//read the chunk
		if (i == chunkCount)	//if it's the last one
			readLength = fileSize % bufferSize;
		else
			readLength = sizeof buffer;
		source.read(buffer, readLength);
		if (!source)
			RSA::fileError(source.eof(), source.bad());
		
		//encrypt the chunk
		std::string chunk(buffer, readLength);
		chunk = RSA::encryptString(chunk, key);
		//write the chunk
		dest.write(chunk.c_str(), chunk.length());
		if (!dest)
			RSA::fileError(dest.eof(), dest.bad());
	}
	
	source.close();
	dest.close();
}

/* Returns the string "cypherText" RSA-decrypted using the key "key". */
string RSA::Decrypt(const string &cypherText, const Key &key)
{
	RSA::checkKeyLength(key);
	
	return RSA::decryptString(cypherText, key);
}

/* Decrypts the file "sourceFile" using the key "key" and saves 
 * the result into the file "destFile". */
void RSA::Decrypt(	const char *sourceFile, const char *destFile, 
					const Key &key)
{
	RSA::checkKeyLength(key);
		
	//open the input and output files
	std::ifstream source(sourceFile, std::ios::in | std::ios::binary);
	if (!source)
		throw "Error RSA08: Opening file \"sourceFile\" failed.";
	std::ofstream dest(destFile, std::ios::out | std::ios::binary);
	if (!dest)
		throw "Error RSA09: Creating file \"destFile\" failed.";
	
	//find the source file length
	source.seekg(0, std::ios::end);
	const unsigned long int fileSize = source.tellg();
	source.seekg(0, std::ios::beg);
	
	//create an input buffer
	const unsigned long int bufferSize = 8192;
	char buffer[bufferSize];
	unsigned long int readCount = 0;
	
	while (readCount < fileSize)
	{
		unsigned long int readLength; 
		//read the chunk
		if (fileSize - readCount >= bufferSize)	//if it's not the last one
			readLength = sizeof buffer;
		else
			readLength = fileSize - readCount;
		source.read(buffer, readLength);
		if (!source)
			RSA::fileError(source.eof(), source.bad());
		
		//encrypt the chunk
		std::string chunk(buffer, readLength);
		chunk = chunk.substr(0, chunk.find_last_of(' ', chunk.length()) + 1);
		readCount += chunk.length();
		source.seekg(readCount, std::ios::beg);
		chunk = RSA::decryptString(chunk, key);
		//write the chunk
		dest.write(chunk.c_str(), chunk.length());
		if (!dest)
			RSA::fileError(dest.eof(), dest.bad());
	}
	
	source.close();
	dest.close();
}

/* Generates a public/private keypair. The keys are retured in a 
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
KeyPair RSA::GenerateKeyPair(	unsigned long int digitCount, 
								unsigned long int k)
{
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
	
	//calculate the totient phi
	BigInt phi((p - BigIntOne) * (q - BigIntOne));
	
	//we don't want the keys to be less than 20 bits long
	//TODO: restore
//	if (phi < "1048576")
//		throw "Error RSA04: Insufficient key strength!";
	
	//select a small odd integer e that is coprime with phi and e < phi
	//usually 65537 is used, and we will use it too if it fits
	//it is recommended that this be the least possible value for e
	BigInt e("65537");
	
	//make sure the requirements are met
	while (RSA::GCD(phi, e) != BigIntOne || e < "65537" || !e.IsOdd())
	{
		PrimeGenerator::MakeRandom(e, 5);
	}
	
	//now we have enough information to create the public key
	//e is the public key exponent, n is the modulus
	Key publicKey(n, e);
	
	//calculate d, de = 1 (mod phi)
	BigInt d(RSA::solveModularLinearEquation(e, BigIntOne, phi));
	
	if (!d.IsPositive())
		return RSA::GenerateKeyPair(digitCount, k);
	
	//we can create the private key
	//d is the private key exponent, n is the modulus
	Key privateKey(n, d);
	
	//finally, the keypair is created and returned
	KeyPair newKeyPair(privateKey, publicKey);
	return newKeyPair;
}
