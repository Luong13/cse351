/*
 * Luong Dinh, 2066624
CSE 351 Lab 4, Part 1 - Mystery Caches
*/

#include <stdlib.h>
#include <stdio.h>

#include "support/mystery-cache.h"

/*
 * NOTES: 
 * 1. When using access_cache() you do not need to provide a "real" memory
 * addresses. You can use any convenient integer value as a memory address,
 * you should not be able to cause a segmentation fault by providing a memory
 * address out of your programs address space as the argument to access_cache.
 * 
 * 2. Do NOT change the provided main function, especially the print statement.
 * If you do so, the autograder may fail to grade your code even if it produces
 * the correct result.
 */

/*
   Returns the size (in B) of each block in the cache.
*/
int get_block_size(void) {
  /* YOUR CODE GOES HERE */
  flush_cache();
  access_cache(0);
  int count = 0;
  while(access_cache(count)){
	  count++;
  }
  return count;
}

/*
   Returns the size (in B) of the cache.
*/
int get_cache_size(int block_size) {
  /* YOUR CODE GOES HERE */
  flush_cache();
  access_cache(0);
  int size = block_size;
  while(access_cache(0)){
	  for(int i = 0; i < size; i += block_size){
		  access_cache(i);
	  }
	  size = size * 2;
  }
  return size / 4;
}

/*
   Returns the associativity of the cache.
*/
int get_cache_assoc(int cache_size) {
  /* YOUR CODE GOES HERE */
  flush_cache();
  access_cache(0);
  int assoc = 1;
  while(access_cache(0)){
	  for(int i = 0; i < assoc; i++){
		  access_cache(i*cache_size);
	  }
	  assoc++;
  }
  return assoc - 2;
}

int main(void) {
  int size;
  int assoc;
  int block_size;

  cache_init(0, 0);

  block_size = get_block_size();
  size = get_cache_size(block_size);
  assoc = get_cache_assoc(size);

  printf("Cache block size: %d bytes\n", block_size);
  printf("Cache size: %d bytes\n", size);
  printf("Cache associativity: %d\n", assoc);

  return EXIT_SUCCESS;
}
