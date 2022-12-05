//
// Created by naqni on 20/11/2022.
//
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "linked_list.h"
#include "markov_chain.h"
#define ALLOCATION_ERROR "Allocation failure"

/**
* Get random number between 0 and max_number [0, max_number).
* @param max_number maximal number to return (not including)
* @return Random number
*/
int get_random_number (int max_number)
{
  return rand () % max_number;
}

/**
 * insert MarkovChain struct
 */
/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */
MarkovNode *get_first_random_node (MarkovChain *markov_chain)
{
  if (!markov_chain)
  {
    return NULL;
  }

  Node *first = markov_chain->database->first;
  Node *cur;
  do//first time anyway - if ran word endswith "." we shall try again
  {
    int ran = get_random_number (markov_chain->database->size);
    cur = first;
    for (int i = 0; i < ran; i++)
    {
      cur = cur->next;
    }
  }
  while (markov_chain->is_last(cur->data->data));
  return cur->data;//markov_node
}

/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr)
{
  if (!state_struct_ptr)
  {
    return NULL;
  }
  int ran =
      get_random_number (state_struct_ptr->overall_prob);
  for (int i = 0; i < state_struct_ptr->length_words_after; i++)
  {
    if (ran < state_struct_ptr->counter_list[i].frequency)
    {
      return state_struct_ptr->counter_list[i].markov_node;
    }
    else
    {
      ran -= state_struct_ptr->counter_list[i].frequency;
    }
  }
  return NULL;
}

/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with, if NULL- choose a random markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_random_sequence (MarkovChain *markov_chain, MarkovNode *
first_node, int max_length)
{
  if(!first_node||markov_chain->is_last(first_node->data)){
    first_node = get_first_random_node (markov_chain);
  }
  markov_chain->print_func(first_node->data);
  for (int i = 1; i < max_length; i++)
  {

    first_node = get_next_random_node (first_node);
    markov_chain->print_func(first_node->data);
    if(markov_chain->is_last(first_node->data)){
      break;
    }
  }
}

/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_markov_chain (MarkovChain **ptr_chain)
{
  if (!ptr_chain)
  {
    return;
  }
  Node *cur = (*ptr_chain)->database->first;
  Node *next = cur->next;
  int counter = 0;
  while ( counter < (*ptr_chain)->database->size)
  {
    (*ptr_chain)->free_data(cur->data->data); //free data and point null
    cur->data->data = NULL;

    if(cur != NULL && cur->data != NULL && cur->data->counter_list != NULL)
    {
      free(cur->data->counter_list); // free counter list
      cur->data->counter_list = NULL;
    }
    free (cur->data);
    free (cur); // free markov node
    cur = next;
    if (next)
    {
      next = next->next; // next in the chain
    }
    counter++;
  }
  free ((*ptr_chain)->database);
  (*ptr_chain)->database = NULL;
  free (*ptr_chain);
  *ptr_chain = NULL;
}
//search in the void

NextNodeCounter *search_frequency_list(
    MarkovNode *first_node,MarkovNode *second_node,MarkovChain markov_chain) {
  void *word = second_node->data;
  NextNodeCounter *frequency_lst = first_node->counter_list;
  for (int i = 0; i < first_node->length_words_after; i++) {
    // Use a comparison function to compare the data in the MarkovNode instead of strcmp.
    if (markov_chain.comp_func(word, frequency_lst[i].markov_node->data) == 0) {
      return &frequency_lst[i];
    }
  }
  return NULL;
}
/**
 * Add the second markov_node to the counter list of the first markov_node.
 * If already in list, update it's counter value.
 * @param first_node
 * @param second_node
 * @param markov_chain
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 */
bool
add_node_to_counter_list (
    MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain)
{
  if (first_node->counter_list == NULL)
  {
    first_node->counter_list =
        (NextNodeCounter *) malloc (sizeof (NextNodeCounter));
    if (!first_node->counter_list)
    {
      fprintf (stdout, ALLOCATION_ERROR);
      return false;
    }
    first_node->length_words_after++;
    first_node->overall_prob++;
    first_node->counter_list[0].markov_node = second_node;
    first_node->counter_list[0].frequency = 1;
    return true;

  }
  NextNodeCounter *already_there =
      search_frequency_list (first_node, second_node,*markov_chain);
  if (already_there)
  {
    already_there->frequency += 1;
    first_node->overall_prob++;
    return true;
  }
  else
  {
    if (first_node->counter_list)
    {
      NextNodeCounter *temp = NULL;
      first_node->length_words_after++;
      first_node->overall_prob++;
      temp = realloc (first_node->counter_list,
                      first_node->length_words_after
                      * sizeof (NextNodeCounter));
      if (!temp)
      {
        fprintf (stdout, ALLOCATION_ERROR);
        return false;
      }
      first_node->counter_list = temp;
      first_node->
          counter_list[first_node->length_words_after - 1].markov_node
          = second_node;
      first_node->counter_list[first_node->length_words_after - 1]
          .frequency = 1;
      return true;
    }
    return false;
  }
}

/**
* Check if data_ptr is in database. If so, return the markov_node wrapping it in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */

Node *get_node_from_database (MarkovChain *markov_chain, void
*data_ptr)
{
  if (!data_ptr || !markov_chain)
  {
    return NULL;
  }
  Node *cur = markov_chain->database->first;
  if (!markov_chain->database->size)
  {//no words yet
    return NULL;
  }
  int found = markov_chain->comp_func (data_ptr, cur->data->data);
  while (found)
  {
    cur = cur->next;
    if (!cur)
    {
      return NULL;
    }
    found = markov_chain->comp_func (data_ptr, cur->data->data);
  }
  return cur;
}

/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return node wrapping given data_ptr in given chain's database
 */
Node *add_to_database (MarkovChain *markov_chain, void
*data_ptr)
{
  Node *found_node = get_node_from_database (markov_chain, data_ptr);
  if (found_node)
  {
    return found_node;
  }
  else
  {
    void *data = markov_chain->copy_func(data_ptr);
    if (!data)
    {
      return NULL;
    }
    MarkovNode *markov_node = malloc (sizeof (MarkovNode));
    if (!markov_node)
    {
      markov_chain->free_data (data);
      return NULL;
    }
    markov_node->length_words_after = 0;
    markov_node->overall_prob = 0;
    markov_node->counter_list = NULL;

    markov_node->data = data;
    if (add (markov_chain->database, markov_node))
    {
      free (markov_node);
      markov_chain->free_data (data);
      return NULL;
    }
    else
    {
      return markov_chain->database->last;
    }
  }
}


