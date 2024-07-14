#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>
#include <stdlib.h>

#define NUM_BUCKETS 10

typedef size_t hashmap_key_t;
typedef size_t hashmap_value_t;

struct bucket_list;

struct hashmap
{
    struct bucket_list *buckets[NUM_BUCKETS];
};

void hashmap_init(struct hashmap *map);

void hashmap_set(struct hashmap *map, hashmap_key_t key, hashmap_value_t value);

bool hashmap_get(const struct hashmap *map, hashmap_key_t key, hashmap_value_t *out_value);

void hashmap_deinit(struct hashmap *map);

#endif // HASHMAP_H
