/* ****************************************************************************
 * 
 * 				test.h
 * 
 * Author: Nedim Srndic
 * Release date: 14th of March 2008
 * 
 * Header file containing declarations of test functions.  
 * 
 * ****************************************************************************
 */
 
#ifndef TEST_H_
#define TEST_H_

void pauseScreen();

/*CONSTRUCTOR, DESTRUCTOR, ASSIGNMENT AND STREAM OUTPUT OPERATOR TEST*/
void TestBigIntFunctions();
/*				ARITHMETIC AND COMPARISON OPERATOR TEST	*/
void TestBigIntOperators();
/*				FACTORIAL COMPUTATION BENCHMARK			*/
void FactorialBenchmark();
/*				RANDOM TEST GENERATOR					*/
void RandomBigIntTest(unsigned long int testCount);
/*				RANDOM DIVISION TEST GENERATOR			*/
void RandomBigIntDivisionTest(unsigned long int testCount);
/*				PRIME GENERATOR TEST					*/
void TestPrimeGenerator(unsigned long int testCount);
/*				KEY GENERATOR TEST						*/
void TestKeyGeneration(unsigned long int testCount);
/*				ENCRYPTION/DECRYPTION TEST				*/
void TestEncryptionDecryption(unsigned long int testCount);
/*				FILE ENCRYPTION/DECRYPTION TEST			*/
void TestFileEncryptionDecryption(unsigned long int testCount);

#endif /*TEST_H_*/
