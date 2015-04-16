#include <stdio.h>
#include <string.h>
#include <unistd.h>

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
  char table[80];
  FILE* table_ref;
  int state = create_table(argv[2]);

  // if (strcmp(argv[1], "get") != 0 && state == NEW_TABLE)
  // {
  //   printf("ERROR");
  //   return EMPTY_TABLE;
  // }

  if (strcmp(argv[1], "get") == 0)
  {
    //return get_table(argv[2], argv[3]);
  }
  else if (strcmp(argv[1], "set") == 0)
  {
    return set_table(argv[2], argv[3], argv[4]);
  }
}

/**
 * Creates a new table when a table with the same name doesn't exist.
 * @author: Marcelo Camargo
 * @param: char*
 * @return: int
 */
int create_table(char* name)
{
  char table[80];
  FILE* table_ref;

  strcpy(table, name);
  strcat(table, ".db");

  if (access(table, F_OK) != -1)
  {
    return EXISTENT_TABLE;
  }
  else
  {
    table_ref = fopen(table, "w+");
    fclose(table_ref);
    return NEW_TABLE;
  }
}

char* get_table(char* table, char* key)
{

}

char* set_table(char* table, char* key, char* value)
{
  char tb[80];
  FILE* table_ref;
  strcpy(tb, table);
  strcat(tb, ".db");
  table_ref = fopen(tb, "w+");
  fprintf(table_ref, "%s\n", "Java");
  return value;
}