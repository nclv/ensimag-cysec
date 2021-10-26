
#ifndef __HASH_TABLE___
#define __HASH_TABLE___

#include "uthash.h" // https://troydhanson.github.io/uthash/

typedef struct hash_msg {
    uint64_t h;  // hash table key
    uint32_t m[4];  // message
    UT_hash_handle hh; /* makes this structure hashable */
} hash_msg;

hash_msg *new_hash_entry(uint64_t h, uint32_t m[4]);
void delete_all(hash_msg *hash_table);
void print_hash_msg(hash_msg *hash_table_entry);

#endif
