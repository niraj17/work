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

int hash(struct hashtable *ht, int key)
{
    return (key % (ht->size));
}

struct hashtable * hashtable_new(int size) 
{
    struct hashtable *ht = malloc(sizeof(struct hashtable));
    if(size) ht->size = size; 
    else ht->size = HASH_SIZE;

    ht->table = malloc(ht->size * (sizeof(struct hash_node *)));
    memset(ht->table, 0, ht->size * (sizeof(struct hash_node *)));

    return ht;
}
void * hashtable_lookup(struct hashtable *ht, int key)
{
    int index = hash(ht,key);
    struct hash_node *tmp = NULL;

    tmp = ht->table[index];

    while(tmp) {
	if (tmp->key == key) return tmp->value;
	tmp = tmp->next;
    }
    return NULL;
}
int hashtable_insert(struct hashtable *ht, int key, void *v)
{
    int index = hash(ht,key);
    struct hash_node *prev= NULL,*tmp = NULL;
    struct hash_node  *n = NULL;

    
    tmp = ht->table[index];

    if (tmp == NULL) {

        n = malloc(sizeof(struct hash_node));
        n->value = v;
        n->key   = key;
        n->next  = NULL;

	ht->table[index] = n;
	return 0;
    }

    while(tmp) {
	if (tmp->key == key) {
	    /* TODO: free previous value */
	    tmp->value = v;
	    return 0;
	}
        prev = tmp;
	tmp = tmp->next;
    }

    /* this key is not there in the table*/
    n = malloc(sizeof(struct hash_node));
    n->value = v;
    n->key   = key;
    n->next  = NULL;

    prev->next = n;
    return 0;
}
int  hashtable_remove(struct hashtable *ht, int key)
{
    int index = hash(ht,key);
    struct hash_node *tmp = NULL, *prev = NULL;

    tmp = ht->table[index];

    if(tmp && (tmp->key == key)) {
        ht->table[index] = tmp->next;
	free(tmp);
	return 0;
    }

    while(tmp) {
	if (tmp->key == key) {
	    prev->next = tmp->next;
	    free(tmp);
	    return 0;  /* success*/
	}
        prev = tmp;
	tmp = tmp->next;
    }
    return -1;  /* failure*/
}
int  hashtable_foreach(struct hashtable *ht, 
	int (*fn)(int key, void *value, void *data), 
	void *data)
{
    int index = 0;
    struct hash_node *tmp = NULL;

    for (index=0; index < ht->size; index++)
    {
        tmp = ht->table[index];
	while (tmp) {
            (*fn)(tmp->key, tmp->value, data);
	    tmp = tmp->next;
	}
    }
    return 0;
}
