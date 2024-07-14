#include "hashmap.h"

/**
 * @brief Simple hash function mapping a `size_t` to a `size_t` hash value.
 *
 * This function was taken from the output of ChatGPT as the details of writing
 * a hash function are out of the scope of this exercise.
 */
static size_t simple_hash(size_t input)
{
    // Constants used in the hash function.
    const size_t prime1 = 0x9e3779b97f4a7c15;
    const size_t prime2 = 0xbf58476d1ce4e5b9;

    // Mix the input using the constants and bitwise operations.
    input ^= input >> 30;
    input *= prime1;
    input ^= input >> 27;
    input *= prime2;

    return input;
}

struct bucket_list
{
    hashmap_key_t key;
    hashmap_value_t value;
    struct bucket_list *next;
};

void hashmap_init(struct hashmap *map)
{
    for (size_t index = 0; index < NUM_BUCKETS; ++index)
    {
        map->buckets[index] = NULL;
    }
}

void hashmap_set(struct hashmap *map, hashmap_key_t key, hashmap_value_t value)
{
    size_t hash_value = simple_hash(key);
    size_t bucket_index = hash_value % NUM_BUCKETS;

    struct bucket_list *list = map->buckets[bucket_index];

    // Walk the list to find the matching key.
    struct bucket_list *previous_node = NULL;
    struct bucket_list *current_node = list;
    while (current_node != NULL && current_node->key != key)
    {
        previous_node = current_node;
        current_node = current_node->next;
    }

    // Matching key not found, append to the list.
    if (current_node == NULL)
    {
        struct bucket_list *new_node = malloc(sizeof(struct bucket_list));
        new_node->key = key;
        new_node->value = value;
        new_node->next = NULL;

        // The bucket was NULL, so the new node is the new head.
        if (previous_node == NULL)
        {
            map->buckets[bucket_index] = new_node;
        }
        else
        {
            previous_node->next = new_node;
        }
    }
    // Matching key found, overwrite the value.
    else
    {
        current_node->value = value;
    }
}

bool hashmap_get(const struct hashmap *map, hashmap_key_t key, hashmap_value_t *out_value)
{
    size_t hash_value = simple_hash(key);
    size_t bucket_index = hash_value % NUM_BUCKETS;

    struct bucket_list *list = map->buckets[bucket_index];

    // Walk the list to find the matching key.
    struct bucket_list *current_node = list;
    while (current_node != NULL && current_node->key != key)
    {
        current_node = current_node->next;
    }

    if (current_node == NULL)
    {
        return false;
    }
    *out_value = current_node->value;
    return true;
}

void hashmap_deinit(struct hashmap *map)
{
    for (size_t index = 0; index < NUM_BUCKETS; ++index)
    {
        if (map->buckets[index] != NULL)
        {
            free(map->buckets[index]);
        }
    }
}
