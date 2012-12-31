#include<string.h>
#include<stdlib.h>

#define HASH_SIZE 101

struct hash_node {
    struct hash_node *next;
    int  key;
    void *value;
};

struct hashtable {
    struct hash_node **table; /* array of pointers*/
    int size;                 /* size of table */
};

struct hashtable * hashtable_new(int);
int                hashtable_insert(struct hashtable *, int key, void *value);
void *             hashtable_lookup(struct hashtable *, int key);
int                hashtable_remove(struct hashtable *, int key);
int                hashtable_foreach(struct hashtable *ht, 
                          	int (*fn)(int key, void *value, void *data), 
                            	void *data);
