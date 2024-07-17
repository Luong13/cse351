/*
 * CSE 351 Lab 1 (Data Lab - Pointers)
 *
 * Luong Dinh, ludinh, 2066624
 *
 * pointer.c - Source file with your solutions to the Lab.
 *             This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc compiler.
 * You can still use printf for debugging without including <stdio.h>. The
 * included file, "common.c" contains a function declaration that should
 * prevent a compiler warning. In general, it's not good practice to ignore
 * compiler warnings, but in this case it's OK.
 */

#ifndef COMMON_H
#include "common.h"
#endif

/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

#if 0
You will provide your solution to this homework by editing the collection of
functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one or more lines of C
  code that implements the function. Your code must conform to the following
  style:

  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are not allowed to
     use big constants such as 0xFFFFFFFF. However, you are allowed to combine
     constants to produce values outside this range (e.g., 250 + 250 = 500) so
     long as the operator you are using to combine the constants is listed as
     "legal" at the top of the method you are writing.
  2. Function arguments and local variables (no global variables).
  3. Any operators listed as "ALLOWED" in the function header comment of the
     method you are writing.
  4. Casting.

  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to one
  operator per line.

  You are expressly forbidden from:
  1. Using control constructs such as if, do, while, for, switch (unless
     otherwise stated).
  2. Defining or using macros.
  3. Defining additional functions in this file.
  4. Calling functions (unless otherwise stated).
  5. Using operators listed as "DISALLOWED" in the function header comment of
     the method you are writing.

  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has *undefined* behavior when shifting by a negative amount or an amount
     greater than or equal to the number of bits in the value being shifted.
     (i.e., x >> n is undefined when n < 0 or n >= *size of x*)
     (e.g., if x is a 32-bit int, shifts by >= 32 bits are undefined)
     Undefined behvaior means that the result of the operation may change in
     different environments -- this should be avoided.
#endif

/*
 * STEP 2: Modify the following functions according the coding rules.
 */

/*
 * Return the size of an integer in bytes.
 *
 * ALLOWED:
 *   Pointer operators: *, &
 *   Binary integer operators: -, +, *
 *   Unary integer operators: !
 *   Shorthand operators based on the above: +=, *=, ++, --, etc.
 *
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 *   Binary integer operators: &, &&, |, ||, <, >, <<, >>, ==, !=, ^, /, %
 *   Unary integer operators: ~, -
 */
int intSize() {
  int intArray[10];
  int* intPtr1;
  int* intPtr2;
  // Write code to compute size of an integer.
  // Assign pointer 1 to the beginning of the array and pointer 2
  // to the next element in the array, then casting both pointers to 
  // char* and then subtracting pointer 2 - pointer 1 to get the number
  // of bytes between them (size of the element)
  intPtr1 = intArray;
  intPtr2 = intPtr1 + 1;
  return (char*)(intPtr2) - (char*)(intPtr1);
}

/*
 * Return the size of a double in bytes.
 *
 * ALLOWED:
 *   Pointer operators: *, &
 *   Binary integer operators: -, +, *
 *   Unary integer opeators: !
 *   Shorthand operators based on the above: +=, *=, ++, --, etc.
 *
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 *   Binary integer operators: &, &&, |, ||, <, >, <<, >>, ==, !=, ^, /, %
 *   Unary integer operators: ~, -
 */
int doubleSize() {
  double doubArray[10];
  double* doubPtr1;
  double* doubPtr2;
  // Write code to compute size of a double.
  // Assign pointer 1 to the beginning of the array and pointer 2
  // to the next element in the array, then casting both pointers to
  // char* and then subtracting pointer 2 - pointer 1 to get the number
  // of bytes between them (size of the element) 
  doubPtr1 = doubArray;
  doubPtr2 = doubPtr1 + 1;
  return (char*)(doubPtr2) - (char*)(doubPtr1);
}

/*
 * Return the size of a pointer in bytes.
 *
 * ALLOWED:
 *   Pointer operators: *, &
 *   Binary integer operators: -, +, *
 *   Unary integer operators: !
 *   Shorthand operators based on the above: +=, *=, ++, --, etc.
 *
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 *   Binary integer operators: &, &&, |, ||, <, >, <<, >>, ==, !=, ^, /, %
 *   Unary integer operators: ~, -
 */
int pointerSize() {
  double* ptrArray[10];
  double** ptrPtr1;
  double** ptrPtr2;
  // Write code to compute size of a pointer.
  // Assign pointer 1 to the beginning of the array and pointer 2
  // to the next element in the array, then casting both pointers to
  // char* and then subtracting pointer 2 - pointer 1 to get the number
  // of bytes between them (size of element)
  ptrPtr1 = ptrArray;
  ptrPtr2 = ptrPtr1 + 1;
  return (char*)(ptrPtr2) - (char*)(ptrPtr1);
}

/*
 * Given pointers to two variables, exchange/swap the values of the variables.
 *
 * ALLOWED:
 *   Pointer operators: *, &
 *   Binary integer operators: -, +, *
 *   Unary integer operators: !
 *   Shorthand operators based on the above: +=, *=, ++, --, etc.
 *
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 *   Binary integer operators: &, &&, |, ||, <, >, <<, >>, ==, !=, ^, /, %
 *   Unary integer operators: ~, -
 */
void swapInts(int* ptr1, int* ptr2) {
  // Your code here
  // Create a temp value that stores the dereferenced value of the first pointer
  // then assign the dereferenced value of the first pointer the value that the
  // second pointer points to and assign the temp value to the dereferenced value
  // of the second pointer
  int temp1 = *ptr1;
  *ptr1 = *ptr2;
  *ptr2 = temp1;
}

/*
 * Modify intArray[5] to be the value 351 using only intArray and pointer
 * arithmetic.
 *
 * ALLOWED:
 *   Pointer operators: *, &
 *   Binary integer operators: -, +, *, <<, >>, ==, ^
 *   Unary integer operators: !, ~
 *   Shorthand operators based on the above: +=, *=, ++, --, ~= etc.
 *
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 *   Binary integer operators: &, &&, |, ||, <, >, !=, /, %
 *   Unary integer operators: -
 */
int changeValue() {
  int intArray[10];
  // Create a pointer at the beginnig of the array and then add 5 to get to
  // the sixth element in the array. Then create two int constants that can
  // add up to 351 and assign the sum to the pointer's referenced value
  int* intPtr1 = intArray + 5;
  int x = 151;
  int y = 200;
  *intPtr1 = x + y;
  // Remember not to use constants greater than 255.
  // Remember to use * to dereference. You cannot use '[<index>]' syntax.
  return intArray[5];
}

/*
 * Return 1 if the addresses stored in ptr1 and ptr2 are within the *same*
 * 64-byte aligned block of memory or return 0 otherwise. Check the spec for
 * examples if you are unsure of what this means.
 *
 * Note that loops and the operators / and % are NOT allowed.
 *
 * ALLOWED:
 *   Pointer operators: *, &
 *   Binary integer operators: -, +, *, <<, >>, ==, ^
 *   Unary integer operators: !, ~
 *   Shorthand operators based on the above: <<=, *=, ++, --, etc.
 *
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 *   Binary integer operators: &, &&, |, ||, <, >, !=, /, %
 *   Unary integer operators: -
 */
int withinSameBlock(int* ptr1, int* ptr2) {
  // Your code here
  // Cast both pointer values to long int and use ^ operator to check for
  // differences between the two binary values before right shifting by 6.
  // A difference beyond the least-significant 6 digits means they are not
  // in the same block
  return !(((long int)ptr1 ^ (long int)ptr2)>>6);
}

/*
 * Return 1 if ptr points to a byte within the specified intArray or return 0
 * otherwise. ptr does not have to point to the beginning of an element. Check
 * the spec for examples if you are unsure of what this means. size is the
 * size of intArray in number of ints; assume size != 0.
 *
 * Note that loops and the operators / and % are NOT allowed.
 *
 * ALLOWED:
 *   Pointer operators: *, &
 *   Binary integer operators: -, +, *, <<, >>, ==, ^
 *   Unary integer operators: !, ~
 *   Shorthand operators based on the above: <<=, *=, ++, --, etc.
 *
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 *   Binary integer operators: &, &&, |, ||, <, >, !=, /, %
 *   Unary integer operators: -
 */
int withinArray(int* intArray, int size, int* ptr) {
  // Your code here
  // sign1 stores the MSB of the result from subtracting (char*)ptr -(char*)intArray
  // and sign2 stores the MSB of the result from subtracting the address of the last
  // element in the array and the address the pointer points to (both casted to char*)
  // The pointer points to a byte within the array iff both operations result in the
  // same sign (same MSB)
  int sign1 = ((char*)ptr - (char*)intArray)>>31;
  int sign2 = (((long int) + size * 4) - (long int)ptr)>>31;
  return sign1 == sign2;
}

/*
 * In C, the end of a "string" is indicated by the null terminator ('\0').
 * Given a pointer to the start of a "string," return the string length.
 * The null terminator is not counted as part of the string length.
 *
 * Note that loops ARE allowed.
 *
 * ALLOWED:
 *   Pointer operators: *, &
 *   Binary integer operators: -, +, *, ==, !=, <, >
 *   Unary integer operators: !
 *   Shorthand operators based on the above: <<=, *=, ++, --, etc.
 *   Control constructs: for, while
 *
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 *   Binary integer operators: &, &&, |, ||, <<, >>, ^, /, %
 *   Unary integer operators: ~, -
 */
int stringLength(char* s) {
  // Your code here
  // While s is not pointing to the value 0 (the null terminator)
  // s keeps incrementing by one char and int length keeps track of
  // the number of increments
  int length = 0;
  while (*s != 0){
	s++;
  	length++;
  }
  return length;
}

/*
 * Change the value pointed to by ptr byte-by-byte so that when returned as an
 * integer the value is 351351.
 *
 * Hints: Recall that an int is 4 bytes and how little endian works for
 * multibyte memory storage. We suggest starting by converting 351351 into
 * binary/hexadecimal.
 *
 * ALLOWED:
 *   Pointer operators: *, &
 *   Binary integer operators: -, +, *
 *   Shorthand operators based on the above: +=, *=, ++, --, etc.
 *   Unary integer operators: !
 *
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 *   Binary integer operators: &, &&, |, ||, <, >, <<, >>, ==, !=, ^, /, %
 *   Unary integer operators: ~, -
 */
int endianExperiment(int* ptr) {
  char* bytePtr;
  // Your code here
  // bytePtr increments through 4 bytes one at a time and set them to values
  // such that the 4 bytes combined can be read as 351351 through little-endian
  // storage system
  bytePtr = (char*)ptr;
  *bytePtr = 119;
  bytePtr++;
  *bytePtr = 92;
  bytePtr++;
  *bytePtr = 5;
  bytePtr++;
  *bytePtr = 0;
  return *ptr;
}

/*
 * Selection sort is a sorting algorithm that works by partitioning the array
 * into a sorted section and unsorted section. Then it repeatedly selects the
 * minimum element from the unsorted section and moves it to the end of the
 * sorted section.
 *
 * Pseudo-code for an array (arr) and its length (n) might look like:
 *
 *   for i = 0 to n - 1
 *     minIndex = i
 *     for  j = i + 1 to n - 1
 *       if arr[minIndex] > arr[j]
 *         minIndex = j
 *       end if
 *     end for
 *     Swap(arr[i], arr[minIndex])
 *   end for
 *
 * Note that control constructs and calling swapInts() ARE allowed.
 *
 * ALLOWED:
 *   Pointer operators: *, &
 *   Binary integer operators: -, +, *, ==, !=, <, >
 *   Unary integer operators: !
 *   Shorthand operators based on the above: +=, *=, ++, --, etc.
 *   Control constructs: for, while, if
 *   Function calls: swapInts()
 *
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 *   Binary integer operators: &, &&, |, ||, <<, >>, ^, /
 *   Unary integer operators: ~, -
 */
void selectionSort(int arr[], int arrLength) {
  int i, j, min_index;
  // Your code here
  // i keeps track of the index of the last element in the sorted partition,
  // min_index keeps track of the minimum value in the unsorted partition,
  // and j traverses through the unsorted partition to find the minimum value
  // to swap with the value at i
  for (i = 0; i < arrLength; i++) {
  	min_index = i;
	for (j = i + 1; j < arrLength; j++) {
		if (*(arr + min_index) > *(arr + j)) {
			min_index = j;
		}	
	}
	swapInts(arr + i, arr + min_index);
  }
}
