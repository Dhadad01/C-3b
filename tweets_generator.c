//
// Created by naqni on 21/11/2022.
//arg1 - seed, no check needed
//arg2 - number of tweats to create
//arg3 - path to file
//arg4 - num of words to read
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "linked_list.h"
#include "markov_chain.h"
#define INVALID_FILE "The given file is invalid.\n"
#define INVALID_NUM_OF_ARGS "Usage:invalid num of args"
#define MAX_LENGTH_TWEET 1000
#define MAX_LEN_OF_TWEET 20

#define FOUR 4
#define THREE 3
#define TEN 10
#define ZERO 0
#define FIVE 5
int fill_database (FILE *fp, int words_to_read, MarkovChain
*markov_chain)
{
  {
    if(!fp||!markov_chain)
    {
      return EXIT_FAILURE;
    }
    int counter = -1;
    if (words_to_read)
    {
      counter = ZERO;
    }
    char line[MAX_LENGTH_TWEET + 1];

    while (fgets (line, MAX_LENGTH_TWEET + 1, fp) &&
           counter < words_to_read)
    {
      char *token = strtok (line, " \n");
      Node *last_node = NULL;
      while (token&&counter < words_to_read)
      {
        Node *temp = add_to_database (markov_chain, token);
        if (words_to_read)
        {
          counter++;
        }
        if (temp && last_node
            && !markov_chain->is_last(last_node->data->data))
        {
          if(!add_node_to_counter_list
              (last_node->data, temp->data,markov_chain)){
           return EXIT_FAILURE;
          }
        }
        last_node = temp;
        token = strtok (NULL, " \n");
      }
    }
  }
  return EXIT_SUCCESS;
}
int comp_func_str(void *s1,void*s2) {
// Convert the generic pointers to char pointers
char* str1 = (char*) s1;
char* str2 = (char*) s2;

// Use strcmp to compare the strings
int result = strcmp(str1, str2);

// Return the result of the comparison
return result;
};
static void print_str(void *s1){
  char* str1 = (char*) s1;
  printf ("%s ",str1);
}
static void free_str(void *s1){
  char* str1 = (char*) s1;
  free (str1);
}
static void* mem_cpy_str(void *s1){
  char *str1 = (char*) s1;
  char* copy = malloc (strlen (str1)+1);
  strcpy (copy,str1);
  return copy;
}
static bool is_last_str(void *s1){
  char *str1 = (char*) s1;
  if(str1[strlen (str1)-1]=='.'){
    return true;
  }
  return false;
}

int main (int argc, char *argv[])
{
  if (argc > FIVE || argc < FOUR)
  {
    fprintf (stdout, INVALID_NUM_OF_ARGS);
    return EXIT_FAILURE;
  }
  if (!argv[THREE])
  {
    fprintf (stdout, INVALID_FILE);
    return EXIT_FAILURE;
  }
  FILE *fp = fopen (argv[THREE], "r");
  if (!fp)
  {
    fprintf (stdout, INVALID_FILE);
    return EXIT_FAILURE;
  }
  //valid input - check on the last var
  unsigned int seed = (unsigned int) strtol (argv[1],NULL,TEN);
  int num_of_tweets = (int) strtol (argv[2], NULL, TEN);
  int words_to_read;
  if (argc == FOUR)
  {
    words_to_read = ZERO;//all
  }
  else
  {
    words_to_read = (int) strtol (argv[FOUR], NULL, TEN);
  }
  MarkovChain *markov_chain=(MarkovChain *) malloc (sizeof (MarkovChain));
  if(!markov_chain){
    fprintf (stdout,ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  markov_chain->database = (LinkedList *) malloc (sizeof (LinkedList));
  if(!markov_chain->database){
    fprintf (stdout,ALLOCATION_ERROR_MASSAGE);
    free (markov_chain);
    return EXIT_FAILURE;
  }
  markov_chain->print_func = print_str;
  markov_chain->comp_func = comp_func_str;
  markov_chain->free_data = free_str;
  markov_chain->copy_func = mem_cpy_str;
  markov_chain->is_last = is_last_str;
  markov_chain->database->first = NULL;
  markov_chain->database->last = NULL;
  markov_chain->database->size = ZERO;
  if (fill_database (fp, words_to_read, markov_chain)){
    free_markov_chain (&markov_chain);
    return EXIT_FAILURE;
  }
  fclose(fp);
  srand (seed);
  for (int i = ZERO; i < num_of_tweets; i++)
  {
    printf ("Tweet %d: ",i+1);
    generate_random_sequence
        (markov_chain, NULL, MAX_LEN_OF_TWEET);
  }
  free_markov_chain (&markov_chain);
  return EXIT_SUCCESS;
}


