/*-------------------------------------------------------------------
 *  UW CSE 351 Lab 5 Starter code:
 *        single doubly-linked free block list with LIFO policy
 *        with support for coalescing adjacent free blocks
 *
 * Terminology:
 * o We will implement an explicit free list allocator.
 * o We use "next" and "previous" to refer to blocks as ordered in
 *   the free list.
 * o We use "following" and "preceding" to refer to adjacent blocks
 *   in memory.
 *-------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "memlib.h"
#include "mm.h"

/* Static functions for unscaled pointer arithmetic to keep other code cleaner.
   Casting to a char* has the effect that pointer arithmetic happens at
   the byte granularity (i.e. POINTER_ADD(0x1, 1) would be 0x2).  (By
   default, incrementing a pointer in C has the effect of incrementing
   it by the size of the type to which it points (e.g. block_info).)
   We cast the result to void* to force you to cast back to the
   appropriate type and ensure you don't accidentally use the resulting
   pointer as a char* implicitly. The first argument to these functions
   is of type void* to enable you to pass in any type of pointer into the function
*/
static inline void* UNSCALED_POINTER_ADD(void* p, int x) { return ((void*)((char*)(p) + (x))); }
static inline void* UNSCALED_POINTER_SUB(void* p, int x) { return ((void*)((char*)(p) - (x))); }


/******** FREE LIST IMPLEMENTATION ***********************************/


/* A block_info contains information about a block, including the size
   and usage tags, as well as pointers to the next and previous blocks
   in the free list.  This is exactly the "explicit free list" structure
   illustrated in the lecture slides.

   Note that the next and prev pointers and the boundary tag are only
   needed when the block is free.  To achieve better utilization, mm_malloc
   should use the space for next and prev as part of the space it returns.

   +--------------+
   | size_and_tags|  <-  block_info pointers in free list point here
   |   (header)   |
   +--------------+
   |     next     |  <-  Pointers returned by mm_malloc point here
   +--------------+
   |     prev     |
   +--------------+
   |  space and   |
   |   padding    |
   |     ...      |
   |     ...      |
   +--------------+
   | boundary tag |
   |   (footer)   |
   +--------------+
*/
struct block_info {
    // Size of the block (in the high bits) and tags for whether the
    // block and its predecessor in memory are in use.  See the SIZE()
    // and TAG macros, below, for more details.
    size_t size_and_tags;
    // Pointer to the next block in the free list.
    struct block_info* next;
    // Pointer to the previous block in the free list.
    struct block_info* prev;
};
typedef struct block_info block_info;

/* Pointer to the first block_info in the free list, the list's head.

   A pointer to the head of the free list in this implementation is
   always stored in the first word in the heap.  mem_heap_lo() returns
   a pointer to the first word in the heap, so we cast the result of
   mem_heap_lo() to a block_info** (a pointer to a pointer to
   block_info) and dereference this to get a pointer to the first
   block_info in the free list. 
*/
#define FREE_LIST_HEAD *((block_info **)mem_heap_lo())

/* Size of a word on this architecture. */
#define WORD_SIZE sizeof(void*)

/* Minimum block size (to account for size header, next ptr, prev ptr,
   and boundary tag) */
#define MIN_BLOCK_SIZE (sizeof(block_info) + WORD_SIZE)

/* Alignment of blocks returned by mm_malloc. */
#define ALIGNMENT 8

/* SIZE(block_info->size_and_tags) extracts the size of a 'size_and_tags' field.
   Also, calling SIZE(size) selects just the higher bits of 'size' to ensure
   that 'size' is properly aligned.  We align 'size' so we can use the low
   bits of the size_and_tags field to tag a block as free/used, etc, like this:

      size_and_tags:
      +-------------------------------------------+
      | 63 | 62 | 61 | 60 |  . . . .  | 2 | 1 | 0 |
      +-------------------------------------------+
        ^                                       ^
      high bit                               low bit

   Since ALIGNMENT == 8, we can utilize the low 3 bits of size_and_tags for tag
   bits, and we use bits 3-63 to store the size.

   Bit 0 (2^0 == 1): TAG_USED
   Bit 1 (2^1 == 2): TAG_PRECEDING_USED
*/

static inline size_t SIZE(size_t x) { return ((x) & ~(ALIGNMENT - 1)); }

/* TAG_USED is the bit mask used in size_and_tags to mark a block as used. */
#define TAG_USED 1

/* TAG_PRECEDING_USED is the bit mask used in size_and_tags to indicate
   that the block preceding it in memory is used. (used in turn for
   coalescing).  If the previous block is not used, we can learn the size
   of the previous block from its boundary tag */
#define TAG_PRECEDING_USED 2

/* Print the heap by iterating through it as an implicit free list. */
static void examine_heap() {
  block_info* block;

  /* print to stderr so output isn't buffered and not output if we crash */
  fprintf(stderr, "FREE_LIST_HEAD: %p\n", (void*) FREE_LIST_HEAD);

  for (block = (block_info*) UNSCALED_POINTER_ADD(mem_heap_lo(), WORD_SIZE); /* first block on heap */
       SIZE(block->size_and_tags) != 0 && block < (block_info*) mem_heap_hi();
       block = (block_info*) UNSCALED_POINTER_ADD(block, SIZE(block->size_and_tags))) {

    /* print out common block attributes */
    fprintf(stderr, "%p: %ld %ld %ld\t",
            (void*) block,
            SIZE(block->size_and_tags),
            block->size_and_tags & TAG_PRECEDING_USED,
            block->size_and_tags & TAG_USED);

    /* and allocated/free specific data */
    if (block->size_and_tags & TAG_USED) {
      fprintf(stderr, "ALLOCATED\n");
    } else {
      fprintf(stderr, "FREE\tnext: %p, prev: %p\n",
              (void*) block->next,
              (void*) block->prev);
    }
  }
  fprintf(stderr, "END OF HEAP\n\n");
}

/* Find a free block of the requested size in the free list.  Returns
   NULL if no free block is large enough. */
static block_info* search_free_list(size_t req_size) {
  block_info* free_block;

  free_block = FREE_LIST_HEAD;
  while (free_block != NULL) {
    if (SIZE(free_block->size_and_tags) >= req_size) {
      return free_block;
    } else {
      free_block = free_block->next;
    }
  }
  return NULL;
}

/* Insert free_block at the head of the list (LIFO). */
static void insert_free_block(block_info* free_block) {
  block_info* old_head = FREE_LIST_HEAD;
  free_block->next = old_head;
  if (old_head != NULL) {
    old_head->prev = free_block;
  }
  free_block->prev = NULL;
  FREE_LIST_HEAD = free_block;
}

/* Remove a free block from the free list. */
static void remove_free_block(block_info* free_block) {
  block_info* next_free;
  block_info* prev_free;

  next_free = free_block->next;
  prev_free = free_block->prev;

  // If the next block is not null, patch its prev pointer.
  if (next_free != NULL) {
    next_free->prev = prev_free;
  }

  // If we're removing the head of the free list, set the head to be
  // the next block, otherwise patch the previous block's next pointer.
  if (free_block == FREE_LIST_HEAD) {
    FREE_LIST_HEAD = next_free;
  } else {
    prev_free->next = next_free;
  }
}

/* Coalesce 'old_block' with any preceeding or following free blocks. */
static void coalesce_free_block(block_info* old_block) {
  block_info* block_cursor;
  block_info* new_block;
  block_info* free_block;
  // size of old block
  size_t old_size = SIZE(old_block->size_and_tags);
  // running sum to be size of final coalesced block
  size_t new_size = old_size;

  // Coalesce with any preceding free block
  block_cursor = old_block;
  while ((block_cursor->size_and_tags & TAG_PRECEDING_USED) == 0) {
    // While the block preceding this one in memory (not the
    // prev. block in the free list) is free:

    // Get the size of the previous block from its boundary tag.
    size_t size = SIZE(*((size_t*) UNSCALED_POINTER_SUB(block_cursor, WORD_SIZE)));
    // Use this size to find the block info for that block.
    free_block = (block_info*) UNSCALED_POINTER_SUB(block_cursor, size);
    // Remove that block from free list.
    remove_free_block(free_block);

    // Count that block's size and update the current block pointer.
    new_size += size;
    block_cursor = free_block;
  }
  new_block = block_cursor;

  // Coalesce with any following free block.
  // Start with the block following this one in memory
  block_cursor = (block_info*) UNSCALED_POINTER_ADD(old_block, old_size);
  while ((block_cursor->size_and_tags & TAG_USED) == 0) {
    // While following block is free:

    size_t size = SIZE(block_cursor->size_and_tags);
    // Remove it from the free list.
    remove_free_block(block_cursor);
    // Count its size and step to the following block.
    new_size += size;
    block_cursor = (block_info*) UNSCALED_POINTER_ADD(block_cursor, size);
  }

  // If the block actually grew, remove the old entry from the free
  // list and add the new entry.
  if (new_size != old_size) {
    // Remove the original block from the free list
    remove_free_block(old_block);

    // Save the new size in the block info and in the boundary tag
    // and tag it to show the preceding block is used (otherwise, it
    // would have become part of this one!).
    new_block->size_and_tags = new_size | TAG_PRECEDING_USED;
    // The boundary tag of the preceding block is the word immediately
    // preceding block in memory where we left off advancing block_cursor.
    *(size_t*) UNSCALED_POINTER_SUB(block_cursor, WORD_SIZE) = new_size | TAG_PRECEDING_USED;

    // Put the new block in the free list.
    insert_free_block(new_block);
  }
  return;
}

/* Get more heap space of size at least req_size. */
static void request_more_space(size_t req_size) {
  size_t pagesize = mem_pagesize();
  size_t num_pages = (req_size + pagesize - 1) / pagesize;
  block_info* new_block;
  size_t total_size = num_pages * pagesize;
  size_t prev_last_word_mask;

  void* mem_sbrk_result = mem_sbrk(total_size);
  if ((size_t) mem_sbrk_result == -1) {
    printf("ERROR: mem_sbrk failed in request_more_space\n");
    exit(0);
  }
  new_block = (block_info*) UNSCALED_POINTER_SUB(mem_sbrk_result, WORD_SIZE);

  /* initialize header, inherit TAG_PRECEDING_USED status from the
     previously useless last word however, reset the fake TAG_USED
     bit */
  prev_last_word_mask = new_block->size_and_tags & TAG_PRECEDING_USED;
  new_block->size_and_tags = total_size | prev_last_word_mask;
  // Initialize boundary tag.
  ((block_info*) UNSCALED_POINTER_ADD(new_block, total_size - WORD_SIZE))->size_and_tags =
          total_size | prev_last_word_mask;

  /* initialize "new" useless last word
     the previous block is free at this moment
     but this word is useless, so its use bit is set
     This trick lets us do the "normal" check even at the end of
     the heap and avoid a special check to see if the following
     block is the end of the heap... */
  *((size_t*) UNSCALED_POINTER_ADD(new_block, total_size)) = TAG_USED;

  // Add the new block to the free list and immediately coalesce newly
  // allocated memory space
  insert_free_block(new_block);
  coalesce_free_block(new_block);
}


/* Initialize the allocator. */
int mm_init() {
  // Head of the free list.
  block_info* first_free_block;

  // Initial heap size: WORD_SIZE byte heap-header (stores pointer to head
  // of free list), MIN_BLOCK_SIZE bytes of space, WORD_SIZE byte heap-footer.
  size_t init_size = WORD_SIZE + MIN_BLOCK_SIZE + WORD_SIZE;
  size_t total_size;

  void* mem_sbrk_result = mem_sbrk(init_size);
  //  printf("mem_sbrk returned %p\n", mem_sbrk_result);
  if ((ssize_t) mem_sbrk_result == -1) {
    printf("ERROR: mem_sbrk failed in mm_init, returning %p\n",
           mem_sbrk_result);
    exit(1);
  }

  first_free_block = (block_info*) UNSCALED_POINTER_ADD(mem_heap_lo(), WORD_SIZE);

  /* Total usable size is full size minus heap-header and heap-footer words
     NOTE: These are different than the "header" and "footer" of a block!
     The heap-header is a pointer to the first free block in the free list.
     The heap-footer is used to keep the data structures consistent (see
     request_more_space() for more info, but you should be able to ignore it). */
  total_size = init_size - WORD_SIZE - WORD_SIZE;

  // The heap starts with one free block, which we initialize now.
  first_free_block->size_and_tags = total_size | TAG_PRECEDING_USED;
  first_free_block->next = NULL;
  first_free_block->prev = NULL;
  // boundary tag
  *((size_t*) UNSCALED_POINTER_ADD(first_free_block, total_size - WORD_SIZE)) = 
	  total_size | TAG_PRECEDING_USED;

  // Tag "useless" word at end of heap as used.
  // This is the heap-footer.
  *((size_t*) UNSCALED_POINTER_SUB(mem_heap_hi(), WORD_SIZE - 1)) = TAG_USED;

  // set the head of the free list to this new free block.
  FREE_LIST_HEAD = first_free_block;
  return 0;
}


// TOP-LEVEL ALLOCATOR INTERFACE ------------------------------------

/* Allocate a block of size size and return a pointer to it. If size is zero,
 * returns null.
 */
void* mm_malloc(size_t size) {
  size_t req_size;
  block_info* ptr_free_block = NULL;
  block_info* ptr_split_block = NULL;
  size_t block_size;
  size_t preceding_use_tag;

  // Zero-size requests get NULL.
  if (size == 0) {
    return NULL;
  }

  // Add one word for the initial size header.
  // Note that we don't need to boundary tag when the block is used!
  size += WORD_SIZE;
  if (size <= MIN_BLOCK_SIZE) {
    // Make sure we allocate enough space for a block_info in case we
    // free this block (when we free this block, we'll need to use the
    // next pointer, the prev pointer, and the boundary tag).
    req_size = MIN_BLOCK_SIZE;
  } else {
    // Round up for correct alignment
    req_size = ALIGNMENT * ((size + ALIGNMENT - 1) / ALIGNMENT);
  }

  // TODO: Implement mm_malloc.  You can change or remove any of the
  // above code.  It is included as a suggestion of where to start.
  // You will want to replace this return statement...

  // searches free list for a free block of at least req_size,
  // if no fitting free block is found, request more heap space
  ptr_free_block = search_free_list(req_size);
  if (ptr_free_block == NULL) {
    request_more_space(req_size);
    ptr_free_block = search_free_list(req_size);
  }

  // get the preceding use tag and the size of the free block
  preceding_use_tag = ptr_free_block->size_and_tags & TAG_PRECEDING_USED;
  block_size = SIZE(ptr_free_block->size_and_tags);

  // if block_size is big enough that it is greater than req_size + MIN_BLOCK_SIZE,
  // we can split the free block
  if (block_size - req_size >= MIN_BLOCK_SIZE) {
    // split free block and create new size_and_tags for both blocks
    ptr_free_block->size_and_tags = req_size | preceding_use_tag | TAG_USED;
    block_size = block_size - req_size;
    ptr_split_block = (block_info*) UNSCALED_POINTER_ADD(ptr_free_block, req_size);
    ptr_split_block->size_and_tags = block_size | TAG_PRECEDING_USED;

    // create footer for split block
    *((size_t*) UNSCALED_POINTER_ADD(ptr_split_block, block_size - WORD_SIZE))
      = ptr_split_block->size_and_tags;

    insert_free_block(ptr_split_block);
  } else if (UNSCALED_POINTER_ADD(ptr_free_block, block_size) < mem_heap_hi()) {
    // no splitting, we just need to set following block preceeding tag to 1,
    // if there is one
    ptr_free_block->size_and_tags |= TAG_USED;
    *((size_t*) UNSCALED_POINTER_ADD(ptr_free_block, block_size))
      |= TAG_PRECEDING_USED;
  }

  remove_free_block(ptr_free_block);

  if (ptr_free_block == NULL) {
    return NULL;
  } else {
    return (void*) UNSCALED_POINTER_ADD(ptr_free_block, WORD_SIZE);
  }
}

/* Free the block referenced by ptr. */
void mm_free(void* ptr) {
  size_t payload_size;
  block_info* block_to_free;
  block_info* following_block;

  // TODO: Implement mm_free.  You can change or remove the declaraions
  // above.  They are included as minor hints.

  // get pointer and payload size of block and set use tag to 1 
  block_to_free = (block_info*) UNSCALED_POINTER_SUB(ptr, WORD_SIZE);
  block_to_free->size_and_tags ^= TAG_USED;
  payload_size = (SIZE(block_to_free->size_and_tags) - WORD_SIZE);

  // if there is a following block, set its preceding use tag to 0
  if (UNSCALED_POINTER_ADD(ptr, payload_size) < mem_heap_hi()) {
    following_block = (block_info*) UNSCALED_POINTER_ADD(ptr, payload_size);
    following_block->size_and_tags ^= TAG_PRECEDING_USED;
  }

  // set footer of the block to be freed then insert and coalesce
  *((size_t*) UNSCALED_POINTER_ADD(block_to_free, payload_size))
    = block_to_free->size_and_tags;
  insert_free_block(block_to_free);
  coalesce_free_block(block_to_free);
}

// TODO: Implement a heap consistency checker as needed. This
// is an optional, but recommended, function to implement and 
// may help debug potential issues with your allocator
int mm_check() {
  return 0;
}
