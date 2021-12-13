#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "smap.h"

typedef struct entry entry;

struct entry
{
    char* key;
    void* value;
    entry* next;
};

typedef struct bucket
{
    int size;
    entry* entries;
} bucket;

struct smap
{
    int size;
    int buckets_occupied;
    int cap;
    bucket* table;
    int (*hash)(const char* s);
};

#define SMAP_INITIAL_CAPACITY 50
bool smap_embiggen(smap* m);
void bucket_destroy(bucket* b);

/*
* djb2 by Dan Bernstein
* taken from: http://www.cse.yorku.ca/~oz/hash.html
*/
int smap_default_hash(const char *s)
{
   int hash = 5381;
   int c;

    while (c = *s++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

smap *smap_create(int (*h)(const char *s))
{
    if (h != NULL)
    {
       smap* m = malloc(sizeof(smap));
       if (m == NULL)
       {
           free(m);
           return NULL;
       }

       m->size = 0;
       m->buckets_occupied = 0;
       m->hash = h;

       m->table = malloc(sizeof(bucket) * SMAP_INITIAL_CAPACITY);
       if(m->table == NULL)
       {
           free(m->table);
           free(m);
           return NULL;
       }
       
       m->cap = SMAP_INITIAL_CAPACITY;

       for(int i = 0; i < SMAP_INITIAL_CAPACITY; i++)
       {
           m->table[i].size = 0;
           m->table[i].entries = NULL;
       }

       return m;

    }
    else return NULL;
}

int smap_size(const smap *m)
{
    if(m != NULL)
    {
        return m->size;
    }
    else return 0;
}

bool smap_put(smap *m, const char *key, void *value)
{
    if(m!=NULL && key != NULL)
    {
        if(m->buckets_occupied > (.75 * m->cap)) //resize array if necessary
        {
            smap_embiggen(m);
        }

        int index = abs(m->hash(key) % m->cap);
        entry* iterator = NULL;
        
        if(smap_contains_key(m, key)) //update existing value
        {
            iterator = m->table[index].entries;
            for(int i = 0; i < m->table[index].size; i++)
            {
                if(strcmp(key, iterator->key) == 0)
                {
                    iterator->value = value;
                    return true;  
                }
                else iterator = iterator->next;
            }
        }
        else //add entry to front of linked list
        {
            iterator = m->table[index].entries;

            m->table[index].entries = malloc(sizeof(entry));

            if (m->table[index].entries != NULL)
            {
                m->table[index].entries->key = malloc(sizeof(char)* (strlen(key) + 1)); 
                if(m->table[index].entries->key == NULL)
                {
                    free(m->table[index].entries);
                    free(m->table[index].entries->key);
                    return false;
                }

                strcpy(m->table[index].entries->key, key);
                m->table[index].entries->value = value;

                m->table[index].entries->next = iterator;

                if(m->table[index].size == 0)
                {
                    m->buckets_occupied++;
                }

                m->table[index].size++;
                m->size++;

                return true;
            }
            else
            {
                free(m->table[index].entries);
                m->table[index].entries = iterator;
            }
        }
    }
    
    return false;
}

bool smap_contains_key(const smap *m, const char *key)
{
    if(m!=NULL && key!= NULL)
    {
        int index = abs(m->hash(key) % m->cap);

        entry* iterator = m->table[index].entries;

        for(int i = 0; i < m->table[index].size; i++)
        {
            if(strcmp(key, iterator->key) == 0)
            {
                return true;   
            }
            else iterator = iterator->next;
        }
    }

    return false;

}

void *smap_get(smap *m, const char *key)
{
    if(m!=NULL && key != NULL)
    {
        int index = abs(m->hash(key) % m->cap);
        entry* iterator = m->table[index].entries;

        for(int i = 0; i < m->table[index].size; i++)
        {
            if(strcmp(key, iterator->key) == 0)
            {
                return iterator->value;   
            }
            else iterator = iterator->next;
        }

    }

    return NULL;
}

void *smap_remove(smap *m, const char *key)
{
    if(m != NULL && key != NULL)
    {
        int index = abs(m->hash(key) % m->cap);

        entry* iterator = m->table[index].entries;
        entry* prev = m->table[index].entries;
        
        for(int i = 0; i < m->table[index].size; i++)
        {
            if(strcmp(key, iterator->key) == 0)
            {
                void* value = iterator->value;
                
                if(i == 0)
                {
                    m->table[index].entries = iterator->next;
                }
                else
                {
                    prev->next = iterator->next;
                }  

                free(iterator->key);
                free(iterator);

                m->table[index].size--;
                m->size--;
                return value; 
            }

            prev = iterator;
            iterator = iterator->next;
        }
    }

    return NULL;
}

void smap_for_each(smap *m, void (*f)(const char *, void *, void *), void *arg)
{
    entry* iterator;

    if(m!= NULL && f!= NULL)
    {
        for(int i = 0; i < m->cap; i++)
        {
            iterator = m->table[i].entries;
            for(int j = 0; j < m->table[i].size; j++)
            {
                f(iterator->key, iterator->value, arg);
                iterator = iterator->next;
            }
        }
    }
}

const char **smap_keys(smap *m)
{
    if (m!=NULL && m->size > 0)
    {
        const char **smap_keys = malloc(sizeof(char*) * m->size);
        if(smap_keys != NULL)
        {
            int array_index = 0;
            entry* iterator;

            for(int i = 0; i < m->cap; i++)
            {
                iterator = m->table[i].entries;

                for(int j = 0; j < m->table[i].size; j++)
                {
                    smap_keys[array_index] = iterator->key;

                    iterator = iterator->next;
                    array_index++;
                }
            }

            return smap_keys;
        }
    }
    
    return NULL;
}


void smap_destroy(smap *m)
{
    if (m!=NULL)
    {
        if(m->table!=NULL)
        {
            for(int i = 0; i < m->cap; i++)
            {
                if(m->table[i].size > 0)
                {
                    bucket_destroy(&m->table[i]);
                }
            }

            free(m->table);
        }

        free(m);
    }
}

/*
* Resizes (x2) and rehashes a map, returns false if there
* was a memory allocation error
*/
bool smap_embiggen(smap *m)
{
    if (m!=NULL)
    {
        int new_index = 0;
        int new_buckets_occupied = 0;
        int new_cap = 2 * m->cap;
        entry* iterator = NULL;
        
        bucket* new_table = malloc(sizeof(bucket) * new_cap);
        
        if(new_table != NULL) //initialize new table
        {
            for(int i = 0; i < new_cap; i++)
            {
                new_table[i].size = 0;
                new_table[i].entries = NULL;
            }
        }
        else return false;



        for(int i = 0; i < m->cap; i++) //populate new table, free old table
        {
            if(m->table[i].size > 0)
            {
                iterator = m->table[i].entries;
                
                for(int j = 0; j < m->table[i].size; j++) //copy each value
                {
                    new_index = abs(m->hash(iterator->key) % new_cap);
                    
                    entry* temp = new_table[new_index].entries;
                    new_table[new_index].entries = malloc(sizeof(entry));

                    if(new_table[new_index].entries == NULL)
                    {
                        return false;
                    }
                    
                    //check if null
                    new_table[new_index].entries->key = malloc(sizeof(char) * (strlen(iterator->key)+1));
                    strcpy(new_table[new_index].entries->key, iterator->key);

                    new_table[new_index].entries->value = iterator->value;
                    new_table[new_index].entries->next = temp;

                    if(new_table[new_index].size == 0)
                    {
                        new_buckets_occupied++;
                    }
                    new_table[new_index].size++;

                    iterator = iterator->next;
                }
            }

            bucket_destroy(&m->table[i]);
        }

        free(m->table);
        m->table = new_table;
        m->cap = new_cap;
        m->buckets_occupied = new_buckets_occupied;

        return true;
    }
    else return false;
}

/*
* Frees all entries in a given bucket, depending on mode
* if mode is 0, does not delete key value pairs
* if mode is 1, does free key value pairs
*
* I implemented the mode part because i wasn't sure if we
* were responsible for freeing key/value pairs, so I wanted to be
* able to chage it quickly.
*/
void bucket_destroy(bucket* b)
{
    if(b!=NULL)
    {
        entry* iterator = b->entries;
        
        for(int i = 0; i < b->size; i++)
        {
            b->entries = b->entries->next;

            iterator->next = NULL;

            if(iterator->key != NULL)
            {
                free(iterator->key);
            }
            free(iterator);
            iterator = b->entries;
        }
        b->size = 0;
    }
}