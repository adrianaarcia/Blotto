#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "smap.h"
#include "entry.h"

typedef struct scoreboard
{
    int* distribution;
    int games_played;
    float wins;
    float total_score; 
    float average;//either wins or score
} scoreboard;

void free_values(scoreboard** values, int size); //frees values
void play(smap* m, FILE* matchups, int bf_count, int* battlefields); //uses matchup file to score games
void average(const char *key, void *value, void *mode);

int main(int argc, char* argv[])
{
   
    //Step 1: check to make sure arguments are valid
    if(argc < 4)
    {
        fprintf(stderr, "ERROR: invalid command line arguments\n");
        return 1;
    }

    //store matchups file
    FILE* matchups = fopen(argv[1], "r");
    if(matchups == NULL)
    {
        fprintf(stderr, "ERROR: could not open file\n");
        return 1;
    }

    char* mode = argv[2];
    if(strcmp(mode, "score") != 0 && strcmp(mode, "win")!= 0)
    {
        fprintf(stderr, "ERROR: invalid mode\n");
        return 1;
    }

    int bf_count = argc - 3;
    int battlefields[bf_count];
    
    for(int i = 3; i < argc; i++)
    {
       battlefields[i - 3] = atoi(argv[i]);
    }

    //create hash table
    smap* m = smap_create(smap_default_hash);
    if(m == NULL)
    {
        fprintf(stderr, "ERROR: could not create smap\n");
        return 1;
    }

    //populate hash table with standard input
    entry curr_entry;
    scoreboard* current;
    while((curr_entry = entry_read(stdin, 31, bf_count)).id != NULL && curr_entry.distribution != NULL)
    {
        //initliaze scoreboard
        current =  malloc(sizeof(scoreboard));
        if(current != NULL)
        {
            int mem_size = sizeof(int) * bf_count;
            current->distribution = malloc(mem_size);
            if(current->distribution == NULL)
            {
                free(current->distribution);
                free(current);
                fprintf(stderr, "ERROR: memory allocation error when initializing entry\n");
                return 1;
            }
            memcpy(current->distribution, curr_entry.distribution, mem_size);

            current->games_played = 0;
            current->wins = 0;
            current->total_score = 0;
            current->average = 0;

            smap_put(m, curr_entry.id, current);


        }
        else 
        {
            free(current);
            fprintf(stderr, "ERROR: memory allocation error when initializing entry\n");
            return 1;
        }

        entry_destroy(&curr_entry);
    }

    //get all keys
    char** keys = (char**) smap_keys(m);
    int size = smap_size(m);
    
    //get all values (scoreboards)
    scoreboard** values = malloc(sizeof(scoreboard*)* size);
    for(int i = 0; i < size; i++)
    {
        values[i] = (scoreboard*) smap_get(m, keys[i]);
    }
     
    //update scoreboard
    play(m, matchups, bf_count, battlefields);
    
    //update average wins/score based on mode
    smap_for_each(m, average, mode);

    scoreboard* temp_val;
    char* temp_key;
    //sort in decreasing order
    int j;
    for(int i = 1; i < size; i++) //insertion sort
    {
        temp_key = keys[i];
        temp_val = values[i];
        j= i-1;
        while(j>= 0 && ((temp_val->average > values[j]->average) || ((temp_val->average == values[j]->average) && (strcmp(keys[j], temp_key) > 0))))
        {
            keys[j+1] = keys[j];
            values[j+1] = values[j];
            j--;
        }
        values[j+1] = temp_val;
        keys[j+1] = temp_key;
    }

    //print
    for(int i = 0; i < size; i++)
    {
        if(values[i]->games_played > 0)
        {
            printf("%7.3f %s\n", values[i]->average,keys[i]);
        }
    }
    
    //free memory and close file
    free_values(values, size);
    free(keys);
    smap_destroy(m);

    fclose(matchups);
    return 0;
}

void play(smap* m, FILE* matchups, int bf_count, int* battlefields)
{
    char* key1 = malloc(sizeof(char) * 31);
    char* key2 = malloc(sizeof(char) * 31);

    if(key1 == NULL || key2 == NULL)
    {
        free(key1);
        free(key2);
        return;
    }

    scoreboard* value1;
    scoreboard* value2;

    float subscore1, subscore2, units;

    while(fscanf(matchups, "%31s %31s", key1, key2) != EOF)
    {
        if(!smap_contains_key(m, key1) || !smap_contains_key(m, key2))
        {
            free(key1);
            free(key2);
            return;
        }

        value1 = (scoreboard*) smap_get(m, key1);
        value2 = (scoreboard*) smap_get(m, key2);

        value1->games_played++;
        value2->games_played++;

        subscore1 = 0;
        subscore2 = 0;

        for(int i = 0; i < bf_count; i++)
        {
            units = battlefields[i];

            if(value1->distribution[i] > value2->distribution[i])
            {
                subscore1 += units;
            }
            else if(value1->distribution[i] < value2->distribution[i])
            {
                subscore2 += units;
            }
            else
            {
                subscore1 += units/2;
                subscore2 += units/2;
            }
        }

        if(subscore1 > subscore2)
        {
            value1->wins += 1;
        }
        else if(subscore1 < subscore2)
        {
            value2->wins += 1;
        }
        else
        {
            value1->wins += 0.5;
            value2->wins += 0.5;
        }
        
        value1->total_score += subscore1;
        value2->total_score += subscore2;
    }

    free(key1);
    free(key2);
}

void average(const char *key, void *value, void *mode)
{
    if(key!= NULL && value != NULL && mode != NULL)
    {
        scoreboard* value_ = (scoreboard*) value;
        char* mode_ = (char*) mode;

        if(value_->games_played > 0)
        {
            if(strcmp(mode_, "win") == 0)
            {
                value_->average = value_->wins / value_->games_played;
            }
            else
            {
                value_->average = value_->total_score / value_->games_played;
            }

        }
    }
}

void free_values(scoreboard** values, int size)
{
    for(int i = 0; i < size; i++)
    {
        
        free(values[i]->distribution);
        free(values[i]);
    }
 
    free(values);
}
