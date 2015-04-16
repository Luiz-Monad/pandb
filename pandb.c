#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define EXISTENT_TABLE 1
#define NEW_TABLE 2

#define EMPTY_TABLE "100"

int create_table(char* name);
char* set_table(char* table, char* key, char* value);
char* get_table(char* table, char* key);

/**
* Entry point.
* Valid options:
* ./pandb <access_mode> <table> <key> [<value>]
* @author: Marcelo Camargo
* @param: int
* @param: char**
* @return: int
*/
char* main(int argc, char** argv)
{
    int state = create_table(argv[2]);

    //if (strcmp(argv[1], "get") != 0 && state == NEW_TABLE)
    //{
    //    printf("ERROR");
    //    return EMPTY_TABLE;
    //}

    if (strcmp(argv[1], "get") == 0)
    {
        return get_table(argv[2], argv[3]);
    }
    else if (strcmp(argv[1], "set") == 0)
    {
        return set_table(argv[2], argv[3], argv[4]);
    }
}

#define OPEN_APPEND "a+b"
#define OPEN_READ "rb"
#define OPEN_READWRITE "r+b"

/**
* Opens a named table, create the file if it doesn't exist before.
*/
static FILE *open_table(char* table, char* mode)
{
    char *fn;
    FILE *f;
    fn = (char*)malloc(strlen(table) + 4);
    strcpy(fn, table);
    strcat(fn, ".db");
    f = fopen(fn, mode);
    free(fn);
    return f;
}

/**
* The record.
*/
typedef struct _record
{
    char key[80];
    char value[80];
    int next;
} record;

/**
* The number of slots in the hash table.
*/
static const int file_size = 256;

/**
* Hashes a key, uses bjb2.
*/
static int hash(char* key)
{
    int i;
    long h = 5381;
    for(i = 0; i < strlen(key); i++)
        h = ((h << 5) + h) + key[i]; 
    return h % file_size;
}

/**
* Creates a new table when a table with the same name doesn't exist.
* @author: Marcelo Camargo
* @param: char*
* @return: int
*/
int create_table(char* name)
{
    record* rec;
    FILE *tb;
    int p;

    //opens the hash table
    tb = open_table(name, OPEN_APPEND);
    if (tb == NULL) 
        return 0;

    //go to the end to see the file size
    fseek(tb, 0, SEEK_END);
    p = ftell(tb);

    //if it's zero, we need to preinit the hash table
    if (p == 0)
    {
        rec = (record*)calloc(1, sizeof(record));
        for(int i = 0; i < file_size; i++)
            fwrite(rec, sizeof(record), 1, tb);
        free(rec);
    }

    //close for now
    fclose(tb);

    //return the status
    if (p == 0)
        return NEW_TABLE;
    else
        return EXISTENT_TABLE;
}

/**
* Finds the record by the specified key.
Returns null if not found.
The file cursor will be on top of the record.
*/
static record* find_table(FILE* tb, char* key)
{
    record* rec;
    int next;

    //temp record
    rec = (record*)malloc(sizeof(record));

    //hash the key
    next = hash(key);

    //go to the hash table position 
    //and jump throught the record until
    //we find the correct record or not
    do
    {
        fseek(tb, sizeof(record) * next, SEEK_SET);
        fread(rec, sizeof(record), 1, tb);
        next = rec->next;
    } while (rec->next != 0 && strcmp(rec->key, key) != 0);

    //rewind one record per spec
    fseek(tb, ftell(tb) - sizeof(record), SEEK_SET);

    return rec;  
}

/**
* Get a value by the key from the table.
*/
char* get_table(char* table, char* key)
{
    FILE* tb;
    record* rec;
    char* value;

    //opens the hash table
    tb = open_table(table, OPEN_READ);
    if (tb == NULL) return NULL;

    //find the record in the hash table
    rec = find_table(tb, key);
    if (rec == NULL) return NULL;

    //copy needed data
    value = (char*)malloc(strlen(rec->value));
    strcpy(value, rec->value);

    //free and return 
    free(rec);
    return value;

}

/**
* Add a key-value pair to the table.
*/
char* set_table(char* table, char* key, char* value)
{
    FILE* tb;
    record* rec;
    int pos;

    //opens the hash table
    tb = open_table(table, OPEN_READWRITE);
    if (tb == NULL) return NULL;

    //find the record in the hash table
    rec = find_table(tb, key);

    //get actual position
    pos = ftell(tb) / sizeof(record);

    //test if it's not an empty slot
    if (rec->key[0] != 0) 
    {

        //go to the end of the file to know 
        //their position
        fseek(tb, 0, SEEK_END);

        //link the next record to the 
        //new record at the end of the file
        rec->next = ftell(tb) / sizeof(record);

        //go back to where we were and save
        fseek(tb, pos * sizeof(record), SEEK_CUR);
        fwrite(rec, sizeof(record), 1, tb);

    } 

    //create the new record
    strncpy(rec->key, key, sizeof(rec->key));
    strncpy(rec->value, value, sizeof(rec->value));
    rec->next = 0;

    //save it to the specified position
    fseek(tb, pos * sizeof(record), SEEK_SET);
    fwrite(rec, sizeof(record), 1, tb);

    //closes the table
    fclose(tb);

    //free memory
    free(rec);

}
