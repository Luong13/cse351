// Written by Porter Jones (pbjones@cs.washington.edu)
//
// This is a file for managing a store of various aisles,
// represented by an array of 64-bit integers

#include <stddef.h>  // To be able to use NULL
#include "aisle_manager.h"
#include "store_client.h"
#include "store_util.h"

// Number of aisles in the store
#define NUM_AISLES 10

// Number of sections per aisle
#define SECTIONS_PER_AISLE 4

// Number of items in the stockroom (2^6 different id combinations)
#define NUM_ITEMS 64

// Global array of aisles in this store. Each unsigned long in the array
// represents one aisle.
unsigned long aisles[NUM_AISLES];

// Array used to stock items that can be used for later. The index of the array
// corresponds to the item id and the value at an index indicates how many of
// that particular item are in the stockroom.
int stockroom[NUM_ITEMS];


/* Starting from the first aisle, refill as many sections as possible using
 * items from the stockroom. A section can only be filled with items that match
 * the section's item id. Prioritizes and fills sections with lower addresses
 * first. Sections with lower addresses should be fully filled (if possible)
 * before moving onto the next section.
 */
void refill_from_stockroom() {
  // TODO: implement this function
  unsigned short item_id = 0;
  unsigned short empty_spaces = 0;
  for (int i = 0; i < NUM_AISLES; i++) {
	  for (int j = 0; j < SECTIONS_PER_AISLE; j++) {
		  item_id = get_id(aisles + i, j);
		  empty_spaces = (unsigned short)(10 - num_items(aisles + i, j));
		  add_items(aisles + i, j, stockroom[item_id]);
		  if (stockroom[item_id] > empty_spaces) {
			  stockroom[item_id] = stockroom[item_id] - empty_spaces;
		  } else {
			  stockroom[item_id] = 0;

		  }
	  }
  }
}

/* Remove at most num items from sections with the given item id, starting with
 * sections with lower addresses, and return the total number of items removed.
 * Multiple sections can store items of the same item id. If there are not
 * enough items with the given item id in the aisles, first remove all the
 * items from the aisles possible and then use items in the stockroom of the
 * given item id to finish fulfilling an order. If the stockroom runs out of
 * items, you should remove as many items as possible.
 */
int fulfill_order(unsigned short id, int num) {
  // TODO: implement this function
  int items_removed = 0;
  int avail_items = 0;
  for (int i = 0; i < NUM_AISLES; i++) {
	  for (int j = 0; j < SECTIONS_PER_AISLE; j++) {
		  if (get_id(aisles+i, j) == id && items_removed < num) {
			  avail_items = (int)num_items(aisles + i, j);
			  remove_items(aisles + i, j, num - items_removed);
			  if (items_removed + avail_items <= num) {
				  items_removed += avail_items;
			  } else {
				  items_removed = num;
			  }
		  }
	  }
  }
  if (items_removed < num) {
	 if (stockroom[id] > (num - items_removed)) {
		 stockroom[id] = stockroom[id] - (num - items_removed);
		 items_removed = num;
	 } else {
		 items_removed += stockroom[id];
		 stockroom[id] = 0;
	 }
  }
  return items_removed;
}

/* Return a pointer to the first section in the aisles with the given item id
 * that has no items in it or NULL if no such section exists. Only consider
 * items stored in sections in the aisles (i.e., ignore anything in the
 * stockroom). Break ties by returning the section with the lowest address.
 */
unsigned short* empty_section_with_id(unsigned short id) {
  // TODO: implement this function
  unsigned short* empty_section = NULL;
  for (int i = 0; i < NUM_AISLES; i++) {
	  for (int j = 0; j < SECTIONS_PER_AISLE; j++) {
		  if (get_id(aisles + i, j) == id && num_items(aisles + i, j) == 0) {
			  empty_section = (unsigned short*)(aisles + i) + j;
			  return empty_section;
		  }
	  }
  }
  return empty_section;
}

/* Return a pointer to the section with the most items in the store. Only
 * consider items stored in sections in the aisles (i.e., ignore anything in
 * the stockroom). Break ties by returning the section with the lowest address.
 */
unsigned short* section_with_most_items() {
  // TODO: implement this function
  int max_items = 0;
  unsigned short* most_full_section = (unsigned short*)aisles;
  for (int i = 0; i < NUM_AISLES; i++) {
	  for (int j = 0; j < SECTIONS_PER_AISLE; j++) {
		  if (num_items(aisles + i, j) > max_items) {
			  max_items = num_items(aisles + i, j);
			  most_full_section = (unsigned short*)(aisles + i) + j;
		  }
	  }
  }
  return most_full_section;
}
